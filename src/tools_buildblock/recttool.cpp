
/*

  PointerTool - Selection and cropping.

*/

#include "src/tools_buildblock/recttool.h"

#include <QRectF>
#include <qwt_symbol.h>


RectTool::RectTool(QWidget *parent)
    : Tool(parent)
{
    name = "rect-tool";
    //curveA = new QwtPlotCurve();
    //curveA->setPen( Qt::yellow,1);
    //curveA->setSymbol(new QwtSymbol(QwtSymbol::Rect ,QBrush(Qt::green),QPen(Qt::black),QSize(5,5)));

    r.setPen( Qt::yellow,1);
    square = false;
}

RectTool::~RectTool()
{
    //   r.detach();
}

void RectTool::finishedSelecting()
{
    //    if(checkPointInRange(_points.first()) &&
    //            checkPointInRange(_points.at(1)) &&
    //            checkPointInRange(_points.at(2)) &&
    //            checkPointInRange(_points.last()))
    //        emit this_selection(_points, c_paintDevice->plot());
    //    else
    //        clearSelection();
}

void RectTool::setPlot(QwtPlot *_plot)
{

}

void RectTool::setSquare(const bool& _b)
{
    square = _b;
}

void RectTool::updateRubberBand()
{
    painterPath.addRect(rubberBand);
    r.setShape(painterPath);
    r.attach( c_paintDevice->plot() );
    c_paintDevice->plot()->replot();
    //    emit refresh_selection(_points, c_paintDevice->plot());
}

void RectTool::new_Selection( const QRectF& rect )
{
}

void RectTool::select( const QPointF &ps )
{
    clearSelection();
    if(square)
        cog = ps;

    rubberBand.setTopLeft(ps);
    rubberBand.setWidth(0.0);
    rubberBand.setHeight(0.0);

    updateRubberBand();
}

void RectTool::append( const QPointF &ps )
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
void RectTool::move( const QPointF &ps )
{
    clearSelection();

    if (square)
    {
        float d = sqrt((cog.x()  -ps.x()) *  (cog.x()  -ps.x()) + (cog.y()  -ps.y()) *  (cog.y()  -ps.y()));
        rubberBand.setTopLeft(QPointF(cog.x() + d, cog.y() + d));
        rubberBand.setBottomRight(QPointF(cog.x() - d ,cog.y() -d));
    }
    else
    {
        rubberBand.setBottomRight(ps);
    }

    updateRubberBand();
}

void RectTool::clearSelection()
{
    //if (curveA != NULL)
    painterPath = QPainterPath();
    r.detach();

    //_points.resize(5);
    c_paintDevice->plot()->replot();
}

void RectTool::clearBeforeUnSet()
{
    painterPath = QPainterPath();
    r.detach();
}
