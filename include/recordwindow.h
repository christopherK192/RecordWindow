#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <Windows.h>

#include "include/settingdialog.h"

#include <QDebug>

namespace Ui {
class RecordWindow;
}

class RecordWindow : public QMainWindow
{
    Q_OBJECT

public:
    /*!
     * RecordWindow::RecordWindow
     * Constructor of the RecordWindow class
     * The constructor initializes the ui, connects the button signals and initializes the languages.
     *
     * \param parent Parent window of the RecordWindow class.
     */
    explicit RecordWindow(QWidget *parent = nullptr);
    /*!
     * RecordWindow::~RecordWindow
     * Destructor.
     */
    ~RecordWindow();


private:
    void set_videoName();


//    QTranslator m_translatorApp;
//    QTranslator m_translatorQt;
//    QString m_currentLanguage;
//    QString m_dirLanguage;

//    void loadLanguage(const QString& language);
//    void switchTranslator(QTranslator& translator, const QString& filename);

protected:
//    void changeEvent(QEvent* event);

protected slots:
//    void languageChange(QAction* action);

private slots:
    void openSettings();
    void freezeWindow(bool set);
    void setTopevel(bool set);
    void startstopRecord();
    // IVR functions
    void startVideo();
    void stopVideo();
    /*!
     * RecordWindow::openVideo1
     * Plays the clicked video using the default system video player.
     *
     * \param index [in] Rowindex of the filepath model (correlates to a video file)
     */
    void openVideo1(const QModelIndex &index);
    /*!
     * RecordWindow::openVideo2
     * Plays the clicked video using the default system video player.
     *
     * \param index [in] Rowindex of the filepath model (correlates to a video file)
     */
    void openVideo2(const QModelIndex &index);
    /*!
     * RecordWindow::renameVideo
     * Renames the selected video.
     *
     * \param cam [in] Sel
     */
    void renameVideo(int cam);

private:
    Ui::RecordWindow*      ui;
    QFileSystemModel*      mp_filemodel1;
    QFileSystemModel*      mp_filemodel2;
    QSortFilterProxyModel* mp_proxymodel1;
    QSortFilterProxyModel* mp_proxymodel2;

    //IVR variables
    HWND        mo_PlayerWindow;
    QString     ms_videoPlayer;
    QString     ms_videoFile;
    QStringList ml_videoContainer1;
    QStringList ml_videoContainer2;
    QDir        mo_savePath1;
    QDir        mo_savePath2;
    int         mi_ivrCounter;
    bool        mb_Kameras2;

    // streamer variables
    HWND    mo_StreamerWindow;
    QString ms_videoStreamer;
    QString ms_hotkeyStart;
    QString ms_hotkeyStop;
    bool    mb_useStreamer;
    bool    mb_recording;
};

#endif // RECORDWINDOW_H
