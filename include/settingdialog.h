#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QFileDialog>
#include <QLineEdit>
#include <Windows.h>

#define WINDOW_TITLE "IVR-Record App"
#define ICON_LOGO    ":/icons/icons/logo.ico"
#define SAVE_FILE    "settings.sav"


namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
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

private:
    static BOOL CALLBACK getOpenWindows(HWND window, LPARAM param);
    QStringList extractFormat(QString videoContainer);
    bool recordHotkey(QLineEdit* hotkey);

private slots:
    void loadSettings();
    void saveSettings();
    void closeSettings(QPushButton *button);

    // IVR Functions
    void toggleCameras(const int camera);
    void setVideoPath(const int camera);
    void setVideoContainer(const int camera);
    void connect2Player();

    // Recording Functions
    void enableStreamer(const int state);
    void connect2Streamer();
    void setHotkey(const int hotkey);

private:   
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

    Ui::SettingDialog *ui;
    IVR_Settings m_SettingsIVR;

public:
    const IVR_Settings Settings(){return m_SettingsIVR;}
};

#endif // SETTINGDIALOG_H

