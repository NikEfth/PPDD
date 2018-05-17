#ifndef POINTERCROSSTOOL_H
#define POINTERCROSSTOOL_H

#include <QVector3D>

#include "src/tools_buildblock/tool.h"
class CrossPointerToolPrivate;

class CrossPointerTool : public Tool
{
    Q_OBJECT
public:
    CrossPointerTool(QWidget *parent = 0);

    ~CrossPointerTool();

    virtual void finishedSelecting();

    virtual void clearBeforeUnSet();

signals:
    void this_location(const QVector3D& ps, const float& v);

protected:

    QwtPlotCurve* curveA, *curveB;

    void select(const QPointF &ps);

    void move( const QPointF & );

    void clearSelection();

    virtual void updateRubberBand()
    {}

};

#endif // POINTERCROSSTOOL_H


