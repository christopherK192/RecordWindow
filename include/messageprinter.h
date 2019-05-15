#ifndef MESSAGEPRINTER_H
#define MESSAGEPRINTER_H

#include <QMessageBox>
#include <QInputDialog>
#include <QIcon>

namespace MessagePrinter
{
/*!
 * MessagePrinter::ErrorBox
 * Opens a Message Box with an error message.
 *
 * \param windowTitle [in] title of the message box.
 * \param iconPath    [in] path to the icon of the message box.
 * \param errorString [in] error message to be displayed.
 */
static void ErrorBox(QString windowTitle, QString iconPath, QString errorString)
{
    QFont font;
    font.setPointSize(9);

    QMessageBox message;
    message.setWindowTitle(windowTitle);
    message.setWindowIcon(QIcon(iconPath));
    message.setIcon(QMessageBox::Critical);
    message.setDefaultButton(QMessageBox::Ok);
    message.setText((QObject::tr("FEHLER:\n") + errorString).toStdString().c_str());
    message.setFont(font);
    message.exec();
}

/*!
 * MessagePrinter::WarningBox
 * Opens a Message Box with an warning message.
 *
 * \param windowTitle   [in] title of the message box.
 * \param iconPath      [in] path to the icon of the message box.
 * \param warningString [in] warning message to be displayed.
 */
static bool WarningBox(QString windowTitle, QString iconPath, QString warningString)
{
    QFont font;
    font.setPointSize(9);

    QMessageBox message;
    message.setWindowTitle(windowTitle);
    message.setWindowIcon(QIcon(iconPath));
    message.setIcon(QMessageBox::Warning);
    message.addButton(QObject::tr("Ja"), QMessageBox::AcceptRole);
    message.setDefaultButton(
                message.addButton(QObject::tr("Nein"), QMessageBox::RejectRole));
    message.setText((QObject::tr("WARNUNG:\n") + warningString).toStdString().c_str());
    message.setFont(font);

    return message.exec();
}

/*!
 * MessagePrinter::InfoBox
 * Opens Message Box with an info message.
 *
 * \param windowTitle [in] title of the message box.
 * \param iconPath    [in] path to the icon of the message box.
 * \param infoString  [in] info message to be displayed.
 */
static void InfoBox(QString windowTitle, QString iconPath, QString infoString)
{
    QFont font;
    font.setPointSize(9);

    QMessageBox message;
    message.setWindowTitle(windowTitle);
    message.setWindowIcon(QIcon(iconPath));
    message.setIcon(QMessageBox::Information);
    message.setDefaultButton(QMessageBox::Ok);
    message.setText(infoString.toStdString().c_str());
    message.setFont(font);
    message.exec();
}

/*!
 * MessagePrinter::inputHandler
 * Opens an Input Dialog for text input.
 *
 * \param text A displayed info text.
 * \param input A preselected input value.
 * \param ok The window return value.
 *
 * \return Entered integer as int.
 */
static QString InputHandler(QString text, QString input, bool &ok)
{
    QFont font;
    font.setPointSize(9);

    QInputDialog dialog;
    dialog.setWindowTitle("IVR-Record App");
    dialog.setWindowIcon(QIcon(":/Icons/logo.ico"));
    dialog.setInputMode(QInputDialog::TextInput);
    dialog.setLabelText(text.toStdString().c_str());
    dialog.setCancelButtonText(QObject::tr("Abbrechen"));
    dialog.setTextValue(input);
    dialog.setFont(font);
    dialog.adjustSize();

    ok = dialog.exec();
    return dialog.textValue();
}

}

#endif // MESSAGEPRINTER_H

