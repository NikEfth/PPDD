#include "worker_thresholder.h"

WorkerThresholder::WorkerThresholder(QObject *parent):
    Worker(parent)
{
    createMask = false;
}

WorkerThresholder::WorkerThresholder(const float &thres, QObject *parent):
    Worker(parent)
{
    createMask = false;
    thres_level = thres;
}

WorkerThresholder::
WorkerThresholder(Array<3, float> *_d, const float &thres, QObject *parent):
    Worker(parent)
{
    data = _d;
    thres_level = thres;
    createMask = false;
}

//WorkerThresholder::
//WorkerThresholder(Array<2, float> *_d, const float &thres, QObject *parent):
//    Worker(parent)
//{
//    data = _d;
//    thres_level = thres;
//    createMask = false;
//}

WorkerThresholder::~WorkerThresholder()
{
}

void WorkerThresholder::apply(QVector<double> & d)
{
    for (double& _d : d)
    {
        if (_d <= thres_level)
            _d = 0;
        else if (createMask)
            _d = 1000;
    }
}

void
WorkerThresholder::process()
{
    Array<3,float>::full_iterator it = data->begin_all();

    for(; it!=data->end_all();++it)
    {
        if (*it <= thres_level)
            *it = 0;
        else if (createMask)
            *it = 1;
    }
}

QVector<float>
WorkerThresholder::getResult()
{
    //    return data;
}
