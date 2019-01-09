#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QTranslator>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
//#include <QDebug>
#include <Windows.h>

#include "include/settingdialog.h"


namespace Ui {
class RecordWindow;
}

class RecordWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RecordWindow(QWidget *parent = nullptr);
    ~RecordWindow();

private:
    Ui::RecordWindow *ui;   

    HWND mo_StreamerWindow;
    HWND mo_PlayerWindow;

    QString ms_videoStreamer;
    QString ms_videoPlayer;
    QString ms_videoFile;
    QString ms_videoContainer1;
    QString ms_videoContainer2;
    QString ms_hotkeyStart;
    QString ms_hotkeyStop;

    QDir mo_savePath1;
    QDir mo_savePath2;

    QFileSystemModel *mp_filemodel1;
    QFileSystemModel *mp_filemodel2;
    QSortFilterProxyModel *mp_proxymodel1;
    QSortFilterProxyModel *mp_proxymodel2;

    int mi_ivrCounter;

    bool mb_useStreamer;
    bool mb_Kameras2;
    bool mb_recording;

//    QTranslator m_translatorApp;
//    QTranslator m_translatorQt;
//    QString m_currentLanguage;
//    QString m_dirLanguage;

    void errorHandler(QString text);
    void infoHandler(QString text);
    QString inputHandler(QString text, QString input, bool &ok);
    void set_videoName();

//    void loadLanguage(const QString& language);
//    void switchTranslator(QTranslator& translator, const QString& filename);

protected:
//    void changeEvent(QEvent* event);

private slots:
    void openSettings();
    void startstopRecord();
    void startVideo();
    void stopVideo();
    void openVideo1(const QModelIndex &index);
    void openVideo2(const QModelIndex &index);
    void renameVideo(int cam);

protected slots:
//    void languageChange(QAction* action);
};
#endif // RECORDWINDOW_H
