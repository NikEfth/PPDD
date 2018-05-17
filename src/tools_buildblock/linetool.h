#ifndef LINETOOL_H
#define LINETOOL_H

#include "src/tools_buildblock/tool.h"

class LineTool : public Tool
{
    Q_OBJECT
public:
    LineTool(QWidget *parent = 0);
    ~LineTool();

    void setPlot(QwtPlot *);

    void unsetPlot();

    virtual void finishedSelecting();

    virtual void clearBeforeUnSet();

protected:
    void select(const QPointF & ps);

    void move( const QPointF & );

    void clearSelection();

    void updateRubberBand();

    QwtPlotCurve *curveA;

};

#endif // LINETOOL_H


