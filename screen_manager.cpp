//
//
/*
   Copyright 2017 Nikos Efthimiou (nikos.efthimiou AT gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
    limitations under the License.
*/
#include "screen_manager.h"
#include "ui_screen_manager.h"

#include <QLayout>
#include <QFileDialog>

Screen_manager::Screen_manager(std::shared_ptr<stir::ProjData> _input_proj_data_sptr, int _num_viewports, int cm_index,
                               int _view_by, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Screen_manager)
{
    ui->setupUi(this);
    projdata_sptr = _input_proj_data_sptr;
    view_by = _view_by;

    current_id = 0;
    num_viewports = _num_viewports;
    segments_initialised = false;

    // Get sizes
    initialise_controls();

    // Create the final widget view.
    set_up_plot_area(num_viewports);

    on_comboBox_currentIndexChanged(QString("0"));
    changeCM(cm_index);
}

Screen_manager::~Screen_manager()
{
    delete ui;
}

void Screen_manager::set_up_plot_area(int _viewsports)
{

    if (_viewsports<=0)
        return;

    if(!my_displays.isEmpty())
    {
        for(int i = 0; i < my_displays.length(); i++)
        {
            ui->gridLayout_5->removeWidget(my_displays.last().get());
        }
        my_displays.clear();
    }

    num_viewports = _viewsports;
    int col_size;

    switch (num_viewports) {
    case 1:
        col_size = 1;
        break;
    case 2:
        col_size = 2;
        break;
    case 4:
        col_size = 2;
        break;
    case 6:
        col_size = 3;
        break;
    }


    for(int i = 0; i < num_viewports; i++)
    {
        std::shared_ptr<display_screen> tmp(new display_screen(i,
                                                               bin_size,
                                                               bin_size,
                                                               num_views,
                                                               num_tangs, this));

        my_displays.append(tmp);
        int row = static_cast<int>((i/(col_size)));
        int col = static_cast<int>((i%(col_size)));

        ui->gridLayout_5->addWidget(my_displays.last().get(), row , col);

        QObject::connect(my_displays.last().get(), &display_screen::save_this_array,
                         this, &Screen_manager::save_as_array);
    }
    ui->displ_frame->updateGeometry();
}

void Screen_manager::draw_plot_area()
{
    for(int i = 0; i < num_viewports; i++)
    {
        QVector<double>::iterator iter = my_displays.at(i)->get_data_ptr();

        Sinogram<float> this_sinogram = this_segment->get_sinogram(i);

        stir::Sinogram<float>::const_full_iterator is = this_sinogram.begin_all_const();
        stir::Sinogram<float>::const_full_iterator ie = this_sinogram.end_all_const();

        double max_value = 0.0;
        for ( ;is != ie; ++is, ++iter)
        {
            *iter = static_cast<double>(*is);
            if (*iter > max_value)
                max_value = *iter;
        }
        my_displays[i]->set_max_value(max_value);
        my_displays[i]->replot_me();
    }
}

void Screen_manager::initialise_controls()
{
    segments_initialised = false;
    min_seg = projdata_sptr->get_min_segment_num();
    max_seg = projdata_sptr->get_max_segment_num();

    int zero_indx = 0;

    for (int i = min_seg; i <= max_seg; ++i)
    {
        ui->comboBox->addItem(QString::number(i));
    }

    segments_initialised = true;
    int index = ui->comboBox->findText("0"); //use default exact match
    if(index >= 0)
        ui->comboBox->setCurrentIndex(index);

    bin_size = projdata_sptr->get_proj_data_info_ptr()->get_scanner_ptr()->get_default_bin_size();
    num_views = projdata_sptr->get_num_views();
    num_tangs = projdata_sptr->get_num_tangential_poss();

}

void Screen_manager::on_comboBox_currentIndexChanged(QString index)
{
    if(segments_initialised && my_displays.size()>0)
    {
        bool ok;
        int num_seg = index.toInt(&ok, 10);
        if (!ok)
            return;

        reset_current_id();
        set_limits(num_seg);

        this_segment =  std::make_shared<SegmentBySinogram<float> > (projdata_sptr->get_segment_by_sinogram(num_seg));
        this_segment_num = num_seg;

        draw_plot_area();
    }
}

void Screen_manager::set_seg_index(int num_seg)
{
    int index = ui->comboBox->findText(QString::number(num_seg));

    if(index==-1)
        return;

    ui->comboBox->setCurrentIndex(index);
    on_comboBox_currentIndexChanged(QString::number(num_seg));
}


void Screen_manager::reset_current_id()
{
    current_id = 0;
    ui->go_left->setEnabled(false);
}

void Screen_manager::set_limits(const int pos)
{
    if (view_by == 1) // By Sinogram
    {
        int poss = projdata_sptr->get_num_axial_poss(pos);
        ui->posss->setText(QString::number(current_id) +" / " +QString::number(poss));
    }
    else // By Viewgram
    {
        int poss = projdata_sptr->get_num_views();
        ui->posss->setText(QString::number(current_id) +" / " +QString::number(poss));
    }
}

void Screen_manager::changeCM(int index)
{
    for(int i = 0; i < num_viewports; i++)
    {
        my_displays[i]->set_color_map(index);
    }
}


void Screen_manager::save_as_array(int this_label)
{

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save array"), "",
                                                    tr(" (*.hv)"));

    if (fileName.size() == 0 )
        return;

    Bin central_bin(this_segment_num,0,0,0, 0.f);

    const float m_spacing = this_segment->get_proj_data_info_ptr()->get_sampling_in_m(central_bin);
    const float s_spacing = this_segment->get_proj_data_info_ptr()->get_sampling_in_s(central_bin);
    const float m = this_segment->get_proj_data_info_ptr()->get_m(central_bin);
    const float s = this_segment->get_proj_data_info_ptr()->get_s(central_bin);

    Sinogram<float> sinogram = this_segment->get_sinogram(this_label);
    IndexRange3D dn(0, 0, sinogram.get_min_index(), sinogram.get_max_index(), sinogram[1].get_min_index(), sinogram[1].get_max_index());

    const Array<2, float> * p_sinogram =
            dynamic_cast<Array<2, float> *>(&sinogram);

    Array<3, float> dummy(dn);

    for(int i=sinogram.get_min_index(); i<=sinogram.get_max_index(); i++)
        for (int j = sinogram[1].get_min_index(); j <=sinogram[1].get_max_index(); j++)
            dummy[0][i][j] =  sinogram[i][j];

    write_basic_interfile(fileName.toStdString() + "_by_sino.hv",
                          dummy,
                          CartesianCoordinate3D<float>(m_spacing, 1.F, s_spacing),
                          CartesianCoordinate3D<float>(m, 0.F, s));
}
