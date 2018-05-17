#include "barscreenhistogram.h"

BarScreenHistogram::BarScreenHistogram(QWidget *parent)
    :BarScreen(parent)
{
    initialisePlotArea();
}

void BarScreenHistogram::initialisePlotArea()
{
    BarScreen::initialisePlotArea();
    // axis
    this->setAxisTitle(QwtPlot::xBottom, "Pixel Value");
    this->setAxisTitle(QwtPlot::yLeft, "Index into colormap");

    curve->setPen(Qt::red,2.0);
    minMarker->setX(0);
    minMarker->setY(0);

    QSize total = this->size();
    width = 1/(total.width()*0.25);
}

void BarScreenHistogram::drawCurve()
{
    QVector<QPointF> points;
    points << *minMarker;
    points << *maxMarker;

    curve->setSamples(points);
    curve->attach(this);
    this->replot();
}

void
BarScreenHistogram::replot_me()
{
    updateHistogram();
    nz_sum = 0;
    double ls, hs;

    for ( int j = 0; j < numBins; j++ )
    {
        gsl_histogram_get_range(h, j, &ls, &hs);
        intervalA[j].setInterval(ls, hs, QwtInterval::IncludeBorders);
        double val = gsl_histogram_get(h, j);
        if (val > 0)
            maxNZBin = j;
        series[j] = QwtIntervalSample(val, intervalA[j]);
    }

    d_histItem->setData(new QwtIntervalSeriesData(series));

    float h_bin = (hs-ls)*0.5f;
    this->setAxisScale( QwtPlot::xBottom,0.0,
                        hs+h_bin);

    this->setAxisScale(QwtPlot::yLeft, -0.02, gsl_histogram_max_val(h)*cutOff);

    maxMarker->setY(gsl_histogram_max_val(h)*cutOff);
    d_histItem->attach(this);
    this->replot();
}

bool BarScreenHistogram::event( QEvent *ev )
{
    if (linked)
    {
        switch( ev->type() )
        {
        case QEvent::MouseButtonPress:
        {
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( ev );
            if(mouseEvent->buttons() == Qt::LeftButton)
            {
                move( mouseEvent->pos());
            }
            break;
        }
        case QEvent::MouseMove:
        {
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( ev );
            if(mouseEvent->buttons() == Qt::LeftButton)
                move( mouseEvent->pos() );

            break;
        }
        default:
            break;
        }
    }

    QwtPlot::event(ev);
}


void
BarScreenHistogram::moveMarkersby(QPointF dis)
{
    qreal tmp_ll = minMarker->x() -  invTransform(Qt::XAxis,dis.rx());

    if (tmp_ll <=  gsl_histogram_min(h))
        return;

    if (tmp_ll >=  gsl_histogram_max(h))
        return;

    ll = tmp_ll;

    moveMarkers(ll, ll);
}

void
BarScreenHistogram::move(QPoint to)
{
    QPointF dis;
    dis.rx() = cog.rx() - to.rx();
    dis.rx() *= this->width;
    moveMarkersby(dis);
    cog = to;
}

void
BarScreenHistogram::moveMarkers(const double& lp, const double& up)
{
    minMarker->setX(lp);
    maxMarker->setX(lp);

    drawCurve();
    //sc->applyVizValuesToAllContainers(lp,uu ,false);
}

void BarScreenHistogram::setThreshold(const float& _f)
{
    ll = _f;
    moveMarkers(ll, ll);
}
