#include "listroistat.h"
#include "ui_listroistat.h"

ListROIStat::ListROIStat(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::ListROIStat)
{
    ui->setupUi(this);
    data = 0;
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
//    ui->tableWidget->resizeColumnsToContents();

    QStringList headers; headers<<"Area"<<"Mean"<<"SD";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
}

ListROIStat::~ListROIStat()
{
    unSetData();
    delete ui;
}

void ListROIStat::setData(const stir::PixelsOnCartesianGrid<float> &_w)
{

}

void ListROIStat::unSetData()
{
    data = 0;
}

bool ListROIStat::analyseAll(const stir::PixelsOnCartesianGrid<float>& _w)
{
	return true;
}

float ListROIStat::getArea(const stir::PixelsOnCartesianGrid<float>& _w)
{
	return 0.0;
}

float ListROIStat::getMean(const stir::PixelsOnCartesianGrid<float>& _w)
{
	return 0.0;
}

float ListROIStat::getSD(const stir::PixelsOnCartesianGrid<float>& _w)
{
	return 0.0;
}
