#include "headers/views/mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/images/mw-icon.png"));

    QTranslator qtTranslator;
    if (qtTranslator.load("qtbase_ru",
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        a.installTranslator(&qtTranslator);
    } else {
        qWarning() << "Couldn't download qtbase_ru translation";
    }

    MainWindow w;
    w.show();

    return a.exec();
}
