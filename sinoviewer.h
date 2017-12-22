#ifndef SINOVIEWER_H
#define SINOVIEWER_H

#include <QWidget>

namespace Ui {
class sinoviewer;
}

class sinoviewer : public QWidget
{
    Q_OBJECT

public:
    explicit sinoviewer(QWidget *parent = 0);
    ~sinoviewer();

private:
    Ui::sinoviewer *ui;
};

#endif // SINOVIEWER_H
