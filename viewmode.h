#ifndef VIEWMODE_H
#define VIEWMODE_H

#include <QDialog>

namespace Ui {
class ViewMode;
}

class ViewMode : public QDialog
{
    Q_OBJECT

public:
    explicit ViewMode(QWidget *parent = 0);
    ~ViewMode();

private:
    Ui::ViewMode *ui;
};

#endif // VIEWMODE_H
