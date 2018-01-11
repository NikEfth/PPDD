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

display_screen::display_screen(int _label,
                               int x_length,
                               int y_length,
                               QWidget *parent) :
    QwtPlot(parent)
  //ui(new Ui::display_screen)
{
    my_label = _label;

    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount(0);
    d_spectrogram->setCachePolicy(QwtPlotRasterItem::CachePolicy::NoCache);

    my_cm = new Viewer::ColorMap();
    d_spectrogram->setColorMap(my_cm);

    setAutoReplot(true);
    plotLayout()->setAlignCanvasToScales( true );

    d_rescaler = new QwtPlotRescaler(this->canvas(), QwtPlot::xBottom, QwtPlotRescaler::Fixed);
    this->setCanvasBackground(QBrush(my_cm->get_background()));
    d_rescaler->setExpandingDirection(QwtPlot::yLeft, QwtPlotRescaler::ExpandBoth);

    p_raster = new QwtMatrixRasterData();
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

void
display_screen::set_sizes(int _h, int _v, float _h_spacing, float _v_spacing)
{
    row_size = _h;
    col_size = _v;
    size = row_size*col_size;
    my_data.resize(size);

//    setAxisScale( QwtPlot::xBottom, static_cast<double>(-(row_size)/2), static_cast<double>((row_size)/2) );
//    setAxisScale( QwtPlot::yLeft, 0.0, static_cast<double>(col_size) );

    p_raster->setInterval( Qt::XAxis, QwtInterval( static_cast<double>(-(row_size)/2),static_cast<double>((row_size)/2) ) );
    p_raster->setInterval( Qt::YAxis, QwtInterval(0, static_cast<double>(col_size) ) );
}


QVector<double>::iterator
display_screen::get_data_begin()
{
    return  my_data.begin();
}

QVector<double>::iterator
display_screen::get_data_end()
{
    return  my_data.end();
}

int
display_screen::get_data_size()
{
    return  my_data.size();
}

void
display_screen::replot_me()
{
    p_raster->discardRaster();

    p_raster->setValueMatrix(my_data, row_size);
    p_raster->setInterval( Qt::ZAxis, QwtInterval( 0.0, max_value ) );
    d_spectrogram->setData(p_raster);

    replot();
}

void display_screen::popUpMenu(const QPoint &pos)
{
    QMenu myMenu;
    QAction *act_saveAs = myMenu.addAction("Save as array");
    act_saveAs->setIcon(QIcon::fromTheme("document-save-as.png"));

    QAction * act_saveAsImage = myMenu.addAction("Save as image");
    act_saveAsImage->setIcon(QIcon::fromTheme("image-x-generic.png"));

    myMenu.addSeparator();

    QAction * act_popOut = myMenu.addAction("Pop Out");
    act_popOut->setIcon(QIcon::fromTheme("view-fullscreen.png"));

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
