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
#include "qwt_color_map.h"
#include "qwt_plot_spectrogram.h"
#include "qwt_scale_widget.h"
#include "qwt_scale_draw.h"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_renderer.h"
#include "qwt_matrix_raster_data.h"

#include <qwt_painter.h>
#include <qwt_plot_canvas.h>

#include <memory>

namespace Ui {
class display_screen;
}

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap()
    {set_BW();}

    inline void set_BW()
    {   setColorInterval(Qt::black, Qt::white); }

    inline void set_WB()
    { setColorInterval(Qt::white, Qt::black); }

    void set_JET()
    {
        setColorInterval(Qt::blue, Qt::red);
        addColorStop( 0.35, Qt::cyan );
        addColorStop( 0.5, Qt::green );
        addColorStop( 0.75, Qt::yellow );
    }

    void set_qwt()
    {
        setColorInterval(Qt::darkCyan, Qt::red);
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.6, Qt::green );
        addColorStop( 0.95, Qt::yellow );
    }

    void set_Viridis()
    {

    }

//    QColor o, one,on, two,tw, three, thre, four, fou, five, fiv, six, si, seven, seve, eight, eigh, nine, nin;
};

class display_screen : public QwtPlot
{
    Q_OBJECT

public:
    explicit display_screen(int label, float x_size, float y_size, int x_length, int y_length, QWidget *parent = 0);
    ~display_screen();

    QVector<double>::iterator get_data_ptr();
    void replot_me();

    void set_color_map(int index);

    void set_max_value(double new_max);

signals:
    void save_this_array(int);

public slots:
private slots:
    void popUpMenu(const QPoint &pos);
private:
//    Ui::display_screen *ui;

    QwtPlotSpectrogram *d_spectrogram;
    QwtMatrixRasterData *p_raster;

    QVector<double> my_data;

    int num_of_data;
    int init_id;

    int my_label;

    int row_size;

    int total_ids;
    bool flag_sinos;

    double max_value;

    ColorMap *my_cm;

};

#endif // DISPLAY_SCREEN_H
