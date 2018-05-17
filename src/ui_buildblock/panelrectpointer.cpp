#include "panelrectpointer.h"
#include "ui_panelrectpointer.h"

PanelRectPointer::PanelRectPointer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelRectPointer)
{
    ui->setupUi(this);
}

PanelRectPointer::~PanelRectPointer()
{
    delete ui;
}

bool PanelRectPointer::selectSquare() const
{
    return ui->radioButton_2->isChecked();
}

bool PanelRectPointer::selectRect() const
{
    return ui->radioButton->isChecked();
}

void PanelRectPointer::on_actionselectionTypeChanged_triggered()
{
    square = selectSquare();
    emit selectionType(square);
}
