#ifndef RECTTOOL_H
#define RECTTOOL_H

#include "src/tools_buildblock/tool.h"
#include <qwt_plot_shapeitem.h>
#include <QRectF>

class RectTool : public Tool
{
    Q_OBJECT
public:
    RectTool(QWidget *parent = 0);
    ~RectTool();

    void setPlot(QwtPlot *);

    void unsetPlot();

    virtual void finishedSelecting();

    virtual void clearBeforeUnSet();

public slots:
    void new_Selection(const QRectF &points);

    void setSquare(const bool& );

protected:
    void select( const QPointF & );

    void move( const QPointF & );

    void append(const QPointF &);

    void clearSelection();

    virtual void updateRubberBand();

    QRectF rubberBand;

    QwtPlotShapeItem r;

    QPointF cog;

    bool square;

};

#endif // RECTTOOL_H


