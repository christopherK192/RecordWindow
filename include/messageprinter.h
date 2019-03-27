#ifndef MESSAGEPRINTER_H
#define MESSAGEPRINTER_H

#include <QMessageBox>
#include <QIcon>

namespace MessagePrinter
{
/*!
 * \brief MessagePrinter::ErrorBox
 *        Opens a Message Box with an error message.
 *
 * \param windowTitle [in] title of the message box.
 * \param iconPath    [in] path to the icon of the message box.
 * \param errorString [in] error message to be displayed.
 */
void ErrorBox(QString windowTitle, QString iconPath, QString errorString)
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
 * \brief MessagePrinter::WarningBox
 *        Opens a Message Box with an warning message.
 *
 * \param windowTitle   [in] title of the message box.
 * \param iconPath      [in] path to the icon of the message box.
 * \param warningString [in] warning message to be displayed.
 */
bool WarningBox(QString windowTitle, QString iconPath, QString warningString)
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
 * \brief MessagePrinter::InfoBox
 *        Opens Message Box with an info message.
 *
 * \param windowTitle [in] title of the message box.
 * \param iconPath    [in] path to the icon of the message box.
 * \param infoString  [in] info message to be displayed.
 */
void InfoBox(QString windowTitle, QString iconPath, QString infoString)
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

}

#endif // MESSAGEPRINTER_H

