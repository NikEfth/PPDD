#ifndef LISTROISTAT_H
#define LISTROISTAT_H

#include <QWidget>
#include "stir/PixelsOnCartesianGrid.h"

namespace Ui {
class ListROIStat;
}

class ListROIStat : public QWidget
{
    Q_OBJECT

public:
    explicit ListROIStat(QWidget *parent = 0);
    ~ListROIStat();

    bool analyseAll(const stir::PixelsOnCartesianGrid<float> &_w);

    void setData(const stir::PixelsOnCartesianGrid<float>& _w);

    void unSetData();

    float getArea(const stir::PixelsOnCartesianGrid<float> &_w);

    float getMean(const stir::PixelsOnCartesianGrid<float> &_w);

    float getSD(const stir::PixelsOnCartesianGrid<float> &_w);


private:
    Ui::ListROIStat *ui;

    stir::PixelsOnCartesianGrid<float> * data;
};

#endif // LISTROISTAT_H
