#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
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

    // translation functions
    /*!
     * RecordWindow::loadLanguage
     * Loads language from .qm file, if language is not the current language.
     *
     * \param language Language to be loaded.
     */
    void loadLanguage(const QString& language);
    /*!
     * RecordWindow::switchTranslator
     * Changes the translator of the App.
     *
     * \param translator Translator which loads the language.
     * \param filename Name of the .qm file.
     */
    void switchTranslator(QTranslator& translator, const QString& filename);

protected:
    /*!
     * RecordWindow::changeEvent
     * Override function for the change Event in order to change the language.
     *
     * \param event Event type which calls the function.
     */
    void changeEvent(QEvent* event);

protected slots:
    /*!
     * RecordWindow::languageChange
     * Callback function for the language Actiongroup signal.
     * Initiates the language change.
     *
     * \param action Action object calling the function.
     */
    void languageChange(QAction* action);

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
     * \param set [in] True of False for freeze.
     */
    void freezeWindow(bool set);
    /*!
     * RecordWindow::setTopevel
     * Sets and resets window as toplevel window.
     *
     * \param set [in] True of False for set toplevel.
     */
    void setTopevel(bool set);
    /*!
     * RecordWindow::startstopRecord
     * Starts and Stops the recording.
     * Sends a simulated 'S'-keyevent (0x53) to the video streamer.
     * Changes the state of the record button and renames video file if possible.
     */
    void startstopRecord();
    /*!
     * RecordWindow::startVideo
     * Starts the IVR video.
     * Uses the same method as the openVideo function.
     * If no video streamer is used then the name has to be changed at this point.
     */
    void startVideo();
    /*!
     * RecordWindow::stopVideo
     * Stops the IVR video playback.
     * Sends a simulated 'F4'-keyevent to the video player.
     * This is only possible if the video player was previously set in thw settings.
     * Is not nescesary if player is manually stopped.
     */
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
    QString WINDOW_TITLE = "IVR-Record App";
    QString ICON_LOGO    = ":/icons/icons/logo.ico";

    Ui::RecordWindow*      ui;
    QFileSystemModel*      mp_filemodel1;
    QFileSystemModel*      mp_filemodel2;
    QSortFilterProxyModel* mp_proxymodel1;
    QSortFilterProxyModel* mp_proxymodel2;

    //IVR variables
    HWND        mp_PlayerWindow;
    QString     ms_videoPlayer;
    QString     ms_videoFile;
    QStringList ml_videoContainer1;
    QStringList ml_videoContainer2;
    QDir        mo_savePath1;
    QDir        mo_savePath2;
    int         mi_ivrCounter;
    int         mi_numCams;

    // streamer variables
    HWND    mp_StreamerWindow;
    QString ms_videoStreamer;
    QString ms_hotkeyStart;
    QString ms_hotkeyStop;
    bool    mb_useStreamer;
    bool    mb_recording;

    // translation variables
    QTranslator m_translatorApp;
//    QTranslator m_translatorQt;
    QString m_currentLanguage;
    QString m_dirLanguage;
};

#endif // RECORDWINDOW_H
