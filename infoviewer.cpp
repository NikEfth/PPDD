#include "infoviewer.h"
#include "ui_infoviewer.h"

InfoViewer::InfoViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoViewer)
{
    ui->setupUi(this);
}

InfoViewer::~InfoViewer()
{
    delete ui;
}
