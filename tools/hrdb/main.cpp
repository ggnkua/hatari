#include "ui/mainwindow.h"

#include <QApplication>
#include <QProcess>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Hatari team");
    QCoreApplication::setApplicationName("hrdb");
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
