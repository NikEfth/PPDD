#ifndef WORKERMANAGER_H
#define WORKERMANAGER_H

#include <QWidget>
#include <QQueue>
#include <QProgressBar>

#include "src/tools_buildblock/worker.h"

namespace Ui {
class WorkerManager;
}

class WorkerManager : public QWidget
{
    Q_OBJECT

public:
    explicit WorkerManager(QWidget *parent = 0);
    ~WorkerManager();

    void appendToQueue();

private:
    Ui::WorkerManager *ui;

    QQueue<int> mainQueue;
};

#endif // WORKERMANAGER_H
