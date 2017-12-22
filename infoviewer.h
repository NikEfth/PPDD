#ifndef INFOVIEWER_H
#define INFOVIEWER_H

#include <QWidget>

namespace Ui {
class InfoViewer;
}

class InfoViewer : public QWidget
{
    Q_OBJECT

public:
    explicit InfoViewer(QWidget *parent = 0);
    ~InfoViewer();

private:
    Ui::InfoViewer *ui;
};

#endif // INFOVIEWER_H
