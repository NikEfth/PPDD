#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <QWidget>

namespace Ui {
class Screen_manager;
}

class Screen_manager : public QWidget
{
    Q_OBJECT

public:
    explicit Screen_manager(QWidget *parent = 0);
    ~Screen_manager();

private:
    Ui::Screen_manager *ui;
};

#endif // SCREEN_MANAGER_H
