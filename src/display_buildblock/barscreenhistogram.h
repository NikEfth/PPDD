#ifndef BARSCREENHISTOGRAM_H
#define BARSCREENHISTOGRAM_H

#include "src/display_buildblock/bar_screen.h"

class BarScreenHistogram : public BarScreen
{
public:
    BarScreenHistogram(QWidget *parent = 0);

    void initialisePlotArea();

    void moveMarkersby(QPointF dis);

    void move(QPoint to);

    virtual bool event( QEvent * );

    void setThreshold(const float& _f);

public slots:
    void replot_me();

protected:
    void drawCurve();

    virtual void moveMarkers(const double &lp, const double &up);
//! Inverse of the width of the window
   float width;
};

#endif // BARSCREENHISTOGRAM_H
