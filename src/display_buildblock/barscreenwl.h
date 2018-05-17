#ifndef BARSCREENWL_H
#define BARSCREENWL_H

#include "src/display_buildblock/bar_screen.h"

class BarScreenWL : public BarScreen
{
    Q_OBJECT
public:
    BarScreenWL(QWidget *parent = 0);

    void setWindow(const float& _m);

    void setLevel(const float& _m);

    void initialisePlotArea();

    void move(QPoint to);

    void moveMarkersby(QPointF dis);

    inline float getWindow()const
    {
        return window;
    }

    inline float getLevel() const
    {
        return level;
    }

    void setMinMarker(const float& _m);

    void setMaxMarker(const float& _m);

    virtual bool event( QEvent * );

    inline void setCOG(QPoint _cog);

    inline void resetCOG(int _state = -1);

public slots:
    //! This is called when the data have been changed
    void replot_me();

protected:
    virtual void moveMarkers(const double &lp, const double &up);

    void drawCurve();

    void fullContrast();

    void optimumContrast();

    qreal window, level;
};

#endif // BARSCREENWL_H
