#ifndef OPTIONS_PANEL_H
#define OPTIONS_PANEL_H

#include <QWidget>

namespace Ui {
class Options_panel;
}

class Options_panel : public QWidget
{
    Q_OBJECT

public:
    explicit Options_panel(QWidget *parent = 0);
    ~Options_panel();

private:
    Ui::Options_panel *ui;
};

#endif // OPTIONS_PANEL_H
