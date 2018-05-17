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

#include "display_screen_curve.h"
#include "stir/is_null_ptr.h"
#include <algorithm>
#include <qwt_symbol.h>
#include <qwt_color_map.h>

#if __has_include("Board.h")
#include "Board.h"
#include "board/Color.h"
#endif

display_screen_curve::display_screen_curve(QWidget *parent) :
    QwtPlot(parent)
{
    setCanvasBackground( Qt::white );
    setAxisScale( QwtPlot::yLeft, 0.0, 10.0 );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen(Qt::lightGray, 1.0);
    grid->attach( this );

    hasX  = false;

    curve  = 0;
    high_curve = new QwtPlotCurve();
    high_curve->setPen(Qt::blue,1);
}

display_screen_curve::~display_screen_curve()
{}

void display_screen_curve::setXdata(const QVector<double> &_d)
{
    x_data = _d;
    this->setAxisScale(QwtPlot::xBottom, 0.0, *std::max_element(x_data.constBegin(), x_data.constEnd()));
    hasX = true;
}

void display_screen_curve::setYdata(const QVector<double> &_d)
{
    y_data = _d;
    if (!hasX)
        return;

    if (curve != 0 )
        curve->detach();

    curve = new QwtPlotCurve();
    curve->setPen(Qt::red,2);
    curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::green),QPen(Qt::black),QSize(10,10)));
    curve->setSamples( x_data, y_data);
    this->setAxisScale(QwtPlot::yLeft, 0.0, *std::max_element(y_data.constBegin(), y_data.constEnd()));
    curve->attach( this );

    this->replot();
}

void display_screen_curve::setHighYdata(const QVector<double> &_d)
{

    high_y_data = _d;
    if (!hasX)
        return;

//    if (high_curve != 0 )
//    {
//        high_curve->detach();
//    }


    high_curve->setSamples( x_data, high_y_data);
    high_curve->attach( this );

    this->replot();
}


