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

#include "bar_screen.h"
#include <qevent.h>

#include "src/display_buildblock/screen_manager_imagedata.h"

BarScreen::BarScreen(QWidget *parent, const int& _numBins,
                     const float& _cutOff) :
    QwtPlot(parent)
{
    numBins = _numBins;
    cutOff = _cutOff;
    globalHist = false;
    globalHistReady = false;
}

void BarScreen::initialisePlotArea()
{
    linked = false;
    h = NULL;
    sc =NULL;
    state = 0;

    curve = new QwtPlotCurve();
    curve->setZ(2);
    minMarker.reset(new QPointF());
    maxMarker.reset(new QPointF());

    d_histItem = new QwtPlotHistogram("Histogram");
    d_histItem->setRenderThreadCount(0);
    d_histItem->setZ(1);
    QBrush b(Qt::cyan);
    d_histItem->setBrush(b);
    setAutoFillBackground( true );
    this->canvas()->setPalette( QColor( "LemonChiffon" ) );

    this->plotLayout()->setAlignCanvasToScales( true );
    this->setAutoReplot(true);

#ifndef QT_NO_CURSOR
    cursor_pixmap = QPixmap(":resources/gimp-pro/images/gimp-tool-brightness-contrast.png");
    QCursor cursor_default = QCursor(cursor_pixmap, 0, 0);
    this->canvas()->setCursor( cursor_default);
#endif
}

void BarScreen::setNumBin(const int& _n)
{
    numBins = _n;
    initialseHistogram();
    replot_me();
}

void BarScreen::initialseHistogram()
{
    gsl_histogram_free(h);
    series.resize(numBins);
    intervalA.resize(numBins);
    h = gsl_histogram_alloc(numBins);
}

bool BarScreen::setScreen(QWidget* _sc, const bool _global)
{
    sc = dynamic_cast<Screen_manager*>(_sc);
    globalHist = _global;
    globalHistReady = false;
    setImageData();
    linked = true;
}

bool BarScreen::setImageData()
{
    initialseHistogram();
    moveMarkers(*sc->getActiveDisplayScreen()->getScreen()->get_max_vis_value_ptr(),
                *sc->getActiveDisplayScreen()->getScreen()->get_max_vis_value_ptr());
    this->setAxisScale( QwtPlot::xBottom,ll,uu);
    replot_me();
    drawCurve();
}

void BarScreen::unsetScreen(bool _state)
{
    linked = _state;
}

bool BarScreen::unsetImageData()
{
    gsl_histogram_free(h);
    h=NULL;
}

BarScreen::~BarScreen()
{
    if(h != NULL)
        gsl_histogram_free (h);
}

void
BarScreen::updateHistogram()
{
    if ( sc->get_min_value_all() >=
         sc->get_max_value_all())
        return;

    if(!globalHist)
    {
        gsl_histogram_set_ranges_uniform (h,
                                          sc->get_min_value_all(),
                                          sc->get_max_value_all());

        for(QVector<double>::ConstIterator cur =
            sc->getActiveDisplayScreen()->getScreen()->get_data_const_begin();
            cur != sc->getActiveDisplayScreen()->getScreen()->get_data_const_end(); ++cur)
            gsl_histogram_increment (h, *cur);
    }
    else
    {
        if (!globalHistReady)
        {
            gsl_histogram_set_ranges_uniform (h,
                                              sc->get_min_value_all(),
                                              sc->get_max_value_all());

            for(stir::VoxelsOnCartesianGrid<float>::full_iterator cur =
                dynamic_cast<Screen_manager_ImageData* >(sc)->getData_ptr()->begin_all();
                cur != dynamic_cast<Screen_manager_ImageData* >(sc)->getData_ptr()->end_all();
                ++cur)
            {
                gsl_histogram_increment (h, *cur);
            }

            globalHistReady = true;
        }
    }
}

QPointF BarScreen::getPointofBin(const int& _index)
{
    if (_index > numBins)
        return QPointF(-1, -1);

    double ls, hs;
    gsl_histogram_get_range(h, _index, &ls, &hs);
    float x_bin = ls + (hs-ls)*0.5f;
    float y_bin = gsl_histogram_get(h,_index);

    return QPointF(x_bin, y_bin);
}

QPointF BarScreen::getMaxFrequencyPoint()
{
    int _index = maxNZBin;
    return getPointofBin(_index);
}

QPointF BarScreen::getLastBinPoint()
{
    int _index = 0;

    bool locked = false;

    for ( int j = 0; j < numBins; j++ )
    {
        double val = gsl_histogram_get(h, j);
        if(val == 0.0 && !locked)
        {
            _index = j-1;
            locked = true;
        }
        if(val > 0.0)
        {
            locked = false;
        }
    }

    return getPointofBin(_index);
}

double BarScreen::getBinValue(const int& _index)
{
    return gsl_histogram_get(h, _index);
}

double BarScreen::getSum()
{
    return gsl_histogram_sum(h);
}
