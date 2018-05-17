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

#include "src/buildblock/common.h"
#include "src/tools_buildblock/worker.h"

#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QVector3D>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_plot_rescaler.h>
#include <qwt_plot_renderer.h>
#include <qwt_painter.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_textlabel.h>

#include <memory>

#include "stir/recon_buildblock/ProjMatrixElemsForOneBin.h"
#include "stir/PixelsOnCartesianGrid.h"

class Tool;

class display_screen : public QwtPlot
{
    Q_OBJECT

public:
    explicit display_screen(QWidget *parent = 0);

    ~display_screen();

    virtual void replot_me() = 0;

    int get_data_size();

    virtual void setOCD(const QString& _l, const QString& _r,
                        const QString& _t, const QString& _b) = 0;

    virtual void setMyTitle(const qint32 &_label) = 0;
    //! Remove every curve is on the screen.
    void clearCurves();
    //! Remove the cross section which indicates the
    //! current position of the cursor.
    virtual void clearCursors() = 0;

    inline void set_active(bool state);

    virtual void mousePressEvent(QMouseEvent *event);

    virtual void keyPressEvent( QKeyEvent * event );

    inline void setAddressAtParent(const qint32& id);

    virtual void setInterval(const double &_min, const double &_max) = 0;

    virtual QVector<double>::ConstIterator get_data_const_begin() const = 0;
    virtual QVector<double>::ConstIterator get_data_const_end() const = 0;

    inline const double* get_max_vis_value_ptr() const;

    inline const double* get_min_vis_value_ptr() const;

    virtual void drawCursor(const bool &_s, const Viewer::SimpleVector2D<int>& ps) = 0;

    virtual Viewer::SimpleVector2D<int> drawCursor(const bool &_s, const QPointF& ps) = 0;

    virtual void xyFromCell(const Viewer::SimpleVector2D<int> &, QPointF&) = 0;

    virtual void cellFromXY(const QPointF& _p, Viewer::SimpleVector2D<int>& _v) = 0;
    //! Change the value of particular pixels in the raster.
    //! This function initially is make to test the location of the profiles
    //! In the future might become a fully operational fuction.
     virtual void writeValuesInData(const stir::ProjMatrixElemsForOneBin &, const float& ) = 0;

    virtual void writeValuesInData(const stir::PixelsOnCartesianGrid<float> &) = 0;

    void applyFunction(Worker *_w)
    {filter = _w;}

    void removeFunction()
    {filter = 0;}

    //! Return the Pixel hint (or grid spacing of the data)
    virtual stir::CartesianCoordinate3D<float> getGridSpacing() = 0;

signals:

    void activateScreen(const qint32& );

protected slots:

    virtual void popUpMenu(const QPoint &pos);

protected:

    //! Draw a frame around the display to indicate that this is the active
    inline void drawSelectionFrame(bool state);
    //! The data which are displayed on screen
    QVector<double> my_data;

    qint32 myAddress;
    //! The number displayed at the bottom right corner. In sync with the scroll bar.
    qint32 my_label;

    int size;

    bool cursorStatus;

    QwtPlotTextLabel *titleItem,*lOCDItem, *rOCDItem, *bOCDItem, *tOCDItem;
    //! The font used for the OCD text
    QFont font;
    //! Text for the OCD texts
    QwtText title, lOCD, rOCD, bOCD, tOCD;
    //! Max value of the display data
    double max_value;
    //! Min value of the display data
    double min_value;
    //! Apparent min value - W&L operations set this value
    double vis_min;
    //! Apparent max value - W&L operations set this value
    double vis_max;
    //! This is a somewhat agly workaround. When a function has to be applied on the
    //! my_data QVector in order to preview the effect of a filter.
    Worker *filter;
};

//#include "display_screen.inl"
#endif // DISPLAY_SCREEN_H
