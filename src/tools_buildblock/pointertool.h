#ifndef POINTERTOOL_H
#define POINTERTOOL_H

#include "src/tools_buildblock/tool.h"
#include <qwt_plot_zoomer.h>

class  PointerTool : public Tool
{
    Q_OBJECT
public:
    PointerTool(QWidget *parent = 0);
    ~PointerTool();

    inline void set_size(const float&, const float&);

    inline void set_width(const float&);

    inline void set_height(const float&);

    void setPlot(QwtPlot *);

    virtual void finishedSelecting();

    virtual void clearBeforeUnSet();

public slots:

    // void DrawSelection(const QRectF &points);

signals:

protected:
    void select( const QPointF & );

    void move( const QPointF & );

    virtual void updateRubberBand(){}

};



#endif // LINETOOL_H


