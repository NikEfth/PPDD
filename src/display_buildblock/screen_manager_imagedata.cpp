#include "screen_manager_imagedata.h"
#include "ui_screen_manager.h"

#include "stir/PixelsOnCartesianGrid.h"
#include "stir/IO/ITKImageInputFileFormat.h"
#include "stir/IO/InputFileFormat.h"
#include "stir/IO/FileSignature.h"
#include "stir/BasicCoordinate.h"
#include <QDir>
#include <cmath>

Screen_manager_ImageData::Screen_manager_ImageData(Configuration *curConfig, QWidget *parent):
    Screen_manager(curConfig, parent)
{
    cur_pos_num = 0;
}

Screen_manager_ImageData::Screen_manager_ImageData(VoxelsOnCartesianGrid<float>* image, Configuration *curConfig, QWidget *parent):
    Screen_manager(curConfig, parent)
{
    cur_pos_num = 0;

    this->density_sptr.reset(image->clone());

    if (is_null_ptr(density_sptr))
    {
        QMessageBox::critical(this, tr("Error reading Image Data"),
                              tr("Unable to load Image Data."),
                              QMessageBox::Ok);
        return;
    }

    afterLoading();
}


QString Screen_manager_ImageData::getDataType() const
{return QString("imageData");}

bool Screen_manager_ImageData::loadFile(const QString fileName)
{

    QString suffix = QFileInfo(fileName).suffix();

    VoxelsOnCartesianGrid<float> * dd = NULL;

    if (suffix == "hv")
    {
        dd = read_interfile_image(fileName.toStdString());

        if (is_null_ptr(dd))
        {
            QMessageBox::critical(this, tr("Error reading Image Data"),
                                  tr("Unable to load Image Data."),
                                  QMessageBox::Abort);
            return false;
        }

        this->density_sptr.reset(dd->clone());
        this_input = interfile;
    }
    else
    {
        FileSignature f(fileName.toStdString());

        InputFileFormat<DiscretisedDensity<3, float> >* d = new ITKImageInputFileFormat();
        if (d->can_read(f, fileName.toStdString()))
        {

            std::unique_ptr<DiscretisedDensity<3, float> > ddu = d->read_from_file(fileName.toStdString());

            dd = dynamic_cast<VoxelsOnCartesianGrid<float> * > (ddu.get());

            if (is_null_ptr(dd))
            {
                QMessageBox::critical(this, tr("Error reading Image Data"),
                                      tr("Unable to load Image Data."),
                                      QMessageBox::Abort);
                return false;
            }

            this->density_sptr = std::move(ddu);
            this_input = dicom;
        }
    }

    if (is_null_ptr(density_sptr))
    {
        QMessageBox::critical(this, tr("Error reading Image Data"),
                              tr("Unable to load Image Data."),
                              QMessageBox::Ok);
        return false;
    }

    myFullFileName = fileName;

    return afterLoading();
}

bool Screen_manager_ImageData::afterLoading()
{
    setWindowTitle(getFileName());
    setUpContainers(3);
    initialisePlotArea();
    setOrientation(All);
    setActiveScreenContainer(0);
    find_min_value(-1);
    find_max_value(-1);
    applyVizValuesToAllContainers(minValueAll, maxValueAll);

    return true;
}

void Screen_manager_ImageData::display()
{
    activeScreenContrainer->updatePlotArea();
}

void Screen_manager_ImageData::initialisePlotArea()
{
    if(containersReady)
    {
        my_containers.at(0)->setDiscretisedDensity_sptr(density_sptr);
        my_containers.at(0)->setViewOrientation(Transverse);
        my_containers.at(0)->updatePlotArea();
        my_containers.at(0)->setHidden(true);

        my_containers.at(1)->setDiscretisedDensity_sptr(density_sptr);
        my_containers.at(1)->setViewOrientation(Coronal);
        my_containers.at(1)->updatePlotArea();
        my_containers.at(1)->setHidden(true);

        my_containers.at(2)->setDiscretisedDensity_sptr(density_sptr);
        my_containers.at(2)->setViewOrientation(Sagittal);
        my_containers.at(2)->updatePlotArea();
        my_containers.at(2)->setHidden(true);
    }
}


void Screen_manager_ImageData::initialise_controls_for_view()
{

}

void Screen_manager_ImageData::showCursor(const bool &state)
{
    if (containersReady)
    {
        cursorStatus = state;
        if(!my_containers.at(0)->isHidden())
            my_containers.at(0)->showCursor(cursorStatus, currentCursor->getCross(Transverse));
        if(!my_containers.at(1)->isHidden())
            my_containers.at(1)->showCursor(cursorStatus, currentCursor->getCross(Coronal));
        if(!my_containers.at(2)->isHidden())
            my_containers.at(2)->showCursor(cursorStatus, currentCursor->getCross(Sagittal));
    }
}

void Screen_manager_ImageData::_setCursor(const QPoint& _p, QwtPlot* _d)
{
    display_screen_container *d = qobject_cast< display_screen_container*>(_d->parent()->parent());

    if (d != 0)
    {
        const Orientation* o = d->getOrientation();

        Viewer::SimpleVector2D<int> _c;
        d->getCursorFromPoint(_p,_c);

        currentCursor->append(_c,o );
        showCursor(true);
        updateCursor(d);
    }
}

void Screen_manager_ImageData::_setCursor(const Viewer::SimpleVector3D<int>& _c)
{
    if (my_containers.at(0)->getMinPos() < _c.z &&
            my_containers.at(0)->getMaxPos() > _c.z &&
            my_containers.at(1)->getMinPos() < _c.y &&
            my_containers.at(1)->getMaxPos() > _c.y &&
            my_containers.at(2)->getMinPos() < _c.x &&
            my_containers.at(2)->getMaxPos() > _c.x)
    {
        *currentCursor = _c;
        showCursor(true);
        updateCursor();
    }
}

void Screen_manager_ImageData::applyVizValuesToAllContainers(const double &_min, const double &_max)
{
    if (containersReady)
    {
        my_containers.at(0)->getScreen()->setInterval(_min, _max);
        my_containers.at(1)->getScreen()->setInterval(_min, _max);
        my_containers.at(2)->getScreen()->setInterval(_min, _max);
        refreshPlots();
    }
}

void Screen_manager_ImageData::applyFunctionToAllContainers(Worker* _w)
{
    if (containersReady)
    {
        my_containers.at(0)->getScreen()->applyFunction(_w);
        my_containers.at(1)->getScreen()->applyFunction(_w);
        my_containers.at(2)->getScreen()->applyFunction(_w);
        refreshPlots();
    }
}

void Screen_manager_ImageData::removeFunctionToAllContainers()
{
    if (containersReady)
    {
        my_containers.at(0)->getScreen()->removeFunction();
        my_containers.at(1)->getScreen()->removeFunction();
        my_containers.at(2)->getScreen()->removeFunction();
        refreshPlots();
    }
}

CartesianCoordinate3D<float>
Screen_manager_ImageData::getCurrentCursorInmm()
{
    return density_sptr->get_physical_coordinates_for_indices(currentCursor->getCartesianCoordinate());
}

void Screen_manager_ImageData::updateCursor(display_screen_container* d)
{
    if ( /*d != my_containers.at(0).get() &&*/ !my_containers.at(0)->isHidden())
    {
        my_containers.at(0)->showCursor(cursorStatus, currentCursor->getCross(Transverse));
        my_containers.at(0)->actuallyScroll(currentCursor->getPosition(Transverse).x);
    }
    if ( /*d != my_containers.at(1).get() &&*/ !my_containers.at(1)->isHidden())
    {
        my_containers.at(1)->showCursor(cursorStatus, currentCursor->getCross(Coronal));
        my_containers.at(1)->actuallyScroll(currentCursor->getPosition(Coronal).x);
    }
    if ( /*d != my_containers.at(2).get()  &&*/ !my_containers.at(2)->isHidden())
    {
        my_containers.at(2)->showCursor(cursorStatus, currentCursor->getCross(Sagittal));
        my_containers.at(2)->actuallyScroll(currentCursor->getPosition(Sagittal).x);
    }
}

QSize Screen_manager_ImageData::setOrientation(Orientation _o)
{
    int total_containers = (_o == All ? 3 : 1);

    QSize q = this->size();

    q.setWidth(15);

    if (_o != All)
    {
        if (_o == Next)
        {
            viewOrder = static_cast<Orientation>(static_cast<int>(viewOrder) + 1);
            if (viewOrder == Next)
                viewOrder = Transverse;
        }
        else
            viewOrder = _o;

        if (_o == Transverse)
        {
            setActiveScreenContainer(0);
            my_containers.at(0)->setHidden(false);
            my_containers.at(1)->setHidden(true);
            my_containers.at(2)->setHidden(true);
        }
        else if (_o == Coronal)
        {
            setActiveScreenContainer(1);
            my_containers.at(0)->setHidden(true);
            my_containers.at(1)->setHidden(false);
            my_containers.at(2)->setHidden(true);
        }
        else
        {
            setActiveScreenContainer(2);
            my_containers.at(0)->setHidden(true);
            my_containers.at(1)->setHidden(true);
            my_containers.at(2)->setHidden(false);
        }

        QSize w = activeScreenContrainer->size();

        q.setWidth(q.width() + w.width());
        this->adjustSize();
        return q;
    }
    else if (_o == All)
    {
        viewOrder = All;
        {
            my_containers.at(0)->setHidden(false);
            QSize w = my_containers.at(0)->size();
            q.setWidth(q.width() + w.width());
        }

        {
            my_containers.at(1)->setHidden(false);
            QSize w = my_containers.at(1)->size();
            q.setWidth(q.width() + w.width());
        }

        {
            my_containers.at(2)->setHidden(false);
            QSize w = my_containers.at(2)->size();
            q.setWidth(q.width() + w.width());
        }

        this->adjustSize();
        return q;
    }
}

double Screen_manager_ImageData::getValueAt(const SimpleVector3D<int> _c)
{
    IndexRange<3> limits = get_IndexRange();

    float dz = fabs(limits.get_max_index() - limits.get_min_index());
    float dy = fabs(limits[0].get_max_index() - limits[0].get_min_index());
    float dx = fabs(limits[0][0].get_max_index() - limits[0][0].get_min_index());
    float cz = (_c.z  - limits.get_min_index() ) ;
    float cy = (_c.y - limits[0].get_min_index());
    float cx = (_c.x- limits[0][0].get_min_index() ) ;
    if ( cz <= dz && cz > 0 &&
              cy<= dy && cy > 0 &&
             cx<= dx && cx > 0
            )
    {
        BasicCoordinate<3,int> coord = make_coordinate(_c.z, _c.y, _c.x);

        return static_cast<double>((*density_sptr)[coord]);
    }
}

bool Screen_manager_ImageData::setState(qint16 newViewOrder,
                                        qint16 new_current_id,
                                        qint16 new_num_viewports)
{
    //    has_set_up = false;

    //    viewOrder = newViewOrder;

    //    remote_scrbr_valueChanged(new_current_id);

    //    if (num_viewports != new_num_viewports)
    //        initialise_plot_area(new_num_viewports);

    //    initialise_controls_for_view();
    //    set_limits();
    //    my_displays.clear();
    //    set_up_plot_area();
    //    draw_plot_area();

    //    if(this->hasHistograms)
    //        this->draw_histograms();

    //    return true;
}

void Screen_manager_ImageData::writeToDisk(const QString & _p)
{
    QFileInfo fi(_p);
    QFileInfo ffi(this->getFullFilePath());

    QString fileNameToSave =ffi.baseName();

//    if (fi.isFile())
//    {
        fileNameToSave.prepend(fi.absoluteFilePath()+QDir::separator());
//    }
//    else
//    {
//        fileNameToSave = QDir::cleanPath(_p + QDir::separator() + getFileName());
//    }

    setFullFilePath(fileNameToSave);

    write_basic_interfile(fileNameToSave.toStdString() + ".hv",
                          *density_sptr.get());

}

void Screen_manager_ImageData::save_as_array(int this_label)
{
    //! TODO: Save as array
    int nikos  =0;
}

void Screen_manager_ImageData::onCursorChanged()
{
    display_screen_container* d = qobject_cast < display_screen_container*> (sender());
    if (cursorStatus && d)
    {
        if (d != my_containers.at(0).get())
            my_containers.at(0)->showCursor(cursorStatus, currentCursor->getCross(Transverse));
        if (d != my_containers.at(1).get())
            my_containers.at(1)->showCursor(cursorStatus, currentCursor->getCross(Coronal));
        if (d != my_containers.at(2).get())
            my_containers.at(2)->showCursor(cursorStatus, currentCursor->getCross(Sagittal));
    }
}

IndexRange<3>
Screen_manager_ImageData::get_IndexRange() const
{
    return density_sptr->get_index_range();
}

void Screen_manager_ImageData::find_min_value(const int& sliceNum,  Orientation _o)
{
    CartesianCoordinate3D<int> coord = make_coordinate(0,0,0);
    float retVal = (*density_sptr)[coord];

    if(_o == All)
    {
        //        for(coord[1]=my_containers.at(0)->getMinPos() ;coord[1]<= my_containers.at(0)->getMaxPos();++coord[1])
        //            for(coord[2]=my_containers.at(1)->getMinPos() ;coord[2]<=my_containers.at(1)->getMaxPos();++coord[2])
        //                for(coord[3]=my_containers.at(2)->getMinPos();coord[3]<=my_containers.at(2)->getMaxPos();++coord[3])
        //                {
        //                    if ((*density_sptr)[coord] < retVal)
        //                        retVal = (*density_sptr)[coord];
        //                }
        retVal = density_sptr->find_min();
    }
    else if (_o == Transverse)
    {
        coord[1]=sliceNum;

        for(coord[2]=my_containers.at(1)->getMinPos() ;coord[2]<=my_containers.at(1)->getMaxPos();++coord[2])
            for(coord[3]=my_containers.at(2)->getMinPos();coord[3]<=my_containers.at(2)->getMaxPos();++coord[3])
            {
                if ((*density_sptr)[coord] < retVal)
                    retVal = (*density_sptr)[coord];
            }
    }
    else if (_o == Coronal)
    {
        coord[2] = sliceNum;
        for(coord[1]=my_containers.at(0)->getMinPos() ;coord[1]<= my_containers.at(0)->getMaxPos();++coord[1])
            for(coord[3]=my_containers.at(2)->getMinPos();coord[3]<=my_containers.at(2)->getMaxPos();++coord[3])
            {
                if ((*density_sptr)[coord] < retVal)
                    retVal = (*density_sptr)[coord];
            }
    }
    else
    {
        coord[3] = sliceNum;
        for(coord[1]=my_containers.at(0)->getMinPos() ;coord[1]<= my_containers.at(0)->getMaxPos();++coord[1])
            for(coord[2]=my_containers.at(1)->getMinPos() ;coord[2]<=my_containers.at(1)->getMaxPos();++coord[2])
            {
                if ((*density_sptr)[coord] < retVal)
                    retVal = (*density_sptr)[coord];
            }
    }

    minValueAll = retVal;
}

void Screen_manager_ImageData::find_max_value(const int& sliceNum, Orientation _o)
{
    CartesianCoordinate3D<int> coord = make_coordinate(0,0,0);
    float retVal = 0;

    if(_o == All)
    {
        //        for(coord[1]=my_containers.at(0)->getMinPos() ;coord[1]<= my_containers.at(0)->getMaxPos();++coord[1])
        //            for(coord[2]=my_containers.at(1)->getMinPos() ;coord[2]<=my_containers.at(1)->getMaxPos();++coord[2])
        //                for(coord[3]=my_containers.at(2)->getMinPos();coord[3]<=my_containers.at(2)->getMaxPos();++coord[3])
        //                {
        //                    if ((*density_sptr)[coord] > retVal)
        //                        retVal = (*density_sptr)[coord];
        //                }
        retVal = density_sptr->find_max();
    }
    else if (_o == Transverse)
    {
        coord[1]=sliceNum;

        for(coord[2]=my_containers.at(1)->getMinPos() ;coord[2]<=my_containers.at(1)->getMaxPos();++coord[2])
            for(coord[3]=my_containers.at(2)->getMinPos();coord[3]<=my_containers.at(2)->getMaxPos();++coord[3])
            {
                if ((*density_sptr)[coord] > retVal)
                    retVal = (*density_sptr)[coord];
            }
    }
    else if (_o == Coronal)
    {
        coord[2] = sliceNum;
        for(coord[1]=my_containers.at(0)->getMinPos() ;coord[1]<= my_containers.at(0)->getMaxPos();++coord[1])
            for(coord[3]=my_containers.at(2)->getMinPos();coord[3]<=my_containers.at(2)->getMaxPos();++coord[3])
            {
                if ((*density_sptr)[coord] > retVal)
                    retVal = (*density_sptr)[coord];
            }
    }
    else
    {
        coord[3] = sliceNum;
        for(coord[1]=my_containers.at(0)->getMinPos() ;coord[1]<= my_containers.at(0)->getMaxPos();++coord[1])
            for(coord[2]=my_containers.at(1)->getMinPos() ;coord[2]<=my_containers.at(1)->getMaxPos();++coord[2])
            {
                if ((*density_sptr)[coord] > retVal)
                    retVal = (*density_sptr)[coord];
            }
    }
    maxValueAll = retVal;
}

int Screen_manager_ImageData::getNumOfArrays()
{
    return density_sptr->get_length();
}

Array<2, float> Screen_manager_ImageData::getArray2D(const int& _i) const
{
    return dynamic_cast<VoxelsOnCartesianGrid<float>* > (density_sptr.get())->get_plane(_i+my_containers.at(0)->getMinPos());
}

void Screen_manager_ImageData::setArray2D(const Array<2,float>& _a, const int& _i)
{
    PixelsOnCartesianGrid<float> f(_a,
                                   density_sptr->get_origin(),
                                   cut_first_dimension(dynamic_cast<VoxelsOnCartesianGrid<float>* > (density_sptr.get())->get_grid_spacing()));

    dynamic_cast<VoxelsOnCartesianGrid<float>* >(density_sptr.get())->set_plane(f, _i);
}

void Screen_manager_ImageData::split(const int& split_slice,  QVector<Screen_manager_ImageData*>& ret_list)
{

    //    Screen_manager_ImageData* tmp_screen;

    //    int mod = (num_poss+1) % split_slice;

    //    int num_subsets = ceil(static_cast<float>(num_poss) / split_slice);

    //    if (mod != 0)
    //    {
    //        QMessageBox::StandardButton reply =
    //                QMessageBox::question(this, " ", "The size of the last split will be less than the rest.Abort?",
    //                                      QMessageBox::Yes|QMessageBox::No);

    //        if(reply == QMessageBox::Yes)
    //            return; // early exit
    //    }

    //    VoxelsOnCartesianGrid<float>* _vox = dynamic_cast<VoxelsOnCartesianGrid<float>* > (density_sptr.get());

    //    for (int i = 0; i < num_subsets; i++)
    //    {
    //        int minZ = i*split_slice ;
    //        int maxZ = minZ+split_slice;

    //        if(maxZ >= num_poss)
    //            maxZ = num_poss;

    //        IndexRange<3> range = density_sptr->get_index_range();

    //        //range.resize(minZ, maxZ);
    //        BasicCoordinate<3,int> new_min = make_coordinate(0, range[1].get_min_index(), range[2].get_min_index());
    //        BasicCoordinate<3,int> new_max = make_coordinate(split_slice, range[1].get_max_index(), range[2].get_max_index());

    //        const IndexRange<3> nrange(new_min, new_max);

    //        VoxelsOnCartesianGrid<float> tmp(nrange,
    //                                         density_sptr->get_origin(),
    //                                         _vox->get_voxel_size());

    //        get_subset(tmp, nrange, minZ, maxZ);

    //        tmp_screen = new Screen_manager_ImageData (&tmp,curConfig, parentWidget());

    //        // It is absolutely crusial to rename the filenames in the new objects
    //        // As a lot of functions rely on the uniqueness.

    //        {
    //            QFileInfo ffi(this->getFullFilePath());

    //            QFileInfo fi(this->getFileName());

    //            QString fn = fi.baseName();
    //            QString number = QString("%1").arg(i, 3, 10, QChar('0'));

    //            fn = ffi.absolutePath() +QDir::separator()+ fn +"_"+number+"."+fi.suffix();
    //            tmp_screen->setFullFilePath(fn);
    //        }
    //        ret_list.append(tmp_screen);
    //    }
}

const IndexRange<3> Screen_manager_ImageData::getRanges3D()
{
    if (containersReady)
    {
        BasicCoordinate<3,int> new_min = make_coordinate(density_sptr.get()[0].get_min_index(),
                density_sptr.get()[0][0].get_min_index(),
                density_sptr.get()[0][0][0].get_min_index());
        BasicCoordinate<3,int> new_max = make_coordinate(density_sptr.get()[0].get_max_index(),
                density_sptr.get()[0][0].get_max_index(),
                density_sptr.get()[0][0][0].get_max_index());

        return IndexRange<3>(new_min, new_max);
    }
}
