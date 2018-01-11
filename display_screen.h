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

#ifndef DISPLAY_SCREEN_H
#define DISPLAY_SCREEN_H

#include <QWidget>
#include <QMenu>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_plot_rescaler.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>

#include <memory>

#include "common.h"

namespace Ui {
class display_screen;
}

class display_screen : public QwtPlot
{
    Q_OBJECT

public:
    explicit display_screen(int _label, int x_length, int y_length, QWidget *parent = 0);
    ~display_screen();

    QVector<double>::iterator get_data_begin();
    QVector<double>::iterator get_data_end();

    void replot_me();

    int get_data_size();

    void set_sizes(int _h, int _v, float _h_spacing, float _v_spacing);

    inline void set_color_map(int index)
    {
        switch (index) {
        case 0:
            my_cm->set_BW();
            break;
        case 1:
            my_cm->set_WB();
            break;
        case 2:
            my_cm->set_Viridis();
            break;
        case 3:
            my_cm->set_qwt();
            break;
        case 4:
            my_cm->set_JET();
            break;
        default:
            break;
        }
        d_spectrogram->setColorMap(my_cm);
        this->setCanvasBackground(QBrush(my_cm->get_background()));
    }

    inline void set_max_value(double new_max)
    { max_value = new_max;}

signals:
    void save_this_array(int);

public slots:
private slots:
    void popUpMenu(const QPoint &pos);
private:
//    Ui::display_screen *ui;

    QwtPlotSpectrogram *d_spectrogram;
    QwtMatrixRasterData *p_raster;
    QwtPlotRescaler *d_rescaler;

    //!
    //! \brief my_data This vector actually holds the data to be displayed.
    //!
    QVector<double> my_data;

    //!
    //! \brief my_unique_id
    //! A unique id within the segment
    int my_unique_id;

    //!
    //! \brief my_label
    //! The id within the displayed viewports.
    int my_label;

    //!
    //! \brief row_size
    //! Num tang poss.
    int row_size;

    int col_size;

    //!
    //! \brief flag_sinos
    //! True: sinograms; False: Viewgrams
    bool flag_sinos;

    //!
    //! \brief max_value
    //! The maximum value in the my_data vector. Is used for color scaling.
    double max_value;

    //!
    //! \brief my_cm
    //!
    Viewer::ColorMap *my_cm;

    int size;

};

#endif // DISPLAY_SCREEN_H
