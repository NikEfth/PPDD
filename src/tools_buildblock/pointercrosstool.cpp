
/*

  PointerTool - Selection and cropping.

*/

#include "src/tools_buildblock/pointercrosstool.h"

#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QMenu>

#define MY_DATA sc->getActiveDisplayScreen()->getActiveScreen()

#define MY_DATA_CONT sc->getActiveDisplayScreen()

CrossPointerTool::CrossPointerTool(QWidget *parent)
    : Tool(parent)
{
    name = "cross-pointer";
    curveA = NULL;
    curveB = NULL;
}

CrossPointerTool::~CrossPointerTool()
{
}

void CrossPointerTool::finishedSelecting()
{

}

void CrossPointerTool::select(const QPointF &ps )
{
    Tool::clearSelection();
    _points.clear();

    curveA = new QwtPlotCurve();
    curveA->setPen( Qt::yellow,1,Qt::DotLine );

    curveB = new QwtPlotCurve();
    curveB->setPen( Qt::yellow,1,Qt::DotLine );

    double x[2];
    double y[2];

    x[0] = ps.x();
    y[0] = d_spectrogram->interval(Qt::YAxis).minValue();
    x[1] = ps.x();
    y[1] = d_spectrogram->interval(Qt::YAxis).maxValue();

    curveA->setSamples( x, y, 2 );
    curveA->attach( c_paintDevice->plot() );

    x[0] = d_spectrogram->interval(Qt::XAxis).minValue();
    y[0] = ps.y();
    x[1] = d_spectrogram->interval(Qt::XAxis).maxValue();
    y[1] = ps.y();

    curveB->setSamples( x, y, 2 );
    curveB->attach( c_paintDevice->plot() );

    c_paintDevice->plot()->replot();

    _points.append(ps);

    emit this_selection(_points, c_paintDevice->plot());
}

// Move the selected point
void CrossPointerTool::move(const QPointF &pos )
{
    _points.clear();
    clearSelection();
    select(pos);
}

void CrossPointerTool::clearSelection()
{
    if (curveA != NULL)
        curveA->detach();
    if (curveB != NULL)
        curveB->detach();

    c_paintDevice->plot()->replot();
}

void CrossPointerTool::clearBeforeUnSet()
{
    if (curveA != NULL)
        curveA->detach();
    if (curveB != NULL)
        curveB->detach();
}
