#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QFileDialog>
#include <QLineEdit>
#include <QAbstractButton>
#include <Windows.h>

#define SAVE_FILE "/settings.sav"


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
     * The constructor initializes the ui, connects the button signals and loads
     * the settings.
     * Additionally it iterates over all open windows and gets the list of all
     * open window names.
     *
     * \param parent Parent window of the SettingDialog class (inherits from
     *               QDialog).
     */
    explicit SettingDialog(QWidget *parent = nullptr);
    /*!
     * SettingDialog::~SettingDialog
     * Destructor
     */
    ~SettingDialog();

    enum Hotkeys
    {
        RECORDING_START = 0,
        RECORDING_STOP
    };

    const QStringList VIDEO_CONTAINERS = {
        "Advanced Streaming Format (*.asf, *.wmv, *.wma)",
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
        QStringList videoContainer1;
        QStringList videoContainer2;
        QString     videoPath1;
        QString     videoPath2;
        int         numberCams;

        // Recording variables
        HWND    videoStreamer;
        QString videoStreamerName;
        char    hotkeyStart;
        char    hotkeyStop;
        int     useStreamer;
    };

private:
    /*!
     * SettingDialog::getOpenWindows
     * Callback function for each open window.
     * Extracts the window name of each window and stores them in a list.
     *
     * \param i_window [in]  Window Handle of the current Window.
     * \param o_param  [out] In this case it's the list of all window names.
     *
     * \return Returns true if successful.
     */
    static BOOL CALLBACK getOpenWindows(HWND i_window, LPARAM o_param);
    /*!
     * SettingDialog::extractFormat
     * Extracts the video format from the combobox item.
     *
     * \param i_sVideoContainer [in] combobox item.
     *
     * \return Returns list of video formats.
     */
    QStringList extractFormat(QString i_sVideoContainer);

private slots:
    /*!
     * SettingDialog::loadSettings
     * Reads the saved settings when constructed.
     *
     * Settings: Windowname of the last selected video streamer and video player
     * as well as the savepaths, videocontainers and hotkeys.
     */
    void loadSettings();
    /*!
     * SettingDialog::writeSettings
     * Saves the current settings when closed.
     *
     * Settings: Windowname of the last selected video streamer and video player
     * as well as the savepaths, videocontainers and hotkeys.
     */
    void saveSettings();
    /*!
     * SettingDialog::closeSettings
     * Writes settings or emits accepted or rejected.
     *
     * \param i_pButton [in] pointer of the pressed button.
     */
    void closeSettings(QAbstractButton* i_pButton);

    // IVR Functions
    /*!
     * SettingDialog::toggleCameras
     * Toggles number of cameras.
     *
     * \param i_nCamera [in] number of cameras to be used.
     */
    void toggleCameras(const int i_nCamera);
    /*!
     * SettingDialog::setVideoPath
     * Gets and sets the savepath over the File Dialog.
     * Called by [...]-button.
     *
     * \param i_nCamera [in] Sets the path for camera number camera.
     */
    void setVideoPath(const int i_nCamera);
    /*!
     * SettingDialog::setVideoContainer
     * Sets the videoContainer by change of comboBox item;
     *
     * \param i_nCamera [in] gets video container for camera number camera.
     */
    void setVideoContainer(const int i_nCamera);

    // Recording Functions
    /*!
     * SettingDialog::toggleStreamer
     * Toggles state of the checkbox and enables or disables videostreamer
     * options.
     *
     * \param i_nState [in] State of the checkbox (Checked, Unchecked).
     */
    void enableStreamer(const int i_nState);
    /*!
     * SettingDialog::connect2Streamer
     * Extract the window handle for the video streamer.
     */
    void connect2Streamer();
    /*!
     * SettingDialog::setHotkey
     * Records a hotkey.
     *
     * \param i_nHotkey [in] Hotkey to be recorded.
     */
    void setHotkey(const int i_nHotkey);

private:   
    const QString WINDOW_TITLE = "TUT-IVR App";
    const QString ICON_LOGO    = ":/icons/icons/logo.ico";

    Ui::SettingDialog *ui;
    IVR_Settings      m_oSettingsIVR;

public:
    inline const IVR_Settings Settings()
    {
        return m_oSettingsIVR;
    }
};

#endif // SETTINGDIALOG_H

