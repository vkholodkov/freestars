/*
 * Copyright (C) 2014 Valery Kholodkov
 */

#include <memory>

#include <QtGui>

#include "advanced_new_game_wizard.h"
#include "game_view.h"

#include "mainwindow.h"

#include "Battle.h"

namespace FreeStars {

MainWindow::MainWindow()
{
    loadGraphics();

    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(tabWidget);
    //tabWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(212, 208, 200);"));
    setCentralWidget(tabWidget);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(activateTab(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

    setCurrentFile("");

    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow() {
    Rules::Cleanup();
    Component::Cleanup();
    Battle::Cleanup();
    Ship::Cleanup();

    delete TheGame;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        //textEdit->clear();
        setCurrentFile("");
    }

    AdvancedNewGameWizard advancedNewGameWizard;

    advancedNewGameWizard.exec();
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if(TheGame != NULL) {
#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

        Player *player = TheGame->GetCurrentPlayer();
        player->SaveXFile();

#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif

        statusBar()->showMessage(tr("File saved"), 2000);
    }
    return true;
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

void MainWindow::documentWasModified()
{
    //setWindowModified(textEdit->document()->isModified());
}

void MainWindow::loadGraphics()
{

    QString fileName("rules/Graphics.xml");
  
    TiXmlDocument doc(fileName.toAscii().constData());
    doc.SetCondenseWhiteSpace(false);
    if (!doc.LoadFile()) {
        QMessageBox::critical(this, tr("Error"),
            tr("Cannot open graphics file %0").arg(fileName));
        return;
    }

    const TiXmlNode *rootNode = doc.FirstChild("GraphicsFile");
    if (!rootNode) {
        QMessageBox::critical(this, tr("Error"),
            tr("Invalid graphics file %0").arg(fileName));
        return;
    }

    const TiXmlNode *componentPicturesNode = rootNode->FirstChild("ComponentPictures");
    if (!componentPicturesNode) {
        QMessageBox::critical(this, tr("Error"),
            tr("Invalid graphics file %0").arg(fileName));
        return;
    }

    componentPictures.reset(new GraphicsArray);

    if(!componentPictures->ParseNode(componentPicturesNode)) {
        QMessageBox::critical(this, tr("Error"),
            tr("Unable to load component pictures from file %0").arg(fileName));
        return;
    }
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New..."), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    /*
     * View menu
     */
    viewRaceAction = new QAction(tr("&Race..."), this);
    viewRaceAction->setShortcut(QKeySequence(Qt::Key_F8));
    viewRaceAction->setStatusTip(tr("View your race"));
    viewRaceAction->setEnabled(false);

    /*
     * Turn menu
     */
    submitTurnAction = new QAction(tr("&Submit"), this);
    submitTurnAction->setShortcut(QKeySequence(Qt::Key_F9));
    submitTurnAction->setStatusTip(tr("Submit current turn"));
    submitTurnAction->setEnabled(false);

    /*
     * Commands menu
     */
    shipDesignAction = new QAction(tr("&Ship Design"), this);
    shipDesignAction->setShortcut(QKeySequence(Qt::Key_F4));
    shipDesignAction->setStatusTip(tr("Design ships and starbases"));
    shipDesignAction->setEnabled(false);

    researchAction = new QAction(tr("&Research"), this);
    researchAction->setShortcut(QKeySequence(Qt::Key_F5));
    researchAction->setStatusTip(tr("Edit research strategy"));
    researchAction->setEnabled(false);

    battlePlansAction = new QAction(tr("&Battle Plans"), this);
    battlePlansAction->setShortcut(QKeySequence(Qt::Key_F6));
    battlePlansAction->setStatusTip(tr("Edit battle plans"));
    battlePlansAction->setEnabled(false);

    playerRelationsAction = new QAction(tr("&Player Rrelations"), this);
    playerRelationsAction->setShortcut(QKeySequence(Qt::Key_F7));
    playerRelationsAction->setStatusTip(tr("Edit player relations"));
    playerRelationsAction->setEnabled(false);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    /*
     * Toolbar actions
     */
    viewModeGroup = new QActionGroup(this);

    normalViewAction = new QAction(QIcon(":/images/normal_map_mode.png"), tr("N&ormal view"), this);
    normalViewAction->setStatusTip(tr("Normal view"));
    normalViewAction->setCheckable(true);
    viewModeGroup->addAction(normalViewAction);

    surfaceMineralsViewAction = new QAction(QIcon(":/images/surface_minerals.png"), tr("S&urface minerals"), this);
    surfaceMineralsViewAction->setStatusTip(tr("Surface Mineral View"));
    surfaceMineralsViewAction->setCheckable(true);
    viewModeGroup->addAction(surfaceMineralsViewAction);

    mineralConcViewAction = new QAction(QIcon(":/images/minconc.png"), tr("Mineral &Concentration view"), this);
    mineralConcViewAction->setStatusTip(tr("Mineral Concentration view"));
    mineralConcViewAction->setCheckable(true);
    viewModeGroup->addAction(mineralConcViewAction);

    planetValueViewAction = new QAction(QIcon(":/images/planet_value.png"), tr("Planet &value view"), this);
    planetValueViewAction->setStatusTip(tr("Planet Value View"));
    planetValueViewAction->setCheckable(true);
    viewModeGroup->addAction(planetValueViewAction);

    populationViewAction = new QAction(QIcon(":/images/population_view.png"), tr("Population view"), this);
    populationViewAction->setStatusTip(tr("Population View"));
    populationViewAction->setCheckable(true);
    viewModeGroup->addAction(populationViewAction);

    viewModeGroup->setEnabled(false);

    viewModeMapper = new QSignalMapper(this);
    viewModeMapper->setMapping(normalViewAction, MM_NORMAL);
    viewModeMapper->setMapping(surfaceMineralsViewAction, MM_SURFACE_MIN);
    viewModeMapper->setMapping(mineralConcViewAction, MM_CONC_MIN);
    viewModeMapper->setMapping(planetValueViewAction, MM_PLANET_VALUE);
    viewModeMapper->setMapping(populationViewAction, MM_POPULATION);

    connect(normalViewAction, SIGNAL(triggered()), viewModeMapper, SLOT(map()));
    connect(surfaceMineralsViewAction, SIGNAL(triggered()), viewModeMapper, SLOT(map()));
    connect(mineralConcViewAction, SIGNAL(triggered()), viewModeMapper, SLOT(map()));
    connect(planetValueViewAction, SIGNAL(triggered()), viewModeMapper, SLOT(map()));
    connect(populationViewAction, SIGNAL(triggered()), viewModeMapper, SLOT(map()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        fileMenu->addAction(recentFileActs[i]);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    updateRecentFileActions();

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(viewRaceAction);

    turnMenu = menuBar()->addMenu(tr("&Turn"));
    turnMenu->addAction(submitTurnAction);

    commandsMenu = menuBar()->addMenu(tr("&Commands"));
    commandsMenu->addAction(shipDesignAction);
    commandsMenu->addAction(researchAction);
    commandsMenu->addAction(battlePlansAction);
    commandsMenu->addAction(playerRelationsAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    toolBar = addToolBar(tr("View Settings"));
    toolBar->addAction(normalViewAction);
    toolBar->addAction(surfaceMineralsViewAction);
    toolBar->addAction(mineralConcViewAction);
    toolBar->addAction(planetValueViewAction);
    toolBar->addAction(populationViewAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("FreeStars Community", "FreeStars");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("FreeStars Community", "FreeStars");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
#if 0
    if (textEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
#endif
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QRegExp mfiles("\\.m[0-9]+$", Qt::CaseInsensitive);

    if(mfiles.indexIn(fileName) > 1) {
        this->loadPlayerFile(fileName);
    }
    else {
        QMessageBox::warning(this, tr("Application"),
                             tr("Unknown file type"));
        return;
    }
}

void MainWindow::loadPlayerFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    file.close();

    delete TheGame;

    TheGame = new Game;

    try {
        std::string filename_ascii(fileName.toAscii());

        if(!TheGame->LoadPlayerFile(filename_ascii.c_str())) {
            QMessageBox::warning(this, tr("Application"),
                                 tr("Cannot open player file %1")
                                 .arg(fileName));
            return;
        }
    }
    catch (const std::exception &e) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(e.what()));
        return;
    }

    setCurrentFile(fileName);

    closeAllViews();
    openGameView();

    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    //textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll("");

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}

void MainWindow::updateModel() {
}

void MainWindow::openGameView() {
    Player *player = TheGame->GetCurrentPlayer();
    GameView *gameView = new GameView(player, componentPictures.get());
    tabWidget->addTab(gameView, QString("%0 -- %1")
        .arg(strippedName(curFile))
        .arg(player->GetPluralName().c_str()));
}

void MainWindow::closeAllViews() {
    std::list<QWidget*> widgets;

    for(int i = 0 ; i != tabWidget->count() ; i++) {
        widgets.push_back(tabWidget->widget(i));
    }

    tabWidget->clear();

    for(std::list<QWidget*>::iterator i = widgets.begin() ; i != widgets.end() ; i++) {
        delete *i;
    }
}

void MainWindow::activateTab(int index)
{
    if(index >= 0) {
        GameView *gameView = dynamic_cast<GameView*>(tabWidget->widget(index));

        if(gameView != NULL) {
            MapView *mapView = gameView->getMapView();

            viewModeMapper->disconnect();

            submitTurnAction->disconnect();
            shipDesignAction->disconnect();
            researchAction->disconnect();
            battlePlansAction->disconnect();
            playerRelationsAction->disconnect();

            connect(viewRaceAction, SIGNAL(triggered()), gameView, SLOT(viewRaceDialog()));

            connect(viewModeMapper, SIGNAL(mapped(int)), mapView, SLOT(setViewMode(int)));

            connect(submitTurnAction, SIGNAL(triggered()), gameView, SLOT(submitTurn()));
            connect(shipDesignAction, SIGNAL(triggered()), gameView, SLOT(shipDesignDialog()));
            connect(researchAction, SIGNAL(triggered()), gameView, SLOT(researchDialog()));
            connect(battlePlansAction, SIGNAL(triggered()), gameView, SLOT(battlePlansDialog()));
            connect(playerRelationsAction, SIGNAL(triggered()), gameView, SLOT(playerRelationsDialog()));

            viewRaceAction->setEnabled(true);

            viewModeGroup->setEnabled(true);

            submitTurnAction->setEnabled(true);
            shipDesignAction->setEnabled(true);
            researchAction->setEnabled(true);
            battlePlansAction->setEnabled(true);
            playerRelationsAction->setEnabled(true);

            return;
        }
    }

    viewRaceAction->setEnabled(false);

    viewModeGroup->setEnabled(false);

    submitTurnAction->setEnabled(false);
    shipDesignAction->setEnabled(false);
    researchAction->setEnabled(false);
    battlePlansAction->setEnabled(false);
    playerRelationsAction->setEnabled(false);
}

void MainWindow::closeTab(int index)
{
    QWidget *tab = tabWidget->widget(index);
    tabWidget->removeTab(index);
    delete tab;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

};
