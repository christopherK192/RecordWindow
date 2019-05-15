#include "include/recordwindow.h"
#include "include/messageprinter.h"
#include "ui_recordwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>
//#include <QDateTime>
//#include <QTreeView>

RecordWindow::RecordWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RecordWindow),  
    mp_filemodel1(new QFileSystemModel),
    mp_filemodel2(new QFileSystemModel),
    mp_proxymodel1(new QSortFilterProxyModel),
    mp_proxymodel2(new QSortFilterProxyModel),
    mp_PlayerWindow(nullptr),
//    ms_videoPlayer(""),
    ml_videoContainer1(""),
    ml_videoContainer2(""),
    mo_savePath1(""),
    mo_savePath2(""),
    mi_ivrCounter(1),
    mi_numCams(false),
    mp_StreamerWindow(nullptr),
//    ms_videoStreamer(""),
    ms_hotkeyStart(""),
    ms_hotkeyStop(""),
    mb_useStreamer(false),
    mb_recording(false),
    m_dirLanguage(QApplication::applicationDirPath().append("/language"))
{
    // initialize ui
    ui->setupUi(this);

    ui->lineEdit_fightNr->clear();
    ui->lineEdit_fightNr->setText("100");

    ui->button_record->setText(tr("Start"));
    ui->frame_recording->setEnabled(mb_recording);

    ui->tabWidget->setTabEnabled(1, mi_numCams == 2);

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
    connect(ui->button_playVideo, &QPushButton::clicked, this, &RecordWindow::startVideo);
    connect(ui->button_stopVideo, &QPushButton::clicked, this, &RecordWindow::stopVideo);

    connect(ui->pushButton_rename1, &QPushButton::clicked, [this](){RecordWindow::renameVideo(1);});
    connect(ui->pushButton_rename2, &QPushButton::clicked, [this](){RecordWindow::renameVideo(2);});
    connect(ui->treeView_folder, &QTreeView::doubleClicked, this, &RecordWindow::openVideo1);
    connect(ui->treeView_folder2, &QTreeView::doubleClicked, this, &RecordWindow::openVideo2);

    connect(ui->actionEinstellungen, &QAction::triggered, this, &RecordWindow::openSettings);
    connect(ui->actionFreeze, &QAction::toggled, this, &RecordWindow::freezeWindow);
    connect(ui->actionSet_Toplevel, &QAction::toggled, this, &RecordWindow::setTopevel);
    connect(ui->actionBeenden, &QAction::triggered, this, &RecordWindow::close);

    // setup language creating an actiongroup
    QActionGroup* langGroup = new QActionGroup(ui->menuSprache);
    langGroup->setExclusive(true);

    ui->actionDeutsch->setData("de");
    langGroup->addAction(ui->actionDeutsch);
    ui->actionEnglish->setData("en");
    langGroup->addAction(ui->actionEnglish);

    connect(langGroup, &QActionGroup::triggered, this, &RecordWindow::languageChange);

    // initialize language using the default locale
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

    ui->retranslateUi(this);
}

RecordWindow::~RecordWindow()
{
    delete ui;
}

void RecordWindow::set_videoName()
{
    // get the newest video file
    mo_savePath1.setFilter(QDir::Files | QDir::NoSymLinks);
    mo_savePath1.setNameFilters(QStringList(ml_videoContainer1));
    mo_savePath1.setSorting(QDir::Time);

    QStringList fileList = mo_savePath1.entryList();
    if(fileList.isEmpty())
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Keine abspielbare Datei gefunden!\nBitte kontrollieren Sie den Pfad!"));
        ms_videoFile = "";
        return;
    }

    // rename the video file
    QString oldName = QString(fileList.first());
    int index = oldName.lastIndexOf('.');
    QStringRef videoFormat(&oldName, index, oldName.size() - index);

    bool ok;
    QString fightnr = MessagePrinter::InputHandler("Fight Nr: ", ui->lineEdit_fightNr->text(), ok);

    if(!ok)
    {
        fightnr = ui->lineEdit_fightNr->text();
    }
    ui->lineEdit_fightNr->setText(fightnr);

    // find the number of the files with the fight number
    QStringList nameFilter2;
    nameFilter2 << (fightnr + "*");
    mo_savePath1.setNameFilters(nameFilter2);

    // increment counter if a file with the name already exists
    mi_ivrCounter = mo_savePath1.entryList().size() + 1;

    ms_videoFile = (fightnr + "." + QString::number(mi_ivrCounter) + videoFormat.toString());
    mo_savePath1.rename(oldName, ms_videoFile);

    if(!(mi_numCams == 2))
    {
        mo_savePath2.setFilter(QDir::Files | QDir::NoSymLinks);
        mo_savePath2.setNameFilters(QStringList(ml_videoContainer2));
        mo_savePath2.setSorting(QDir::Time);

        QStringList fileList2 = mo_savePath2.entryList();
        if(fileList2.isEmpty())
        {
            MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                     tr("Keine abspielbare Datei gefunden!\nBitte kontrollieren Sie den Pfad!"));
            ms_videoFile = "";
            return;
        }

        // rename the video file
        QString oldName2 = QString(fileList.first());
        mo_savePath2.rename(oldName2, ms_videoFile);
    }
}

void RecordWindow::loadLanguage(const QString &language)
{
    if(m_currentLanguage != language)
    {
        m_currentLanguage = language;
        QLocale locale = QLocale(m_currentLanguage);
        QLocale::setDefault(locale);

        switchTranslator(m_translatorApp, QString("translation_%1.qm").arg(language));
//        switchTranslator(m_translatorQt, QString("designer_%1.qm").arg(language));
    }
}

void RecordWindow::switchTranslator(QTranslator &translator, const QString &filename)
{
    qApp->removeTranslator(&translator);

    if(translator.load(filename, m_dirLanguage))
        qApp->installTranslator(&translator);
}

void RecordWindow::changeEvent(QEvent *event)
{
    // not empty
    if(event != nullptr)
    {
        switch(event->type())
        {
        // retranslate Ui
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;

        // load new language after locale change (simulated)
        case QEvent::LocaleChange:
            QString locale = QLocale::system().name();
            locale.truncate(locale.lastIndexOf('_'));
            loadLanguage(locale);
            break;
        }
    }

    // call the changeEvent of the MainWindow
    QMainWindow::changeEvent(event);
}

void RecordWindow::languageChange(QAction *action)
{
    if(action != nullptr)
        loadLanguage(action->data().toString());
}

void RecordWindow::openSettings()
{ 
    SettingDialog dialog;
    dialog.setWindowTitle(tr("Einstellungen"));
    dialog.setWindowIcon(QIcon(ICON_LOGO));
    dialog.resize(dialog.height(), dialog.height());
    bool ok = dialog.exec();

    if(!ok)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Einstellungen konnten nicht geladen werden!"));
        return;
    }

    SettingDialog::IVR_Settings settings = dialog.Settings();

    // get IVR variables
    mp_PlayerWindow    = settings.videoPlayer;
    mi_numCams         = settings.numberCams;

    mo_savePath1       = QDir(settings.videoPath1);
    ml_videoContainer1 = settings.videoContainer1;

    if(mi_numCams == 2)
    {
        mo_savePath2       = QDir(settings.videoPath2);
        ml_videoContainer2 = settings.videoContainer2;
    }

    // streamer variables
    mb_useStreamer    = settings.useStreamer;

    if(mb_useStreamer)
    {
        mp_StreamerWindow = settings.videoStreamer;
        ms_hotkeyStart    = settings.hotkeyStart;
        ms_hotkeyStop     = settings.hotkeyStop;
    }

    // get window handle from name
//    mp_StreamerWindow = FindWindowA(nullptr, ms_videoStreamer.toStdString().c_str());
//    mp_PlayerWindow = FindWindowA(nullptr, ms_videoPlayer.toStdString().c_str());

    // setting the displayed folder to the video folder
    mp_filemodel1->setRootPath(mo_savePath1.absolutePath());
    ui->treeView_folder->setRootIndex(mp_proxymodel1->mapFromSource(mp_filemodel1->index(mo_savePath1.absolutePath())));
    mp_filemodel1->setNameFilters(QStringList(ml_videoContainer1));
    ui->treeView_folder->setColumnWidth(0, 150);

    if(mi_numCams == 2)
    {
        // setting the displayed folder to the video folder
        mp_filemodel2->setRootPath(mo_savePath2.absolutePath());
        ui->treeView_folder2->setRootIndex(mp_proxymodel2->mapFromSource(mp_filemodel2->index(mo_savePath2.absolutePath())));
        mp_filemodel2->setNameFilters(QStringList(ml_videoContainer1));
        ui->treeView_folder2->setColumnWidth(0, 150);
    }

    ui->tabWidget->setTabEnabled(1, mi_numCams == 2);
    ui->frame_recording->setEnabled(mb_useStreamer);
}

void RecordWindow::freezeWindow(bool set)
{
    setWindowFlag(Qt::FramelessWindowHint, set);
    show();
    ui->actionFreeze->setChecked(set);
}

void RecordWindow::setTopevel(bool set)
{
    setWindowFlag(Qt::WindowStaysOnTopHint, set);
    show();
    ui->actionSet_Toplevel->setChecked(set);
}

void RecordWindow::startstopRecord()
{
    if(mp_PlayerWindow == nullptr)
    {
        if(mb_recording)
        {
             MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                      tr("Aufnahme konnte nicht gestoppt werden!\nBitte kontrollieren Sie die Einstellungen!"));
        }
        else
        {
             MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                      tr("Aufnahme konnte nicht gestartet werden!\nBitte kontrollieren Sie die Einstellungen!"));
        }

        return;
    }

    SetFocus(*mp_StreamerWindow);

    if(mb_recording)
    {
        SendMessageA(*mp_StreamerWindow, WM_KEYDOWN, 0x53, 0);
        SendMessageA(*mp_StreamerWindow, WM_KEYUP, 0x53, 0);

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
        SendMessageA(*mp_StreamerWindow, WM_KEYDOWN, 0x53, 0);
        SendMessageA(*mp_StreamerWindow, WM_KEYUP, 0x53, 0);

        ui->button_record->setText(tr("Stoppen"));
        ui->label_record->setEnabled(true);
        ui->frame_replay->setEnabled(false);
        mb_recording = true;
    }
}

void RecordWindow::startVideo()
{
    if(mo_savePath1 == QDir(""))
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Wiedergabe konnte nicht gestartet werden!\nBitte kontrollieren Sie die Einstellungen und starten Sie die Wiedergabe manuell!"));
        return;
    }

    if(!ms_videoFile.isEmpty())
        QDesktopServices::openUrl(QUrl(mo_savePath1.absoluteFilePath(ms_videoFile), QUrl::TolerantMode));
}

void RecordWindow::stopVideo()
{
    if(mp_PlayerWindow == nullptr)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Wiedergabe konnte nicht gestoppt werden!\nBitte kontrollieren Sie die Einstellungen und stoppen Sie die Wiedergabe manuell!"));
        return;
    }

    SetForegroundWindow(*mp_PlayerWindow);
    keybd_event(VK_F4, 0, 0, 0);
    keybd_event(VK_F4, 0, WM_KEYUP, 0);
}

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
    QString oldName;

    if(cam == 1)
    {
        QModelIndex indexSource = mp_proxymodel1->mapToSource(ui->treeView_folder->currentIndex());
        oldName = mp_filemodel1->filePath(indexSource);
    }
    else if(cam == 2)
    {
        QModelIndex indexSource = mp_proxymodel2->mapToSource(ui->treeView_folder2->currentIndex());
        oldName = mp_filemodel2->filePath(indexSource);
    }

    int index = oldName.lastIndexOf('.');
    QStringRef videoFormat(&oldName, index, oldName.size()-index);

    bool ok;
    QString fightnr = MessagePrinter::InputHandler("Fight Nr: ", ui->lineEdit_fightNr->text(), ok);

    if(!ok)
    {
        fightnr = ui->lineEdit_fightNr->text();
    }

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

