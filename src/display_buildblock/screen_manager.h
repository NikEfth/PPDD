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

#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <QWidget>
#include <QMessageBox>
#include <QVector3D>

#include "display_screen_container.h"
#include "src/buildblock/configuration.h"
#include "src/buildblock/dialog.h"

#include "stir/Sinogram.h"
#include "stir/Viewgram.h"

#include "stir/SegmentBySinogram.h"
#include "stir/SegmentByView.h"

#include "stir/IO/interfile.h"
#include "stir/CartesianCoordinate3D.h"
#include "stir/Bin.h"
#include "stir/IO/interfile.h"

#include "stir/is_null_ptr.h"
#include "qwt_plot_picker.h"

using namespace stir;

namespace Ui {
class Screen_manager;
}

class Screen_manager : public QWidget
{
    Q_OBJECT

public:
    explicit Screen_manager(Configuration* _curConfig, QWidget *parent);

    ~Screen_manager();

    virtual bool loadFile(const QString fileName) = 0;

    //! Returns the currently active container.
    display_screen_container* getActiveDisplayScreen() const;

    virtual QSize setOrientation(Orientation _o = Next) = 0;
    //! Get the myFullFileName
    inline QString getFullFilePath() const;

    inline const QString* getFullFilePath_ptr() const;
    //! Set the myFullFileName
    inline void setFullFilePath(const QString&);
    //! Returns imageData or projData.
    virtual QString getDataType() const = 0;
    //! Returns only the fileName.
    inline QString getFileName() const;
    //! Returnes the index of the current ColorMap.
    inline qint16 getColorMapIndex() const;
    //! \obsolete
    inline qint16 getViewportsIndex() const;
    //! The current position of the currentCursor, overloaded function
    //! returning SimpleVector3D<int>
    inline SimpleVector3D<int>* getCurrentCursor() const;
    //! The current position of the currentCursor, overloaded function
    //! returning CartesianCoordinate3D<int>
    inline CartesianCoordinate3D<int> getCurrentCursorCartesianCoordinate();
    //! The current position of the currentCursor
    virtual  CartesianCoordinate3D<float>  getCurrentCursorInmm() = 0;
    //! Returns the current display layout
    inline const Orientation* getDisplayLayout() const;
    //! \obsolete
    void set_num_viewports(qint16 num);

    inline void setGrouped(bool);

    inline bool isGrouped() const;

    inline bool getAxisStatus() const;

    bool operator==(const Screen_manager &other) const;

    bool operator!=(const Screen_manager &other) const;

    Configuration* get_configuration() const;

    virtual VoxelsOnCartesianGrid<float> getSlice(const int& num, const Orientation& o = Transverse)
    {
		VoxelsOnCartesianGrid<float> ret;
		return ret;
	}

    void block_activity(bool state);
    //! Return the minimum value of the entire dataset.
    inline double get_min_value_all() const;
    //! Return the maximum value of the entire dataset.
    inline double get_max_value_all() const;

    virtual int getNumOfArrays() = 0;

    virtual Array<2, float> getArray2D(const int&) const = 0;

    virtual void setArray2D(const Array<2,float>&, const int&) = 0;

    virtual void display() = 0;

    void set_multiple_selections_allowed(bool state);

    QVector<std::shared_ptr<display_screen_container> > my_containers;
    //! Display a blue cross on the location of currentCursor
    virtual void showCursor(const bool& state) = 0;

    inline bool getCursorStatus() const;

    double getCurrentValue();

    virtual const IndexRange<3> getRanges3D() = 0;
    //! Write data to disk using STIR functions.
    virtual void writeToDisk(const QString&) = 0;
    //! Set the viz_min and viz_max values at all containers.
    virtual void applyVizValuesToAllContainers(const double& _min, const double& _max) = 0;

    virtual void applyFunctionToAllContainers(Worker *_w) = 0;

    virtual void removeFunctionToAllContainers() = 0;

    virtual VoxelsOnCartesianGrid<float>* getData_ptr() =0;

    virtual std::shared_ptr<VoxelsOnCartesianGrid<float> > getData_sptr() =0;
    //! Show / hide axis
    void showAxis(const bool& _s);

signals:
    void aboutToClose();

    void scrolledTo(qint16);

    void activeScreenUpdated();

    //!
    //! \brief segmentChangedTo
    //! This signal is emitted when the segment changes,
    //! in order to notify the rest of the group.
    void segmentChangedTo(QString);

    void setColorMap(const qint16&);

    void updatedContainer();

public slots:

    virtual void save_as_array(int this_label) = 0;

    void remote_scrbr_valueChanged(const qint16 &value);

    void remote_relative_scrbr_valueChanged(qint32 value);

    virtual void onCursorChanged() = 0;

    virtual void _setCursor(const QPoint&, QwtPlot*) = 0;

    virtual void _setCursor(const Viewer::SimpleVector3D<int>& ) = 0;

    virtual void updateCursor(display_screen_container* d = 0) = 0;
    //! It will recalculate the minimum and maximum values and
    //! replot the screens.
    void updateAll();
    //! Calls display_screen_container::updatePlotArea for every
    //! my_container. Effectively the data will be reread.
    void rePlotPlots();
    //! This is a ligher version  of rePlotPlots(), where the plots are
    //! replotted without rereading the data.
    void refreshPlots();

protected slots:  

    void setActiveScreenContainer(const qint32 &);

protected:

    Ui::Screen_manager *ui;

    virtual void set_up_plot_area();

    virtual double getValueAt(const SimpleVector3D<int> _c) = 0;

    void closeEvent(QCloseEvent *event) override;

    QString myFullFileName;
    //! The current Orientation on display. Since Orientation::All
    Orientation viewOrder;
    //! Configuration doesn't do a lot, yet.
    Configuration* curConfig;
    //! The current position it is shared as a pointer in the display_screen_container.
    SimpleVector3D<int> *currentCursor;
    //! This is the active display_screen_container.
    display_screen_container* activeScreenContrainer;
    //! Initializes the display_screen_container. /todo The default 3 must change to zero.
    void setUpContainers(int _num = 3);
    //! Erase the display_screen_container. Since I keep all views in memory this
    //! might be \obsolete.
    void clearContainers();
    //! It indicates wether the blue cross is printed on the displays.
    bool cursorStatus;

    bool axisStatus;
    //! True if the containers have been initialized properly
    bool containersReady;
    //! The minimum value of the entire dataset.
    double minValueAll;
    //! The maximum value of the entire dataset.
    double maxValueAll;

private:
    //! True if the object has been grouped.
    //! If true, it emits signals on changes and in general tries to be in
    //! sync with remote objects.
    bool is_grouped;
    //! Find the min value of thie dataset if the orientation is set to All.
    //! In any other case get the min value of the selected slice.
    virtual void find_min_value(const int& sliceNum, Orientation _o = All) = 0;
    //! Find the min value of thie dataset if the orientation is set to All.
    //! In any other case get the min value of the selected slice.
    virtual void find_max_value(const int& sliceNum, Orientation _o = All) = 0;
};

#include "src/display_buildblock/screen_manager.inl"
#endif // SCREEN_MANAGER_H
