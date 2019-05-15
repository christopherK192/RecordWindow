#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QFileDialog>
#include <QLineEdit>
#include <QAbstractButton>
#include <Windows.h>

#define SAVE_FILE "settings.sav"


namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     * SettingDialog::SettingDialog
     * Constructor of the SettingDialog class
     * The constructor initializes the ui, connects the button signals and loads the settings.
     * Additionally it iterates over all open windows and gets the list of all open window names.
     *
     * \param parent Parent window of the SettingDialog class (inherits form QDialog).
     */
    explicit SettingDialog(QWidget *parent = nullptr);
    /*!
     * SettingDialog::~SettingDialog
     * Destructor
     */
    ~SettingDialog();

    enum Hotkeys{RECORDING_START = 0,
                 RECORDING_STOP
                };

    const QStringList VIDEO_CONTAINERS = {"Advanced Streaming Format (*.asf, *.wmv, *.wma)",
                                          "Audio Video Interleave (*.avi)",
                                          "BDAV MPEG-2 Transport Stream (*.m2ts, *.mts)",
                                          "Digital Video (*.dv)",
                                          "DivX Media Format (*.divx)",
                                          "Enhanced VOB (*.evo)",
                                          "Flash Video (*.flv)",
                                          "Material Exchange Format (*.mxf)",
                                          "Matroska (*.mkv, *.mka)",
                                          "MPEG-2-Program Stream (*.mpg, *.mpeg, *.ps)",
                                          "MPEG-2 Transport Stream (*.ts, *.tsp)",
                                          "MP4 (*.mp4)",
                                          "Ogg Media (*.ogg, *.ogv, *.ogm)",
                                          "OMFI (*.omf)",
                                          "QuickTime (*.mov, *.qt)",
                                          "RealMedia (*.rm, *.rmvb, *.ra, *.ram)",
                                          "Video Object (*.vob)",
                                          "WebM (*.webm)"
                                         };

    struct IVR_Settings
    {
        // IVR variables
        HWND*       videoPlayer;
        QStringList videoContainer1;
        QStringList videoContainer2;
        QString     videoPlayerName;
        QString     videoPath1;
        QString     videoPath2;
        int         numberCams;

        // Recording variables
        HWND*   videoStreamer;
        QString videoStreamerName;
        QString hotkeyStart;
        QString hotkeyStop;
        bool    useStreamer;
    };

private:
    /*!
     * SettingDialog::getOpenWindows
     * Callback function for each open window.
     * Extracts the window name of each window and stores them in a list.
     *
     * \param window [in]  Window Handle of the current Window.
     * \param param  [out] In this case it's the list of all window names.
     *
     * \return Returns true if successful.
     */
    static BOOL CALLBACK getOpenWindows(HWND window, LPARAM param);
    /*!
     * SettingDialog::extractFormat
     * Extracts the video format from the combobox item.
     *
     * \param videoContainer [in] combobox item.
     *
     * \return Returns list of video formats.
     */
    QStringList extractFormat(QString videoContainer);
    /*!
     * SettingDialog::recordHotkey
     * Transforms keyinputs into a hotkey sequenz.
     *
     * \param hotkey [in] line edit of the hotkey to be recorded.
     */
    bool recordHotkey(QLineEdit* hotkey);

private slots:
    /*!
     * SettingDialog::loadSettings
     * Reads the saved settings when constructed.
     *
     * Settings: Windowname of the last selected video streamer and video player as well as the savepaths, videocontainers and hotkeys.
     */
    void loadSettings();
    /*!
     * SettingDialog::writeSettings
     * Saves the current settings when closed.
     *
     * Settings: Windowname of the last selected video streamer and video player as well as the savepaths, videocontainers and hotkeys.
     */
    void saveSettings();
    /*!
     * SettingDialog::closeSettings
     * Writes settings or emits accepted or rejected.
     *
     * \param button [in] pointer of the pressed button.
     */
    void closeSettings(QAbstractButton* button);

    // IVR Functions
    /*!
     * SettingDialog::toggleCameras
     * Toggles number of cameras.
     *
     * \param camera [in] number of cameras to be used.
     */
    void toggleCameras(const int camera);
    /*!
     * SettingDialog::setVideoPath
     * Gets and sets the savepath over the File Dialog.
     * Called by [...]-button.
     *
     * \param camera [in] Sets the path for camera number camera.
     */
    void setVideoPath(const int camera);
    /*!
     * SettingDialog::setVideoContainer
     * Sets the videoContainer by change of comboBox item;
     *
     * \param camera [in] gets video container for camera number camera.
     */
    void setVideoContainer(const int camera);
    /*!
     * SettingDialog::connect2Player
     * Extract the window handle for the video player.
     */
    void connect2Player();

    // Recording Functions
    /*!
     * SettingDialog::toggleStreamer
     * Toggles state of the checkbox and enables or disables videostreamer options.
     *
     * \param state [in] State of the checkbox (Checked, Unchecked).
     */
    void enableStreamer(const int state);
    /*!
     * SettingDialog::connect2Streamer
     * Extract the window handle for the video streamer.
     */
    void connect2Streamer();
    /*!
     * SettingDialog::setHotkey
     * Records a hotkey.
     *
     * \param hotkey [in] Hotkey to be recorded.
     */
    void setHotkey(const int hotkey);

private:   
    QString WINDOW_TITLE = "IVR-Record App";
    QString ICON_LOGO    = ":/icons/icons/logo.ico";

    Ui::SettingDialog *ui;
    IVR_Settings m_SettingsIVR;

public:
    inline const IVR_Settings Settings(){return m_SettingsIVR;}
};

#endif // SETTINGDIALOG_H

