
/*

  Generic Tool class. All Tools will extend this class.

*/

#include "src/tools_buildblock/tool.h"

#include <qevent.h>

#include "stir/is_null_ptr.h"

#include <QApplication>

Tool::Tool(QWidget *parent)
    : QObject(parent)
{
    c_paintDevice = NULL;
}

Tool::~Tool()
{
    this->deleteLater();
}

QString Tool::get_name() const
{
    return name;
}


bool Tool::eventFilter( QObject *object, QEvent *event )
{
    c_paintDevice = dynamic_cast< QwtPlotCanvas *>(object);

    if (stir::is_null_ptr(c_paintDevice))
        return QObject::eventFilter( object, event );

    switch( event->type() )
    {
    case QEvent::MouseButtonPress:
    {
        QPointF local;
        const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        transformPointFromQTtoDisplay(mouseEvent->pos(), local);
        if(painterPath.contains(local))
            moveSelection(local);
        else
        {
            clearSelection();
            if (checkPointInRange(local))
            {
                select(local);
                return QObject::eventFilter( object, event );
            }
            else
                return QObject::eventFilter( object, event );
        }

        return QObject::eventFilter( object, event );

    }
    case QEvent::MouseMove:
    {
        QPointF local;
        const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        transformPointFromQTtoDisplay(mouseEvent->pos(), local);

        if(painterPath.contains(local))
            moveSelection(local);
        else
        {
            if (checkPointInRange(local))
            {
                 move(local);
                return QObject::eventFilter( object, event );
            }
            else
                return QObject::eventFilter( object, event );
        }
        return QObject::eventFilter( object, event );
    }
    case QEvent::MouseButtonRelease:
    {
        this->finishedSelecting();
    }

    default:
        break;
    }
    return QObject::eventFilter( object, event );
}

void Tool::clearSelection()
{
    if(!stir::is_null_ptr(c_paintDevice))
    {
        for(QwtPlotItem* d : c_paintDevice->plot()->itemList(QwtPlotItem::Rtti_PlotCurve))
        {
            d->detach();
        }
        c_paintDevice->plot()->replot();
    }
}

void Tool::append(const QPointF & ps )
{

}

void Tool::transformPointFromQTtoDisplay(const QPoint& _global, QPointF& _local)
{
    _local.setY(c_paintDevice->plot()->invTransform(QwtPlot::yLeft, _global.y()));
    _local.setX(c_paintDevice->plot()->invTransform(QwtPlot::xBottom, _global.x()));
}

bool Tool::checkPointInRange(const QPointF& _local)
{
    d_spectrogram = dynamic_cast<QwtPlotSpectrogram*> (c_paintDevice->plot()->itemList(QwtPlotItem::Rtti_PlotSpectrogram).first());

    float dy = fabs(d_spectrogram->interval(Qt::YAxis).maxValue() - d_spectrogram->interval(Qt::YAxis).minValue());
    float dx = fabs(d_spectrogram->interval(Qt::XAxis).maxValue()  - d_spectrogram->interval(Qt::XAxis).minValue());
    float cy = (_local.y() + d_spectrogram->interval(Qt::YAxis).minValue()) ;
    float cx = (_local.x() + d_spectrogram->interval(Qt::XAxis).minValue());

    if (!stir::is_null_ptr(d_spectrogram))
    {
        if ( cy<= dy && cy >= 0 &&
            cx<= dx && cx >= 0)
            return false;

        return true;
    }
    return false;
}

void Tool::moveSelection(const QPointF &)
{
    //    // Move the selected point
    //    void RectTool::move( const QPointF &ps )
    //    {
    //        clearSelection();

    //        if (concentic)
    //        {
    //        float d = sqrt((cog.x()  -ps.x()) *  (cog.x()  -ps.x()) + (cog.y()  -ps.y()) *  (cog.y()  -ps.y()));

    //        rubberBand.setTopLeft(QPointF(cog.x() + d, cog.y() + d));
    //        rubberBand.setBottomRight(QPointF(cog.x() - d ,cog.y() -d));
    //        }
    //        else
    //        {
    //            rubberBand.setBottomRight(ps);
    //        }

    this->updateRubberBand();
    //    }
}

void Tool::get_data(QVector<QPointF>& _p, QwtPlot* _plt)
{
    _p = _points;
    _plt =  c_paintDevice->plot();
}
