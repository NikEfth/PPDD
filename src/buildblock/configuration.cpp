#include "configuration.h"
#include "ui_configuration.h"

Configuration::Configuration(QWidget *parent) : QWidget(parent)
{
    ui->setupUi(this);
    curColorMap_index = 0;
    numViewPorts = 1;
    showHistogram = Qt::Unchecked;
    viewing_orientation = Transverse;
}
