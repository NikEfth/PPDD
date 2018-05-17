#include "panelellpointer.h"
#include "ui_panelellpointer.h"

PanelEllPointer::PanelEllPointer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelEllPointer)
{
    ui->setupUi(this);
}

PanelEllPointer::~PanelEllPointer()
{
    delete ui;
}

bool PanelEllPointer::selectEllipse() const
{
    return ui->radioButton->isChecked();
}

bool PanelEllPointer::selectCircle() const
{
    return ui->radioButton_2->isChecked();
}

void PanelEllPointer::on_actionselectionTypeChanged_triggered()
{
    concentric = selectCircle();
    emit selectionType(concentric);
}
