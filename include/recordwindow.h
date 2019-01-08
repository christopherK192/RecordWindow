#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>
#include <QTranslator>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QTreeView>
#include <Windows.h>
//#include <QDebug>

#include "include/settingdialog.h"


namespace Ui {
class RecordWindow;
}

class RecordWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit RecordWindow(QWidget *parent = 0);
    ~RecordWindow();

private:
    Ui::RecordWindow *ui;   

    HWND m_StreamerWindow; 
    HWND m_PlayerWindow;
    QDir m_savePath1;
    QDir m_savePath2;
    QString m_videoStreamer;
    QString m_videoPlayer;
    QString m_videoFile;
    QStringList m_videoContainer;

    QFileSystemModel *m_filemodel;
    QSortFilterProxyModel *m_proxymodel;
    QFileSystemModel *m_filemodel2;
    QSortFilterProxyModel *m_proxymodel2;

    bool m_useStreamer;
    bool m_2Kameras;
    bool m_recording;
    int m_ivrCounter;

    QTranslator m_translatorApp;
    QTranslator m_translatorQt;
    QString m_currentLanguage;
    QString m_dirLanguage;

    void errorHandler(QString text);
    void infoHandler(QString text);
    QString inputHandler(QString text, QString input, bool &ok);
    void set_videoName();

    void loadLanguage(const QString& language);
    void switchTranslator(QTranslator& translator, const QString& filename);

protected:
    void changeEvent(QEvent* event);

private slots:
    void openSettings();
    void startstopRecord();
    void startVideo();
    void stopVideo();
    void openVideo1(const QModelIndex &index);
    void openVideo2(const QModelIndex &index);
    void renameVideo(int cam);

protected slots:
    void languageChange(QAction* action);
};
#endif // RECORDWINDOW_H
