/*
 * Copyright (C) 2026 Valery Kholodkov
 */
#include <cmath>

#include <QGuiApplication>

#include "message_widget.h"

#include "message_formatter.h"
#include "message_location_extractor.h"

namespace FreeStars {

MessageWidget::MessageWidget(const Player *_player, QWidget *parent)
    : QWidget(parent)
    , player(_player)
    , currentMessage(0)
    , context(nullptr)
{
    setupUi(this);

    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextMessage()));
    connect(gotoButton, SIGNAL(clicked()), this, SLOT(gotoClicked()));
    connect(prevButton, SIGNAL(clicked()), this, SLOT(prevMessage()));
}

MessageWidget::~MessageWidget()
{
}

void MessageWidget::setupMessages(const deque<Message*> &_messages) {
    messages.reserve(_messages.size());

    for(deque<Message *>::const_iterator i = _messages.begin() ; i != _messages.end() ; i++) {
        messages.push_back(*i);
    }

    currentMessage = 0;

    displayMessage(*messages[currentMessage]);
}

void MessageWidget::displayMessage(const Message &_message) {
    MessageFormatter messageFormatter(player);
    _message.ApplyVisitor(messageFormatter);
    messagePane->setText(messageFormatter.toString());
    titleLabel->setText(tr("Year %0   Messages: %1 of %2")
        .arg(player->GetGame()->GetTurn())
        .arg(currentMessage + 1)
        .arg(messages.size()));

    nextButton->setEnabled(currentMessage < messages.size()-1);
    gotoButton->setEnabled(false);
    prevButton->setEnabled(currentMessage > 0);

    context = nullptr;

    MessageLocationExtractor messageLocationExtractor(player, [&](const Location *loc) {
        context = dynamic_cast<const SpaceObject*>(loc);

        if(context != NULL) {
            gotoButton->setEnabled(true);
            return;
        }
    });

    _message.ApplyVisitor(messageLocationExtractor);

    isResearch = messageLocationExtractor.IsResearch();

    if(isResearch) {
        gotoButton->setEnabled(true);
    }
}

void MessageWidget::gotoClicked()
{
    if(context != NULL) {
        emit selectionChanged(context);
    }
    else {
        if(isResearch) {
            emit openResearchDialog();
        }
    }
}

void MessageWidget::nextMessage()
{
    if(currentMessage < messages.size()-1) {
        currentMessage++;
        displayMessage(*messages[currentMessage]);
    }
}

void MessageWidget::prevMessage()
{
    if(currentMessage > 0) {
        currentMessage--;
        displayMessage(*messages[currentMessage]);
    }
}


};
