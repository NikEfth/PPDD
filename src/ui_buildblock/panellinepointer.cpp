#include "panellinepointer.h"
#include "ui_panellinepointer.h"
#include <qglobal.h>

PanelLinePointer::PanelLinePointer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanelLinePointer)
{
    ui->setupUi(this);

    numRays = ui->spinRayNum->value();
    connect(ui->spinRayNum,  static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &PanelLinePointer::updateOptions);
}

PanelLinePointer::~PanelLinePointer()
{
    delete ui;
}

int PanelLinePointer::getNumRays() const
{
    return numRays;
}

bool PanelLinePointer::getFidelity() const
{
    return ui->radioHighFid->isChecked();
}

bool PanelLinePointer::getAntialiazing() const
{
    return ui->chkAntiAlias->isChecked();
}

void PanelLinePointer::updateOptions()
{
//    if(ui->spinRayNum->hasFocus())
    {
        numRays = ui->spinRayNum->value();
    }
    emit new_options();
}
