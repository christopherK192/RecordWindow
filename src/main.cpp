#include "include/recordwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RecordWindow w;
    w.show();

    return a.exec();
}
