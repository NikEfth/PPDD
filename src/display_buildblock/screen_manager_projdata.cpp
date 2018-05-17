#include <QFileDialog>

#include "screen_manager_projdata.h"
#include "ui_screen_manager.h"

#include "stir/IndexRange3D.h"

screen_manager_ProjData::screen_manager_ProjData(Configuration *curConfig, QWidget *parent):
    Screen_manager(curConfig, parent)
{
    cur_segment_num = 0;
    curTOFnum = 0;
    createActions();

    setUpContainers(1);

}

QString screen_manager_ProjData::getDataType() const
{return QString("projData");}


void screen_manager_ProjData::createActions()
{
//    selectSeg_cmb = new QComboBox(this);
//    connect(selectSeg_cmb, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_selectSeg_cmb_currentIndexChanged(QString)));
//    ui->gridLayout->addWidget(selectSeg_cmb);

}

int screen_manager_ProjData::getNumOfArrays()
{
    return 1;
}

Array<2, float> screen_manager_ProjData::getArray2D(const int& _i) const
{
    // should follow a relavant reasoning.
}

void screen_manager_ProjData::setArray2D(const Array<2,float>&, const int&)
{

}

void screen_manager_ProjData::applyFunctionToAllContainers(Worker* _w)
{
//    if (containersReady)
//    {
//        my_containers.at(0)->getActiveScreen()->applyFunction(_w);
//        my_containers.at(1)->getActiveScreen()->applyFunction(_w);
//        my_containers.at(2)->getActiveScreen()->applyFunction(_w);
//    }
}

void screen_manager_ProjData::removeFunctionToAllContainers()
{
//    if (containersReady)
//    {
//        my_containers.at(0)->getActiveScreen()->removeFunction();
//        my_containers.at(1)->getActiveScreen()->removeFunction();
//        my_containers.at(2)->getActiveScreen()->removeFunction();
//    }
}

bool screen_manager_ProjData::loadFile(const QString fileName)
{
//    projData_sptr = ProjData::read_from_file(fileName.toStdString());

//    if (is_null_ptr(projData_sptr))
//    {
//        QMessageBox::critical(this, tr("Error reading ProjData"),
//                              tr("Unable to load Projection Data."),
//                              QMessageBox::Ok);
//        return false;
//    }

//    Dialog *view_msg = new Dialog("projSpace",this);
//    view_msg->show();
//    viewOrder = view_msg->exec();

//    if (viewOrder == 0)
//        return false;

//    if (projData_sptr->get_num_tof_poss()>1)
//        has_tof = true;

//    if (has_tof)
//    {
//        selectTOF_cmb = new QComboBox(this);
//        connect(selectTOF_cmb, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_selectTOF_cmb_currentIndexChanged(QString)));
//        ui->gridLayout->addWidget(selectTOF_cmb);
//    }

//    initialise_controls();

//    myFullFileName = fileName;

//    return true;
}

void screen_manager_ProjData::display()
{
    set_up_plot_area();
}

void screen_manager_ProjData::onCursorChanged()
{
    if (cursorStatus)
    {

    }
}

void screen_manager_ProjData::showCursor(const bool &state)
{
    if (containersReady)
    {
        cursorStatus = state;
    }
}

void screen_manager_ProjData::_setCursor(const QPoint&, QwtPlot *)
{
    int nikos = 0;
}

void screen_manager_ProjData::_setCursor(const Viewer::SimpleVector3D<int>& _c)
{
    int nikos = 0;
}

void screen_manager_ProjData::updateCursor(display_screen_container *d)
{
    int nikos = 0;
}

void screen_manager_ProjData::initialisePlotArea()
{
//    min_seg = projData_sptr->get_min_segment_num();
//    max_seg = projData_sptr->get_max_segment_num();

//    selectSeg_cmb->blockSignals(true);
//    for (int i = min_seg; i <= max_seg; ++i)
//        selectSeg_cmb->addItem(QString::number(i));

//    int index = selectSeg_cmb->findText("0"); //use default exact match

//    if(index >= 0)
//        selectSeg_cmb->setCurrentIndex(index);

//    selectSeg_cmb->blockSignals(false);

//    cur_segment_num = 0;
//    initialise_controls_for_seg();
//    set_limits();

//    bin_size = projData_sptr->get_proj_data_info_ptr()->get_scanner_ptr()->get_default_bin_size();

//    initialise_controls_for_view();

//    if (has_tof)
//        initialise_controls_for_TOF();
}

void screen_manager_ProjData::initialise_controls_for_view()
{
//    if (viewOrder == 1) //Sinograms
//    {
//        vertical_size = projData_sptr->get_num_views();
//        horizontal_size = projData_sptr->get_num_tangential_poss();
//    }
//    else if (viewOrder == 2) //Viewgrams
//    {
//        vertical_size  = projData_sptr->get_num_axial_poss(cur_segment_num);
//        horizontal_size  = projData_sptr->get_num_tangential_poss();
//    }
//    this->min_x_axis = 0;
//    this->max_x_axis = horizontal_size;

//    this->min_y_axis =  0;
//    this->max_y_axis = vertical_size;
//    this->update_display_size();
}

void screen_manager_ProjData::initialise_controls_for_seg()
{

//    if (viewOrder == 1) //Sinograms
//    {
//        num_poss = projData_sptr->get_num_axial_poss(cur_segment_num);
//        cur_segment =  std::make_shared<SegmentBySinogram<float> > (projData_sptr->get_segment_by_sinogram(cur_segment_num,curTOFnum));
//    }
//    else if (viewOrder == 2) //Viewgrams
//    {
//        num_poss = projData_sptr->get_num_views();
//        cur_segment =  std::make_shared<SegmentByView<float> > (projData_sptr->get_segment_by_view(cur_segment_num,curTOFnum));
//    }

//    Bin central_bin(cur_segment_num,0,0,0, 0.f);
//    x_spacing = cur_segment->get_proj_data_info_ptr()->get_sampling_in_m(central_bin);
//    y_spacing = cur_segment->get_proj_data_info_ptr()->get_sampling_in_s(central_bin);
}

void screen_manager_ProjData::initialise_controls_for_TOF()
{
    minTOFpos = this->projData_sptr->get_min_tof_pos_num();
    maxTOFpos = this->projData_sptr->get_max_tof_pos_num();

    curTOFnum = 0;

    selectTOF_cmb->blockSignals(true);
    for (int i = minTOFpos; i <= maxTOFpos; ++i)
        selectTOF_cmb->addItem(QString::number(i));

    int index = selectTOF_cmb->findText("0"); //use default exact match

    if(index >= 0)
        selectTOF_cmb->setCurrentIndex(index);

    selectTOF_cmb->blockSignals(false);

}

//void screen_manager_ProjData::draw_plot_area(int view_order)
//{
//    double max_value = 0.0;
//    double min_value = 0.0;
//    for(qint16 i = 0; i < num_viewports; i++)
//    {
//        qint32 current_pos = current_view_id + i;

//        QVector<double>::iterator iter = my_displays.at(i)->get_data_begin();

//        if (current_pos < num_poss)
//        {
//            if(viewOrder == 1) // Sinograms
//            {
//                Sinogram<float> cur_sinogram = cur_segment->get_sinogram(current_pos);

//                stir::Sinogram<float>::const_full_iterator is = cur_sinogram.begin_all_const();
//                stir::Sinogram<float>::const_full_iterator ie = cur_sinogram.end_all_const();

//                min_value = static_cast<double>(*is);

//                for ( ;is != ie; ++is, ++iter)
//                {
//                    *iter = static_cast<double>(*is);
//                    if (*iter > max_value)
//                        max_value = *iter;
//                    if (*iter < min_value)
//                        min_value = *iter;
//                }
//            }
//            else if (viewOrder == 2) //Viewgrams
//            {
//                Viewgram<float> cur_sinogram = cur_segment->get_viewgram(current_pos);

//                stir::Viewgram<float>::const_full_iterator is = cur_sinogram.begin_all_const();
//                stir::Viewgram<float>::const_full_iterator ie = cur_sinogram.end_all_const();

//                min_value = static_cast<double>(*is);

//                for ( ;is != ie; ++is, ++iter)
//                {
//                    *iter = static_cast<double>(*is);
//                    if (*iter > max_value)
//                        max_value = *iter;
//                    if (*iter < min_value)
//                        min_value = *iter;
//                }
//            }

//            my_displays[i]->setMyTitle(current_pos);
//            //            qApp->processEvents();
//        }
//        else
//        {
//            for (int ii = 0; ii < my_displays.at(i)->get_data_size(); ii++, ++iter)
//            {  *iter = 0.0; }

//        }

//        my_displays[i]->set_min_max_values(min_value, max_value);
//        my_displays[i]->replot_me();

//        max_value = 0.0;
//    }
//}

void screen_manager_ProjData::on_selectSeg_cmb_currentIndexChanged(QString index)
{
//    if(my_displays.size()>0)
//    {
//        bool ok;
//        int num_seg = index.toInt(&ok, 10);
//        if (!ok)
//            return;

//        cur_segment_num = num_seg;

//        initialise_controls_for_seg();
//        set_limits();

//        if(isGrouped())
//            emit segmentChangedTo(index);

//        if(has_set_up)
//        {
//            initialise_controls_for_view();
//            draw_plot_area();
//        }
//    }
}

CartesianCoordinate3D<float> screen_manager_ProjData::getCurrentCursorInmm()
{
 int nikos = 0;
}

double screen_manager_ProjData::getValueAt(const SimpleVector3D<int> _c)
{
int nikos = 0;
}

void screen_manager_ProjData::on_selectTOF_cmb_currentIndexChanged(QString index)
{
//    if(my_displays.size()>0)
//    {
//        bool ok;
//        int tof_pos = index.toInt(&ok, 10);
//        if (!ok)
//            return;

//        curTOFnum = tof_pos;

//        initialise_controls_for_seg();
//        //        set_limits();

//        //        if(isGrouped())
//        //            emit segmentChangedTo(index);

//        if(has_set_up)
//        {
//            //            initialise_controls_for_view();
//            draw_plot_area();
//        }
//    }
}

void screen_manager_ProjData::remote_segment_change(QString index)
{
//    if(my_displays.size()>0)
//    {
//        bool ok;
//        int num_seg = index.toInt(&ok, 10);
//        if (!ok)
//            return;

//        cur_segment_num = num_seg;

//        selectSeg_cmb->blockSignals(true);
//        selectSeg_cmb->setCurrentText(index);
//        selectSeg_cmb->blockSignals(false);

//        initialise_controls_for_seg();
//        set_limits();

//        if(has_set_up)
//        {
//            initialise_controls_for_view();
//            draw_plot_area();
//        }
//    }
}


bool screen_manager_ProjData::setState(qint16 newViewOrder,
                                       qint16 new_current_id,
                                       qint16 new_seg_num,
                                       qint16 new_num_viewports)
{
//    has_set_up = false;

//    if (new_seg_num < min_seg || new_seg_num > max_seg)
//        return false;
//    else
//    {
//        int index = selectSeg_cmb->findText(QString::number(new_seg_num));
//        if(index >= 0)
//            selectSeg_cmb->setCurrentIndex(index);
//    }

//    viewOrder = newViewOrder;

//    remote_scrbr_valueChanged(new_current_id);

//    if (num_viewports != new_num_viewports)
//        initialise_plot_area(new_num_viewports);


//    initialise_controls_for_seg();
//    initialise_controls_for_view();
//    set_limits();
//    my_displays.clear();
//    set_up_plot_area();
//    draw_plot_area();

//    if(this->hasHistograms)
//        this->draw_histograms();

//    return true;
}

void screen_manager_ProjData::applyVizValuesToAllContainers(const double &_min, const double &_max)
{
    if (containersReady)
    {
        //my_containers[0]->getActiveScreen()->intervalChanged(_min, _max, unset);
  //      my_containers[1]->getActiveScreen()->intervalChanged(_min, _max, unset);
    }
}

void screen_manager_ProjData::find_min_value(const int& sliceNum,  Orientation _o)
{
    int nikos = 1;
}

void screen_manager_ProjData::find_max_value(const int& sliceNum, Orientation _o)
{
    int nikos = 1;
}

void screen_manager_ProjData::set_seg_index(qint16 num_seg)
{
    qint16 index = static_cast<qint16>(selectSeg_cmb->findText(QString::number(num_seg)));

    if(index==-1)
        return;

    selectSeg_cmb->setCurrentIndex(index);
    on_selectSeg_cmb_currentIndexChanged(QString::number(num_seg));
}

QSize screen_manager_ProjData::setOrientation(Orientation _o)
{
//    has_set_up = false;

//    viewOrder == 1 ? viewOrder = 2:
//            viewOrder = 1;

//    initialise_controls_for_seg();
//    initialise_controls_for_view();

//    set_limits();
//    my_displays.clear();
//     active_screen->set_active(false);
//    active_screen = 0;
//    set_up_plot_area();

//    draw_plot_area();
//    Screen_manager::swap_view();
}

void screen_manager_ProjData::save_as_array(int this_label)
{

//    QString fileName = QFileDialog::getSaveFileName(this,
//                                                    tr("Save array"));

//    if (fileName.size() == 0 )
//        return;

//    Bin central_bin(cur_segment_num,0,0,0, 0.f);

//    const float m = cur_segment->get_proj_data_info_ptr()->get_m(central_bin);
//    const float s = cur_segment->get_proj_data_info_ptr()->get_s(central_bin);

//    if(viewOrder == 1)
//    {
//        Sinogram<float> sinogram = cur_segment->get_sinogram(this_label);
//        IndexRange3D dn(0, 0, sinogram.get_min_index(), sinogram.get_max_index(), sinogram[1].get_min_index(), sinogram[1].get_max_index());

//        const Array<2, float> * p_sinogram =
//                dynamic_cast<Array<2, float> *>(&sinogram);

//        Array<3, float> dummy(dn);

//        for(int i=sinogram.get_min_index(); i<=sinogram.get_max_index(); i++)
//            for (int j = sinogram[1].get_min_index(); j <=sinogram[1].get_max_index(); j++)
//                dummy[0][i][j] =  sinogram[i][j];

//        write_basic_interfile(fileName.toStdString() + "_by_sino.hv",
//                              dummy,
//                              CartesianCoordinate3D<float>(x_spacing, 1.F, y_spacing),
//                              CartesianCoordinate3D<float>(m, 0.F, s));
//    }
//    else if (viewOrder == 2)
//    {
//        Viewgram<float> viewgram = cur_segment->get_viewgram(this_label);
//        IndexRange3D dn(0, 0, viewgram.get_min_index(), viewgram.get_max_index(), viewgram[1].get_min_index(), viewgram[1].get_max_index());

//        const Array<2, float> * p_viewgram =
//                dynamic_cast<Array<2, float> *>(&viewgram);

//        Array<3, float> dummy(dn);

//        for(int i=viewgram.get_min_index(); i<=viewgram.get_max_index(); i++)
//            for (int j = viewgram[1].get_min_index(); j <=viewgram[1].get_max_index(); j++)
//                dummy[0][i][j] =  viewgram[i][j];

//        write_basic_interfile(fileName.toStdString() + "_by_view.hv",
//                              dummy,
//                              CartesianCoordinate3D<float>(x_spacing, 1.F, y_spacing),
//                              CartesianCoordinate3D<float>(m, 0.F, s));

//    }
}

void screen_manager_ProjData::writeToDisk(const QString &)
{
//    QString fileName = QFileDialog::getSaveFileName(this,
//                                                    tr("Save array"), "",
//                                                    tr(" (*.hv)"));

//    if (fileName.size() == 0 )
//        return;

//    QFileInfo f(fileName);
//    fileName = f.baseName();

//    for (int segment_num = this->projData_sptr->get_min_segment_num();
//         segment_num <=  this->projData_sptr->get_max_segment_num();
//         ++segment_num)
//    {
//        Bin central_bin(cur_segment_num,0,0,0, 0.f);

//        const float m = cur_segment->get_proj_data_info_ptr()->get_m(central_bin);
//        const float s = cur_segment->get_proj_data_info_ptr()->get_s(central_bin);


//        fileName+="seg"+QString::number(segment_num);

//        if(viewOrder == 1)
//        {
//            SegmentBySinogram<float> segment = this->projData_sptr->get_segment_by_sinogram(segment_num);
//            write_basic_interfile(fileName.toStdString() + "_by_sino.hv",
//                                  segment,
//                                  CartesianCoordinate3D<float>(x_spacing, 1.F, y_spacing),
//                                  CartesianCoordinate3D<float>(m, 0.F, s));
//        }
//        else if (viewOrder == 2)
//        {
//            SegmentByView <float> segment= this->projData_sptr->get_segment_by_view(segment_num);
//            write_basic_interfile(fileName.toStdString() + "_by_view.hv",
//                                  segment,
//                                  CartesianCoordinate3D<float>(x_spacing, 1.F, y_spacing),
//                                  CartesianCoordinate3D<float>(m, 0.F, s));

//        }
//    }
}

const IndexRange<3> screen_manager_ProjData::getRanges3D()
{
    if (containersReady)
    {
        int nikos  =0;
    }
}
