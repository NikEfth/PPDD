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

#include "display_screen_raster.h"

#include <QtAlgorithms>
#include <QAction>
#include <QFileDialog>
#include <QMenu>

#include "stir/is_null_ptr.h"

#if __has_include("Board.h")
#include "Board.h"
#include "board/Color.h"
#endif

#include <qwt_color_map.h>

display_screen_raster::display_screen_raster(QWidget *parent) :
    display_screen(parent)
{
    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount(0);
    d_spectrogram->setCachePolicy(QwtPlotRasterItem::CachePolicy::NoCache);

    myColorMap = new Viewer::ColorMap();
    d_spectrogram->setColorMap(myColorMap);

    this->enableAxis(QwtPlot::xBottom, false);
    this->enableAxis(QwtPlot::yLeft, false);

    d_rescaler = new QwtPlotRescaler(this->canvas(),QwtPlot::xBottom, QwtPlotRescaler::Fixed);
    d_rescaler->setExpandingDirection(QwtPlot::yLeft, QwtPlotRescaler::ExpandBoth);
    this->setCanvasBackground(QBrush(myColorMap->get_background()));

    p_raster = new QwtMatrixRasterData();
    d_spectrogram->setData( p_raster);
    d_spectrogram->attach(this);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    title.setColor(myColorMap->get_peak_color());

    curveA = new QwtPlotCurve();
    curveA->setPen( Qt::blue,1,Qt::DashLine );

    curveB = new QwtPlotCurve();
    curveB->setPen( Qt::blue,1,Qt::DashLine );
}

display_screen_raster::~display_screen_raster()
{
    for(QwtPlotItem* d : this->itemList(QwtPlotItem::Rtti_PlotShape))
    {
        d->detach();
    }
    for(QwtPlotItem* d : this->itemList(QwtPlotItem::Rtti_PlotCurve))
    {
        d->detach();
    }
}

void
display_screen_raster::set_sizes(const int& _h, const int& _v,
                                 const int& _min_index_h, const int& _min_index_v,
                                 const int& _max_index_h, const int& _max_index_v,
                                 const float& _h_spacing, const float& _v_spacing,
                                 const float& min_x, const float& max_x,
                                 const float& min_y, const float& max_y)
{
    row_size = _h;
    col_size = _v;
    h_spacing = _h_spacing;
    v_spacing = _v_spacing;
    inv_h_spacing = 1.f / h_spacing;
    inv_v_spacing = 1.f / v_spacing;
    min_index_h = _min_index_h;
    min_index_v = _min_index_v;
    max_index_h = _max_index_h;
    max_index_v = _max_index_v;
    my_data.resize(row_size*col_size);

    pixelHint.setHeight(h_spacing);
    pixelHint.setWidth(v_spacing);

    p_raster->setInterval( Qt::YAxis, QwtInterval( static_cast<double>(min_y),static_cast<double>(max_y),QwtInterval::ExcludeBorders ) );
    p_raster->setInterval( Qt::XAxis, QwtInterval(static_cast<double>(min_x),static_cast<double>(max_x),QwtInterval::ExcludeBorders ) );
    setAxisScale(QwtPlot::xBottom, min_x, max_x,h_spacing);
    d_rescaler->setIntervalHint(Qt::YAxis,QwtInterval( static_cast<double>(min_y),static_cast<double>(max_y),QwtInterval::ExcludeBorders ));
    d_rescaler->setIntervalHint(Qt::XAxis,QwtInterval(static_cast<double>(min_x),static_cast<double>(max_x),QwtInterval::ExcludeBorders ));
    setAxisScale(QwtPlot::yLeft, min_y, max_y,v_spacing);
    p_raster->pixelHint(pixelHint);
}

void
display_screen_raster::setMyTitle(const qint32& label)
{
    my_label = label;

    title.setText(QString::number(my_label));
    titleItem->detach();
    titleItem->setText( title );
    titleItem->attach( this );
}

void
display_screen_raster::setOCD(const QString& _l, const QString& _r,
                              const QString& _t, const QString& _b)
{
    lOCD.setText(_l);
    rOCD.setText(_r);
    tOCD.setText(_t);
    bOCD.setText(_b);

    lOCDItem->detach();
    lOCDItem->setText( lOCD );
    lOCDItem->attach( this );

    rOCDItem->detach();
    rOCDItem->setText( rOCD );
    rOCDItem->attach( this );

    tOCDItem->detach();
    tOCDItem->setText( tOCD );
    tOCDItem->attach( this );

    bOCDItem->detach();
    bOCDItem->setText( bOCD );
    bOCDItem->attach( this );
}

void display_screen_raster::setColorMap(const qint16& index)
{
    switch (index) {
    case 0:
        myColorMap->set_BW();
        break;
    case 1:
        myColorMap->set_WB();
        break;
    case 2:
        myColorMap->set_Viridis();
        break;
    case 3:
        myColorMap->set_qwt();
        break;
    case 4:
        myColorMap->set_JET();
        break;
    default:
        break;
    }
    d_spectrogram->setColorMap(myColorMap);

    title.setColor(myColorMap->get_peak_color());
    lOCD.setColor(myColorMap->get_peak_color());
    rOCD.setColor(myColorMap->get_peak_color());
    tOCD.setColor(myColorMap->get_peak_color());
    bOCD.setColor(myColorMap->get_peak_color());
    setCanvasBackground(QBrush(myColorMap->get_background()));

    replot();
}

void
display_screen_raster::replot_me()
{
    if (filter != 0)
        filter->apply(my_data);

    if(cursorStatus)
        redrawCursor();

    p_raster->setValueMatrix(my_data, row_size);
    p_raster->setInterval( Qt::ZAxis, QwtInterval(vis_min, vis_max));
    d_spectrogram->setData(p_raster);

    replot();
}

void display_screen_raster::setInterval(const double& _min, const double& _max)
{
    vis_min = _min;
    vis_max = _max;
}

void display_screen_raster::keyPressEvent( QKeyEvent * keyEvent )
{
    //    if (curveA)
    //    {
    //        QVector<QPointF> n_selection_points;

    //        switch( keyEvent->key() )
    //        {
    //        case Qt::Key_Up:
    //        {
    //            QPointF d(0, pixelHint.height());


    //            for(QPointF i : selection_points)
    //            {
    //                n_selection_points.append(QPointF(i + d));

    //                if (n_selection_points.last().y() >= fabs(p_raster->interval(Qt::YAxis).maxValue()))
    //                    return;
    //            }
    //        }
    //           break;
    //        case Qt::Key_Down:
    //        {
    //            QPointF d(0, -pixelHint.height());

    //            for(QPointF i : selection_points)
    //            {
    //                n_selection_points.append(QPointF(i + d));

    //                if (n_selection_points.last().y() <= fabs(p_raster->interval(Qt::YAxis).minValue()))
    //                    return;
    //            }
    //        }
    //            break;

    //        case Qt::Key_Right:
    //        {
    //            QPointF d(pixelHint.width(), 0);

    //            for(QPointF i : selection_points)
    //            {
    //                n_selection_points.append(QPointF(i + d));

    //                if (n_selection_points.last().x() >= fabs(p_raster->interval(Qt::XAxis).maxValue()))
    //                    return;
    //            }
    //        }
    //            break;

    //        case Qt::Key_Left:
    //        {
    //            QPointF d(-pixelHint.width(), 0);
    //            for(QPointF i : selection_points)
    //            {
    //                n_selection_points.append(QPointF(i + d));

    //                if (n_selection_points.last().x() <= fabs(p_raster->interval(Qt::XAxis).minValue()))
    //                    return;
    //            }
    //        }
    //            break;
    //        }

    //        clearSelections();
    //        drawSelection(n_selection_points);
    //    }

    //    //    keyEvent->ignore();
}

Viewer::SimpleVector2D<int> display_screen_raster::drawCursor(const bool &_s, const QPointF& ps)
{
    clearCursors();

    if (cursorStatus)
    {
        double x[2];
        double y[2];

        x[0] = ps.x();
        y[0] = d_spectrogram->interval(Qt::YAxis).minValue();
        x[1] = ps.x();
        y[1] = d_spectrogram->interval(Qt::YAxis).maxValue();

        curveA->setSamples( x, y, 2 );
        curveA->attach( this );

        x[0] = d_spectrogram->interval(Qt::XAxis).minValue();
        y[0] = ps.y();
        x[1] = d_spectrogram->interval(Qt::XAxis).maxValue();
        y[1] = ps.y();

        curveB->setSamples( x, y,2);
        curveB->attach( this );
    }

    this->replot();

    Viewer::SimpleVector2D<int> _v;
    cellFromXY(ps, _v);
    return _v;
}

void display_screen_raster::drawCursor(const bool &_s, const Viewer::SimpleVector2D<int>& ps)
{
    cursorStatus = _s;
    QPointF _local;
    xyFromCell(ps, _local);
    drawCursor(_s, _local);
}

void display_screen_raster::redrawCursor()
{
    curveA->attach( this );
    curveB->attach( this );
}

void display_screen_raster::clearCursors()
{
    if(!stir::is_null_ptr(curveA))
        curveA->detach();
    if(!stir::is_null_ptr(curveB))
        curveB->detach();
}

void display_screen_raster::xyFromCell(const Viewer::SimpleVector2D<int>& _v, QPointF& _p)
{
    _p.setX(h_spacing*_v.x);
    _p.setY(v_spacing*_v.y);
}

void display_screen_raster::cellFromXY(const QPointF &_p, Viewer::SimpleVector2D<int>& _v)
{
    _v.x = stir::round(_p.x() * inv_h_spacing);
    _v.y = stir::round(_p.y() * inv_v_spacing);
}

void display_screen_raster::writeValuesInData(const stir::ProjMatrixElemsForOneBin & _p, const float& _v)
{
    stir::ProjMatrixElemsForOneBin::const_iterator it = _p.begin();

    QVector<double> _d = my_data;

    for(; it != _p.end(); ++it)
    {
        if (it->get_coords()[3] < min_index_h ||
                it->get_coords()[3] > max_index_h ||
                it->get_coords()[2] < min_index_v ||
                it->get_coords()[2] > max_index_v)
            continue;

        int i = (it->get_coords()[3] -min_index_h) + (it->get_coords()[2]-min_index_v)*row_size;
        _d[i-1] = it->get_value();
    }
    p_raster->setValueMatrix(_d, row_size);
    d_spectrogram->setData(p_raster);

    replot();
}

void display_screen_raster::writeValuesInData(const stir::PixelsOnCartesianGrid<float> &_p)
{
    stir::PixelsOnCartesianGrid<float>::const_full_iterator it = _p.begin_all();

    QVector<double> _d = my_data;
    QVector<double>::iterator _it = _d.begin();

    for(; it != _p.end_all(); ++it, ++_it)
        *_it = *it;

    p_raster->setValueMatrix(_d, row_size);
    d_spectrogram->setData(p_raster);

    replot();
}


void display_screen_raster::popUpMenu(const QPoint &pos)
{
    QMenu myMenu;
    QAction *act_saveAs = myMenu.addAction("Save as array");
    act_saveAs->setIcon(QIcon::fromTheme("document-save-as.png"));

    QAction * act_saveAsImage = myMenu.addAction("Save as image");
    act_saveAsImage->setIcon(QIcon::fromTheme("image-x-generic.png"));

    QAction * act_saveAsPDF = myMenu.addAction("Save as PDF");
    act_saveAsPDF->setIcon(QIcon::fromTheme("viewpdf.png"));

#if __has_include("Board.h")
    QAction * act_saveAsVector = myMenu.addAction("Save as Vector");
    act_saveAsVector->setIcon(QIcon::fromTheme("im.vector.svg"));
#endif
    myMenu.addSeparator();

    QAction * act_popOut = myMenu.addAction("Copy");
    act_popOut->setIcon(QIcon(":/resources/gimp-pro/images/gtk-copy.png"));

    QObject::connect(act_saveAs, &QAction::triggered, [=]()
    {emit save_this_array(my_label);}
    );
#if __has_include("Board.h")
    QObject::connect(act_saveAsVector, &QAction::triggered, [=]()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save vector"), QDir::home().absolutePath(), tr("Scalable images (*.eps *.svg *.fig)"));

        LibBoard::Board board;
        std::vector<LibBoard::Color> coloured_data(size);

        QVector<double>::iterator in_b = my_data.begin();
        const QVector<double>::iterator in_e = my_data.end();

        std::vector<LibBoard::Color>::iterator out_b = coloured_data.begin();

        QwtInterval range(min_value, max_value);

        for( ; in_b!=in_e; ++in_b, ++out_b)
        {
            QRgb nn = myColorMap->rgb( range, *in_b);
            LibBoard::Color dn(nn);
            *out_b = dn;
        }

        board.drawArray(coloured_data, row_size, col_size, h_spacing, v_spacing);

        if(fileName.contains(".eps"))
            board.saveEPS(fileName.toLocal8Bit());
        else if (fileName.contains(".svg"))
            board.saveSVG(fileName.toLocal8Bit());
        else if (fileName.contains(".fig"))
            board.saveFIG(fileName.toLocal8Bit());
    }
    );
#endif
    QObject::connect(act_saveAsImage, &QAction::triggered, [=]()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::home().absolutePath(), tr("Images (*.png *.tiff *.jpg)"));

        if (fileName.size() == 0)
            return;

        QwtPlotRenderer renderer;

        renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground , true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground , true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasFrame , true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardTitle , true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasFrame, true);

        qint32 w = this->canvas()->width();
        qint32 h = this->canvas()->height();
        QPixmap pixmap(w, h);

        renderer.renderTo(this, pixmap );

        if(fileName.contains(".png"))
            pixmap.save(fileName, "PNG");
        else if(fileName.contains("jpg"))
            pixmap.save(fileName,"JPG");
        else if(fileName.contains("gif"))
            pixmap.save(fileName,"GIF");

    });

    QObject::connect(act_saveAsPDF, &QAction::triggered, [=]()
    {
        QString fileName;
        QwtPlotRenderer renderer;
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground , true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground , true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasFrame , true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardTitle , true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasFrame, true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardFooter, true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend, true);

        renderer.exportTo(this, fileName );
    });

    myMenu.exec(this->mapToGlobal(pos));
}

stir::CartesianCoordinate3D<float>
display_screen_raster::getGridSpacing()
{
    return stir::CartesianCoordinate3D<float>(1, h_spacing, v_spacing);
}
