#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QFileDialog>
#include <Windows.h>

#define ICON_CHECK_OK ":/icons/checkOK.svg"
#define ICON_CHECK_NO ":/checkNO.svg"
#define ICON_LOGO ":/icons/logo.ico"

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

    const QString videoStreamer(){return ms_videoStreamer;}
    const QString videoPlayer(){return ms_videoPlayer;}
    const QString savePath1(){return ms_savePath1;}
    const QString savePath2(){return ms_savePath2;}
    const QString videoContainer1(){return ms_videoContainer1;}
    const QString videoContainer2(){return ms_videoContainer2;}
    const QString hotkeyStart(){return ms_hotkeyStart;}
    const QString hotkeyStop(){return ms_hotkeyStop;}

    bool useStreamer(){return mb_useStreamer;}
    bool Kameras2(){return mb_Kameras2;}

private:
    Ui::SettingDialog *ui;

    enum class ReturnState{ACCEPT, REJECT};

    QStringList ml_videoContainers = {"Advanced Streaming Format (*.asf, *.wmv, *.wma)",
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

    QString ms_videoStreamer;
    QString ms_videoPlayer;
    QString ms_savePath1;
    QString ms_savePath2;
    QString ms_videoContainer1;
    QString ms_videoContainer2;
    QString ms_hotkeyStart;
    QString ms_hotkeyStop;

    bool mb_useStreamer;
    bool mb_Kameras2;

    void errorHandler(QString text);
    void infoHandler(QString text);
    static BOOL CALLBACK getOpenWindows(HWND window, LPARAM param);

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
