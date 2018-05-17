#ifndef DISPLAY_SCREEN_CONTAINER_H
#define DISPLAY_SCREEN_CONTAINER_H

#include <QWidget>
#include <memory>

#include "src/buildblock/common.h"

#include "src/tools_buildblock/tool.h"

#include "src/display_buildblock/display_screen_raster.h"

#include "stir/DiscretisedDensity.h"
#include "stir/DiscretisedDensityOnCartesianGrid.h"

namespace Ui {
class display_screen_container;
}

using namespace stir;
using namespace Viewer;

//! An interaction interface between screen_manager and display_screen.
//! It will sort out sizes, Orientation and scrolling between differetn positions
class display_screen_container : public QWidget
{
    Q_OBJECT

public:
    explicit display_screen_container(QWidget *parent = 0,
                                      SimpleVector3D<int> *masterCur = 0);
    ~display_screen_container();

    void setViewOrientation(const Orientation& _o);

    void updatePlotArea();

    std::shared_ptr<display_screen_raster> getScreen() const;

    void actuallyScroll(const int& value);

    inline qint16 getColorMapIndex() const
    { return currentColorMap; }

    inline void setDiscretisedDensity_sptr(std::shared_ptr<DiscretisedDensity<3,float> > _d)
    {
        density_sptr = _d;
        initialiseSizes();
    }

    inline const Orientation* getOrientation() const
    {
        return &currentOrientation;
    }

    inline void setAddressAtParent(const qint32& id)
    {
        myAddress = id;
    }

    virtual void mousePressEvent(QMouseEvent *event);

    inline void set_active(bool state)
    {
        active = state;
        my_screen->set_active(state);
    }

    std::shared_ptr<display_screen_raster> my_screen;

    inline void showCursor(const bool& _s, const SimpleVector2D<int>& _cur)
    {
        my_screen->drawCursor(_s, _cur);
    }

    inline Viewer::SimpleVector2D<int> showCursor(const bool& _s, const QPoint& _cur)
    {
        return my_screen->drawCursor(_s, _cur);
    }

    inline void getCursorFromPoint(const QPointF& _p, Viewer::SimpleVector2D<int>& _c)
    {
        my_screen->cellFromXY(_p,_c);
    }

    inline int getMinPos() const
    {
        return *minScrolPos;
    }

    inline int getMaxPos() const
    {
        return *maxScrolPos;
    }

    inline int getCurPos() const
    {
        return *currentCur;
    }

    //! A light function to trigger the display_screen::replot_me() function.
    void refreshPlotArea();

signals:

    void activateScreenContainer(const qint32& );

    void plotAreaUpdated();

    void scrolled();

public slots:

    void setColorMap(const qint16&);

private slots:

    void setActiveScreen(const qint32 &_a);

    void activeScreenContainer(QWidget* _a);

    void on_posSelect_scrbr_valueChanged(int value);

private:

    void initialiseSizes();

    void initialisePlotArea();

    void setUpPlotArea();

    void setUpControls();

    bool active;

    qint16 myAddress;

    QString numPagesPrint;

    Ui::display_screen_container *ui;

    std::shared_ptr<Tool> currentTool;

    //!
    //! \brief curColorMap
    //! The colormap used by the viewports in this widget.
    //! As on certain occations the object of the viewports are deleted,
    //! storing this here makes the experience smoother.
    qint16 currentColorMap;

    bool hasSetUp;

    std::shared_ptr<DiscretisedDensity<3,float> > density_sptr;

    SimpleVector3D<int> iMin, iMax, iRange, *iCur;

    SimpleVector3D<float> fSpacing, fOrigin, fRange;

    QString locd, rocd, tocd, bocd;

    Orientation currentOrientation;

    int* minScrolPos, *maxScrolPos;

    int* numRow, *numCol, *minX, *maxX, *minY, *maxY, *currentCur;

    float *hintRow, *hintCol, *minXAxis, *maxXAxis, *minYAxis, *maxYAxis;
};

#endif // DISPLAY_SCREEN_CONTAINER_H
