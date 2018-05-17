#include "contrastman.h"
#include "ui_contrastman.h"

ContrastMan::ContrastMan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContrastMan)
{
    ui->setupUi(this);

    my_histogram = new BarScreenWL(ui->groupBox_2);
    ui->gridLayout_3->addWidget(my_histogram);

    sc = 0;

    connect(my_histogram, &BarScreenWL::markersUpdated, this, &ContrastMan::updateMarkers);
    connect(my_histogram, &BarScreenWL::WL, this, &ContrastMan::updateWL);
}

ContrastMan::~ContrastMan()
{
    delete ui;
}

void ContrastMan::setScreen(Screen_manager * _s)
{
    if(sc == 0)
    {
        sc = _s;
        my_histogram->setEnabled(true);
        my_histogram->setScreen(sc);
        connect(sc, &Screen_manager::activeScreenUpdated, my_histogram, &BarScreen::replot_me);

        updateMarkers(my_histogram->getMinMarker(), my_histogram->getMaxMarker());
        my_histogram->setNumBin(ui->spinBox->text().toInt());
        my_histogram->setCutOff(ui->doubleSpinBox->value());
        // Get the initial values.
        ui->le_max->setText(QString::number(my_histogram->getMaxMarker()));
        ui->le_min->setText(QString::number(my_histogram->getMinMarker()));
        ui->le_win->setText(QString::number(my_histogram->getWindow()));
        ui->le_level->setText(QString::number(my_histogram->getLevel()));
    }
}

void ContrastMan::unsetScreen()
{
    if (sc!=0)
        disconnect(sc, &Screen_manager::activeScreenUpdated, my_histogram, &BarScreen::replot_me);
    sc = 0;
    my_histogram->setEnabled(false);
    my_histogram->unsetScreen(false);
}

void ContrastMan::on_spinBox_valueChanged(int arg1)
{
    my_histogram->setNumBin(arg1);
}

void ContrastMan::on_doubleSpinBox_valueChanged(double arg1)
{
    my_histogram->setCutOff(static_cast<float>(arg1));
}

void ContrastMan::updateMarkers(const float& _min, const float& _max)
{
    ui->le_min->setText(QString::number(_min));
    ui->le_max->setText(QString::number(_max));
}

void ContrastMan::updateWL(const float& window, const float& lavel)
{
    ui->le_win->setText(QString::number(window));
    ui->le_level->setText(QString::number(lavel));
}

void ContrastMan::on_btn_reset_clicked()
{
     my_histogram->resetCOG(0);
}

void ContrastMan::on_btn_auto_clicked()
{
    my_histogram->resetCOG();
}

void ContrastMan::on_le_max_returnPressed()
{
    bool ok;
    float f = ui->le_max->text().toFloat(&ok);
    if (ok)
        my_histogram->setMaxMarker(f);
}

void ContrastMan::on_le_min_returnPressed()
{
    bool ok;
    float f = ui->le_min->text().toFloat(&ok);
    if (ok)
        my_histogram->setMinMarker(f);
}

void ContrastMan::on_le_level_returnPressed()
{
    bool ok;
    float f = ui->le_level->text().toFloat(&ok);
    if (ok)
        my_histogram->setLevel(f);
}

void ContrastMan::on_le_win_returnPressed()
{
    bool ok;
    float f = ui->le_win->text().toFloat(&ok);
    if (ok)
        my_histogram->setWindow(f);
}
