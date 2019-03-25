#include "include/settingdialog.h"
#include "ui_settingdialog.h"

#include <QMessageBox>
#include <QTextStream>
//#include <QSettings>

class RecordWindow;


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

    // IVR options
    ui->radioButton_Kamera1->setChecked(true);

    ui->lineEdit_videoPath1->clear();
    ui->comboBox_videoContainer1->addItems(VIDEO_CONTAINERS);
    ui->comboBox_videoContainer1->setCurrentIndex(0);

    ui->lineEdit_videoPath2->clear();
    ui->comboBox_videoContainer2->addItems(VIDEO_CONTAINERS);
    ui->comboBox_videoContainer2->setCurrentIndex(0);
    ui->groupBox_Kamera2->setEnabled(false);

    ui->comboBox_videoPlayer->setCurrentIndex(0);

    // videoStreamer options
    ui->checkBox_videoStreamer->setChecked(false);
    ui->comboBox_videoStreamer->setCurrentIndex(0);
    ui->frame_recorder->setEnabled(false);

    ui->lineEdit_hotkeyStart->clear();
    ui->frame_hotkeyStart->setEnabled(false);

    ui->lineEdit_hotkeyStop->clear();
    ui->frame_hotkeyStop->setEnabled(false);

    // set video player as start view
    ui->tabWidget_settings->setCurrentIndex(0);

    // getting the window names of all open windows
    // SettingDialog::getOpenWindows is the callback function for all windows.
    QStringList windowList;
    EnumWindows(&SettingDialog::getOpenWindows, reinterpret_cast<LPARAM>(&windowList));
    ui->comboBox_videoStreamer->addItems(windowList);
    ui->comboBox_videoPlayer->addItems(windowList);

    // get saved settings
//    loadSettings();

//    // connect signals to buttons
//    connect(ui->button_videoStreamer, &QToolButton::clicked, this, SettingDialog::connect2Streamer);
//    connect(ui->button_videoPlayer,   &QToolButton::clicked, this, SettingDialog::connect2Player);

//    connect(ui->radioButton_Kamera1, &QRadioButton::clicked, [this](){SettingDialog::toggleCams(false);});
//    connect(ui->radioButton_Kamera2, &QRadioButton::clicked, [this](){SettingDialog::toggleCams(true);});

//    connect(ui->button_videoPath1, &QToolButton::clicked, [this](){SettingDialog::setPath(1);});
//    connect(ui->button_videoPath2, &QToolButton::clicked, [this](){SettingDialog::setPath(2);});

//    connect(ui->comboBox_videoContainer1, &QComboBox::currentTextChanged, this, &SettingDialog::setVideoContainer1);
//    connect(ui->comboBox_videoContainer2, &QComboBox::currentTextChanged, this, &SettingDialog::setVideoContainer2);

//    connect(ui->checkBox_videoStreamer, &QCheckBox::stateChanged, this, &SettingDialog::toggleStreamer);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, [this](){SettingDialog::closeSettings(ReturnState::ACCEPT);});
    connect(ui->buttonBox, &QDialogButtonBox::rejected, [this](){SettingDialog::closeSettings(ReturnState::REJECT);});

//    toggleCams(false);
//    toggleStreamer(Qt::CheckState::Unchecked);
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
 * \brief SettingDialog::getOpenWindows
 *        Callback function for each open window.
 *
 * Extracts the window name of each window and stores them in a list.
 *
 * \param window [in]  Window Handle of the current Window.
 * \param param  [out] In this case it's the list of all window names.
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
 * \param videoContainer [in] combobox item.
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
 * \brief SettingDialog::recordHotkey
 *        Extracts the video format from the combobox item.
 */
void SettingDialog::recordHotkey()
{
}

/*!
 * \brief SettingDialog::loadSettings
 *        Reads the saved settings when constructed.
 *
 * Settings: Windowname of the last selected video streamer and video player as well as the savepaths, videocontainers and hotkeys.
 */
void SettingDialog::loadSettings()
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
void SettingDialog::saveSettings()
{
    QFile saveFile(QDir::currentPath() + "/savefiles/settings.sav");

    if(saveFile.open(QIODevice::WriteOnly))
    {
        QTextStream saveStream(&saveFile);

        saveFile.close();
       tr("Einstellungen wurden erfolgreich gespeichert.");
    }
    tr("FEHLER:\nEinstellungen konnten nicht gespeichert werden.");
}

/*!
 * \brief SettingDialog::closeSettings
 *        Writes settings and returns accepted or rejected.
 * \param mode Mode of the return (accepted or rejected)
 */
void SettingDialog::closeSettings(ReturnState state)
{
    switch(state)
    {
        case ReturnState::ACCEPT:
            saveSettings();

            emit accept();
            break;

        case ReturnState::REJECT:
            emit reject();
            break;
    }
}

/**************************************************
 * IVR Functions
 **************************************************
 */

/*!
 * \brief SettingDialog::toggleCameras
 *        Toggles number of cameras.
 *
 * \param camera [in] number of cameras to be used.
 */
void SettingDialog::toggleCameras(const int camera)
{
    m_nNumberCamers = camera;
    ui->groupBox_Kamera2->setEnabled(camera == 2);
}

/*!
 * \brief SettingDialog::setVideoPath
 *        Gets and sets the savepath over the File Dialog.
 *        Called by [...]-button.
 *
 * \param camera [in] Sets the path for camera number camera.
 */
QString SettingDialog::setVideoPath(const int camera)
{
    QString* path = nullptr;
    QLineEdit* pathLineEdit = nullptr;

    switch(camera)
    {
        case 1:
            path = &m_sVideoPath1;
            pathLineEdit= ui->lineEdit_videoPath1;
            break;
        case 2:
            path = &m_sVideoPath2;
            pathLineEdit = ui->lineEdit_videoPath2;
            break;
    }

    QString savepath = QFileDialog::getExistingDirectory(this, tr("Ordner Öffnen"), *path,
                                                         QFileDialog::DontResolveSymlinks |
                                                         QFileDialog::ShowDirsOnly);

    if(savepath.isEmpty())
    {
        return tr("FEHLER:\nEs wurde kein Ordner ausgewählt!\nBitte wählen Sie einen Ordner aus!");
    }

    pathLineEdit->setText(savepath);
    *path = pathLineEdit->text();

    return "";
}

/*!
 * \brief SettingDialog::setVideoContainer
 *        Sets the videoContainer by change of comboBox item;
 *
 * \param camera [in] gets video container for camera number camera.
 */
void SettingDialog::setVideoContainer(const int camera)
{
    switch(camera)
    {
        case 1:
            m_slVideoContainer1 = extractFormat(ui->comboBox_videoContainer1->currentText());
            break;
        case 2:
            m_slVideoContainer2 = extractFormat(ui->comboBox_videoContainer2->currentText());
            break;
    }
}

/*!
 * \brief SettingDialog::connect2Player
 *        Extract the window handle for the video player.
 */
QString SettingDialog::connect2Player()
{
    m_pVideoPlayer = nullptr;
    QString player = ui->comboBox_videoPlayer->currentText();

    if (player == "-" || player.isEmpty())
    {
        return tr("FEHLER:\nEs wurde kein Programm ausgewählt!\nBitte wählen Sie ein Programm aus!");
    }

    //HWND windowHandler = FindWindowA(player.toStdString().c_str(), nullptr);
    HWND windowHandler = FindWindowA(nullptr, player.toStdString().c_str());

    if(windowHandler == nullptr)
    {
        return tr("FEHLER:\nEs konnte keine Instanz des angegebenen Programms gefunden werden!\nBitte überprüfen Sie, ob das Programm geöffnet ist!");
    }

    m_pVideoPlayer = &windowHandler;
    return tr("Verbindung mit dem Programm wurde erfolgreich hergestellt.");
}


/**************************************************
 * Recording Functions
 **************************************************
 */

/*!
 * \brief SettingDialog::toggleStreamer
 *        Toggles state of the checkbox and enables or disables videostreamer options.
 *
 * \param state [in] State of the checkbox (Checked, Unchecked).
 */
void SettingDialog::enableStreamer(const int state)
{
    switch(state)
    {
        case Qt::CheckState::Checked:
            m_bUseStreamer = true;
            break;
        case Qt::CheckState::Unchecked:
            m_bUseStreamer = false;
            break;
    }

    ui->frame_recorder->setEnabled(m_bUseStreamer);
    ui->frame_hotkeyStart->setEnabled(m_bUseStreamer);
    ui->frame_hotkeyStop->setEnabled(m_bUseStreamer);
}

/*!
 * \brief SettingDialog::connect2Streamer
 *        Extract the window handle for the video streamer.
 */
QString SettingDialog::connect2Streamer()
{
    m_pVideoStreamer = nullptr;
    QString streamer = ui->comboBox_videoStreamer->currentText();

    if (streamer == "-" || streamer.isEmpty())
    {
        dynamic_cast<RecordWindow*>(parentWidget());
        return tr("FEHLER:\nEs wurde kein Programm ausgewählt!\nBitte wählen Sie ein Programm aus!");
    }

    //HWND windowHandler = FindWindowA(streamer.toStdString().c_str(), nullptr);
    HWND windowHandler = FindWindowA(nullptr, streamer.toStdString().c_str());

    if(windowHandler == nullptr)
    {
        return tr("FEHLER:\nEs konnte keine Instanz des angegebenen Programms gefunden werden!\nBitte überprüfen Sie, ob das Programm geöffnet ist!");
    }

    m_pVideoStreamer = &windowHandler;
    return tr("Verbindung mit dem Programm wurde erfolgreich hergestellt.");
}

/*!
 * \brief SettingDialog::setHotkey
 *        Records a hotkey.
 *
 * \param hotkey [in] Hotkey to be recorded.
 */
void SettingDialog::setHotkey(const int hotkey)
{
    QString* keyCombi = nullptr;
    QLineEdit* keyLineEdit = nullptr;

    switch(hotkey)
    {
        case Hotkeys::RECORDING_START:
            keyCombi = &m_sHotkeyStart;
            keyLineEdit= ui->lineEdit_hotkeyStart;
            break;
        case Hotkeys::RECORDING_STOP:
            keyCombi = &m_sHotkeyStop;
            keyLineEdit = ui->lineEdit_hotkeyStop;
            break;
    }

//    QString savepath = QFileDialog::getExistingDirectory(this, tr("Ordner Öffnen"), *path,
//                                                         QFileDialog::DontResolveSymlinks |
//                                                         QFileDialog::ShowDirsOnly);

//    if(savepath.isEmpty())
//    {
//        return tr("FEHLER:\nEs wurde kein Ordner ausgewählt!\nBitte wählen Sie einen Ordner aus!");
//    }

    *keyCombi = keyLineEdit->text();
}

