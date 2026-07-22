/*
 * Copyright (C) 2026 Valery Kholodkov
 */

#ifndef _MESSAGE_WIDGET_H_
#define _MESSAGE_WIDGET_H_

#include <vector>

#include <QDialog>
#include <QButtonGroup>

#include "FSServer.h"

#include "ui_message_widget.h"

namespace FreeStars {

class MessageWidget : public QWidget, public Ui_MessageWidget {
    Q_OBJECT

public:
    MessageWidget(const Player*, QWidget *parent = 0);
    ~MessageWidget();

    void setupMessages(const deque<Message*>&);

signals:
    void selectionChanged(const SpaceObject*);
    void openResearchDialog();

private slots:
    void gotoClicked();
    void prevMessage();
    void nextMessage();

private:
    void displayMessage(const Message&);

private:
    const Player *player;
    std::vector<Message*> messages;
    unsigned currentMessage;
    const SpaceObject *context;
    bool isResearch;
};

};

#endif
