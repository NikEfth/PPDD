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

#ifndef BAR_SCREEN_H
#define BAR_SCREEN_H

#include <QWidget>
#include <QMenu>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_plot_layout.h>

#include <memory>

#include <gsl/gsl_histogram.h>
#include "display_screen.h"
#include "display_screen_raster.h"
#include "src/buildblock/common.h"

#include "src/display_buildblock/screen_manager.h"

//! \bug There is currently a bug on the maximum bin. It is not getting printed on the
//! histogram.
class BarScreen : public QwtPlot
{
    Q_OBJECT

public:
    explicit BarScreen(QWidget *parent = 0, const int &_numBins = 16, const float &_cutOff = 0.2f);

    ~BarScreen();

    QVector<QwtIntervalSample>::iterator get_data_begin();
    QVector<QwtIntervalSample>::iterator get_data_end();

    void setNumBin(const int& _n);

    inline void setCutOff(const float& _n)
    {
        cutOff = _n/100.f;
        double ls, hs;
        //this->setAxisScale(QwtPlot::yLeft, 0.0, nz_max*cutOff);
        for ( int j = 0; j < numBins; j++ )
        {
            gsl_histogram_get_range(h, j, &ls, &hs);
            series[j].value = gsl_histogram_get(h, j) / (gsl_histogram_max_val(h)*cutOff);
        }
        d_histItem->attach(this);
        d_histItem->setData(new QwtIntervalSeriesData(series));

        float h_bin = (hs-ls)*0.5f;
        this->setAxisScale( QwtPlot::xBottom,0.0,
                            hs+h_bin);

        this->setAxisScale(QwtPlot::yLeft, -0.02, 1);

        this->replot();
    }

    inline float getMinMarker()const
    {
        return ll;
    }

    inline float getMaxMarker() const
    {
        return uu;
    }

    inline float getMinValue()const
    {
        return gsl_histogram_min(h);
    }

    inline float getMaxValue() const
    {
        return gsl_histogram_max(h);
    }

    virtual bool setImageData();

    virtual void move(QPoint to) = 0;

    virtual void moveMarkersby(QPointF dis) = 0;

    virtual void initialisePlotArea();

    QPointF getPointofBin(const int &_index);
    //! The the Point of the maximum non-Zero bin in the current
    //! histogram
    QPointF getMaxFrequencyPoint();
    //! Get the index of the first bin which does not correspond to the
    //! zero value and it is nonzero
    int getIndexOfFistNZBin();
    //! Get the Point of minimun frequency which is not the zero
    //! value
    QPointF getMinFrequencyPoint();

    double getBinValue(const int& _index);

    double getSum();

public slots:
    void unsetScreen(bool _state);

public slots:
    bool setScreen(QWidget *, const bool _global = false);

    virtual void replot_me() = 0;

signals:

    void markersUpdated(const float&, const float&);

    void WL(const float&, const float&);

protected:
    //! Intialises the histogram. Should be called when changes on the
    //! numBins or the data occur.
    void initialseHistogram();
    //! Update the histogram. Call this when data are changed.
    void updateHistogram();

    virtual void moveMarkers(const double &lp, const double &up)=0;

    Screen_manager* sc;
    //! Lower threshold
    qreal ll;
    //! Upper threshold
    qreal uu;

    std::unique_ptr<QPointF> minMarker, maxMarker;

    virtual void drawCurve() = 0;

    gsl_histogram * h;

    QwtPlotCurve * curve;

    bool unsetImageData();

    qint8 state;

    QwtPlotHistogram *d_histItem;

    QVector<QwtIntervalSample> series;

    QVector<QwtInterval> intervalA;

    int numBins;

    float cutOff;

    double def_min, def_max;

    double range, cc;

    QPoint cog;
    QPixmap cursor_pixmap;
    QwtSymbol *d;
    //! The max of all bins excluding the first bin, which includes the voxels with value 0
    float nz_max;
    //! The sum of all bins excluding the first bin, which includes the voxels with value 0
    float nz_sum;
    //! Start listening of events afther an image has been set.
    bool linked;

    bool globalHist;

    bool globalHistReady;
    //! Max Bin which has a non-zero value;
    int maxNZBin;
};

#endif // DISPLAY_SCREEN_H
