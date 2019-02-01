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

    const QString videoStreamer(){return m_sVideoStreamer;}
    const QString videoPlayer(){return m_sVideoPlayer;}
    const QString savePath1(){return m_sSavePath1;}
    const QString savePath2(){return m_sSavePath2;}
    const QStringList videoContainer1();
    const QStringList videoContainer2();
    const QString hotkeyStart(){return m_sHotkeyStart;}
    const QString hotkeyStop(){return m_sHotkeyStop;}

    bool useStreamer(){return m_bUseStreamer;}
    bool Kameras2(){return m_bKameras2;}

private:
    Ui::SettingDialog *ui;
    enum class ReturnState{ACCEPT, REJECT};

    QStringList m_lVideoContainers = {"Advanced Streaming Format (*.asf, *.wmv, *.wma)",
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
                                      "WebM (*.webm)"};

    QString m_sVideoStreamer;
    QString m_sVideoPlayer;
    QString m_sSavePath1;
    QString m_sSavePath2;
    QString m_sVideoContainer1;
    QString m_sVideoContainer2;
    QString m_sHotkeyStart;
    QString m_sHotkeyStop;

    bool m_bUseStreamer;
    bool m_bKameras2;

    void errorHandler(QString text);
    void infoHandler(QString text);
    static BOOL CALLBACK getOpenWindows(HWND window, LPARAM param);
    QStringList extractFormat(QString videoContainer);

private slots:
    void readSettings();
    void writeSettings();
    void connect2Streamer(bool manuell);
    void connect2Player(bool manuell);
    void setPath(int cam);
    void toggleCams(bool cam2);
    void toggleStreamer(int state);
    void setVideoContainer1(const QString container);
    void setVideoContainer2(const QString container);
    void closeSettings(SettingDialog::ReturnState mode);
};

#endif // SETTINGDIALOG_H
