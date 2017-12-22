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

#include "display_screen.h"
#include "ui_display_screen.h"

#include <QtAlgorithms>
#include <QAction>
#include <QFileDialog>

#include <qwt_color_map.h>

display_screen::display_screen(int label, float x_size, float y_size, int x_length, int y_length, QWidget *parent) :
    QwtPlot(parent)
  //ui(new Ui::display_screen)
{
    //    ui->setupUi(this);
    num_of_data = 1;
    init_id = 1;
    flag_sinos = true;

    my_label = label;

    int size = x_length*y_length;
    row_size = y_length;
    my_data.resize(size);

    setAutoReplot(true);

    setTitle(QString::number(label));

    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount(0);
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    my_cm = new ColorMap();
    d_spectrogram->setColorMap(my_cm);


    plotLayout()->setAlignCanvasToScales( true );
    setAxisScale( QwtPlot::xBottom, static_cast<double>(-(y_length)/2), static_cast<double>((y_length)/2) );
    setAxisScale( QwtPlot::yLeft, 0.0, static_cast<double>(x_length) );

    p_raster = new QwtMatrixRasterData();
    p_raster->setInterval( Qt::XAxis, QwtInterval( static_cast<double>(-(y_length)/2),static_cast<double>((y_length)/2) ) );
    p_raster->setInterval( Qt::YAxis, QwtInterval(0, static_cast<double>(x_length) ) );

    d_spectrogram->setData( p_raster);
    d_spectrogram->attach(this);
    //    this->resize(x_size*x_length, y_size*y_length);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(const QPoint &  )),this,SLOT(popUpMenu(const QPoint &)));
}

display_screen::~display_screen()
{
    //    delete ui;
}


QVector<double>::iterator
display_screen::get_data_ptr()
{
    return  my_data.begin();
}


void display_screen::set_max_value(double new_max)
{
    max_value = new_max;
}

void
display_screen::replot_me()
{
    p_raster->setValueMatrix(my_data, row_size);
    p_raster->setInterval( Qt::ZAxis, QwtInterval( 0.0, max_value ) );

    QwtPlot::replot();
    repaint();
}

void display_screen::set_color_map(int index)
{
    switch (index) {
    case 0:
        my_cm->set_BW();
        break;
    case 1:
        my_cm->set_WB();
        break;
    case 2:
        my_cm->set_JET();
        break;
    case 3:
        my_cm->set_qwt();
        break;
    default:
        break;
    }
    d_spectrogram->setColorMap(my_cm);
}

void display_screen::popUpMenu(const QPoint &pos)
{
    QMenu myMenu;
    QAction *act_saveAs = myMenu.addAction("Save as array");
    act_saveAs->setIcon(QIcon::fromTheme("document-save-as.png"));

    QAction * act_saveAsImage = myMenu.addAction("Save as image");
    act_saveAsImage->setIcon(QIcon::fromTheme("image-x-generic.png"));

    QObject::connect(act_saveAs, &QAction::triggered, [=]()
    {emit save_this_array(my_label);}
                     );

    QObject::connect(act_saveAsImage, &QAction::triggered, []()
    {
        QString fileName = QFileDialog::getSaveFileName(0,
               tr("Save array"), "",
               tr(" (*.abk);;All Files (*)"));
    });

    myMenu.exec(this->mapToGlobal(pos));
}
