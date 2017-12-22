#include "viewmode.h"
#include "ui_viewmode.h"

ViewMode::ViewMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewMode)
{
    ui->setupUi(this);
}

ViewMode::~ViewMode()
{
    delete ui;
}
