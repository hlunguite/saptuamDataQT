#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QCoreApplication::setApplicationName("SaptuamData");
    QCoreApplication::setOrganizationName("Guite Software Inc");
    QCoreApplication::setApplicationVersion("SaptuamData 5.0");
    QCoreApplication::setOrganizationDomain("hlunguite@gmail.com");

    QApplication a(argc, argv);
    MainWindow w;
    if (w.getIsError() == false) {
        w.show();
         return a.exec();
    }
    return 0;

}
