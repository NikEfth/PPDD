#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <memory>
#include <QVector>

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(QObject *parent = 0);

    virtual void apply(QVector<double> & d)
    {}

public slots:
    virtual void process() = 0;

signals:
    void resultReady();
};

#endif
