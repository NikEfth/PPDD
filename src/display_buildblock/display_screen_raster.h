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

#ifndef DISPLAY_SCREEN_RASTER_H
#define DISPLAY_SCREEN_RASTER_H

#include "display_screen.h"
#include <qwt_plot_panner.h>

class display_screen_raster : public display_screen
{
    Q_OBJECT

public:
    explicit display_screen_raster(QWidget *parent = 0);
    ~display_screen_raster();

    void replot_me();

    void set_sizes(const int& _h, const int& _v,
                   const int &_min_index_h, const int &_min_index_v,
                   const int &_max_index_h, const int &_max_index_v,
                   const float& _h_spacing, const float& _v_spacing,
                   const float& min_x, const float& max_x,
                   const float& min_y, const float& max_y);

    void draw_curve(const QVector<QPointF>&);

    virtual void setOCD(const QString& _l, const QString& _r,
                        const QString& _t, const QString& _b);

    virtual void setMyTitle(const qint32 &_label);

    inline void set_max_value(const double& new_max);

    inline void set_min_value(const double& new_min);

    inline void set_min_max_values(const double& new_min, const double& new_max);

    inline QVector<double>::iterator get_data_begin();

    inline QVector<double>::iterator get_data_end();

    inline QVector<double>::ConstIterator get_data_const_begin() const;

    inline QVector<double>::ConstIterator get_data_const_end() const;

    inline const double get_max_value_ptr() const;

    inline const double get_min_value_ptr() const;

    //! Sets the vis_min and vis_max values. Essentially this affects the W&L operations
    //! on the display.
    void setInterval(const double &_min, const double &_max);

    void keyPressEvent(QKeyEvent * keyEvent ) override;

    void drawCursor(const bool &_s, const Viewer::SimpleVector2D<int> &ps);

    Viewer::SimpleVector2D<int> drawCursor(const bool& _s, const QPointF &ps);

    void xyFromCell(const Viewer::SimpleVector2D<int> &, QPointF &);

    void cellFromXY(const QPointF& _p, Viewer::SimpleVector2D<int>& _v);

    virtual void writeValuesInData(const stir::ProjMatrixElemsForOneBin &, const float& );

    virtual void writeValuesInData(const stir::PixelsOnCartesianGrid<float> &);

    virtual stir::CartesianCoordinate3D<float> getGridSpacing();

    void clearCursors();
    //! Show / Hide axis
    inline void showAxis(const bool& _d)
    {
        this->enableAxis(QwtPlot::xBottom, _d);
        this->enableAxis(QwtPlot::yLeft, _d);
    }

public slots:

    void setColorMap(const qint16&);

signals:

    void save_this_array(int);

protected slots:

    virtual void popUpMenu(const QPoint &pos) override;

private:
    //! On some occations the cursors might be deleted,
    //! call this to attach again.
    void redrawCursor();

    QwtPlotSpectrogram *d_spectrogram;

    QwtMatrixRasterData *p_raster;

    QwtPlotRescaler *d_rescaler;

    QwtPlotPanner *d_panner;

    QRectF pixelHint;

    QwtPlotCurve* curveA, *curveB;

    int row_size;

    int col_size;
    //! Pixel spacing, Horizontal, this will follow the convension in the header
    float h_spacing;
    //! Pixel spacing, Vertical, this will follow the convensions in the header
    float v_spacing;

    float inv_h_spacing;

    float inv_v_spacing;
    //! The minimum pixel index along the x axis (horizontally)
    int min_index_h;
    //! The minimum pixel index along the y axis (vertically)
    int min_index_v;
    //! The maximum pixel index along the x axis (horizontally)
    int max_index_h;
    //! The maximum pixel index along the y axis (vertically)
    int max_index_v;
    //! Current ColorMap
    Viewer::ColorMap *myColorMap;
};

#include "display_screen.inl"

#endif // DISPLAY_SCREEN_H
