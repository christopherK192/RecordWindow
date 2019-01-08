#include "include/settingdialog.h"
#include "ui_settingdialog.h"

/*!
 * \brief SettingDialog::SettingDialog
 *        Constructor of the SettingDialog class
 *
 * The constructor initializes the ui, connects the button signals and loads the settings.
 * Additionally it iterates over all open windows and gets the list of all open window names.
 *
 * \param parent Parent window of the SettingDialog class (inherits form QDialog).
 */
SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    // initialize ui
    ui->setupUi(this);

    // uncheck non-set options
    ui->check_videoStreamer->setVisible(false);
    ui->check_videoPlayer->setVisible(false);
    ui->check_videoPath->setVisible(false);
    ui->check_videoPath_2->setVisible(false);

    // set used options
    ui->radioButton_vMix->setChecked(true);
    ui->radioButton_Kamera1->setChecked(true);
    ui->frame_kameras->setEnabled(false);
    ui->lineEdit_Path_2->setEnabled(false);
    ui->button_openFolder_2->setEnabled(false);

    // getting the window names of all open windows
    // SettingDialog::getOpenWindows is the callback function for all windows.
    QStringList windowList;
    EnumWindows(&SettingDialog::getOpenWindows, reinterpret_cast<LPARAM>(&windowList));
    ui->comboBox_Windows->addItems(windowList);
    ui->comboBox_Player->addItems(windowList);

    ui->comboBox_videoContainer->addItem("Advanced Streaming Format (*.asf, *.wmv, *.wma)");
    ui->comboBox_videoContainer->addItem("Audio Video Interleave (*.avi)");
    ui->comboBox_videoContainer->addItem("BDAV MPEG-2 Transport Stream (*.m2ts, *.mts)");
    ui->comboBox_videoContainer->addItem("Digital Video (*.dv)");
    ui->comboBox_videoContainer->addItem("DivX Media Format (*.divx)");
    ui->comboBox_videoContainer->addItem("Enhanced VOB (*.evo)");
    ui->comboBox_videoContainer->addItem("Flash Video (*.flv)");
    ui->comboBox_videoContainer->addItem("Material Exchange Format (*.mxf)");
    ui->comboBox_videoContainer->addItem("Matroska (*.mkv, *.mka)");
    ui->comboBox_videoContainer->addItem("MPEG-2-Program Stream (*.mpg, *.mpeg, *.ps)");
    ui->comboBox_videoContainer->addItem("MPEG-2 Transport Stream (*.ts, *.tsp)");
    ui->comboBox_videoContainer->addItem("MP4 (*.mp4)");
    ui->comboBox_videoContainer->addItem("Ogg Media (*.ogg, *.ogv, *.ogm)");
    ui->comboBox_videoContainer->addItem("OMFI (*.omf)");
    ui->comboBox_videoContainer->addItem("QuickTime (*.mov, *.qt)");
    ui->comboBox_videoContainer->addItem("RealMedia (*.rm, *.rmvb, *.ra, *.ram)");
    ui->comboBox_videoContainer->addItem("Video Object (*.vob)");
    ui->comboBox_videoContainer->addItem("WebM (*.webm)");

    // get saved settings
    readSettings();

    // connect signals to buttons
    connect(ui->button_connect2App, &QToolButton::clicked, [this](){SettingDialog::connect2Streamer(true);});
    connect(ui->button_connect2Player, &QToolButton::clicked, [this](){SettingDialog::connect2Player(true);});
    connect(ui->comboBox_videoContainer, &QComboBox::currentTextChanged, this, &SettingDialog::setVideoContainer);
    connect(ui->button_openFolder, &QToolButton::clicked, [this](){SettingDialog::setPath(1);});
    connect(ui->button_openFolder_2, &QToolButton::clicked, [this](){SettingDialog::setPath(2);});
    connect(ui->radioButton_vMix, &QRadioButton::clicked, this, &SettingDialog::setUseStreamer);
    connect(ui->radioButton_foscam, &QRadioButton::clicked, this, &SettingDialog::unsetUseStreamer);
    connect(ui->radioButton_Kamera1, &QRadioButton::clicked, this, &SettingDialog::setKameras);
    connect(ui->radioButton_Kamera2, &QRadioButton::clicked, this, &SettingDialog::unsetKameras);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingDialog::closeSettings);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SettingDialog::reject);
}

/*!
 * \brief SettingDialog::~SettingDialog
 *        Destructor
 */
SettingDialog::~SettingDialog()
{
    delete ui;
}

/*!
 * \brief SettingDialog::errorHandler
 *        Opens a Message Box with an error message.
 *
 * \param text The text of the error message.
 */
void SettingDialog::errorHandler(QString text)
{
    QMessageBox message;
    message.setWindowTitle("IVR-Record App");
    message.setWindowIcon(QIcon(":/Icons/logo.ico"));
    message.setIcon(QMessageBox::Critical);
    message.setDefaultButton(QMessageBox::Ok);
    message.setText(text.toStdString().c_str());
    QFont font;
    font.setPointSize(9);
    message.setFont(font);
    message.exec();
}

/*!
 * \brief SettingDialog::infoHandler
 *        Opens a Message Box with an info message.
 *
 * \param text The text of the info message.
 */
void SettingDialog::infoHandler(QString text)
{
    QMessageBox message;
    message.setWindowTitle("IVR-Record App");
    message.setWindowIcon(QIcon(":/Icons/logo.ico"));
    message.setIcon(QMessageBox::Information);
    message.setDefaultButton(QMessageBox::Ok);
    message.setText(text.toStdString().c_str());
    QFont font;
    font.setPointSize(9);
    message.setFont(font);
    message.exec();
}

/*!
 * \brief SettingDialog::getOpenWindows
 *        Callback function for each open window.
 *
 * Extracts the window name of each window and stores them in a list.
 *
 * \param window Window Handle of the current Window.
 * \param param In this case it's the list of all window names.
 *
 * \return Returns true if successful.
 */
BOOL CALLBACK SettingDialog::getOpenWindows(HWND window, LPARAM param)
{
    const int titleSize = 1024;
    char windowTitle[titleSize];

//    GetClassNameA(window, windowTitle, titleSize);
    GetWindowTextA(window, windowTitle, titleSize);
    QString title = QString::fromUtf8(windowTitle);

    if(IsWindowVisible(window) && !title.isEmpty() && title != "Program Manager")
    {
        QStringList& windowList = *reinterpret_cast<QStringList*>(param);
        windowList << title;
    }

    return true;
}

/*!
 * \brief SettingDialog::get_useStreamer
 *        Get method for useStreamer.
 *
 * \return Bool if Streamer is used.
 */
bool SettingDialog::get_useStreamer()
{
    return m_useStreamer;
}

/*!
 * \brief SettingDialog::get_2Kameras
 *        Get method for 2Kameras.
 *
 * \return Bool if Streamer is used.
 */
bool SettingDialog::get_2Kameras()
{
    return m_2Kameras;
}

/*!
 * \brief SettingDialog::get_savePath1
 *        Get method for the savepath.
 *
 * \return Savepath as QString.
 */
QString SettingDialog::get_savePath()
{
    return m_savePath1;
}

/*!
 * \brief SettingDialog::get_savePath2
 *        Get method for the savepath2.
 *
 * \return Savepath2 as QString.
 */
QString SettingDialog::get_savePath2()
{
    return m_savePath2;
}

/*!
 * \brief SettingDialog::get_videoStreamer
 *        Get method for the video streamer or recorder.
 *
 *  \return Video streamer windowname as QString
 */
QString SettingDialog::get_videoStreamer()
{
    return m_videoStreamer;
}

/*!
 * \brief SettingDialog::get_videoPlayer
 *        Get method for the video player.
 *
 * \return Video player windowname as QString.
 */
QString SettingDialog::get_videoPlayer()
{
    return m_videoPlayer;
}

/*!
 * \brief SettingDialog::get_videoContainer
 *        Get method for the video container.
 *
 * \return Video container as QString.
 */
QStringList SettingDialog::get_videoContainer()
{
    QStringList liste;
    liste = m_videoContainer.replace("(", ", ").replace(")", ", ").split(", ", QString::SkipEmptyParts);
    liste.removeFirst();
    return liste;
}

/*!
 * \brief SettingDialog::closeEvent
 *        Override for the close event of the QDialog.
 *
 * Overrides the close event in order to save the settings.
 *
 * \param event QCloseEvent
 */
void SettingDialog::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

/*!
 * \brief SettingDialog::readSettings
 *        Reads the saved settings when constructed.
 *
 * Settings: Windoname of the last selected video streamer and video player as well as the savepath.
 */
void SettingDialog::readSettings()
{
    QSettings settings("CHK-Soft", "IVR-Record App");
    settings.beginGroup("IVRsettings");

    // video streamer if open
    const QString video_Streamer = settings.value("videoStreamer", "-").toString();
    if(!video_Streamer.isEmpty())
    {
        int index = ui->comboBox_Windows->findText(video_Streamer, Qt::MatchExactly);
        ui->comboBox_Windows->setCurrentIndex(index);
        connect2Streamer(false);
    }
    else
    {
        ui->comboBox_Windows->setCurrentIndex(0);
        ui->check_videoStreamer->setVisible(false);
    }

    // savepath if existing
    const QString save_Path1 = settings.value("savePath", "").toString();
    if(!save_Path1.isEmpty())
    {
        ui->lineEdit_Path->setText(save_Path1);
        ui->check_videoPath->setVisible(true);
        m_savePath1 = ui->lineEdit_Path->text();
    }
    else
    {
        ui->lineEdit_Path->setText("");
        ui->check_videoPath->setVisible(false);
    }

    // savepath2 if existing
    const QString save_Path2 = settings.value("savePath2", "").toString();
    if(!save_Path2.isEmpty())
    {
        ui->lineEdit_Path_2->setText(save_Path2);
        ui->check_videoPath_2->setVisible(true);
        m_savePath2 = ui->lineEdit_Path_2->text();
    }
    else
    {
        ui->lineEdit_Path_2->setText("");
        ui->check_videoPath_2->setVisible(false);
    }

    // video player if open
    const QString video_Player = settings.value("videoPlayer", "-").toString();
    if(!video_Player.isEmpty())
    {
        int index = ui->comboBox_Player->findText(video_Player, Qt::MatchExactly);
        ui->comboBox_Player->setCurrentIndex(index);
        connect2Player(false);
    }
    else
    {
        ui->comboBox_Player->setCurrentIndex(0);
        ui->check_videoPlayer->setVisible(false);
    }

    const QString video_container = settings.value("videoContainer", "").toString();
    if(!video_container.isEmpty())
    {
        int index = ui->comboBox_videoContainer->findText(video_container, Qt::MatchExactly);
        ui->comboBox_videoContainer->setCurrentIndex(index);
        m_videoContainer = ui->comboBox_videoContainer->currentText();
    }
    else
    {
        ui->comboBox_videoContainer->setCurrentIndex(0);
    }

    settings.endGroup();
}

/*!
 * \brief SettingDialog::writeSettings
 *        Saves the current settings when closed.
 *
 * Settings: Windoname of the last selected video streamer and video player as well as the savepath.
 */
void SettingDialog::writeSettings()
{
    QSettings settings("MySoft", "IVR-Record App");

    settings.beginGroup("IVRsettings");
    settings.setValue("videoStreamer", ui->comboBox_Windows->currentText());
    settings.setValue("videoContainer", ui->comboBox_videoContainer->currentText());
    settings.setValue("savePath", ui->lineEdit_Path->text());
    settings.setValue("savePath2", ui->lineEdit_Path_2->text());
    settings.setValue("videoPlayer", ui->comboBox_Player->currentText());
    settings.endGroup();
}

/*!
 * \brief SettingDialog::connect2Streamer
 *        Extract the window handle for the video streamer.
 *
 * \param manuell Suppresses the error or info message if false (called by readSettings()).
 */
void SettingDialog::connect2Streamer(bool manuell)
{
    m_videoStreamer = ui->comboBox_Windows->currentText();

    if (!(m_videoStreamer == "-"))
    {
//        HWND windowName = FindWindowA(m_videoStreamer.toStdString().c_str(), NULL);
        HWND windowName = FindWindowA(NULL, m_videoStreamer.toStdString().c_str());
        if(windowName == NULL)
        {
            ui->check_videoStreamer->setVisible(false);
            m_videoStreamer = "";

            if(manuell)
                errorHandler(tr("FEHLER:\nEs konnte keine Instanz des angegebenen Programms gefunden werden!\nBitte überprüfen Sie, ob das Programm geöffnet ist!"));
        }
        else
        {
            ui->check_videoStreamer->setVisible(true);
            if(manuell)
                infoHandler(tr("Verbindung mit dem Programm wurde erfolgreich hergestellt."));
        }
    }
    else
    {
        ui->check_videoStreamer->setVisible(false);
        if(manuell)
            errorHandler(tr("FEHLER:\nEs wurde kein Programm ausgewählt!\nBitte wählen Sie ein Programm aus!"));
    }
}

/*!
 * \brief SettingDialog::connect2Player
 *        Extract the window handle for the video player.
 *
 * \param manuell Suppresses the error or info message if false (called by readSettings()).
 */
void SettingDialog::connect2Player(bool manuell)
{
    m_videoPlayer = ui->comboBox_Player->currentText();

    if (!(m_videoPlayer == "-"))
    {
//       HWND windowHandler = FindWindowA(m_videoPlayer.toStdString().c_str(), NULL);
        HWND windowHandler = FindWindowA(NULL, m_videoPlayer.toStdString().c_str());
        if(windowHandler == NULL)
        {
            ui->check_videoPlayer->setVisible(false);
            m_videoPlayer = "";

            if(manuell)
                errorHandler(tr("FEHLER:\nEs konnte keine Instanz des angegebenen Programms gefunden werden!\nBitte überprüfen Sie, ob das Programm geöffnet ist!"));
        }
        else
        {
            ui->check_videoPlayer->setVisible(true);
            if(manuell)
                infoHandler(tr("Verbindung mit dem Programm wurde erfolgreich hergestellt."));
        }
    }
    else
    {
        ui->check_videoPlayer->setVisible(false);
        if(manuell)
            errorHandler(tr("FEHLER:\nEs wurde kein Programm ausgewählt!\nBitte wählen Sie ein Programm aus!"));
    }
}

/*!
 * \brief SettingDialog::setPath
 *        Gets and sets the savepath over the File Dialog.
 *        Called by [...]-button.
 */
void SettingDialog::setPath(int path)
{
    if (path == 1)
    {
        QString savepath = QFileDialog::getExistingDirectory(this, tr("Ordner Öffnen"), m_savePath1, QFileDialog::DontResolveSymlinks);

        if(savepath.isEmpty())
        {
            ui->check_videoPath->setVisible(false);
            errorHandler(tr("FEHLER:\nEs wurde kein Ordner ausgewählt!\nBitte wählen Sie einen Ordner aus!"));
        }
        else
        {
            ui->lineEdit_Path->setText(savepath);
            ui->check_videoPath->setVisible(true);
            m_savePath1 = ui->lineEdit_Path->text();
        }
    }
    else if (path == 2)
    {
        QString savepath = QFileDialog::getExistingDirectory(this, tr("Ordner Öffnen"), m_savePath2, QFileDialog::DontResolveSymlinks);

        if(savepath.isEmpty())
        {
            ui->check_videoPath_2->setVisible(false);
            errorHandler(tr("FEHLER:\nEs wurde kein Ordner ausgewählt!\nBitte wählen Sie einen Ordner aus!"));
        }
        else
        {
            ui->lineEdit_Path_2->setText(savepath);
            ui->check_videoPath_2->setVisible(true);
            m_savePath2 = ui->lineEdit_Path_2->text();
        }
    }
}

/*!
 * \brief SettingDialog::setUseStreamer
 *        Sets the useStreamer variable;
 */
void SettingDialog::setUseStreamer()
{
    m_useStreamer = true;
    ui->comboBox_Windows->setEnabled(true);
    ui->button_connect2App->setEnabled(true);
    ui->radioButton_Kamera1->setChecked(true);
    ui->frame_kameras->setEnabled(false);
}

void SettingDialog::unsetUseStreamer()
{
    m_useStreamer = false;
    ui->comboBox_Windows->setEnabled(false);
    ui->button_connect2App->setEnabled(false);
    ui->frame_kameras->setEnabled(true);
}

/*!
 * \brief SettingDialog::setKameras
 *        Sets the 2Kameras variable;
 */
void SettingDialog::setKameras()
{
    m_2Kameras = false;
    ui->lineEdit_Path_2->setEnabled(false);
    ui->button_openFolder_2->setEnabled(false);
}

void SettingDialog::unsetKameras()
{
    m_2Kameras = true;
    ui->lineEdit_Path_2->setEnabled(true);
    ui->button_openFolder_2->setEnabled(true);
}

/*!
 * \brief SettingDialog::setVideoContainer
 *        Sets the videoContainer by change of comboBox item;
 */
void SettingDialog::setVideoContainer(const QString container)
{
    m_videoContainer = container;
}

/*!
 * \brief SettingDialog::closeSettings
 *        Returns message that settings were successfully saved, when accepted.
 */
void SettingDialog::closeSettings()
{
    infoHandler(tr("Einstellungen wurden erfolgreich gespeichert."));
    close();
}

