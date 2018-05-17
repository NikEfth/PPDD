#include "eclipsetool.h"
#include <qwt_symbol.h>

EclipseTool::EclipseTool(QWidget *parent)
    : Tool(parent)
{
    name = "ecl-tool";

    r.setPen( Qt::yellow,1);
}


EclipseTool::~EclipseTool()
{
    //   r.detach();
}

void EclipseTool::finishedSelecting()
{
    //    if(checkPointInRange(_points.first()) &&
    //            checkPointInRange(_points.at(1)) &&
    //            checkPointInRange(_points.at(2)) &&
    //            checkPointInRange(_points.last()))
    //        emit this_selection(_points, c_paintDevice->plot());
    //    else
    //        clearSelection();
}

void EclipseTool::setPlot(QwtPlot *_plot)
{

}

void EclipseTool::setCircle(const bool& _c)
{
    circle =_c;
}


void EclipseTool::updateRubberBand()
{
    painterPath.addEllipse(rubberBand);
    r.setShape(painterPath);
    r.attach( c_paintDevice->plot() );
    c_paintDevice->plot()->replot();
    //    emit refresh_selection(_points, c_paintDevice->plot());
}

void EclipseTool::new_Selection( const QRectF& rect )
{
}

void EclipseTool::select( const QPointF &ps )
{
    clearSelection();
    if(circle)
        cog = ps;
    rubberBand.setTopLeft(ps);
    rubberBand.setWidth(0.0);
    rubberBand.setHeight(0.0);

    updateRubberBand();
}

void EclipseTool::append( const QPointF &ps )
{
    //    clearSelection();
    //    _points[0] = ps;
    //    _points[1] = ps;
    //    _points[2] = ps;
    //    _points[3] = ps;
    //    _points[4] = ps;
    //    updateRubberBand();
}

// Move the selected point
void EclipseTool::move( const QPointF &ps )
{
    clearSelection();
    rubberBand.setBottomRight(ps);
    if(circle)
    {
        float d = sqrt((cog.x()  -ps.x()) *  (cog.x()  -ps.x()) + (cog.y()  -ps.y()) *  (cog.y()  -ps.y()));
        rubberBand.setTopLeft(QPointF(cog.x() + d, cog.y() + d));
        rubberBand.setBottomRight(QPointF(cog.x() - d ,cog.y() -d));
    }
    else
        rubberBand.setBottomRight(ps);


    updateRubberBand();
}

void EclipseTool::clearSelection()
{
    //if (curveA != NULL)
    painterPath = QPainterPath();
    r.detach();

    //_points.resize(5);
    c_paintDevice->plot()->replot();
}

void EclipseTool::clearBeforeUnSet()
{
    painterPath = QPainterPath();
    r.detach();
}
