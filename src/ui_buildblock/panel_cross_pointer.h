#ifndef PANEL_CROSS_POINTER_H
#define PANEL_CROSS_POINTER_H

#include "src/buildblock/common.h"
#include <QWidget>
#include <QVector3D>

#include <QTableWidgetItem>

namespace Ui {
class panel_cross_pointer;
}

class p_data
{
public:
    p_data(){}
    p_data(const Viewer::SimpleVector3D<int>& _v, const float& _f) :
    cur_pos(_v), cur_value(_f){}

    Viewer::SimpleVector3D<int> cur_pos;
    float cur_value;
};

class panel_cross_pointer : public QWidget
{
    Q_OBJECT

public:
    explicit panel_cross_pointer(QWidget *parent = 0);
    ~panel_cross_pointer();

signals:
    void select_this_point(const Viewer::SimpleVector3D<int>&);

public slots:
    void print_point(const Viewer::SimpleVector3D<int> & ps, const double &_v);

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::panel_cross_pointer *ui;

    QVector<p_data> my_data;

    Viewer::SimpleVector3D<int> cur_pos;

    float cur_value;
};

#endif // PANEL_CROSS_POINTER_H
