/*

  Tools manager class. Handles the different drawing tools.

*/

#include "ToolManager.h"
#include "src/tools_buildblock/ui_ToolManager.h"

#include "src/tools_buildblock/pointertool.h"
#include "src/tools_buildblock/linetool.h"
#include "src/tools_buildblock/pointercrosstool.h"
#include "src/tools_buildblock/recttool.h"
#include "src/tools_buildblock/eclipsetool.h"

#include "stir/Shape/Box3D.h"
#include "stir/Shape/Shape3D.h"
#include "stir/IndexRange3D.h"

#include "src/display_buildblock/screen_manager_imagedata.h"

ToolManager::ToolManager(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::ToolManager)
{
    ui->setupUi(this);

    sc = 0;

    doAntializing = false;

    mousePointer_sptr.reset(new PointerTool());

    mouseLinePointer_sptr.reset(new LineTool());

    mouseCrossPointer_sptr.reset(new CrossPointerTool());

    mouseRectPointer_sptr.reset(new RectTool());

    mouseEllPointer_sptr.reset(new EclipseTool());

    p_cp = new panel_cross_pointer(this);
    p_lp = new PanelLinePointer(this);
    p_rp = new PanelRectPointer(this);
    p_ep = new PanelEllPointer(this);

    p_cp->setHidden(true);
    p_lp->setHidden(true);
    p_rp->setHidden(true);
    p_ep->setHidden(true);

    ui->verticalLayout_5->addWidget(p_cp);
    ui->verticalLayout_5->addWidget(p_lp);
    ui->verticalLayout_5->addWidget(p_rp);
    ui->verticalLayout_5->addWidget(p_ep);

    ui->chkShowSelection->setHidden(true);

    connect(mouseCrossPointer_sptr.get(), &PointerTool::this_selection, this, &ToolManager::processCursor);
    connect(p_cp, &panel_cross_pointer::select_this_point, this, &ToolManager::setCursor);

    connect(mouseLinePointer_sptr.get(), &LineTool::this_selection, this, &ToolManager::processLine);

    connect(mouseRectPointer_sptr.get(), &LineTool::this_selection, this, &ToolManager::processRect);
    //connect(mouseRectPointer_sptr.get(), &LineTool::refresh_selection, this, &ToolManager::refreshRect);
    connect(p_rp, &PanelRectPointer::selectionType, mouseRectPointer().get(), &RectTool::setSquare);

    connect(p_ep, &PanelEllPointer::selectionType, mouseEllPointer().get(), &EclipseTool::setCircle);

    // Currently deactivated.
    // connect(p_lp, &PanelLinePointer::new_options, this, &ToolManager::refreshLine);
    on_toolButtonPointer_clicked();
}

ToolManager::~ToolManager()
{}


void ToolManager::processCursor(const QVector<QPointF> & _p, QwtPlot* _d)
{
    display_screen_container *d = qobject_cast< display_screen_container*>(_d->parent()->parent());

    if (sc !=0 )
    {
        const Orientation* o = d->getOrientation();

        Viewer::SimpleVector2D<int> _c;
        d->getCursorFromPoint(_p.first(),_c);

        sc->getCurrentCursor()->append(_c, o);

        sc->updateCursor(d);

        _processCursor(*sc->getCurrentCursor());
    }
}

void ToolManager::refreshLine()
{
    QVector<QPointF> d;
    QwtPlot * plt;
    mouseLinePointer_sptr.get()->get_data(d, plt);
    if(d.size() > 0 && plt != 0 )
    {
        processLine(d, plt);
    }
}

void ToolManager::processLine(const QVector<QPointF> & _p, QwtPlot* _d)
{
    display_screen_container * disp = qobject_cast< display_screen_container*>(_d->parent()->parent());
    //display_screen_container *d = sc->getActiveDisplayScreen();

    stir::CartesianCoordinate3D<float> grid = disp->getScreen()->getGridSpacing();
    // It the start and stop points too close ... stop
    if(!(fabs(_p.last().x() - _p.first().x()) > 3*grid.x() ||
         fabs(_p.last().y() - _p.first().y()) > 3*grid.y()))
    {
        disp->updatePlotArea();
        return;
    }

    stir::Bin bins(0,0,0,0);
    stir::ProjMatrixElemsForOneBin preSelectedPixels(bins);
    int numRays = p_lp->getNumRays();

    float xRaysp =0.0;
    float yRaysp =0.0;

    if(numRays >1)
    {
        float slope = (_p.last().y() - _p.first().y())/(_p.last().x() - _p.first().x());
        float cPhi = 1/(1+slope*slope);
        float sPhi = slope/ (1 + slope*slope);
        yRaysp = (grid.x()*cPhi)/ (2*numRays);
        xRaysp = (grid.y() *sPhi)/ (2*numRays);
    }

    stir::CartesianCoordinate3D<float> start_point;
    stir::CartesianCoordinate3D<float> stop_point;

    for (int iRay = 0; iRay< numRays; iRay++)
    {
        stir::ProjMatrixElemsForOneBin tmp(bins);
        float xR = 0.5*xRaysp*(iRay-numRays*0.5);
        float yR = 0.5*yRaysp*(iRay-numRays*0.5);
        if (p_lp->getFidelity())
        {
            stir::CartesianCoordinate3D<float> f_start(0.f,static_cast<float>(_p.first().y()),static_cast<float>( _p.first().x()));
            stir::CartesianCoordinate3D<float> f_stop(0.f,static_cast<float>(_p.last().y()),static_cast<float>( _p.last().x()));

            stir::BasicCoordinate<3,int> i_start = sc->getData_ptr()->get_indices_closest_to_physical_coordinates(f_start);
            stir::BasicCoordinate<3,int> i_stop = sc->getData_ptr()->get_indices_closest_to_physical_coordinates(f_stop);

            stir::CartesianCoordinate3D<float> nf_start = sc->getData_ptr()->get_physical_coordinates_for_indices(i_start);
            stir::CartesianCoordinate3D<float> nf_stop = sc->getData_ptr()->get_physical_coordinates_for_indices(i_stop);

            start_point.z() = 0.5; start_point.y() = (nf_start.y()+yR)/grid.x(); start_point.x() = (nf_start.x()+xR)/grid.y();
            stop_point.z() = 0.5; stop_point.y() = (nf_stop.y()+yR)/grid.x(); stop_point.x() = (nf_stop.x()+xR)/grid.y();
        }
        else
        {
            start_point.z() = 0.5; start_point.y() =(_p.first().y()+yR)/grid.x(); start_point.x() = (_p.first().x()+xR)/grid.y();
            stop_point.z() = 0.5; stop_point.y() = (_p.last().y()+yR)/grid.x(); stop_point.x() = (_p.last().x()+xR)/grid.y();
        }

        stir::RayTraceVoxelsOnCartesianGrid(tmp, start_point, stop_point, grid, 1.f/numRays);
        preSelectedPixels.merge(tmp);
    }

    preSelectedPixels.sort();
    doAntializing = p_lp->getAntialiazing();

    if (ui->chkShowSelection->isChecked())
        disp->getScreen()->writeValuesInData(preSelectedPixels, 1000);
}

void ToolManager::_processCursor(const Viewer::SimpleVector3D<int> & _p)
{
    if (sc != 0)
    {
        double val = sc->getCurrentValue();
        p_cp->print_point(*sc->getCurrentCursor(), val);
    }
}

void ToolManager::refreshRect(const QVector<QPointF> & _p, QwtPlot* _d)
{
    display_screen_container * disp = qobject_cast< display_screen_container*>(_d->parent()->parent());
    //display_screen_container *d = sc->getActiveDisplayScreen();

    stir::CartesianCoordinate3D<float> grid = disp->getScreen()->getGridSpacing();
    // It the start and stop points too close ... stop
    if(!(fabs(_p[2].x() - _p.first().x()) > 3*grid.x() ||
         fabs(_p[2].y() - _p.first().y()) > 3*grid.y()))
    {
        disp->updatePlotArea();
        return;
    }

    //    VoxelsOnCartesianGrid<float> current_image(
    //                stir::IndexRange3D(0,sc->getData_ptr()->get_z_size()-1,
    //                                   -(sc->getData_ptr()->get_y_size()/2),
    //                                   -(sc->getData_ptr()->get_y_size()/2)+sc->getData_ptr()->get_y_size()-1,
    //                                   -(sc->getData_ptr()->get_x_size()/2),
    //                                   -(sc->getData_ptr()->get_x_size()/2)+sc->getData_ptr()->get_x_size()-1),
    //                CartesianCoordinate3D<float>(0,0,0),
    //                grid);
    //    CartesianCoordinate3D<int> numSamples(5,5,5);
//    stir::Box3D *box = new stir::Box3D(sizeX, sizeY, sizeZ, center);
//    box->construct_volume(current_image, numSamples);
//    current_image *= 1000000;


//    if (ui->chkShowSelection->isChecked())
//        disp->getActiveScreen()->
//                writeValuesInData(current_image.get_plane(f.z()));

    CartesianCoordinate3D<int> icurCur = sc->getCurrentCursorCartesianCoordinate();
    stir::CartesianCoordinate3D<float> fcurCur = sc->getData_ptr()->get_physical_coordinates_for_indices(icurCur);

    float minX = std::min(_p[2].x(),_p.first().x() );
    float minY = std::min(_p[2].y(),_p.first().y() );

    const Orientation* o = disp->getOrientation();
    float sizeX = 0.0, sizeY = 0.0, sizeZ = 0.0;

    if(*o == Transverse)
    {
        sizeX = fabs(_p[2].x() - _p.first().x()) ;
        sizeY = fabs(_p[2].y() - _p.first().y()) ;
        sizeZ = grid.z();

        stir::CartesianCoordinate3D<float> center(fcurCur.z(),
                                                  minY + sizeY*0.5,
                                                  minX + sizeX*0.5);
    }
    else if (*o == Coronal)
    {
        sizeX = fabs(_p[2].x() - _p.first().x()) ;
        sizeY =  grid.y();
        sizeZ = fabs(_p[2].y() - _p.first().y());

        stir::CartesianCoordinate3D<float> center(minY + sizeY*0.5,
                                                  fcurCur.y(),
                                                  minX + sizeX*0.5);
    }
    else if (*o == Sagittal)
    {
        sizeX =  grid.x();
        sizeY =  fabs(_p[2].x() - _p.first().x());
        sizeZ = fabs(_p[2].y() - _p.first().y());

        stir::CartesianCoordinate3D<float> center(minY + sizeY*0.5,
                                                  minX + sizeX*0.5,
                                                  fcurCur.x());
    }


}

void ToolManager::processRect(const QVector<QPointF> & _p, QwtPlot* _d)
{
    display_screen_container * disp = qobject_cast< display_screen_container*>(_d->parent()->parent());
    //display_screen_container *d = sc->getActiveDisplayScreen();

    stir::CartesianCoordinate3D<float> grid = disp->getScreen()->getGridSpacing();
    // It the start and stop points too close ... stop
    if(!(fabs(_p[2].x() - _p.first().x()) > 3*grid.x() ||
         fabs(_p[2].y() - _p.first().y()) > 3*grid.y()))
    {
        disp->updatePlotArea();
        return;
    }

    VoxelsOnCartesianGrid<float> current_image(
                stir::IndexRange3D(0,sc->getData_ptr()->get_z_size()-1,
                                   -(sc->getData_ptr()->get_y_size()/2),
                                   -(sc->getData_ptr()->get_y_size()/2)+sc->getData_ptr()->get_y_size()-1,
                                   -(sc->getData_ptr()->get_x_size()/2),
                                   -(sc->getData_ptr()->get_x_size()/2)+sc->getData_ptr()->get_x_size()-1),
                CartesianCoordinate3D<float>(0,0,0),
                grid);

    float minX = std::min(_p[2].x(),_p.first().x() );
    float minY = std::min(_p[2].y(),_p.first().y() );

    float sizeX = fabs(_p[2].x() - _p.first().x()) ;
    float sizeY = fabs(_p[2].y() - _p.first().y()) ;
    float sizeZ = grid.z();
    //
    CartesianCoordinate3D<int> f = sc->getCurrentCursorCartesianCoordinate();

    stir::CartesianCoordinate3D<float> nf_start = current_image.get_physical_coordinates_for_indices(f);

    stir::CartesianCoordinate3D<float> center(nf_start.z(),
                                              minY + sizeY*0.5,
                                              minX + sizeX*0.5);

    CartesianCoordinate3D<int> numSamples(5,5,5);

    stir::Box3D *box = new stir::Box3D(sizeX, sizeY, sizeZ, center);
    box->construct_volume(current_image, numSamples);
    current_image *= 1000000;


    if (ui->chkShowSelection->isChecked())
        disp->getScreen()->
                writeValuesInData(current_image.get_plane(f.z()));
}

void ToolManager::setCursor(const Viewer::SimpleVector3D<int> & _p)
{
    if (sc != 0)
    {
        sc->_setCursor(_p);
        sc->updateCursor();
        _processCursor(*sc->getCurrentCursor());
    }
}

void ToolManager::on_toolButtonPointer_clicked()
{
    clearToolPalette();
    currentTool = mousePointer();
    ui->chkShowSelection->setHidden(true);
    emit(tool_selected(mousePointer()));
}

void ToolManager::on_toolButtonCrossPointer_clicked()
{
    clearToolPalette();
    currentTool = mouseCrossPointer();
    ui->chkShowSelection->setHidden(true);
    emit(tool_selected(mouseCrossPointer()));
    p_cp->setHidden(false);
}

void ToolManager::on_toolButtonLinePointer_clicked()
{
    clearToolPalette();
    currentTool = mouseLinePointer();
    ui->chkShowSelection->setHidden(false);
    emit(tool_selected(mouseLinePointer()));
    p_lp->setHidden(false);
}

void ToolManager::on_toolButtonRectPointer_clicked()
{
    clearToolPalette();
    currentTool = mouseRectPointer();
    ui->chkShowSelection->setHidden(false);
    emit(tool_selected(mouseRectPointer()));
    p_rp->setHidden(false);
}


void ToolManager::on_toolButtonEllPointer_clicked()
{
    clearToolPalette();
    currentTool = mouseEllPointer();
    ui->chkShowSelection->setHidden(false);
    emit(tool_selected(mouseEllPointer()));
    p_ep->setHidden(false);
}


void ToolManager::updateMenus(QWidget * _w)
{
    bool state = (_w != 0);
    ui->toolButtonCrossPointer->setEnabled(state);
    ui->toolButtonLinePointer->setEnabled(state);
    ui->toolButtonPointer->setEnabled(state);
    ui->toolButtonRectPointer->setEnabled(state);
    ui->pg_tools->setEnabled(state);
}

PixelsOnCartesianGrid<float> ToolManager::getSelection() const
{
    return selectedPixels;
}

void ToolManager::on_chkShowSelection_stateChanged(int arg1)
{
    if (arg1 == 0)
        sc->refreshPlots();
}


