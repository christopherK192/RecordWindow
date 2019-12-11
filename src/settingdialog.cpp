#include "include/settingdialog.h"
#include "include/messageprinter.h"
#include "ui_settingdialog.h"

#include <QTextStream>
#include <QPushButton>


SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    // initialize ui
    ui->setupUi(this);

    // IVR options
    ui->radioButton_1camera->setChecked(true);

    ui->lineEdit_videoPath1->clear();
    ui->comboBox_videoContainer1->addItems(VIDEO_CONTAINERS);
    ui->comboBox_videoContainer1->setCurrentIndex(0);

    ui->lineEdit_videoPath2->clear();
    ui->comboBox_videoContainer2->addItems(VIDEO_CONTAINERS);
    ui->comboBox_videoContainer2->setCurrentIndex(0);

    ui->groupBox_camera2->setEnabled(false);

    // videoStreamer options
    ui->checkBox_videoStreamer->setChecked(false);
    ui->comboBox_videoStreamer->setCurrentIndex(0);
    ui->frame_recorder->setEnabled(false);

    ui->lineEdit_hotkeyStart->clear();
    ui->frame_hotkeyStart->setEnabled(false);
    ui->lineEdit_hotkeyStart->setReadOnly(true);

    ui->lineEdit_hotkeyStop->clear();
    ui->frame_hotkeyStop->setEnabled(false);
    ui->lineEdit_hotkeyStop->setReadOnly(true);

    // set video player as start view
    ui->tabWidget_settings->setCurrentIndex(0);

    // getting the window names of all open windows
    // SettingDialog::getOpenWindows is the callback function for all windows.
    QStringList windowList;
    EnumDesktopWindows(nullptr, &SettingDialog::getOpenWindows,
                       reinterpret_cast<LPARAM>(&windowList));
    ui->comboBox_videoStreamer->addItems(windowList);
    ui->comboBox_videoStreamer->removeItem(
                ui->comboBox_videoStreamer->findText(WINDOW_TITLE));

    // get saved settings
    loadSettings();

    // connect signals to buttons
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this,
            &SettingDialog::closeSettings);

    connect(ui->radioButton_1camera, &QRadioButton::clicked,
            [this](){SettingDialog::toggleCameras(1);});
    connect(ui->radioButton_2camera, &QRadioButton::clicked,
            [this](){SettingDialog::toggleCameras(2);});
    connect(ui->button_videoPath1, &QToolButton::clicked,
            [this](){SettingDialog::setVideoPath(1);});
    connect(ui->button_videoPath2, &QToolButton::clicked,
            [this](){SettingDialog::setVideoPath(2);});
    connect(ui->comboBox_videoContainer1, &QComboBox::currentTextChanged,
            [this](){SettingDialog::setVideoContainer(1);});
    connect(ui->comboBox_videoContainer2, &QComboBox::currentTextChanged,
            [this](){SettingDialog::setVideoContainer(2);});

    connect(ui->checkBox_videoStreamer, &QCheckBox::stateChanged, this,
            &SettingDialog::enableStreamer);
    connect(ui->button_videoStreamer, &QToolButton::clicked, this,
            &SettingDialog::connect2Streamer);
    connect(ui->button_hotkeyStart, &QRadioButton::clicked,
            [this](){SettingDialog::setHotkey(Hotkeys::RECORDING_START);});
    connect(ui->button_hotkeyStop, &QRadioButton::clicked,
            [this](){SettingDialog::setHotkey(Hotkeys::RECORDING_STOP);});
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

BOOL CALLBACK SettingDialog::getOpenWindows(HWND i_window, LPARAM o_param)
{
    const int titleSize = 1024;
    char windowTitle[titleSize];

//    GetClassNameA(window, windowTitle, titleSize);
    GetWindowTextA(i_window, windowTitle, titleSize);
    QString title = QString::fromUtf8(windowTitle);

    if(IsWindow(i_window) && IsWindowVisible(i_window) && !title.isEmpty())
    {
        QStringList& windowList = *reinterpret_cast<QStringList*>(o_param);
        windowList << title;
    }

    return true;
}

QStringList SettingDialog::extractFormat(QString i_sVideoContainer)
{
    QStringList list;
    list = i_sVideoContainer.replace('(',',').replace(')',',').split(',', QString::SkipEmptyParts);
    list.removeFirst();
    return list;
}

void SettingDialog::loadSettings()
{
    QFile saveFile(QDir::currentPath() + SAVE_FILE);

    if(saveFile.open(QIODevice::ReadOnly))
    {
        QTextStream saveStream(&saveFile);
        QString line;

        saveStream.readLineInto(&line);
        int num_cameras = line.split("::").last().toInt();

        if(num_cameras == 1)
        {
           ui->radioButton_1camera->setChecked(true);
        }
        else if(num_cameras == 2)
        {
            ui->radioButton_2camera->setChecked(true);
        }

        toggleCameras(num_cameras);

        saveStream.readLineInto(&line);
        ui->lineEdit_videoPath1->setText(line.split("::").last());
        m_oSettingsIVR.videoPath1 = ui->lineEdit_videoPath1->text();

        saveStream.readLineInto(&line);
        ui->comboBox_videoContainer1->setCurrentIndex(line.split("::").last().toInt());
        m_oSettingsIVR.videoContainer1 = extractFormat(ui->comboBox_videoContainer1->currentText());

        if(ui->groupBox_camera2->isEnabled())
        {
            saveStream.readLineInto(&line);
            ui->lineEdit_videoPath2->setText(line.split("::").last());
            m_oSettingsIVR.videoPath2 = ui->lineEdit_videoPath2->text();

            saveStream.readLineInto(&line);
            ui->comboBox_videoContainer2->setCurrentIndex(line.split("::").last().toInt());
            m_oSettingsIVR.videoContainer2 = extractFormat(ui->comboBox_videoContainer2->currentText());
        }
        else
        {
            saveStream.readLineInto(&line);
            saveStream.readLineInto(&line);
        }

        saveStream.readLineInto(&line);
        m_oSettingsIVR.useStreamer = line.split("::").last().toInt();
        ui->checkBox_videoStreamer->setCheckState(
                    static_cast<Qt::CheckState>(m_oSettingsIVR.useStreamer));
        enableStreamer(m_oSettingsIVR.useStreamer);

        saveStream.readLineInto(&line);
        ui->lineEdit_hotkeyStart->setText(line.split("::").last());
        m_oSettingsIVR.hotkeyStart = ui->lineEdit_hotkeyStart->text().toStdString().c_str()[0];

        saveStream.readLineInto(&line);
        ui->lineEdit_hotkeyStop->setText(line.split("::").last());
        m_oSettingsIVR.hotkeyStop = ui->lineEdit_hotkeyStop->text().toStdString().c_str()[0];

        saveFile.close();

        MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO,
                                tr("Settings loaded successfully."));
        return;
    }
    MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                             tr("Settings couldn't be loaded!"));
}

void SettingDialog::saveSettings()
{
    QFile saveFile(QDir::currentPath() + SAVE_FILE);

    if(saveFile.open(QIODevice::WriteOnly))
    {
        QTextStream saveStream(&saveFile);

        saveStream << "NUMBER_CAMS::"       << m_oSettingsIVR.numberCams                    << endl
                   << "VIDEO_DIR_1::"       << m_oSettingsIVR.videoPath1                    << endl
                   << "VIDEO_CONTAINER_1::" << ui->comboBox_videoContainer1->currentIndex() << endl
                   << "VIDEO_DIR_2::"       << m_oSettingsIVR.videoPath2                    << endl
                   << "VIDEO_CONTAINER_2::" << ui->comboBox_videoContainer2->currentIndex() << endl
                   << "STREAMER_ENABLED::"  << m_oSettingsIVR.useStreamer                   << endl
                   << "HOTKEY_START::"      << m_oSettingsIVR.hotkeyStart                   << endl
                   << "HOTKEY_STOP::"       << m_oSettingsIVR.hotkeyStop                    << endl;

        saveFile.close();

        MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO,
                                tr("Settings saved successfully."));
        return;
    }

    MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                             tr("Settings couldn't be saved!"));
}

void SettingDialog::closeSettings(QAbstractButton* i_pButton)
{
    if(ui->buttonBox->button(QDialogButtonBox::Ok) == dynamic_cast<QPushButton*>(i_pButton))
    {
        emit accept();
    }
    else if(ui->buttonBox->button(QDialogButtonBox::Cancel) == dynamic_cast<QPushButton*>(i_pButton))
    {
        emit reject();
    }
    else if(ui->buttonBox->button(QDialogButtonBox::Save) == dynamic_cast<QPushButton*>(i_pButton))
    {
        saveSettings();
    }
    else
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Unexpected button pressed!!"));
    }
}

/**************************************************
 * IVR Functions
 **************************************************
 */
void SettingDialog::toggleCameras(const int i_nCamera)
{
    m_oSettingsIVR.numberCams = i_nCamera;
    ui->groupBox_camera2->setEnabled(i_nCamera == 2);
}

void SettingDialog::setVideoPath(const int i_nCamera)
{
    QString* path = nullptr;
    QLineEdit* pathLineEdit = nullptr;

    switch(i_nCamera)
    {
        case 1:
            path = &m_oSettingsIVR.videoPath1;
            pathLineEdit= ui->lineEdit_videoPath1;
            break;
        case 2:
            path = &m_oSettingsIVR.videoPath2;
            pathLineEdit = ui->lineEdit_videoPath2;
            break;        
    }

    if(path == nullptr)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Unexpected path error!"));
        return;
    }

    QString savepath = QFileDialog::getExistingDirectory(this, tr("Open Folder"), *path,
                                                         QFileDialog::DontResolveSymlinks |
                                                         QFileDialog::ShowDirsOnly);

    if(savepath.isEmpty())
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("No folder selected!\nPlease select a folder!"));
        return;
    }

    pathLineEdit->setText(savepath);
    *path = pathLineEdit->text();
}

void SettingDialog::setVideoContainer(const int i_nCamera)
{
    switch(i_nCamera)
    {
        case 1:
            m_oSettingsIVR.videoContainer1 = extractFormat(ui->comboBox_videoContainer1->currentText());
            break;
        case 2:
            m_oSettingsIVR.videoContainer2 = extractFormat(ui->comboBox_videoContainer2->currentText());
            break;
    }
}

/**************************************************
 * Recording Functions
 **************************************************
 */
void SettingDialog::enableStreamer(const int i_nState)
{
    m_oSettingsIVR.useStreamer = i_nState;

    ui->frame_recorder->setEnabled(i_nState == Qt::Checked);
    ui->frame_hotkeyStart->setEnabled(i_nState == Qt::Checked);
    ui->frame_hotkeyStop->setEnabled(i_nState == Qt::Checked);
}

void SettingDialog::connect2Streamer()
{
    m_oSettingsIVR.videoStreamer = nullptr;
    QString streamer = ui->comboBox_videoStreamer->currentText();

    if (streamer == "-" || streamer.isEmpty())
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("No video streamer/recorder selected!\nPlease select a video streamer/recorder!"));
        return;
    }

    //HWND windowHandler = FindWindowA(streamer.toStdString().c_str(), nullptr);
    HWND windowHandler = FindWindowA(nullptr, streamer.toStdString().c_str());

    if(windowHandler == nullptr)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("No instance of selected program found!\n\
                                     Please check if program is open!"));
        return;
    }

    m_oSettingsIVR.videoStreamer = windowHandler;
    m_oSettingsIVR.videoStreamerName = streamer;

    MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO,
                            tr("Connection to program established successfully."));
}

void SettingDialog::setHotkey(const int i_nHotkey)
{
    char* keyCombi = nullptr;
    QLineEdit* keyLineEdit = nullptr;

    switch(i_nHotkey)
    {
        case Hotkeys::RECORDING_START:
            keyCombi = &m_oSettingsIVR.hotkeyStart;
            keyLineEdit= ui->lineEdit_hotkeyStart;
            break;
        case Hotkeys::RECORDING_STOP:
            keyCombi = &m_oSettingsIVR.hotkeyStop;
            keyLineEdit = ui->lineEdit_hotkeyStop;
            break;
    }

    bool no_err;
    QString hotkey_string = MessagePrinter::InputHandler(
                tr("Please enter shortcut:"), keyLineEdit->text(), no_err);

    if(!no_err)
    {
        MessagePrinter::ErrorBox(WINDOW_TITLE, ICON_LOGO,
                                 tr("Shortcut couldn't be recorded! Please try again!"));
        return;
    }

    keyLineEdit->setText(hotkey_string.toUpper());
    *keyCombi = keyLineEdit->text().toStdString().c_str()[0];

    MessagePrinter::InfoBox(WINDOW_TITLE, ICON_LOGO,
                            tr("Shortcut recorded successfully."));
}

