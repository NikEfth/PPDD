#ifndef WORKER_THRESHOLDER_H
#define WORKER_THRESHOLDER_H

#include "src/tools_buildblock/worker.h"
#include "stir/Array.h"

using namespace stir;

class WorkerThresholder : public Worker
{
    Q_OBJECT
public:
    WorkerThresholder(QObject *parent = 0);

    WorkerThresholder(Array<3, float>*,
                      const float& thres,
                      QObject *parent = 0);

//    WorkerThresholder(Array<2, float>*,
//                      const float& thres,
//                      QObject *parent = 0);

    WorkerThresholder(const float& thes,
                      QObject *parent = 0);

    ~WorkerThresholder();

    inline void setThresValue(const double& _v)
    { thres_level = _v;}

    inline void setMask(const bool& _v)
    { createMask = _v; }

    virtual void apply(QVector<double> & d);

    QVector<float> getResult();

public slots:
    virtual void process();

private:
    stir::Array<3,float> *data;

    float thres_level;

    bool createMask;

};

#endif // WORKER_THRESHOLDER_H
