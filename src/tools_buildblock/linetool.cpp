
/*

  PointerTool - Selection and cropping.

*/

#include "src/tools_buildblock/linetool.h"
#include <qwt_symbol.h>


LineTool::LineTool(QWidget *parent)
    : Tool(parent)
{
    name = "line-pointer";
    curveA = new QwtPlotCurve();
    curveA->setPen( Qt::yellow,1);
    curveA->setSymbol(new QwtSymbol(QwtSymbol::Rect ,QBrush(Qt::green),QPen(Qt::black),QSize(5,5)));
}

LineTool::~LineTool()
{
}

void LineTool::setPlot(QwtPlot *_plot)
{
}

void LineTool::finishedSelecting()
{
    if(checkPointInRange(_points.first()) &&
            checkPointInRange(_points.last()))
        emit this_selection(_points, c_paintDevice->plot());
    else
        clearSelection();
}

void LineTool::select( const QPointF &ps )
{
    clearSelection();
    _points[0] = ps;
    _points[1] = ps;
    updateRubberBand();
}

void LineTool::updateRubberBand()
{
        curveA->setSamples(_points);
        curveA->attach( c_paintDevice->plot() );
        c_paintDevice->plot()->replot();
}

// Move the selected point
void LineTool::move( const QPointF &ps )
{
    _points[1] = ps;
    updateRubberBand();
}

void LineTool::clearSelection()
{
    if (curveA != NULL)
        curveA->detach();

    _points.resize(2);
    c_paintDevice->plot()->replot();
}

void LineTool::clearBeforeUnSet()
{
    if (curveA != NULL)
        curveA->detach();
}


