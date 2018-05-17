#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QWidget>

#include "common.h"

using namespace Viewer;

namespace Ui {
class Configuration;
}

class Configuration : public QWidget, public CommonToAll
{
    Q_OBJECT
public:
    explicit Configuration(QWidget *parent = nullptr);

    qint8 curColorMap_index;
    qint8 numViewPorts;
    bool showHistogram;
    qint8 nBins;

    Orientation viewing_orientation;

private:
    Ui::Configuration *ui;

};

#endif // CONFIGURATION_H
