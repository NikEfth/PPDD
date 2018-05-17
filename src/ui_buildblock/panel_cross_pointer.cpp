#include "panel_cross_pointer.h"
#include "ui_panel_cross_pointer.h"

panel_cross_pointer::panel_cross_pointer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_cross_pointer)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

panel_cross_pointer::~panel_cross_pointer()
{
    delete ui;
}

void panel_cross_pointer::print_point(const Viewer::SimpleVector3D<int> &ps, const double &_v)
{
    ui->label_x->setNum(ps.x);
    ui->label_y->setNum(ps.y);
    ui->label_z->setNum(ps.z);
    ui->label_v->setNum(_v);
    cur_pos = ps;
    cur_value = _v;
}

void panel_cross_pointer::on_pushButton_clicked()
{
    my_data.append(p_data(cur_pos, cur_value));
    ui->tableWidget->insertRow( ui->tableWidget->rowCount() );
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(QString("%1 %2 %3").arg(my_data.last().cur_pos.x).arg(my_data.last().cur_pos.y).arg(my_data.last().cur_pos.z)));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new QTableWidgetItem(QString::number(my_data.last().cur_value)));
}

void panel_cross_pointer::on_pushButton_2_clicked()
{
    if (my_data.count()>1)
    {
        my_data.remove(ui->tableWidget->currentRow());
        ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    }
}

void panel_cross_pointer::on_tableWidget_cellDoubleClicked(int row, int column)
{
    emit select_this_point(my_data[row].cur_pos);
}
