#include "barscreenwl.h"

BarScreenWL::BarScreenWL(QWidget *parent)
    :BarScreen(parent)
{
    initialisePlotArea();
}

void BarScreenWL::initialisePlotArea()
{
    BarScreen::initialisePlotArea();
    // axis
    this->setAxisTitle(QwtPlot::xBottom, "Pixel Value");
    this->setAxisTitle(QwtPlot::yLeft, "Frequency");

    curve->setPen(Qt::red);
    curve->setSymbol(new QwtSymbol(QwtSymbol::Ellipse,QBrush(Qt::green),QPen(Qt::black),QSize(10,10)));
    minMarker->setY(0);
    maxMarker->setY(1);
}

void
BarScreenWL::move(QPoint to)
{
    QPointF dis;
    QSize total = this->size();

    dis.ry() = cog.ry() - to.ry();
    dis.ry() /= (total.height()*0.25);

    dis.rx() = cog.rx() - to.rx();
    dis.rx() /= (total.width()*0.25);

    moveMarkersby(dis);
    cog = to;
}

void
BarScreenWL::moveMarkersby(QPointF dis)
{
    cc = (maxMarker->x() + minMarker->x())*0.5;
    qreal tmp_ll = (minMarker->x() + (cc - minMarker->x()) * dis.ry())+dis.rx()*cc;
    qreal tmp_uu = (maxMarker->x() + (cc - maxMarker->x()) * dis.ry()) +dis.rx()*cc;

    if (tmp_ll <=  gsl_histogram_min(h))
        return;

    if (tmp_uu >=  gsl_histogram_max(h))
        return;

    double increment = 0.0;
    gsl_histogram_increment(h, increment);

    if ((tmp_uu - tmp_ll) < increment)
        return;

    ll = tmp_ll;
    uu = tmp_uu;

    moveMarkers(ll, uu);
    //    MY_DATA->intervalChanged(ll, uu,false);
}



void BarScreenWL::setWindow(const float& _m)
{
    minMarker->setX(level - _m*0.5);
    maxMarker->setX(level + _m*0.5);
    uu = maxMarker->x();
    ll = minMarker->x();
    drawCurve();
    emit markersUpdated(ll, uu);
}

void BarScreenWL::setLevel(const float& _m)
{
    minMarker->setX(_m - window*0.5);
    maxMarker->setX(_m + window*0.5);
    uu = maxMarker->x();
    ll = minMarker->x();
    drawCurve();
    emit markersUpdated(ll, uu);
    //emit WL(window, level);
    //    sc->applyVizValuesToAllContainers(ll, uu,false);
}


void BarScreenWL::drawCurve()
{
    QVector<QPointF> points;
    points << QPointF(gsl_histogram_min(h), 0);
    points << *minMarker;
    points << *maxMarker;
    points << QPointF(gsl_histogram_max(h), 1);

    curve->setSamples(points);
    curve->attach(this);

    level = (minMarker->x() + maxMarker->x())*0.5f;
    window = (maxMarker->x() - minMarker->x());
    this->replot();
}

void
BarScreenWL::replot_me()
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

        if (j > 0)
            nz_sum += val;

        val /= (gsl_histogram_max_val(h)*cutOff);

        series[j] = QwtIntervalSample(val, intervalA[j]);
    }

    d_histItem->setData(new QwtIntervalSeriesData(series));

    this->setAxisScale(QwtPlot::yLeft, -0.02, 1);

    d_histItem->attach(this);
    this->replot();
}

bool BarScreenWL::event( QEvent *ev )
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
                this->setCOG( mouseEvent->pos() );
            }
            if(mouseEvent->buttons() == Qt::MiddleButton)
            {
                this->resetCOG();
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
        case QEvent::MouseButtonRelease:
        {

            break;
        }
        default:
            break;
        }
    }
    QwtPlot::event(ev);
	return true;
}

void
BarScreenWL::moveMarkers(const double& lp, const double& up)
{
    ll = lp;
    uu = up;
    minMarker->setX(ll);
    maxMarker->setX(uu);

    drawCurve();
    emit markersUpdated(ll, uu);
    emit WL(window, level);
    sc->applyVizValuesToAllContainers(ll, uu);
}

void BarScreenWL::setMinMarker(const float& _m)
{
    ll = _m;
    minMarker->setX(ll);
    drawCurve();
    emit markersUpdated(ll, uu);
    emit WL(window, level);
    sc->applyVizValuesToAllContainers(ll, uu);
}

void BarScreenWL::setMaxMarker(const float& _m)
{
    uu = _m;
    maxMarker->setX(uu);
    drawCurve();
    emit markersUpdated(ll, uu);
    emit WL(window, level);
    sc->applyVizValuesToAllContainers(ll, uu);
}

void
BarScreenWL::resetCOG(int _state)
{
    bool unsetInterval = false;

    if (_state > -1)
        state = _state;

    if (state == 0)
    {
        fullContrast();
        unsetInterval = true;
        state++;
    }
    else if(state > 0)
    {
        optimumContrast();
        state++;
    }

    if (state == 5)
        state = 0;

    ll = minMarker->x();
    uu = maxMarker->x();

    this->moveMarkers(ll, uu);
}

void
BarScreenWL::setCOG(QPoint _cog)
{
    cog = _cog;
    state = 0;
}

void BarScreenWL::fullContrast()
{
    minMarker->setX(sc->get_min_value_all());
    maxMarker->setX(sc->get_max_value_all());
}

void BarScreenWL::optimumContrast()
{
    double sum = nz_sum;
    double thres = (0.05*state);
    double fifth  = sum * thres;
    double nineFifth = sum * (1.0-thres);
    double acc = 0.0;
    double lc = 0.0, hc = 0.0;

    for ( int j = 1; j < numBins; j++ )
    {
        acc += gsl_histogram_get(h,j);

        if (acc >= fifth && lc ==0 )
        {
            gsl_histogram_get_range(h,j,&lc, &hc);
            minMarker->setX(lc);
        }
        if (acc >= nineFifth)
        {
            gsl_histogram_get_range(h,j,&lc, &hc);
            maxMarker->setX(hc);
            return;
        }
    }
    // Default here...
    gsl_histogram_get_range(h,numBins-1,&lc, &hc);
    maxMarker->setX(lc);
}
