#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QFileDialog>
#include <Windows.h>

#define ICON_CHECK_OK ":/icons/icons/checkOK.svg"
#define ICON_CHECK_NO ":/icons/icons/checkNO.svg"
#define ICON_LOGO     ":/icons/icons/logo.ico"


namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

    enum ReturnState{ACCEPT = 0,
                     REJECT
                    };

    enum Hotkeys{RECORDING_START = 0,
                 RECORDING_STOP
                };

    enum SettingList{NUMBER_CAMS = 0,
                     VIDEO_DIR_1,
                     VIDEO_CONTAINER_1,
                     VIDEO_DIR_2,
                     VIDEO_CONTAINER_2,
                     VIDEO_PLAYER,
                     STREAMER_ENABLED,
                     VIDEO_STREAMER,
                     HOTKEY_START,
                     HOTKEY_STOP,
                     NUMBER_SETTINGS
                    };

    QStringList SettingText{"NUMBER_CAMS",
                            "VIDEO_DIR_1",
                            "VIDEO_CONTAINER_1",
                            "VIDEO_DIR_2",
                            "VIDEO_CONTAINER_2",
                            "VIDEO_PLAYER",
                            "STREAMER_ENABLED",
                            "VIDEO_STREAMER",
                            "HOTKEY_START",
                            "HOTKEY_STOP",
                            "NUMBER_SETTINGS"
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
    void recordHotkey();

private slots:
    void loadSettings();
    void saveSettings();
    void closeSettings(SettingDialog::ReturnState state);

    // IVR Functions
    void toggleCameras(const int camera);
    QString setVideoPath(const int camera);
    void setVideoContainer(const int camera);
    QString connect2Player();

    // Recording Functions
    void enableStreamer(const int state);
    QString connect2Streamer();
    void setHotkey(const int hotkey);

private:
    Ui::SettingDialog *ui;  

    // IVR variables
    HWND*       m_pVideoPlayer;
    QStringList m_slVideoContainer1;
    QStringList m_slVideoContainer2;
    QString     m_sVideoPath1;
    QString     m_sVideoPath2;
    int         m_nNumberCamers;

    // Recording variables
    HWND*   m_pVideoStreamer;
    QString m_sHotkeyStart;
    QString m_sHotkeyStop;
    bool    m_bUseStreamer;


};

#endif // SETTINGDIALOG_H

