/*

  Tools manager class. Handles the different drawing tools.

*/


#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <memory>
#include <QWidget>

#include "src/tools_buildblock/tool.h"

#include "src/ui_buildblock/panel_cross_pointer.h"
#include "src/ui_buildblock/panellinepointer.h"
#include "src/ui_buildblock/panelrectpointer.h"
#include "src/ui_buildblock/panelellpointer.h"
#include "src/display_buildblock/screen_manager.h"

#include "stir/PixelsOnCartesianGrid.h"
#include "stir/recon_buildblock/RayTraceVoxelsOnCartesianGrid.h"
#include "stir/recon_buildblock/ProjMatrixElemsForOneBin.h"

class PointerTool;
class RectTool;
class LineTool;
class CrossPointerTool;
class EclipseTool;

namespace Ui {
class ToolManager;
}

class ToolManager : public QWidget
{
    Q_OBJECT
public:

    explicit ToolManager(QWidget *parent = 0);

    ~ToolManager();

    inline std::shared_ptr<PointerTool> mousePointer() const;

    inline std::shared_ptr<LineTool> mouseLinePointer() const;

    inline std::shared_ptr<CrossPointerTool> mouseCrossPointer() const;

    inline std::shared_ptr<RectTool> mouseRectPointer() const;

    inline std::shared_ptr<EclipseTool> mouseEllPointer() const;

    inline std::shared_ptr<Tool>  getCurrentTool() const;

    inline void setScreen(Screen_manager * _s);

    inline void unsetScreen();
    //! Returns the current selection.
    //! If no selection has been made the full slice will return.
    PixelsOnCartesianGrid<float> getSelection() const;

signals:
    void tool_selected(std::shared_ptr<Tool>);

public slots:
    void updateMenus(QWidget* _w = 0);

private slots:

    void on_toolButtonPointer_clicked();

    void on_toolButtonCrossPointer_clicked();

    void on_toolButtonLinePointer_clicked();

    void on_toolButtonRectPointer_clicked();

    void on_toolButtonEllPointer_clicked();

    void processCursor(const QVector<QPointF> &_p, QwtPlot* _d);

    void processLine(const QVector<QPointF> & _p, QwtPlot* _d);

    void processRect(const QVector<QPointF> & _p, QwtPlot* _d);

    void refreshRect(const QVector<QPointF> & _p, QwtPlot* _d);

    void setCursor(const Viewer::SimpleVector3D<int> & _p);

    void _processCursor(const Viewer::SimpleVector3D<int> & _p);

    void on_chkShowSelection_stateChanged(int arg1);

private:

    inline void clearToolPalette();

    inline void unsetTool();
    //! A function to recall the points and the paint device from the memory and
    //! redraw the selection line with (probably) new settings.
    //! \todo This fuction was a bit risky because either I have to save the old paint device, but there
    //! is not guaranttee that the user has not closed the file in the meanwhile. Or call the active screen
    //! manager to send the current active container - which ever that may be -. But this doesnot guarantee
    //! the the points are going to fit anymore. Finally somehow I broke it and I am not sure wether I should
    //! try to fix it.
    void refreshLine();

    Ui::ToolManager *ui;
    //! Control Panel for the mouseCrossPointer_sptr
    panel_cross_pointer * p_cp;
    //! Control Panel for the mouseLinePointer_sptr
    PanelLinePointer *p_lp;
    //! Control Panel for the mouseRectPointer_sptr
    PanelRectPointer *p_rp;

    PanelEllPointer *p_ep;

    std::shared_ptr<PointerTool> mousePointer_sptr;

    std::shared_ptr<CrossPointerTool> mouseCrossPointer_sptr;

    std::shared_ptr<LineTool> mouseLinePointer_sptr;

    std::shared_ptr<RectTool> mouseRectPointer_sptr;

    std::shared_ptr<EclipseTool> mouseEllPointer_sptr;

    std::shared_ptr<Tool> currentTool;

    PixelsOnCartesianGrid<float> selectedPixels;

    //! Circular and line selection can benefit from this option.
    //! In the case of line selections the antialiazing is done by keeping
    //! the ray tracers propabilities.
    bool doAntializing;

protected:
    Screen_manager* sc;
};

#include "src/tools_buildblock/ToolManager.inl"

#endif // TOOLMANAGER_H
