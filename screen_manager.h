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
    void on_selectSeg_cmb_currentIndexChanged(QString index);

    void reset_current_id();

    void on_posSelect_scrbr_valueChanged(int value);

    void on_posSelect_scrbr_sliderReleased();

private:
    Ui::Screen_manager *ui;

    //!
    //! \brief num_viewports
    //! Number of active views per page.
    int num_viewports;

    void initialise_controls();

    void draw_plot_area();

    void set_limits();

    std::shared_ptr<stir::ProjData> projdata_sptr;

    QVector< std::shared_ptr<display_screen> > my_displays;

    //!
    //! \brief view_by
    //! Viewmode: 1: Sinogram, 2: Viewgram
    int view_by;

    //!
    //! \brief current_id
    //! The id of the current position.
    //! \details
    //! On Sinograms the id corresponds to axial poss and on
    //! Viewgrams on views.
    int current_id;

    //!
    //! \brief min_seg
    //! Min segment of the ProjData
    int min_seg;

    //!
    //! \brief max_seg
    //! Max segment of the ProjData
    int max_seg;

    float bin_size;

    //!
    //! \brief vertical_size
    //! Vertical axis of the prints
    //! \details
    //! On Sinogram mode that is the views on Viewgrams tha tangestial
    int vertical_size;

    //!
    //! \brief horizontal_size
    //! Horizontal axis of the prints
    //! \details
    //! On Sinogram mode that is the tangestial positions
    //!  on Viewgrams are axial positions.
    int horizontal_size;

    //!
    //! \brief num_poss
    //! Total number of positions to scroll.
    int num_poss;

    //!
    //! \brief hasSegments
    //! A flag for the initialised controls
    bool hasSegments;

    //!
    //! \brief cur_segment
    //! The current segment under display
    shared_ptr<Segment<float> > cur_segment;

    //!
    //! \brief cur_segment_num
    //! The current segment number under display.
    int cur_segment_num;

    int cur_ColMap;

};

#endif // SCREEN_MANAGER_H
