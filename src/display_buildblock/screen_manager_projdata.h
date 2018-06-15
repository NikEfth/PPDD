#ifndef SCREEN_MANAGER_PROJDATA_H
#define SCREEN_MANAGER_PROJDATA_H

#include <QObject>
#include <QComboBox>

#include "screen_manager.h"

#include "stir/ProjData.h"
#include "stir/ProjDataFromStream.h"
#include "stir/ProjDataInMemory.h"
#include "stir/ProjDataInterfile.h"

namespace Ui {
class screen_manager_ProjData;
}

class screen_manager_ProjData : public Screen_manager
{

    Q_OBJECT
public:
    screen_manager_ProjData(Configuration* curConfig, QWidget *parent);

    virtual bool loadFile(const QString fileName);

    bool setState(qint16 newViewOrder,
                  qint16 new_current_id,
                  qint16 new_seg_num,
                  qint16 new_num_viewports);

    inline qint16 getCurrentSegmentNumber() const;

    void set_seg_index(qint16 num_seg);

    virtual QSize setOrientation(Orientation _o = Next);

    virtual QString getDataType() const;

    virtual int getNumOfArrays();

    virtual Array<2, float> getArray2D(const int&) const;

    virtual void setArray2D(const Array<2,float>&, const int&);

    virtual void display();

    virtual void showCursor(const bool& state = false);

    virtual const IndexRange<3> getRanges3D();
    //! Write image to disk using STIR functions.
    virtual void writeToDisk(const QString&);

    void applyVizValuesToAllContainers(const double& _min, const double& _max);

    virtual void applyFunctionToAllContainers(Worker *_w);

    virtual void removeFunctionToAllContainers();

    virtual VoxelsOnCartesianGrid<float>* getData_ptr()
    {
        //return dynamic_cast<VoxelsOnCartesianGrid<float> * > (density_sptr.get());
		 VoxelsOnCartesianGrid<float>* ret; 
		 return ret;
    }

    virtual std::shared_ptr<VoxelsOnCartesianGrid<float> > getData_sptr()
    {
		std::shared_ptr<VoxelsOnCartesianGrid<float> > ret; 
		return ret;
    }

    //! The current position of the currentCursor
    virtual  CartesianCoordinate3D<float>  getCurrentCursorInmm();

public slots:

    //!
    //! \brief remote_segment_change
    //! \param index
    //! Change the segment remotely.
    void remote_segment_change(QString index);

    void onCursorChanged();

    virtual void _setCursor(const QPoint &, QwtPlot *);

    virtual void _setCursor(const Viewer::SimpleVector3D<int>& );

    virtual void updateCursor(display_screen_container* d = 0);

protected:
    virtual void initialisePlotArea();

    virtual void initialise_controls_for_view();

    virtual void initialise_controls_for_TOF();

    virtual void createActions();

    virtual void save_as_array(int this_label);

    virtual double getValueAt(const SimpleVector3D<int> _c);

    virtual void find_min_value(const int& sliceNum, Orientation _o = All);

    virtual void find_max_value(const int& sliceNum, Orientation _o = All);

private slots:
    void on_selectSeg_cmb_currentIndexChanged(QString index);

    void on_selectTOF_cmb_currentIndexChanged(QString index);


private:

    //!
    //! \brief min_seg
    //! Min segment of the ProjData
    qint16 min_seg;

    //!
    //! \brief max_seg
    //! Max segment of the ProjData
    qint16 max_seg;

    //!
    //! \brief cur_segment_num
    //! The current segment number under display.
    qint16 cur_segment_num;

    qint32 minTOFpos;

    qint32 maxTOFpos;

    qint32 curTOFnum;

    float bin_size;

    void initialise_controls_for_seg();

    std::shared_ptr<stir::ProjData> projData_sptr;

    //!
    //! \brief hasSegments
    //! A flag for the initialised controls
    bool hasSegments;

    //!
    //! \brief cur_segment
    //! The current segment under display
    shared_ptr<Segment<float> > cur_segment;

    QComboBox* selectSeg_cmb, *selectTOF_cmb;

    bool has_tof;

};


#include "src/display_buildblock/screen_manager_projdata.inl"

#endif // SCREEN_MANAGER_PROJDATA_H
