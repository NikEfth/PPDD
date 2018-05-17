#ifndef SCREEN_MANAGER_IMAGEDATA_H
#define SCREEN_MANAGER_IMAGEDATA_H

#include <QObject>

#include "src/display_buildblock/screen_manager.h"
#include "stir/common.h"
#include "stir/IO/write_to_file.h"
#include "stir/IO/read_from_file.h"

#include "stir/Succeeded.h"
#include "stir/is_null_ptr.h"

#include "stir/unique_ptr.h"

#include "stir/VoxelsOnCartesianGrid.h"
#include "stir/DiscretisedDensity.h"
#include "stir/DiscretisedDensityOnCartesianGrid.h"

#include "stir/IO/interfile.h"
#include "src/tools_buildblock/worker.h"


namespace Ui {
class Screen_manager_ImageData;
}

class Screen_manager_ImageData : public Screen_manager
{
    Q_OBJECT
public:
    explicit Screen_manager_ImageData(Configuration* curConfig, QWidget *parent);

    explicit Screen_manager_ImageData(VoxelsOnCartesianGrid<float>* image, Configuration* curConfig, QWidget *parent);

    virtual bool loadFile(const QString fileName);

    bool setState(qint16 newViewOrder,
                  qint16 new_current_id,
                  qint16 new_num_viewports);

    virtual QSize setOrientation(Orientation _o = Next);

    virtual QString getDataType() const;

    IndexRange<3> get_IndexRange() const;

    virtual VoxelsOnCartesianGrid<float>* getData_ptr()
    {
        return dynamic_cast<VoxelsOnCartesianGrid<float> * > (density_sptr.get());
    }

    virtual VoxelsOnCartesianGrid<float> getSlice(const int& num, const Orientation& o = Transverse)
    {
        VoxelsOnCartesianGrid<float> *ret;
        if(o == Transverse)
        {
            BasicCoordinate<3,int> coord = make_coordinate(num, 0, 0);

            BasicCoordinate<3,int> min = make_coordinate<int>(1,
                                                         (*density_sptr)[0].get_min_index(),
                                                         (*density_sptr)[0][0].get_min_index());

            BasicCoordinate<3,int> max = make_coordinate<int>(1,
                                                         (*density_sptr)[0].get_max_index(),
                                                         (*density_sptr)[0][0].get_max_index());

            IndexRange<3> range(min, max);

           ret = new VoxelsOnCartesianGrid<float>(range,
                                              dynamic_cast<VoxelsOnCartesianGrid<float> *> (density_sptr.get())->get_origin(),
                                              dynamic_cast<VoxelsOnCartesianGrid<float> *> (density_sptr.get())->get_grid_spacing());

           VoxelsOnCartesianGrid<float>::full_iterator iter = (*ret).begin_all();

            for(coord[2]=(*density_sptr)[0].get_min_index() ;coord[2]<=(*density_sptr)[0].get_max_index();++coord[2])
                for(coord[3]=(*density_sptr)[0][0].get_min_index();coord[3]<=(*density_sptr)[0][0].get_max_index();++coord[3])
                {

                    *iter = (*density_sptr)[coord];
                    ++iter;
                }
        }
//        else if (o == Coronal) //
//        {
//            BasicCoordinate<3,int> coord = make_coordinate(0, num, 0);
//            for(coord[1]=density_sptr->get_min_index() ;coord[1]<=density_sptr->get_max_index();++coord[1])
//                for(coord[3]=(*density_sptr)[0][0].get_min_index();coord[3]<=(*density_sptr)[0][0].get_max_index();++coord[3])
//                {
//                    *iter = static_cast<double>((*density_sptr)[coord]);
//                    ++iter;
//                }
//        }
//        else if (o == Sagittal) //
//        {
//            BasicCoordinate<3,int> coord = make_coordinate(0, 0, num);
//            for(coord[1]=density_sptr->get_min_index();coord[1]<=density_sptr->get_max_index();++coord[1])
//                for(coord[2]=(*density_sptr)[0].get_min_index();coord[2]<=(*density_sptr)[0].get_max_index();++coord[2])
//                {
//                    *iter = static_cast<double>((*density_sptr)[coord]);
//                    ++iter;
//                }
//        }

        return *ret;
    }

    virtual std::shared_ptr<VoxelsOnCartesianGrid<float> > getData_sptr()
    {
        return std::dynamic_pointer_cast<VoxelsOnCartesianGrid<float> >(density_sptr);;
    }

    virtual int getNumOfArrays();

    virtual Array<2, float> getArray2D(const int&) const;

    virtual void setArray2D(const Array<2,float>&, const int&);

    virtual void display();

    void split(const int& split_slice, QVector<Screen_manager_ImageData *> &ret_list);

    virtual void showCursor(const bool& state = false);

    virtual const IndexRange<3> getRanges3D();
    //! Write image to disk using STIR functions.
    virtual void writeToDisk(const QString& _p);

    virtual void applyVizValuesToAllContainers(const double& _min, const double& _max);

    virtual void applyFunctionToAllContainers(Worker *_w);

    virtual void removeFunctionToAllContainers();
    //! The current position of the currentCursor
    virtual  CartesianCoordinate3D<float>  getCurrentCursorInmm();

public slots:

    void onCursorChanged();

    virtual void _setCursor(const QPoint &, QwtPlot *);

    virtual void _setCursor(const Viewer::SimpleVector3D<int>& );

    virtual void updateCursor(display_screen_container* d = 0);

protected:
    //! Runs if containersReady. It will initialize the container content by
    //! setting the pointer to the density_sptr, a
    //! unique  Orientation to every container and call  updatePlotArea() for
    //! all of them.
    virtual void initialisePlotArea();
    //! obsolete
    virtual void initialise_controls_for_view();

    virtual void save_as_array(int this_label);
    //! Returns the value of the voxel pointed by _c
    virtual double getValueAt(const SimpleVector3D<int> _c);

    virtual void find_min_value(const int& sliceNum, Orientation _o = All);

    virtual void find_max_value(const int& sliceNum, Orientation _o = All);
    //! This function is called after loading a new dataset.
    //! It groups several funcions as initialisePlotArea, setOrientation,
    //! setActiveScreenContainer, find_min_value and find_max_value
    bool afterLoading();

private:

    int cur_pos_num;
    //! The shared_ptr holding the 3D array.
    shared_ptr<DiscretisedDensity<3,float> > density_sptr;
    //! Enum the supported image types
    enum DataFormat {interfile, dicom};

    DataFormat this_input;

};

#endif // SCREEN_MANAGER_IMAGEDATA_H
