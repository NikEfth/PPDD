#ifndef DISPLAY_SCREEN_H
#define DISPLAY_SCREEN_H

#include <QWidget>

namespace Ui {
class display_screen;
}

class display_screen : public QWidget
{
    Q_OBJECT

public:
    explicit display_screen(QWidget *parent = 0);
    ~display_screen();

private:
    Ui::display_screen *ui;
};

#endif // DISPLAY_SCREEN_H
