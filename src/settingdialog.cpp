#include "include/settingdialog.h"
#include "ui_settingdialog.h"

#include <QMessageBox>
#include <QSettings>

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
    m_bEnableMessages(true),
    ui(new Ui::SettingDialog)
{
    // initialize ui
    ui->setupUi(this);

    // videoStreamer options
    ui->checkBox_videoStreamer->setChecked(false);
    ui->comboBox_videoStreamer->setCurrentIndex(0);
    ui->frame_recorder->setEnabled(false);

    ui->lineEdit_hotkeyStart->clear();
    ui->frame_hotkeyStart->setEnabled(false);

    ui->lineEdit_hotkeyStop->clear();
    ui->frame_hotkeyStop->setEnabled(false);

    // videoPlayer options
    ui->radioButton_Kamera1->setChecked(true);
    ui->comboBox_videoPlayer->setCurrentIndex(0);

    ui->lineEdit_videoPath1->clear();
    ui->comboBox_videoContainer1->addItems(m_lVideoContainers);
    ui->comboBox_videoContainer1->setCurrentIndex(0);

    ui->lineEdit_videoPath2->clear();
    ui->comboBox_videoContainer2->addItems(m_lVideoContainers);
    ui->comboBox_videoContainer2->setCurrentIndex(0);

    ui->groupBox_Kamera2->setEnabled(false);

    // set video player as start view
    ui->tabWidget_settings->setCurrentIndex(0);

    // getting the window names of all open windows
    // SettingDialog::getOpenWindows is the callback function for all windows.
    QStringList windowList;
    EnumWindows(&SettingDialog::getOpenWindows, reinterpret_cast<LPARAM>(&windowList));
    ui->comboBox_videoStreamer->addItems(windowList);
    ui->comboBox_videoPlayer->addItems(windowList);

    // get saved settings
    readSettings();

    // connect signals to buttons
    connect(ui->button_videoStreamer, &QToolButton::clicked, this, SettingDialog::connect2Streamer);
    connect(ui->button_videoPlayer,   &QToolButton::clicked, this, SettingDialog::connect2Player);

    connect(ui->radioButton_Kamera1, &QRadioButton::clicked, [this](){SettingDialog::toggleCams(false);});
    connect(ui->radioButton_Kamera2, &QRadioButton::clicked, [this](){SettingDialog::toggleCams(true);});

    connect(ui->button_videoPath1, &QToolButton::clicked, [this](){SettingDialog::setPath(1);});
    connect(ui->button_videoPath2, &QToolButton::clicked, [this](){SettingDialog::setPath(2);});

    connect(ui->comboBox_videoContainer1, &QComboBox::currentTextChanged, this, &SettingDialog::setVideoContainer1);
    connect(ui->comboBox_videoContainer2, &QComboBox::currentTextChanged, this, &SettingDialog::setVideoContainer2);

    connect(ui->checkBox_videoStreamer, &QCheckBox::stateChanged, this, &SettingDialog::toggleStreamer);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, [this](){SettingDialog::closeSettings(ReturnState::ACCEPT);});
    connect(ui->buttonBox, &QDialogButtonBox::rejected, [this](){SettingDialog::closeSettings(ReturnState::REJECT);});

    toggleCams(false);
    toggleStreamer(Qt::CheckState::Unchecked);
}

/*!
 * \brief SettingDialog::~SettingDialog
 *        Destructor
 */
SettingDialog::~SettingDialog()
{
    delete ui;
}

const QStringList SettingDialog::videoContainer1()
{
    return extractFormat(m_sVideoContainer1);
}

const QStringList SettingDialog::videoContainer2()
{
    return extractFormat(m_sVideoContainer2);
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
    message.setWindowIcon(QIcon(ICON_LOGO));
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
    message.setWindowIcon(QIcon(ICON_LOGO));
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

    GetClassNameA(window, windowTitle, titleSize);
//    GetWindowTextA(window, windowTitle, titleSize);
    QString title = QString::fromUtf8(windowTitle);

    if(IsWindow(window) && IsWindowVisible(window) && !title.isEmpty())
    {
        QStringList& windowList = *reinterpret_cast<QStringList*>(param);
        windowList << title;
    }

    return true;
}

/*!
 * \brief SettingDialog::extractFormat
 *        Extracts the video format from the combobox item.
 *
 * \param videoContainer combobox item.
 *
 * \return Returns list of video formats.
 */
QStringList SettingDialog::extractFormat(QString videoContainer)
{
    QStringList list;
    list = videoContainer.replace('(',',').replace(')',',').split(',', QString::SkipEmptyParts);
    list.removeFirst();
    return list;
}

/*!
 * \brief SettingDialog::readSettings
 *        Reads the saved settings when constructed.
 *
 * Settings: Windowname of the last selected video streamer and video player as well as the savepaths, videocontainers and hotkeys.
 */
void SettingDialog::readSettings()
{
    m_bEnableMessages = false;

    QSettings settings("CHK-Soft", "IVR-Record App");
    settings.beginGroup("IVRsettings");

    // video streamer if open
    const QString video_Streamer = settings.value("videoStreamer", "-").toString();
    if(!video_Streamer.isEmpty())
    {
        int index = ui->comboBox_videoStreamer->findText(video_Streamer, Qt::MatchExactly);
        ui->comboBox_videoStreamer->setCurrentIndex(index);
        connect2Streamer(false);
    }
    else
    {
        m_sVideoStreamer = "";
        ui->comboBox_videoStreamer->setCurrentIndex(0);
    }

    // video player if open
    const QString video_Player = settings.value("videoPlayer", "-").toString();
    if(!video_Player.isEmpty())
    {
        int index = ui->comboBox_videoPlayer->findText(video_Player, Qt::MatchExactly);
        ui->comboBox_videoPlayer->setCurrentIndex(index);
        connect2Player(false);
    }
    else
    {
        m_sVideoPlayer = "";
        ui->comboBox_videoPlayer->setCurrentIndex(0);
    }

    // savepath1 if existing
    const QString save_Path1 = settings.value("savePath1", "").toString();
    if(!save_Path1.isEmpty())
    {
        ui->lineEdit_videoPath1->setText(save_Path1);
    }
    else
    {
        ui->lineEdit_videoPath1->setText("");
    }

    m_sSavePath1 = ui->lineEdit_videoPath1->text();

    // savepath2 if existing
    const QString save_Path2 = settings.value("savePath2", "").toString();
    if(!save_Path2.isEmpty())
    {
        ui->lineEdit_videoPath2->setText(save_Path2);
    }
    else
    {
        ui->lineEdit_videoPath2->setText("");
    }

    m_sSavePath2 = ui->lineEdit_videoPath2->text();

    // video_container1
    const QString video_container1 = settings.value("videoContainer1", "").toString();
    if(!video_container1.isEmpty())
    {
        int index = ui->comboBox_videoContainer1->findText(video_container1, Qt::MatchExactly);
        ui->comboBox_videoContainer1->setCurrentIndex(index);
    }
    else
    {
        ui->comboBox_videoContainer1->setCurrentIndex(0);
    }

    m_sVideoContainer1 = ui->comboBox_videoContainer1->currentText();

    // video_container2
    const QString video_container2 = settings.value("videoContainer2", "").toString();
    if(!video_container2.isEmpty())
    {
        int index = ui->comboBox_videoContainer2->findText(video_container2, Qt::MatchExactly);
        ui->comboBox_videoContainer2->setCurrentIndex(index);
    }
    else
    {
        ui->comboBox_videoContainer2->setCurrentIndex(0);
    }

    m_sVideoContainer1 = ui->comboBox_videoContainer2->currentText();

    // hotkeyStart if existing
    const QString hotkey_start = settings.value("hotkeyStart", "").toString();
    if(!hotkey_start.isEmpty())
    {
        ui->lineEdit_hotkeyStart->setText(hotkey_start);
    }
    else
    {
        ui->lineEdit_hotkeyStart->setText("");
    }

    m_sHotkeyStart = ui->lineEdit_hotkeyStart->text();

    // hotkeyStop if existing
    const QString hotkey_stop = settings.value("hotkeyStop", "").toString();
    if(!hotkey_start.isEmpty())
    {
        ui->lineEdit_hotkeyStop->setText(hotkey_stop);
    }
    else
    {
        ui->lineEdit_hotkeyStop->setText("");
    }

    m_sHotkeyStop = ui->lineEdit_hotkeyStop->text();

    settings.endGroup();

    m_bEnableMessages = true;
}

/*!
 * \brief SettingDialog::writeSettings
 *        Saves the current settings when closed.
 *
 * Settings: Windowname of the last selected video streamer and video player as well as the savepaths, videocontainers and hotkeys.
 */
void SettingDialog::writeSettings()
{
    QSettings settings("MySoft", "IVR-Record App");

    settings.beginGroup("IVRsettings");

    settings.setValue("videoStreamer", ui->comboBox_videoStreamer->currentText());
    settings.setValue("videoPlayer", ui->comboBox_videoPlayer->currentText());
    settings.setValue("videoPath1", ui->lineEdit_videoPath1->text());
    settings.setValue("videoPath2", ui->lineEdit_videoPath2->text());
    settings.setValue("videoContainer1", ui->comboBox_videoContainer1->currentText());
    settings.setValue("videoContainer2", ui->comboBox_videoContainer2->currentText());
    settings.setValue("hotkeyStart", ui->lineEdit_hotkeyStart->text());
    settings.setValue("hotkeyStop", ui->lineEdit_hotkeyStop->text());

    settings.endGroup();
}

/*!
 * \brief SettingDialog::connect2Streamer
 *        Extract the window handle for the video streamer.
 *
 * \param manuell Suppresses the error or info message if false (called by readSettings()).
 */
void SettingDialog::connect2Streamer()
{
    m_sVideoStreamer = ui->comboBox_videoStreamer->currentText();

    if (m_sVideoStreamer == "-")
    {
        m_sVideoStreamer = "";
        if(m_bEnableMessages)
            errorHandler(tr("FEHLER:\nEs wurde kein Programm ausgewählt!\nBitte wählen Sie ein Programm aus!"));

        return;
    }

    HWND windowName = FindWindowA(m_videoStreamer.toStdString().c_str(), nullptr);
//    HWND windowName = FindWindowA(nullptr, m_sVideoStreamer.toStdString().c_str());
    if(windowName == nullptr)
     {
        m_sVideoStreamer = "";
        if(m_bEnableMessages)
            errorHandler(tr("FEHLER:\nEs konnte keine Instanz des angegebenen Programms gefunden werden!\nBitte überprüfen Sie, ob das Programm geöffnet ist!"));
     }
    else
    {
        if(m_bEnableMessages)
            infoHandler(tr("Verbindung mit dem Programm wurde erfolgreich hergestellt."));
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
    m_sVideoPlayer = ui->comboBox_videoPlayer->currentText();

    if (!(m_sVideoPlayer == "-"))
    {
//       HWND windowHandler = FindWindowA(m_videoPlayer.toStdString().c_str(), nullptr);
        HWND windowHandler = FindWindowA(nullptr, m_sVideoPlayer.toStdString().c_str());
        if(windowHandler == nullptr)
        {
            m_sVideoPlayer = "";
//            ui->check_videoPlayer->setPixmap(QPixmap(ICON_CHECK_NO));
            if(manuell)
                errorHandler(tr("FEHLER:\nEs konnte keine Instanz des angegebenen Programms gefunden werden!\nBitte überprüfen Sie, ob das Programm geöffnet ist!"));
        }
        else
        {
//            ui->check_videoPlayer->setPixmap(QPixmap(ICON_CHECK_OK));
            if(manuell)
                infoHandler(tr("Verbindung mit dem Programm wurde erfolgreich hergestellt."));
        }
    }
    else
    {
        m_sVideoPlayer = "";
//        ui->check_videoPlayer->setPixmap(QPixmap(ICON_CHECK_NO));
        if(manuell)
            errorHandler(tr("FEHLER:\nEs wurde kein Programm ausgewählt!\nBitte wählen Sie ein Programm aus!"));
    }
}

/*!
 * \brief SettingDialog::setPath
 *        Gets and sets the savepath over the File Dialog.
 *        Called by [...]-button.
 *
 * \param cam Sets the path for camera 1 or two.
 */
void SettingDialog::setPath(int cam)
{
    if (cam == 1)
    {
        QString savepath = QFileDialog::getExistingDirectory(this, tr("Ordner Öffnen"), m_sSavePath1, QFileDialog::DontResolveSymlinks);

        if(savepath.isEmpty())
        {
//            ui->check_videoPath1->setPixmap(QPixmap(ICON_CHECK_NO));
            errorHandler(tr("FEHLER:\nEs wurde kein Ordner ausgewählt!\nBitte wählen Sie einen Ordner aus!"));
        }
        else
        {
            ui->lineEdit_videoPath1->setText(savepath);
            m_sSavePath1 = ui->lineEdit_videoPath1->text();
//            ui->check_videoPath1->setPixmap(QPixmap(ICON_CHECK_OK));
        }
    }
    else if (cam == 2)
    {
        QString savepath = QFileDialog::getExistingDirectory(this, tr("Ordner Öffnen"), m_sSavePath2, QFileDialog::DontResolveSymlinks);

        if(savepath.isEmpty())
        {
//            ui->check_videoPath2->setPixmap(QPixmap(ICON_CHECK_NO));
            errorHandler(tr("FEHLER:\nEs wurde kein Ordner ausgewählt!\nBitte wählen Sie einen Ordner aus!"));
        }
        else
        {
            ui->lineEdit_videoPath2->setText(savepath);
            m_sSavePath2 = ui->lineEdit_videoPath2->text();
//            ui->check_videoPath2->setPixmap(QPixmap(ICON_CHECK_OK));
        }
    }
}

/*!
 * \brief SettingDialog::toggleKameras
 *        Toggles number of cameras.
 *
 * \param camera2 True if two cameras are to be used.
 */
void SettingDialog::toggleCams(bool cam2)
{
    m_bKameras2 = cam2;
    ui->groupBox_Kamera2->setEnabled(cam2);
}

/*!
 * \brief SettingDialog::toggleStreamer
 *        Toggles state of the checkbox and enables or disables videostreamer options.
 *
 * \param state State of the checkbox (Checked, Unchecked).
 */
void SettingDialog::toggleStreamer(int state)
{
    switch(state)
    {
        case Qt::CheckState::Checked:
            m_bUseStreamer = true; break;
        case Qt::CheckState::Unchecked:
            m_bUseStreamer = false; break;
    }

    ui->frame_recorder->setEnabled(m_bUseStreamer);
    ui->frame_hotkeyStart->setEnabled(m_bUseStreamer);
    ui->frame_hotkeyStop->setEnabled(m_bUseStreamer);
}

/*!
 * \brief SettingDialog::setVideoContainer1
 *        Sets the videoContainer1 by change of comboBox item;
 */
void SettingDialog::setVideoContainer1(const QString container)
{
    m_sVideoContainer1 = container;
}

/*!
 * \brief SettingDialog::setVideoContainer2
 *        Sets the videoContainer2 by change of comboBox item;
 */
void SettingDialog::setVideoContainer2(const QString container)
{
    m_sVideoContainer2 = container;
}

/*!
 * \brief SettingDialog::closeSettings
 *        Writes settings and returns accepted or rejected.
 * \param mode Mode of the return (accepted or rejected)
 */
void SettingDialog::closeSettings(ReturnState mode)
{
    switch(mode)
    {
        case ReturnState::ACCEPT:
            writeSettings();
            infoHandler(tr("Einstellungen wurden erfolgreich gespeichert."));
            emit accept();
            break;

        case ReturnState::REJECT:
            emit reject();
            break;
    }
}

