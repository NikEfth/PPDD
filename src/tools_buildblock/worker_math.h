#ifndef WORKER_MATH_H
#define WORKER_MATH_H

#include <QObject>

#include "src/tools_buildblock/worker.h"
#include "stir/VoxelsOnCartesianGrid.h"


class Worker_Math : public Worker
{
    Q_OBJECT

public:
    explicit Worker_Math(QObject *parent = 0);

    explicit Worker_Math(QString* expr,
                         std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > _A,
                         std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > _B,
                         std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > _C,
                         QObject *parent = 0);

    ~Worker_Math();

public slots:
    virtual void process();

protected:

    QString* expr;
    std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > A;
    std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > B;
    std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > C;

};

#endif // WORKER_MATH_H
