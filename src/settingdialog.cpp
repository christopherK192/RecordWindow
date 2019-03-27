#include "include/settingdialog.h"
#include "include/messageprinter.h"
#include "ui_settingdialog.h"

#include <QTextStream>

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
    loadSettings();

    // connect signals to buttons
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingDialog::closeSettings);

    connect(ui->radioButton_Kamera1, &QRadioButton::clicked, [this](){SettingDialog::toggleCameras(1);});
    connect(ui->radioButton_Kamera2, &QRadioButton::clicked, [this](){SettingDialog::toggleCameras(2);});
    connect(ui->button_videoPath1, &QToolButton::clicked, [this](){SettingDialog::setVideoPath(1);});
    connect(ui->button_videoPath2, &QToolButton::clicked, [this](){SettingDialog::setVideoPath(2);});
    connect(ui->comboBox_videoContainer1, &QComboBox::currentTextChanged, [this](){SettingDialog::setVideoContainer(1);});
    connect(ui->comboBox_videoContainer2, &QComboBox::currentTextChanged, [this](){SettingDialog::setVideoContainer(2);});
    connect(ui->button_videoPlayer, &QToolButton::clicked, this, &SettingDialog::connect2Player);

    connect(ui->checkBox_videoStreamer, &QCheckBox::stateChanged, this, &SettingDialog::enableStreamer);
    connect(ui->button_videoStreamer, &QToolButton::clicked, this, &SettingDialog::connect2Streamer);
    connect(ui->button_hotkeyStart, &QRadioButton::clicked, [this](){SettingDialog::setHotkey(Hotkeys::RECORDING_START);});
    connect(ui->button_hotkeyStop, &QRadioButton::clicked, [this](){SettingDialog::setHotkey(Hotkeys::RECORDING_STOP);});
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

//    GetClassNameA(window, windowTitle, titleSize);
    GetWindowTextA(window, windowTitle, titleSize);
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
 *        Transforms keyinputs into a hotkey sequenz.
 *
 * \param hotkey [in] line edit of the hotkey to be recorded.
 */
bool SettingDialog::recordHotkey(QLineEdit* hotkey)
{
    return true;
}

/*!
 * \brief SettingDialog::loadSettings
 *        Reads the saved settings when constructed.
 *
 * Settings: Windowname of the last selected video streamer and video player as well as the savepaths, videocontainers and hotkeys.
 */
void SettingDialog::loadSettings()
{
    QFile saveFile(QDir::currentPath() + SAVE_FILE);

    if(saveFile.open(QIODevice::ReadOnly))
    {
        QTextStream saveStream(&saveFile);
        QString line;

        saveStream.readLineInto(&line);
        toggleCameras(line.split("::")[-1].toInt());

        saveStream.readLineInto(&line);
        ui->lineEdit_videoPath1->setText(line.split("::")[-1]);

        saveStream.readLineInto(&line);
        ui->comboBox_videoContainer1->setCurrentIndex(line.split("::")[-1].toInt());

        if(ui->groupBox_Kamera2->isEnabled())
        {
            saveStream.readLineInto(&line);
            ui->lineEdit_videoPath2->setText(line.split("::")[-1]);

            saveStream.readLineInto(&line);
            ui->comboBox_videoContainer2->setCurrentIndex(line.split("::")[-1].toInt());
        }

        saveStream.readLineInto(&line);
        enableStreamer(line.split("::")[-1].toInt());

        saveStream.readLineInto(&line);
        ui->lineEdit_hotkeyStart->setText(line.split("::")[-1]);

        saveStream.readLineInto(&line);
        ui->lineEdit_hotkeyStop->setText(line.split("::")[-1]);

        saveFile.close();
        MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO, tr("Einstellungen wurden erfolgreich geladen."));
    }
    MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO, tr("Einstellungen konnten nicht geladen werden."));
}

/*!
 * \brief SettingDialog::writeSettings
 *        Saves the current settings when closed.
 *
 * Settings: Windowname of the last selected video streamer and video player as well as the savepaths, videocontainers and hotkeys.
 */
void SettingDialog::saveSettings()
{
    QFile saveFile(QDir::currentPath() + SAVE_FILE);

    if(saveFile.open(QIODevice::WriteOnly))
    {
        QTextStream saveStream(&saveFile);

        saveStream << "NUMBER_CAMS::"       << m_SettingsIVR.numberCams                     << endl
                   << "VIDEO_DIR_1::"       << m_SettingsIVR.videoPath1                     << endl
                   << "VIDEO_CONTAINER_1::" << ui->comboBox_videoContainer1->currentIndex() << endl
                   << "VIDEO_DIR_2::"       << m_SettingsIVR.videoPath2                     << endl
                   << "VIDEO_CONTAINER_2::" << ui->comboBox_videoContainer2->currentIndex() << endl
                   << "STREAMER_ENABLED::"  << m_SettingsIVR.useStreamer                    << endl
                   << "HOTKEY_START::"      << m_SettingsIVR.hotkeyStart                    << endl
                   << "HOTKEY_STOP::"       << m_SettingsIVR.hotkeyStop                     << endl;

        saveFile.close();
        MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO, tr("Einstellungen wurden erfolgreich gespeichert."));
    }
    MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO, tr("Einstellungen konnten nicht gespeichert werden."));
}

/*!
 * \brief SettingDialog::closeSettings
 *        Writes settings or emits accepted or rejected.
 *
 * \param button [in] pointer of the pressed button.
 */
void SettingDialog::closeSettings(QPushButton *button)
{
    if(ui->buttonBox->button(QDialogButtonBox::Ok) == button)
    {
        emit accept();
    }
    else if(ui->buttonBox->button(QDialogButtonBox::Cancel) == button)
    {
        emit reject();
    }
    else if(ui->buttonBox->button(QDialogButtonBox::Save) == button)
    {
        saveSettings();
    }
    else
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO, tr("Unerwarteter Knopf gedrückt!"));
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
    m_SettingsIVR.numberCams = camera;
    ui->groupBox_Kamera2->setEnabled(camera == 2);
}

/*!
 * \brief SettingDialog::setVideoPath
 *        Gets and sets the savepath over the File Dialog.
 *        Called by [...]-button.
 *
 * \param camera [in] Sets the path for camera number camera.
 */
void SettingDialog::setVideoPath(const int camera)
{
    QString* path = nullptr;
    QLineEdit* pathLineEdit = nullptr;

    switch(camera)
    {
        case 1:
            path = &m_SettingsIVR.videoPath1;
            pathLineEdit= ui->lineEdit_videoPath1;
            break;
        case 2:
            path = &m_SettingsIVR.videoPath2;
            pathLineEdit = ui->lineEdit_videoPath2;
            break;        
    }

    if(path == nullptr)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO, tr("Unerwartete Pfadfehler!"));
        return;
    }

    QString savepath = QFileDialog::getExistingDirectory(this, tr("Ordner Öffnen"), *path,
                                                         QFileDialog::DontResolveSymlinks |
                                                         QFileDialog::ShowDirsOnly);

    if(savepath.isEmpty())
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Es wurde kein Ordner ausgewählt!\nBitte wählen Sie einen Ordner aus!"));
        return;
    }

    pathLineEdit->setText(savepath);
    *path = pathLineEdit->text();
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
            m_SettingsIVR.videoContainer1 = extractFormat(ui->comboBox_videoContainer1->currentText());
            break;
        case 2:
            m_SettingsIVR.videoContainer2 = extractFormat(ui->comboBox_videoContainer2->currentText());
            break;
    }
}

/*!
 * \brief SettingDialog::connect2Player
 *        Extract the window handle for the video player.
 */
void SettingDialog::connect2Player()
{
    m_SettingsIVR.videoPlayer = nullptr;
    QString player = ui->comboBox_videoPlayer->currentText();

    if (player == "-" || player.isEmpty())
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Es wurde kein Programm ausgewählt!\nBitte wählen Sie ein Programm aus!"));
        return;
    }

    //HWND windowHandler = FindWindowA(player.toStdString().c_str(), nullptr);
    HWND windowHandler = FindWindowA(nullptr, player.toStdString().c_str());

    if(windowHandler == nullptr)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Es konnte keine Instanz des angegebenen Programms gefunden werden!\n\
                                    Bitte überprüfen Sie, ob das Programm geöffnet ist!"));
        return;
    }

    m_SettingsIVR.videoPlayer = &windowHandler;
    m_SettingsIVR.videoPlayerName = player;

    MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO, tr("Verbindung mit dem Programm wurde erfolgreich hergestellt."));
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
    bool* checked = &m_SettingsIVR.useStreamer;

    switch(state)
    {
        case Qt::CheckState::Checked:
            *checked = true;
            break;
        case Qt::CheckState::Unchecked:
            *checked = false;
            break;
    }

    ui->frame_recorder->setEnabled(*checked);
    ui->frame_hotkeyStart->setEnabled(*checked);
    ui->frame_hotkeyStop->setEnabled(*checked);
}

/*!
 * \brief SettingDialog::connect2Streamer
 *        Extract the window handle for the video streamer.
 */
void SettingDialog::connect2Streamer()
{
    m_SettingsIVR.videoStreamer = nullptr;
    QString streamer = ui->comboBox_videoStreamer->currentText();

    if (streamer == "-" || streamer.isEmpty())
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Es wurde kein Programm ausgewählt!\nBitte wählen Sie ein Programm aus!"));
        return;
    }

    //HWND windowHandler = FindWindowA(streamer.toStdString().c_str(), nullptr);
    HWND windowHandler = FindWindowA(nullptr, streamer.toStdString().c_str());

    if(windowHandler == nullptr)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Es konnte keine Instanz des angegebenen Programms gefunden werden!\n\
                                    Bitte überprüfen Sie, ob das Programm geöffnet ist!"));
        return;
    }

    m_SettingsIVR.videoStreamer = &windowHandler;
    m_SettingsIVR.videoStreamerName = streamer;

    MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO, tr("Verbindung mit dem Programm wurde erfolgreich hergestellt."));
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
            keyCombi = &m_SettingsIVR.hotkeyStart;
            keyLineEdit= ui->lineEdit_hotkeyStart;
            break;
        case Hotkeys::RECORDING_STOP:
            keyCombi = &m_SettingsIVR.hotkeyStop;
            keyLineEdit = ui->lineEdit_hotkeyStop;
            break;
    }

    if(!recordHotkey(keyLineEdit))
    {
        MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO, tr("Shotcut konnte nicht gesetzt werden! Bitte nochmal versuchen!"));
    }

    *keyCombi = keyLineEdit->text();
    MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO, tr("Shotcut wurde erfolgreich gesetzt."));
}

