#ifndef WORKER_SPLITTER_H
#define WORKER_SPLITTER_H

#include "src/tools_buildblock/worker.h"
#include "src/display_buildblock/screen_manager_imagedata.h"
#include "stir/DiscretisedDensity.h"

class Worker_Splitter : public Worker
{
    Q_OBJECT
public:
    Worker_Splitter( QWidget *parent = 0);
    ~Worker_Splitter();

    QVector<VoxelsOnCartesianGrid<float> > split_list;

    void setSpitSlide(const int & _s)
    {
        split_slice = _s;
    }

    virtual void apply(QVector<double> & d)
    {

    }

    void setData(VoxelsOnCartesianGrid<float>* _d)
    {
        data = _d;
    }
public slots:
    virtual void process();

private:
    void get_subset(VoxelsOnCartesianGrid<float>& _tmp, const IndexRange<3> &new_range, const int& _minZ, const int& _maxZ);

    VoxelsOnCartesianGrid<float>* data;

    int split_slice;
};

#endif

/*
class Worker_Splitter : public Worker
{
    Q_OBJECT
public:
    Worker_Splitter(QWidget *parent = 0);

    ~Worker_Splitter();

    void setSliceLocation(const int& _s)
    {splitSlice = _s;}

    void setSliceSize(const int& _s)
    {sliceSize = _s;}

    QVector<QVector<float> > getResult();

public slots:
    virtual void process();

private:
    void get_subset(VoxelsOnCartesianGrid<float>& _tmp, const IndexRange<3> &new_range, const int& _minZ, const int& _maxZ);

    QVector<QVector<float> > split_list;

    int splitSlice;
    int sliceSize;
};

#endif
*/
