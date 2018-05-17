 
/*

  PointerTool - Selection and cropping.

*/

#include "src/tools_buildblock/pointertool.h"

PointerTool::PointerTool(QWidget *parent)
    : Tool(parent)
{
    name = "pointer";
}

PointerTool::~PointerTool()
{}


void PointerTool::setPlot(QwtPlot *_plot)
{
}

void PointerTool::finishedSelecting()
{

}


void PointerTool::select( const QPointF &pos )
{

}

// Move the selected point
void PointerTool::move(const QPointF &pos )
{

}

void PointerTool::clearBeforeUnSet()
{

}
