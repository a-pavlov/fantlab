#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QLibraryInfo>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Load translation
    QTranslator qtTranslator;
    QTranslator translator;

    QString locale = QLocale::system().name();
    if(locale.isEmpty()) locale = "en_GB";
    qDebug("Program locale: %s", qPrintable(locale));

    if(qtTranslator.load(QString::fromUtf8("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
       qDebug("Qt %s locale recognized, using translation.", qPrintable(locale));
    } else {
       qDebug("Qt %s locale unrecognized, using default (en_GB).", qPrintable(locale));
    }

    a.installTranslator(&qtTranslator);

    if(translator.load(QString::fromUtf8(":/languages/f_") + locale)) {
       qDebug("%s locale recognized, using translation.", qPrintable(locale));
    }
    else {
       qDebug("%s locale unrecognized, using default (en_GB).", qPrintable(locale));
    }

    a.installTranslator(&translator);

    MainWindow w;
    w.show();
    return a.exec();
}
