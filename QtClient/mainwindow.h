/*
 * Copyright (C) 2014 Valery Kholodkov
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QActionGroup>
#include <QSignalMapper>

#include <memory>

#include "FSServer.h"
#include "graphics_array.h"

class QAction;
class QMenu;
class QPlainTextEdit;

namespace FreeStars {

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    void openRecentFile();
    void about();
    void documentWasModified();
    void activateTab(int);
    void closeTab(int);

private:
    void loadGraphics();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString&);
    void loadPlayerFile(const QString&);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();

    void updateModel();    
    void openGameView();
    void closeAllViews();

    QString strippedName(const QString &fullFileName);

    std::auto_ptr<GraphicsArray> componentPictures;

    QTabWidget *tabWidget;
    QString curFile;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *turnMenu;
    QMenu *commandsMenu;
    QMenu *helpMenu;
    QToolBar *toolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;

    QAction *viewRaceAction;

    QAction *submitTurnAction;

    QAction *shipDesignAction;
    QAction *researchAction;
    QAction *battlePlansAction;
    QAction *playerRelationsAction;

    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *separatorAct;

    QActionGroup *viewModeGroup;
    QAction *normalViewAction;
    QAction *surfaceMineralsViewAction;
    QAction *mineralConcViewAction;
    QAction *planetValueViewAction;
    QAction *populationViewAction;
    QSignalMapper *viewModeMapper;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
};

};

#endif
