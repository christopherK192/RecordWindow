#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QCloseEvent>
//#include <QPushButton>
#include <Windows.h>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

    bool get_useStreamer();
    bool get_2Kameras();
    QString get_savePath();
    QString get_savePath2();
    QString get_videoStreamer();
    QString get_videoPlayer();
    QStringList get_videoContainer();

private:
    Ui::SettingDialog *ui;

    QString m_videoStreamer;
    QString m_videoContainer;
    QString m_savePath1;
    QString m_savePath2;
    QString m_videoPlayer;

    bool m_useStreamer;
    bool m_2Kameras;

    void errorHandler(QString text);
    void infoHandler(QString text);
    static BOOL CALLBACK getOpenWindows(HWND window, LPARAM param);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void readSettings();
    void writeSettings();
    void connect2Streamer(bool manuell);
    void connect2Player(bool manuell);
    void setPath(int path);
    void setUseStreamer();
    void unsetUseStreamer();
    void setKameras();
    void unsetKameras();
    void setVideoContainer(const QString container);
    void closeSettings();
};

#endif // SETTINGDIALOG_H
