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

#ifndef DISPLAY_SCREEN_CURVE_H
#define DISPLAY_SCREEN_CURVE_H

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

#include "src/buildblock/common.h"

class display_screen_curve : public QwtPlot
{
    Q_OBJECT

public:
    explicit display_screen_curve(QWidget *parent = 0);
    ~display_screen_curve();

    void setXdata(const QVector<double> & );

    void setYdata(const QVector<double> & );

    void setHighYdata(const QVector<double> &_d);
signals:


protected slots:
    
private:

    QVector<double> x_data;

    QVector<double> y_data;

    QVector<double> high_y_data;

    bool hasX;

     QwtPlotCurve* curve;

      QwtPlotCurve* high_curve;
    
};

#endif // DISPLAY_SCREEN_H
