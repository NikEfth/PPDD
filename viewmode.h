#ifndef VIEWMODE_H
#define VIEWMODE_H

#include <QDialog>
#include <QDialogButtonBox>

namespace Ui {
class ViewMode;
}

class ViewMode : public QDialog
{
    Q_OBJECT

public:
    explicit ViewMode(QWidget *parent = 0);
    ~ViewMode();

private slots:
void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::ViewMode *ui;
    QDialogButtonBox *buttonBox;

    QPushButton *btn_sino, *btn_view;
};

#endif // VIEWMODE_H
