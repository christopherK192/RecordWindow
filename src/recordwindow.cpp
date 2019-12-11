#include "include/recordwindow.h"
#include "include/messageprinter.h"
#include "ui_recordwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QDesktopServices>
#include <QScreen>

RecordWindow::RecordWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RecordWindow),  
    m_pFilemodel1(new QFileSystemModel),
    m_pFilemodel2(new QFileSystemModel),
    m_pProxymodel1(new QSortFilterProxyModel),
    m_pProxymodel2(new QSortFilterProxyModel),
    m_pPlayerWindow(nullptr),
    m_slVideoContainer1(""),
    m_slVideoContainer2(""),
    m_oSavePath1(""),
    m_oSavePath2(""),
    m_nIvrCounter(1),
    m_nNumCams(false),
    m_pStreamerWindow(nullptr),
    m_cHotkeyStart(NULL),
    m_cHotkeyStop(NULL),
    m_nUseStreamer(Qt::Unchecked),
    m_bRecording(false),
    m_bShowFolder(true)
{
    // initialize ui
    ui->setupUi(this);

    ui->lineEdit_fightNr->clear();
    ui->lineEdit_fightNr->setText("100");

    ui->tabWidget->setTabEnabled(1, m_nNumCams == 2);
    showHideFolder();

    // initialize displayed folder
    m_pFilemodel1->setRootPath(QDir::currentPath());
    m_pFilemodel1->setFilter(QDir::Files | QDir::NoSymLinks);
    m_pFilemodel1->setNameFilterDisables(false);
    // using proxymodel in order to sort items
    m_pProxymodel1->setSourceModel(m_pFilemodel1);
    ui->treeView_folder->setModel(m_pProxymodel1);
    ui->treeView_folder->setSortingEnabled(true);  
    // hide all columns except name and last changed
    ui->treeView_folder->setColumnHidden(1,true);
    ui->treeView_folder->setColumnHidden(2,true);
    ui->treeView_folder->setColumnHidden(3,true);

    // initialize scond displayed folder (hidden)
    m_pFilemodel2->setRootPath(QDir::currentPath());
    m_pFilemodel2->setFilter(QDir::Files | QDir::NoSymLinks);
    m_pFilemodel2->setNameFilterDisables(false);
    // using proxymodel in order to sort items
    m_pProxymodel2->setSourceModel(m_pFilemodel2);
    ui->treeView_folder2->setModel(m_pProxymodel2);
    ui->treeView_folder2->setSortingEnabled(true);
    // hide all columns except name and last changed
    ui->treeView_folder2->setColumnHidden(1,true);
    ui->treeView_folder2->setColumnHidden(2,true);
    ui->treeView_folder2->setColumnHidden(3,true);

    // connect signals to buttons
    connect(ui->pushButton_record, &QPushButton::clicked,
            this, &RecordWindow::startstopRecord);

    connect(ui->pushButton_rename1, &QPushButton::clicked,
            [this](){RecordWindow::renameVideo(1);});
    connect(ui->pushButton_rename2, &QPushButton::clicked,
            [this](){RecordWindow::renameVideo(2);});
    connect(ui->treeView_folder, &QTreeView::doubleClicked,
            this, &RecordWindow::openVideo1);
    connect(ui->treeView_folder2, &QTreeView::doubleClicked,
            this, &RecordWindow::openVideo2);

    connect(ui->actionSettings, &QAction::triggered,
            this, &RecordWindow::openSettings);
    connect(ui->actionFreeze, &QAction::toggled,
            this, &RecordWindow::freezeWindow);
    connect(ui->actionSet_Toplevel, &QAction::toggled,
            this, &RecordWindow::setTopevel);
    connect(ui->actionShowFolder, &QAction::triggered,
            this, &RecordWindow::showHideFolder);
    connect(ui->actionExit, &QAction::triggered,
            this, &RecordWindow::close);

    this->resize(this->minimumWidth(), this->minimumHeight());

    QRect screenrect = qApp->primaryScreen()->geometry();
    this->move(screenrect.right() - 78, screenrect.top());

    setTopevel(true);
    freezeWindow(true);
}

RecordWindow::~RecordWindow()
{
    delete m_pProxymodel1;
    delete m_pProxymodel2;
    delete m_pFilemodel1;
    delete m_pFilemodel2;
    delete ui;
}

void RecordWindow::set_videoName()
{
    // get the newest video file
    m_oSavePath1.setFilter(QDir::Files | QDir::NoSymLinks);
    m_oSavePath1.setNameFilters(QStringList(m_slVideoContainer1));
    m_oSavePath1.setSorting(QDir::Time);

    QStringList fileList = m_oSavePath1.entryList();
    if(fileList.isEmpty())
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("No playable file found!\nPlease check the path!"));
        m_sVideoFile = "";
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
    m_oSavePath1.setNameFilters(nameFilter2);

    // increment counter if a file with the name already exists
    m_nIvrCounter = m_oSavePath1.entryList().size() + 1;
    m_sVideoFile = (fightnr + "." + QString::number(m_nIvrCounter) + videoFormat.toString());
    m_oSavePath1.rename(oldName, m_sVideoFile);
}

void RecordWindow::openSettings()
{ 
    SettingDialog dialog;
    dialog.setWindowTitle(tr("Settings"));
    dialog.setWindowIcon(QIcon(ICON_LOGO));
    dialog.resize(dialog.height(), dialog.height());
    bool ok = dialog.exec();

    if(!ok)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Settings couldn't be loaded!"));
        return;
    }

    SettingDialog::IVR_Settings settings = dialog.Settings();

    // get IVR variables
    m_nNumCams          = settings.numberCams;
    m_oSavePath1        = QDir(settings.videoPath1);
    m_slVideoContainer1 = settings.videoContainer1;

    if(m_nNumCams == 2)
    {
        m_oSavePath2        = QDir(settings.videoPath2);
        m_slVideoContainer2 = settings.videoContainer2;
    }

    // streamer variables
    m_nUseStreamer = settings.useStreamer;

    if(m_nUseStreamer == Qt::Checked)
    {
        m_cHotkeyStart = settings.hotkeyStart;
        m_cHotkeyStop  = settings.hotkeyStop;
    }

    // setting the displayed folder to the video folder
    m_pFilemodel1->setRootPath(m_oSavePath1.absolutePath());
    ui->treeView_folder->setRootIndex(m_pProxymodel1->mapFromSource(m_pFilemodel1->index(m_oSavePath1.absolutePath())));
    m_pFilemodel1->setNameFilters(QStringList(m_slVideoContainer1));
    ui->treeView_folder->setColumnWidth(0, 50);

    if(m_nNumCams == 2)
    {
        // setting the displayed folder to the video folder
        m_pFilemodel2->setRootPath(m_oSavePath2.absolutePath());
        ui->treeView_folder2->setRootIndex(m_pProxymodel2->mapFromSource(m_pFilemodel2->index(m_oSavePath2.absolutePath())));
        m_pFilemodel2->setNameFilters(QStringList(m_slVideoContainer2));
        ui->treeView_folder2->setColumnWidth(0, 50);
    }

    ui->tabWidget->setTabEnabled(1, m_nNumCams == 2);
    ui->pushButton_record->setEnabled(m_nUseStreamer == Qt::Checked);

    // get window handle from name
    m_sVideoStreamer  = settings.videoStreamerName;
    m_pStreamerWindow = FindWindowA(nullptr, m_sVideoStreamer.toStdString().c_str());
    m_pPlayerWindow   = FindWindowA(nullptr, m_sVideoPlayer.toStdString().c_str());
}

void RecordWindow::freezeWindow(bool i_bMode)
{
    setWindowFlag(Qt::FramelessWindowHint, i_bMode);
    show();
    ui->actionFreeze->setChecked(i_bMode);
    ui->actionSet_Toplevel->setEnabled(!i_bMode);
}

void RecordWindow::setTopevel(bool i_bMode)
{
    setWindowFlag(Qt::WindowStaysOnTopHint, i_bMode);
    show();
    ui->actionSet_Toplevel->setChecked(i_bMode);
}

void RecordWindow::startstopRecord()
{
    if(!IsWindow(m_pStreamerWindow) || m_pStreamerWindow == NULL)
    {
         m_pStreamerWindow = FindWindowA(nullptr, m_sVideoStreamer.toStdString().c_str());
         if(m_pStreamerWindow == NULL)
         {
             if(m_bRecording)
             {
                  MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                           tr("Recording couldn't be stopped!\nPlease check the settings!"));
             }
             else
             {
                  MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                           tr("Recording couldn't be stopped!\nPlease check the settings!"));
             }

             return;
         }
    }

    SetFocus(m_pStreamerWindow);

    if(m_bRecording)
    {
        SendMessageA(m_pStreamerWindow, WM_KEYDOWN, m_cHotkeyStop, 0);
        SendMessageA(m_pStreamerWindow, WM_KEYUP, m_cHotkeyStop, 0);

        ui->pushButton_record->setIcon(QIcon(":/icons/icons/record2.svg"));
        m_bRecording = false;

        if(m_nUseStreamer == Qt::Checked)
        {
            Sleep(uint(1000));
            set_videoName();
            Sleep(uint(1000));
            startstopVideo();
        }
    }
    else
    {
        SendMessageA(m_pStreamerWindow, WM_KEYDOWN, m_cHotkeyStart, 0);
        SendMessageA(m_pStreamerWindow, WM_KEYUP, m_cHotkeyStart, 0);

        ui->pushButton_record->setIcon(QIcon(":/icons/icons/record.svg"));
        m_bRecording = true;
    }
}

void RecordWindow::startstopVideo()
{
    if(m_oSavePath1 == QDir(""))
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Playback couldn't be started!\nPlease check the settings and restart the playback!"));
        return;
    }

    if(!m_sVideoFile.isEmpty())
    {
        QDesktopServices::openUrl(QUrl(m_oSavePath1.absoluteFilePath(m_sVideoFile), QUrl::TolerantMode));
    }
    else
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Playback couldn't be started!\nNo recorded video found!"));
    }
}

void RecordWindow::openVideo1(const QModelIndex &i_index)
{
    QModelIndex indexSource = m_pProxymodel1->mapToSource(i_index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_pFilemodel1->filePath(indexSource)));
}

void RecordWindow::openVideo2(const QModelIndex &i_index)
{
    QModelIndex indexSource = m_pProxymodel2->mapToSource(i_index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_pFilemodel2->filePath(indexSource)));
}

void RecordWindow::renameVideo(const int i_nCamera)
{
    QString oldName;
    QDir savePath;

    if(i_nCamera == 1)
    {
        QModelIndex indexSource = m_pProxymodel1->mapToSource(ui->treeView_folder->currentIndex());
        savePath = m_oSavePath1;
        oldName  = m_pFilemodel1->filePath(indexSource);
    }
    else if(i_nCamera == 2)
    {
        QModelIndex indexSource = m_pProxymodel2->mapToSource(ui->treeView_folder2->currentIndex());
        savePath = m_oSavePath2;
        oldName  = m_pFilemodel2->filePath(indexSource);
    }

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
    savePath.setNameFilters(nameFilter2);

    // increment counter if a file with the name already exists
    m_nIvrCounter = savePath.entryList().size() + 1;
    m_sVideoFile = (fightnr + "." + QString::number(m_nIvrCounter) + videoFormat.toString());
    savePath.rename(oldName, m_sVideoFile);
}

void RecordWindow::showHideFolder()
{
    if(m_bShowFolder)
    {

        ui->tabWidget->setVisible(false);
        m_bShowFolder = false;
        this->resize(100, 200);
    }
    else
    {
        ui->tabWidget->setVisible(true);
        m_bShowFolder = true;
    }
}

