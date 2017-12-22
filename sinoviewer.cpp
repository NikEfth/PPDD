#include "sinoviewer.h"
#include "ui_sinoviewer.h"

sinoviewer::sinoviewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::sinoviewer)
{
    ui->setupUi(this);
}

sinoviewer::~sinoviewer()
{
    delete ui;
}
