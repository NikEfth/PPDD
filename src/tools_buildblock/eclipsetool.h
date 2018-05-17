#ifndef ECLIPSETOOL_H
#define ECLIPSETOOL_H

#include "src/tools_buildblock/tool.h"
#include <qwt_plot_shapeitem.h>

class EclipseTool : public Tool
{
    Q_OBJECT

public:
    EclipseTool(QWidget *parent = 0);

    ~EclipseTool();

    void setPlot(QwtPlot *);

    void unsetPlot();

    virtual void finishedSelecting();

    virtual void clearBeforeUnSet();

public slots:
    void new_Selection(const QRectF &points);

    void setCircle(const bool&);

protected:
    void select( const QPointF & );

    void move( const QPointF & );

    void append(const QPointF &);

    void clearSelection();

    void updateRubberBand();

    QRectF rubberBand;

    QwtPlotShapeItem r;

       QPointF cog;

    bool circle;
};

#endif // ECLIPSETOOL_H
