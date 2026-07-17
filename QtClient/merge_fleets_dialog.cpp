/*
 * Copyright (C) 2026 Valery Kholodkov
 */
#include <cmath>

#include <QGuiApplication>

#include "merge_fleets_dialog.h"

namespace FreeStars {

MergeFleetsDialog::MergeFleetsDialog(const Fleet*, QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

MergeFleetsDialog::~MergeFleetsDialog()
{
}

};
