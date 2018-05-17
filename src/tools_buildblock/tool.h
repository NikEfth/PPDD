#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_spectrogram.h>

#include "src/buildblock/common.h"
#include "src/display_buildblock/display_screen_raster.h"

/*!
 * This is the base class for all paint tools
 */
class Tool:public QObject
{
    Q_OBJECT
public:
    explicit Tool(QWidget *parent = 0);
    ~Tool();

    virtual bool eventFilter(QObject *, QEvent *);

    virtual QString get_name() const;

    virtual void finishedSelecting() = 0;

    void get_data(QVector<QPointF> &, QwtPlot*);
    //! Cal this function to clean before unsetting a tool.
    //! It will try to tight up the screen.
    virtual void clearBeforeUnSet() = 0;

signals:

    void this_selection(const QVector<QPointF>&, QwtPlot*);

    void refresh_selection(const QVector<QPointF>&, QwtPlot*);

protected:

    //! Remove all curves from the c_paintDevice.
    virtual void clearSelection();

    QString name;

    QwtPlotCanvas *c_paintDevice;
    //! This is a preliminiry check to see if the point is within the
    //! axis range. It has been proven somewhat week condition, as in
    //! many cases it fails to provide realible results.
    bool checkPointInRange(const QPointF &_local);

    QwtPlotSpectrogram* d_spectrogram;
    //! On mouse press action.
    virtual void select( const QPointF & ) = 0;
    //! On mouse press and drag action.
    virtual void move( const QPointF & ) = 0;

    void moveSelection(const QPointF& );

    virtual void append(const QPointF &);
    //! This vector holds the characteristic points of the selected ROI.
    //! The CrossPointerTool has just one point.
    //! The LineTool has two points (e.g. start and stop) e.t.c
    QVector<QPointF> _points;

     QPainterPath painterPath;

     virtual void updateRubberBand() = 0;

private:
    //! Transforms QT coordinates from the QWidget coordinate system, to
    //! the system of the display screen. For more information on the
    //! QT coordinate sytem look in the API.
    void transformPointFromQTtoDisplay(const QPoint& _global, QPointF &_local);

};



#endif // TOOL_H


