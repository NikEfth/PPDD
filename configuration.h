#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>

class Configuration : public QObject
{
    Q_OBJECT
public:
    explicit Configuration(QObject *parent = nullptr);

    int curColorMap_index;
    int numViewPorts;
signals:

public slots:
};

#endif // CONFIGURATION_H
