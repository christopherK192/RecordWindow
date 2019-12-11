#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <Windows.h>

#include "include/settingdialog.h"


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
    /*!
     * RecordWindow::set_videoName
     * Renames the video to the fightnumber.
     * Picks automatically the newest file and renames it according to the fight number input.
     * The number of the IVR is also logged.
     */
    void set_videoName();

private slots:
    /*!
     * RecordWindow::openSettings
     * Opens the Setting Dialog using the SettingDialog class.
     * Applies the settings after the setting window is closed.
     */
    void openSettings();
    /*!
     * RecordWindow::freezeWindow
     * Freezes and refreezes window in set places.
     *
     * \param i_bMode [in] True of False for freeze.
     */
    void freezeWindow(bool i_bMode);
    /*!
     * RecordWindow::setTopevel
     * Sets and resets window as toplevel window.
     *
     * \param i_bMode [in] True of False for set toplevel.
     */
    void setTopevel(bool i_bMode);
    /*!
     * RecordWindow::startstopRecord
     * Starts and Stops the recording.
     * Sends a simulated keyevent to the video streamer.
     * Changes the state of the record button and renames video file if possible.
     */
    void startstopRecord();
    /*!
     * RecordWindow::startVideo
     * Starts the IVR video.
     * Uses the same method as the openVideo function.
     * If no video streamer is used then the name has to be changed at this point.
     */
    void startstopVideo();
    /*!
     * RecordWindow::openVideo1
     * Plays the clicked video using the default system video player.
     *
     * \param i_index [in] Rowindex of the filepath model (correlates to a video file)
     */
    void openVideo1(const QModelIndex &i_index);
    /*!
     * RecordWindow::openVideo2
     * Plays the clicked video using the default system video player.
     *
     * \param i_index [in] Rowindex of the filepath model (correlates to a video file)
     */
    void openVideo2(const QModelIndex &i_index);
    /*!
     * RecordWindow::renameVideo
     * Renames the selected video.
     *
     * \param i_nCamera [in] Sel
     */
    void renameVideo(const int i_nCamera);
    /*!
     * RecordWindow::showhideFolder
     * Shows or hides the folder tabWidget for the video files in the
     * selected fiolders.
     */
    void showHideFolder();

private:
    QString WINDOW_TITLE = "TUT-IVR App";
    QString ICON_LOGO    = ":/icons/icons/logo.ico";

    Ui::RecordWindow*      ui;
    QFileSystemModel*      m_pFilemodel1;
    QFileSystemModel*      m_pFilemodel2;
    QSortFilterProxyModel* m_pProxymodel1;
    QSortFilterProxyModel* m_pProxymodel2;

    //IVR variables
    HWND        m_pPlayerWindow;
    QString     m_sVideoPlayer;
    QString     m_sVideoFile;
    QStringList m_slVideoContainer1;
    QStringList m_slVideoContainer2;
    QDir        m_oSavePath1;
    QDir        m_oSavePath2;
    int         m_nIvrCounter;
    int         m_nNumCams;
    bool        m_bShowFolder;

    // streamer variables
    HWND    m_pStreamerWindow;
    QString m_sVideoStreamer;
    char    m_cHotkeyStart;
    char    m_cHotkeyStop;
    int     m_nUseStreamer;
    bool    m_bRecording;
};

#endif // RECORDWINDOW_H

