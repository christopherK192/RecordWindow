#include "include/recordwindow.h"
#include "ui_recordwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>
//#include <QDateTime>
//#include <QTreeView>


/*!
 * \brief RecordWindow::RecordWindow
 *        Constructor of the RecordWindow class
 *
 * The constructor initializes the ui, connects the button signals and initializes the languages.
 *
 * \param parent Parent window of the RecordWindow class.
 */
RecordWindow::RecordWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RecordWindow),  
    mo_StreamerWindow(nullptr),
    mo_PlayerWindow(nullptr),
    ms_videoStreamer(""),
    ms_videoPlayer(""),
    ms_videoContainer1(""),
    ms_videoContainer2(""),
    ms_hotkeyStart(""),
    ms_hotkeyStop(""),
    mo_savePath1(""),
    mo_savePath2(""),
    mp_filemodel1(new QFileSystemModel),
    mp_filemodel2(new QFileSystemModel),
    mp_proxymodel1(new QSortFilterProxyModel),
    mp_proxymodel2(new QSortFilterProxyModel),
    mi_ivrCounter(1),
    mb_useStreamer(false),
    mb_Kameras2(false),
    mb_recording(false)
//    m_dirLanguage(QApplication::applicationDirPath().append("/language"))
{
    // initialize ui
    ui->setupUi(this);
    setWindowFlags(//Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint);

    ui->lineEdit_fightNr->clear();
    ui->lineEdit_fightNr->setText("100");

    ui->button_record->setText(tr("Start"));
    ui->label_record->setEnabled(mb_recording);
    ui->button_record->setEnabled(mb_useStreamer);

    ui->tabWidget->setTabEnabled(1, mb_Kameras2);

    // initialize displayed folder
    mp_filemodel1->setRootPath(QDir::currentPath());
    mp_filemodel1->setFilter(QDir::Files | QDir::NoSymLinks);
    mp_filemodel1->setNameFilterDisables(false);
    // using proxymodel in order to sort items
    mp_proxymodel1->setSourceModel(mp_filemodel1);
    ui->treeView_folder->setModel(mp_proxymodel1);
    ui->treeView_folder->setSortingEnabled(true);  
    // hide all columns except name and last changed
    ui->treeView_folder->setColumnHidden(1,true);
    ui->treeView_folder->setColumnHidden(2,true);

    // initialize scond displayed folder (hidden)
    mp_filemodel2->setRootPath(QDir::currentPath());
    mp_filemodel2->setFilter(QDir::Files | QDir::NoSymLinks);
    mp_filemodel2->setNameFilterDisables(false);
    // using proxymodel in order to sort items
    mp_proxymodel2->setSourceModel(mp_filemodel2);
    ui->treeView_folder2->setModel(mp_proxymodel2);
    ui->treeView_folder2->setSortingEnabled(true);
    // hide all columns except name and last changed
    ui->treeView_folder2->setColumnHidden(1,true);
    ui->treeView_folder2->setColumnHidden(2,true);

    // connect signals to buttons
    connect(ui->button_record, &QPushButton::clicked, this, &RecordWindow::startstopRecord);
    connect(ui->actionEinstellungen, &QAction::triggered, this, &RecordWindow::openSettings);
    connect(ui->button_playVideo, &QPushButton::clicked, this, &RecordWindow::startVideo);
    connect(ui->button_stopVideo, &QPushButton::clicked, this, &RecordWindow::stopVideo);
    connect(ui->treeView_folder, &QTreeView::doubleClicked, this, &RecordWindow::openVideo1);
    connect(ui->treeView_folder2, &QTreeView::doubleClicked, this, &RecordWindow::openVideo2);
    connect(ui->pushButton_rename1, &QPushButton::clicked, [this](){RecordWindow::renameVideo(1);});
    connect(ui->pushButton_rename2, &QPushButton::clicked, [this](){RecordWindow::renameVideo(2);});
    connect(ui->actionBeenden, &QAction::triggered, this, &RecordWindow::close);

//    // setup language creating an actiongroup
//    QActionGroup* langGroup = new QActionGroup(ui->menuSprache);
//    langGroup->setExclusive(true);
//    ui->actionDeutsch->setData("de");
//    langGroup->addAction(ui->actionDeutsch);
//    ui->actionEnglish->setData("en");
//    langGroup->addAction(ui->actionEnglish);

////    connect(langGroup, &QActionGroup::triggered, this, &RecordWindow::languageChange);

//    // initialize language using the default locale
//    QString defaultLocale = "en";
//    if(defaultLocale == "de")
//    {
//        ui->actionDeutsch->setChecked(true);
//        emit langGroup->triggered(ui->actionDeutsch);
//    }
//    else if(defaultLocale == "en")
//    {
//        ui->actionEnglish->setChecked(true);
//        emit langGroup->triggered(ui->actionEnglish);
//    }

//    ui->retranslateUi(this);
}

/*!
 * \brief RecordWindow::~RecordWindow
 *        Destructor.
 */
RecordWindow::~RecordWindow()
{
    delete ui;
}

/*!
 * \brief SettingDialog::errorHandler
 *        Opens a Message Box with an error message.
 *
 * \param text The text of the error message.
 */
void RecordWindow::errorHandler(QString text)
{
    QMessageBox message;
    message.setWindowTitle("IVR-Record App");
    message.setWindowIcon(QIcon(":/Icons/logo.ico"));
    message.setIcon(QMessageBox::Critical);
    message.setDefaultButton(QMessageBox::Ok);
    message.setText(text.toStdString().c_str());
    QFont font;
    font.setPointSize(9);
    message.setFont(font);
    message.exec();
}

/*!
 * \brief RecordWindow::infoHandler
 *        Opens Message Box with an info message.
 *
 * \param text The text of the info message.
 */
void RecordWindow::infoHandler(QString text)
{
    QMessageBox message;
    message.setWindowTitle("IVR-Record App");
    message.setWindowIcon(QIcon(":/Icons/logo.ico"));
    message.setIcon(QMessageBox::Information);
    message.setDefaultButton(QMessageBox::Ok);
    message.setText(text.toStdString().c_str());
    QFont font;
    font.setPointSize(9);
    message.setFont(font);
    message.exec();
}

/*!
 * \brief RecordWindow::inputHandler
 *        Opens an Input Dialog for text input.
 *
 * \param text A displayed info text.
 * \param input A preselected input value.
 * \param ok The window return value.
 *
 * \return Entered integer as int.
 */
QString RecordWindow::inputHandler(QString text, QString input, bool &ok)
{
    QInputDialog dialog;
    dialog.setWindowTitle("IVR-Record App");
    dialog.setWindowIcon(QIcon(":/Icons/logo.ico"));
    dialog.setInputMode(QInputDialog::TextInput);
    dialog.setLabelText(text.toStdString().c_str());
    dialog.setCancelButtonText(tr("Abbrechen"));
    dialog.setTextValue(input);
    QFont font;
    font.setPointSize(9);
    dialog.setFont(font);
    dialog.adjustSize();
    ok = dialog.exec();

    return dialog.textValue();
}

/*!
 * \brief RecordWindow::loadLanguage
 *        Loads language from .qm file, if language is not the current language.
 *
 * \param language Language to be loaded.
 */
//void RecordWindow::loadLanguage(const QString &language)
//{
//    if(m_currentLanguage != language)
//    {
//        m_currentLanguage = language;
//        QLocale locale = QLocale(m_currentLanguage);
//        QLocale::setDefault(locale);

//        switchTranslator(m_translatorApp, QString("translation_%1.qm").arg(language));
//        switchTranslator(m_translatorQt, QString("designer_%1.qm").arg(language));
//    }
//}

/*!
 * \brief RecordWindow::switchTranslator
 *        Changes the translator of the App.
 *
 * \param translator Translator which loads the language.
 * \param filename Name of the .qm file.
 */
//void RecordWindow::switchTranslator(QTranslator &translator, const QString &filename)
//{
//    qApp->removeTranslator(&translator);

//    if(translator.load(filename, m_dirLanguage))
//        qApp->installTranslator(&translator);
//}

/*!
 * \brief RecordWindow::set_videoName
 *        Renames the video to the fightnumber.
 *
 * Picks automatically the newest file and renames it according to the fight number input.
 * The number of the IVR is also logged.
 */
void RecordWindow::set_videoName()
{
    // get the newest video file
    mo_savePath1.setFilter(QDir::Files | QDir::NoSymLinks);
    mo_savePath1.setNameFilters(QStringList(ms_videoContainer1));
    mo_savePath1.setSorting(QDir::Time);

    QStringList fileList = mo_savePath1.entryList();
    if(fileList.isEmpty())
    {
        errorHandler(tr("FEHLER:\nKeine abspielbare Datei gefunden!\nBitte kontrollieren Sie den Pfad!"));
        ms_videoFile = "";
        return;
    }

    // rename the video file
    QString oldName = QString(fileList.first());
    int index = oldName.lastIndexOf('.');
    QStringRef videoFormat(&oldName, index, oldName.size()-index);

    bool ok;
    QString fightnr = inputHandler("Fight Nr: ", ui->lineEdit_fightNr->text(), ok);

    if(!ok)
        fightnr = ui->lineEdit_fightNr->text();

    ui->lineEdit_fightNr->setText(fightnr);

    // find the number of the files with the fight number
    QStringList nameFilter2;
    nameFilter2 << (fightnr + "*");
    mo_savePath1.setNameFilters(nameFilter2);

    // increment counter if a file with the name already exists
    mi_ivrCounter = mo_savePath1.entryList().size() + 1;

    ms_videoFile = (fightnr + "." + QString::number(mi_ivrCounter) + videoFormat.toString());
    mo_savePath1.rename(oldName, ms_videoFile);

    if(!mb_Kameras2)
    {
        mo_savePath2.setFilter(QDir::Files | QDir::NoSymLinks);
        mo_savePath2.setNameFilters(QStringList(ms_videoContainer1));
        mo_savePath2.setSorting(QDir::Time);

        QStringList fileList2 = mo_savePath2.entryList();
        if(fileList2.isEmpty())
        {
            errorHandler(tr("FEHLER:\nKeine abspielbare Datei gefunden!\nBitte kontrollieren Sie den Pfad!"));
            ms_videoFile = "";
            return;
        }

        // rename the video file
        QString oldName2 = QString(fileList.first());
        mo_savePath2.rename(oldName2, ms_videoFile);
    }
}

/*!
 * \brief RecordWindow::changeEvent
 *        Override function for the change Event in order to change the language.
 *
 * \param event Event type which calls the function.
 */
//void RecordWindow::changeEvent(QEvent *event)
//{
//    // not empty
//    if(event != nullptr)
//    {
//        switch(event->type())
//        {
//        // retranslate Ui
//        case QEvent::LanguageChange:
//            ui->retranslateUi(this);
//            break;

//        // load new language after locale change (simulated)
//        case QEvent::LocaleChange:
//            QString locale = QLocale::system().name();
//            locale.truncate(locale.lastIndexOf('_'));
//            loadLanguage(locale);
//            break;
//        }
//    }

//    // call the changeEvent of the MainWindow
//    QMainWindow::changeEvent(event);
//}

/*!
 * \brief RecordWindow::languageChange
 *        Callback function for the language Actiongroup signal.
 *        Initiates the language change.
 *
 * \param action Action object calling the function.
 */
//void RecordWindow::languageChange(QAction *action)
//{
//    if(action != nullptr)
//        loadLanguage(action->data().toString());
//}

/*!
 * \brief RecordWindow::openSettings
 *        Opens the Setting Dialog using the SettingDialog class.
 *
 * Applies the settings after the setting window is closed.
 */
void RecordWindow::openSettings()
{ 
    SettingDialog dialog;
    dialog.setWindowTitle(tr("Einstellungen"));
    dialog.setWindowIcon(QIcon(":/Icons/logo.ico"));
    dialog.resize(dialog.height(), dialog.height());
    bool ok = dialog.exec();

    if(!ok)
    {
        return;
    }

    mb_Kameras2    = dialog.Kameras2();
    mo_savePath1 = QDir(dialog.savePath1());
    ms_videoContainer1 = dialog.videoContainer1();
    if(mb_Kameras2)
    {
        mo_savePath2 = QDir(dialog.savePath2());
        ms_videoContainer2 = dialog.videoContainer2();
    }

    ms_videoPlayer = dialog.videoPlayer();
    mb_useStreamer = dialog.useStreamer();
    if(mb_useStreamer)
    {
        ms_videoStreamer = dialog.videoStreamer();
        ms_hotkeyStart = dialog.hotkeyStart();
        ms_hotkeyStop = dialog.hotkeyStop();
    }

    // get window handle from name
    mo_StreamerWindow = FindWindowA(nullptr, ms_videoStreamer.toStdString().c_str());
    mo_PlayerWindow = FindWindowA(nullptr, ms_videoPlayer.toStdString().c_str());

    // setting the displayed folder to the video folder
    mp_filemodel1->setRootPath(mo_savePath1.absolutePath());
    ui->treeView_folder->setRootIndex(mp_proxymodel1->mapFromSource(mp_filemodel1->index(mo_savePath1.absolutePath())));
    mp_filemodel1->setNameFilters(QStringList(ms_videoContainer1));
    ui->treeView_folder->setColumnWidth(0, 150);

    ui->tabWidget->setTabEnabled(1, mb_Kameras2);
    if(mb_Kameras2)
    {
        // setting the displayed folder to the video folder
        mp_filemodel2->setRootPath(mo_savePath2.absolutePath());
        ui->treeView_folder2->setRootIndex(mp_proxymodel2->mapFromSource(mp_filemodel2->index(mo_savePath2.absolutePath())));
        mp_filemodel2->setNameFilters(QStringList(ms_videoContainer1));
        ui->treeView_folder2->setColumnWidth(0, 150);
    }

    ui->frame_recording->setEnabled(mb_useStreamer);
}

/*!
 * \brief RecordWindow::startstopRecord
 *        Starts and Stops the recording.
 *
 * Sends a simulated 'S'-keyevent (0x53) to the video streamer.
 * Changes the state of the record button and renames video file if possible.
 */
void RecordWindow::startstopRecord()
{
    if(ms_videoStreamer == "" && ms_videoStreamer == "-")
    {
        if(mb_recording)
            errorHandler(tr("FEHLER:\nAufnahme konnte nicht gestoppt werden!\nBitte kontrollieren Sie die Einstellungen!"));
        else
            errorHandler(tr("FEHLER:\nAufnahme konnte nicht gestartet werden!\nBitte kontrollieren Sie die Einstellungen!"));

        return;
    }

    SetFocus(mo_StreamerWindow);
    SendMessageA(mo_StreamerWindow, WM_KEYDOWN, 0x53, 0);
    SendMessageA(mo_StreamerWindow, WM_KEYUP, 0x53, 0);

    if(mb_recording)
    {
        ui->button_record->setText(tr("Starten"));
        ui->label_record->setEnabled(false);
        ui->frame_replay->setEnabled(true);
        mb_recording = false;

        if(mb_useStreamer)
        {
            Sleep(uint(1000));
            set_videoName();
        }
    }
    else
    {
        ui->button_record->setText(tr("Stoppen"));
        ui->label_record->setEnabled(true);
        ui->frame_replay->setEnabled(false);
        mb_recording = true;
    }
}

/*!
 * \brief RecordWindow::startVideo
 *        Starts the IVR video.
 *
 * Uses the same method as the openVideo function.
 * If no video streamer is used then the name has to be changed at this point.
 */
void RecordWindow::startVideo()
{
    if(mo_savePath1 == QDir(""))
    {
        errorHandler(tr("FEHLER:\nWiedergabe konnte nicht gestartet werden!\nBitte kontrollieren Sie die Einstellungen und starten Sie die Wiedergabe manuell!"));
        return;
    }

    if(!ms_videoFile.isEmpty())
        QDesktopServices::openUrl(QUrl(mo_savePath1.absoluteFilePath(ms_videoFile), QUrl::TolerantMode));
}

/*!
 * \brief RecordWindow::stopVideo
 *        Stops the IVR video playback.
 *
 * Sends a simulated 'F4'-keyevent to the video player.
 * This is only possible if the video player was previously set in thw settings.
 * Is not nescesary if player is manually stopped.
 */
void RecordWindow::stopVideo()
{
    if(ms_videoPlayer == "" || ms_videoPlayer == "-")
    {
        errorHandler(tr("FEHLER:\nWiedergabe konnte nicht gestoppt werden!\nBitte kontrollieren Sie die Einstellungen und stoppen Sie die Wiedergabe manuell!"));
        return;
    }

    SetForegroundWindow(mo_PlayerWindow);
    keybd_event(VK_F4, 0, 0, 0);
    keybd_event(VK_F4, 0, WM_KEYUP, 0);
}

/*!
 * \brief RecordWindow::openVideo
 *        Plays the clicked video using the default system video player.
 *
 * \param index Rowindex of the filepath model (correlates to a video file)
 */
void RecordWindow::openVideo1(const QModelIndex &index)
{
    QModelIndex indexSource = mp_proxymodel1->mapToSource(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(mp_filemodel1->filePath(indexSource)));
}

void RecordWindow::openVideo2(const QModelIndex &index)
{
    QModelIndex indexSource = mp_proxymodel2->mapToSource(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(mp_filemodel2->filePath(indexSource)));
}

void RecordWindow::renameVideo(int cam)
{
    if(cam == 1)
    {
        QModelIndex indexSource = mp_proxymodel1->mapToSource(ui->treeView_folder->currentIndex());
        QString oldName = mp_filemodel1->filePath(indexSource);

        int index = oldName.lastIndexOf('.');
        QStringRef videoFormat(&oldName, index, oldName.size()-index);

        bool ok;
        QString fightnr = inputHandler("Fight Nr: ", ui->lineEdit_fightNr->text(), ok);

        if(!ok)
            fightnr = ui->lineEdit_fightNr->text();

        ui->lineEdit_fightNr->setText(fightnr);

        // find the number of the files with the fight number
        QStringList nameFilter2;
        nameFilter2 << (fightnr + "*");
        mo_savePath1.setNameFilters(nameFilter2);

        // increment counter if a file with the name already exists
        mi_ivrCounter = mo_savePath1.entryList().size() + 1;

        ms_videoFile = (fightnr + "." + QString::number(mi_ivrCounter) + videoFormat.toString());
        mo_savePath1.rename(oldName, ms_videoFile);
    }
    else if(cam == 2)
    {
        QModelIndex indexSource = mp_proxymodel2->mapToSource(ui->treeView_folder2->currentIndex());
        QString oldName = mp_filemodel2->filePath(indexSource);

        int index = oldName.lastIndexOf('.');
        QStringRef videoFormat(&oldName, index, oldName.size()-index);

        bool ok;
        QString fightnr = inputHandler("Fight Nr: ", ui->lineEdit_fightNr->text(), ok);

        if(!ok)
            fightnr = ui->lineEdit_fightNr->text();

        ui->lineEdit_fightNr->setText(fightnr);

        // find the number of the files with the fight number
        QStringList nameFilter2;
        nameFilter2 << (fightnr + "*");
        mo_savePath2.setNameFilters(nameFilter2);

        // increment counter if a file with the name already exists
        mi_ivrCounter = mo_savePath2.entryList().size() + 1;

        ms_videoFile = (fightnr + "." + QString::number(mi_ivrCounter) + videoFormat.toString());
        mo_savePath2.rename(oldName, ms_videoFile);
    }
}

