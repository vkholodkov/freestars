/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(application);

    QApplication app(argc, argv);
    app.setOrganizationName("None");
    app.setApplicationName("FreeStars Qt Client");

    /*
     * Load Qt translations
     */
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator messagesTranslator;
    //freestarsTranslator.load("messages_" + QLocale::system().name());
    messagesTranslator.load("messages", qApp->applicationDirPath());
    app.installTranslator(&messagesTranslator);

    QTranslator freestarsTranslator;
    //freestarsTranslator.load("messages_" + QLocale::system().name());
    freestarsTranslator.load("freestars", qApp->applicationDirPath());
    app.installTranslator(&freestarsTranslator);

    FreeStars::MainWindow mainWin;
#if defined(Q_OS_SYMBIAN)
    mainWin.showMaximized();
#else
    mainWin.show();
#endif
    return app.exec();    
}
