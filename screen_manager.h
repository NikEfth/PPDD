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

#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <QWidget>

#include "display_screen.h"

#include "stir/Sinogram.h"
#include "stir/Viewgram.h"

#include "stir/SegmentBySinogram.h"
#include "stir/SegmentByView.h"

#include "stir/ProjData.h"
#include "stir/ProjDataFromStream.h"
#include "stir/ProjDataInMemory.h"
#include "stir/ProjDataInterfile.h"
#include "stir/IO/interfile.h"
#include "stir/CartesianCoordinate3D.h"
#include "stir/Bin.h"
#include "stir/IO/interfile.h"
#include "stir/Array.h"
#include "stir/IndexRange3D.h"

using namespace stir;

namespace Ui {
class Screen_manager;
}

class Screen_manager : public QWidget
{
    Q_OBJECT

public:
    explicit Screen_manager(std::shared_ptr<stir::ProjData> _input_proj_data_sptr, int _num_viewports,
                            int cm_index =1, int _view_by=1, QWidget *parent = 0);
    ~Screen_manager();

    void set_seg_index(int num_seg);
    void changeCM(int index);

public slots:
    void set_up_plot_area(int _viewsports);

    void save_as_array(int this_label);

private slots:
    void on_comboBox_currentIndexChanged(QString index);

    void reset_current_id();

private:
    Ui::Screen_manager *ui;

    int num_viewports;

    void initialise_controls();

    void draw_plot_area();

    void set_limits(const int pos);

    std::shared_ptr<stir::ProjData> projdata_sptr;

    QVector< std::shared_ptr<display_screen> > my_displays;

    int view_by;

    int current_id;

    int min_seg;

    int max_seg;

    float bin_size;

    int num_views;

    int num_tangs;

    bool segments_initialised;

    shared_ptr<SegmentBySinogram<float> > this_segment;

    int this_segment_num;

};

#endif // SCREEN_MANAGER_H
