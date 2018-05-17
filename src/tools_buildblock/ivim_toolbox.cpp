#include "ivim_toolbox.h"
#include "ui_ivim_toolbox.h"

#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>

#include "src/ui_buildblock/myqlistwidgetitem.h"
#include "src/buildblock/dataseries.h"

#include "linalg.h"
#include <math.h>
#include "interpolation.h"

IVIM_toolbox::IVIM_toolbox(const QVector<Screen_manager *> & _scList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IVIM_toolbox)
{
    ui->setupUi(this);
    imagesList_orig = _scList;
    setWindowTitle("IVIM ToolBox");

    imageD.reset(new display_screen_raster(ui->widget_2));

    //    my_screen->setColorMap(currentColorMap);
    imageD->setAddressAtParent(0);
    ui->verticalLayout_6->addWidget(imageD.get());
    //    QObject::connect(my_screen.get(), &display_screen_raster::activateScreen,
    //                     this, &display_screen_container::setActiveScreen);
    //! Todo: resort
    //    connect(ui->listWidget->model(),
    //            SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(myFunction()));
}

IVIM_toolbox::IVIM_toolbox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IVIM_toolbox)
{
    ui->setupUi(this);
}

IVIM_toolbox::~IVIM_toolbox()
{
    delete ui;
}

bool IVIM_toolbox::checkListItems()
{
    const stir::IndexRange<3> range =  imagesList_orig.first()->getRanges3D();

    for (Screen_manager * f: imagesList_orig)
    {
        const stir::IndexRange<3> nrange = f->getRanges3D();
        if (nrange.get_length() != range.get_length())
            return false;
    }

    return true;
}

bool IVIM_toolbox::initialize()
{
    if (!checkListItems())
        return false;

    if (!loadList())
        return false;

    if (!loadSlices())
        return false;

    plotter = new display_screen_curve(this);
    ui->verticalLayout_2->addWidget(plotter);

    return true;
}

bool IVIM_toolbox::loadList()
{
    for (Screen_manager * f: imagesList_orig)
    {
        myQListWidgetItem* tmp_itm =
                new myQListWidgetItem();

        tmp_itm->set_mdiWindow_ptr(f);

        ui->listWidget->addItem(tmp_itm);
    }

    return true;
}

bool IVIM_toolbox::loadSlices()
{
    stir::IndexRange<3> r = imagesList_orig.first()->getData_ptr()->get_index_range();
    QStringList f;
    for (int i = r.get_min_index(); i < r.get_max_index(); i++)
    {
        f << QString::number(i);
    }
    ui->cmb_sliceToProcess->addItems(f);
    ui->cmb_sliceToProcess->setCurrentIndex(f.size()/2);

    stir::IndexRange<3> ranges =  imagesList_orig.first()->getRanges3D();

    minYindx = ranges[0].get_min_index();
    maxYindx = ranges[0].get_max_index();

    minXindx = ranges[0][0].get_min_index();
    maxXindx = ranges[0][0].get_max_index();

    ui->spinYcoord->setMaximum(ranges[0].get_max_index());
    ui->spinYcoord->setMinimum(ranges[0].get_min_index());

    ui->spinXcoord->setMaximum(ranges[0][0].get_max_index());
    ui->spinXcoord->setMinimum(ranges[0][0].get_min_index());

    imageD->set_sizes(ranges[0].get_length(),ranges[0][0].get_length(),
            minXindx,minYindx,
            maxXindx, maxYindx,
            imagesList_orig.first()->getData_ptr()->get_grid_spacing()[3],
            imagesList_orig.first()->getData_ptr()->get_grid_spacing()[2],
            -128, 128, -128,128);

    imageD->replot_me();
    return true;
}

void IVIM_toolbox::on_pushButton_clicked()
{
    numBetaValues= getBetaValues();

    if(numBetaValues<= 0 )
        return;

    imagesList = imagesList_orig;

    Viewer::sortVectors(betaValues, std::less<double>(), betaValues, imagesList);

    stir::IndexRange<3> ranges =  imagesList.first()->getRanges3D();

    int num_sclices = 1;
    int currentSlice = ui->cmb_sliceToProcess->currentIndex();

    if(currentSlice < -1)
        num_sclices = ranges.get_length();

    int num_voxs_y = ranges[0].get_length();

    int num_voxs_x = ranges[0][0].get_length();

    int totalNumSeries = num_voxs_x * num_voxs_y;

    betaPerVoxel.resize(totalNumSeries);

    for (int ibeta = 0; ibeta < numBetaValues ; ibeta++)
    {
        stir::PixelsOnCartesianGrid<float> tmp = imagesList.at(ibeta)->getData_ptr()->get_plane(currentSlice);
        int seriesCounter = 0;
        for(int i = tmp.get_min_index(); i <= tmp.get_max_index(); ++i)
        {
            for (int j = tmp[0].get_min_index(); j <= tmp[0].get_max_index(); ++j)
            {
                betaPerVoxel[seriesCounter].append(tmp[i][j]);
                betaPerVoxel[seriesCounter].coords.x = j;
                betaPerVoxel[seriesCounter].coords.y = i;
                seriesCounter++;
            }
        }
    }

    int betaZeroIndx = betaValues.indexOf(0.0);

    // Normalize
    int ii = 0;
    for (BSerious& b : betaPerVoxel)
    {
        if (b.data.size() == 0)
        {
            continue;
        }

        if ( b.data.at(betaZeroIndx) == 0)
        {
            for (double& d : b.data)
            {
                d = 0.0 ;
            }
        }

        ii++;
        double norm_factor = b.data.at(betaZeroIndx);
        for (double& d : b.data)
        {
            d /= norm_factor ;
        }
    }

    plotCurrentSeries();
}

int IVIM_toolbox::getBetaValues()
{
    bool ok;

    QString line = ui->le_betaValues->text();

    betaValues.clear();

    QStringList splits = line.split(",", QString::SkipEmptyParts);

    if (splits.size() != imagesList_orig.size())
    {
        QMessageBox::critical(this, tr("Error loading beta values"),
                              tr("The number of beta values is different than the number of loaded files."),
                              QMessageBox::Abort);
        return false;
    }

    for (QString split : splits)
    {
        int val = split.toDouble(&ok);
        if (!ok)
        {
            QMessageBox::critical(this, tr("Error loading beta values"),
                                  tr("Error on reading a beta value!"),
                                  QMessageBox::Abort);
            return -1;
        }
        betaValues.append(val);
    }

    plotter->setXdata(betaValues);
    return betaValues.size();
}

void IVIM_toolbox::on_cmb_sliceToProcess_currentIndexChanged(int index)
{
    currentSlice = index;
}

void IVIM_toolbox::on_spinXcoord_valueChanged(int arg1)
{
    dispXcoord = arg1;
    plotCurrentSeries();
}

void IVIM_toolbox::on_spinYcoord_valueChanged(int arg1)
{
    dispYcoord = arg1;
    plotCurrentSeries();
}

void IVIM_toolbox::plotSeries(const int& _cX, const int& _cY)
{
    int i = betaPerVoxel.indexOf(Viewer::BSerious(_cX, _cY));

    if (i < betaPerVoxel.size())
        plotter->setYdata(betaPerVoxel.at(i).data);
}

void IVIM_toolbox::plotCurrentSeries()
{
    int cX = ui->spinXcoord->value();
    int cY = ui->spinYcoord->value();
    plotSeries(cX, cY);
}

void IVIM_toolbox::on_cmb_sliceToProcess_currentIndexChanged(const QString &arg1)
{
    bool ok;
    int tmp = ui->cmb_sliceToProcess->currentText().toInt(&ok);
    if (ok)
    {
        currentSlice = tmp;
        betaPerVoxel.clear();
    }
}

void IVIM_toolbox::on_sld_browser_sliderMoved(int position)
{

}

void IVIM_toolbox::on_checkBox_clicked()
{

}

void function_cx_1_func(const alglib::real_1d_array &c,
                        const alglib::real_1d_array &x,
                        double &func, void *ptr)
{
    func =  c[0] * exp(-c[1]*x[0]);
}

Viewer::B2B<float>  IVIM_toolbox::actual_fit(const alglib::real_2d_array& _x,
                                             const alglib::real_1d_array& _y,
                                             const alglib::real_1d_array& _min_bd,
                                             const alglib::real_1d_array& _max_bd)
{
    bool ok;
    QString d1  = ui->lineEdit->text();
    double ini1 = d1.toDouble(&ok);

    QString d2  = ui->lineEdit_2->text();
    double ini2 = d2.toDouble(&ok);

    if (!ok)
    {
        Viewer::B2B<float> gg(0.0, 0.0);
        gg.status = false;
        return gg;
    }

    alglib::real_1d_array c;
    c.setlength(2);
    c[0] = ini1;
    c[1] = ini2;

    double epsx = 0.0;
    alglib::ae_int_t maxits = 0;
    alglib::ae_int_t info;
    alglib::lsfitstate state;
    alglib::lsfitreport rep;
    double diffstep = 0.01;

    std::exception_ptr eptr;

    try {
        alglib::lsfitcreatef(_x, _y, c, diffstep, state);
    } catch( ... ) {
        //std::cout << msg << std::endl;
        eptr = std::current_exception(); // capture

        Viewer::B2B<float> gg(c[0], c[1]);
        gg.status = false;
        return gg;
    }

    ui->groupBox->setEnabled(false);

    //    alglib::lsfitsetscale();
    alglib::lsfitsetbc(state,_min_bd, _max_bd);
    alglib::lsfitsetcond(state, epsx, maxits);
    alglib::lsfitfit(state, function_cx_1_func);
    alglib::lsfitresults(state, info, c, rep);

    std::cout<< int(info) << std::endl;  // EXPECTED: 2

    ui->groupBox->setEnabled(true);

    if (int(info)!= 2)
    {
        Viewer::B2B<float> gg(0.0, 0.0);
        gg.status = false;
        return gg;
    }
    else
    {
        Viewer::B2B<float> gg(c[0], c[1]);
        gg.status = true;
        return gg;
    }
}


Viewer::B2B<float>  IVIM_toolbox::get_b_high(const int& idx, const int& thres)
{

    if (betaPerVoxel.at(idx).data.size() == 0 ||
            betaPerVoxel.at(idx).meanValue() < 0.2f)
    {
        Viewer::B2B<float> ret(0.0, 0.0);
        ret.status = false;

        return ret;
    }
    int size = betaPerVoxel.at(idx).data.size() - thres;

    double *x_values = new  double[betaValues.size()-thres];
    for (int i=thres; i<betaValues.size(); ++i )
        x_values[i-thres] = betaValues.at(i);

    //    for (int i=0; i<betaValues.size()-thres;i++ )
    //       std::cout<< x_values[i] <<std::endl;

    alglib::real_2d_array x;
    x.attach_to_ptr(size, 1, x_values);

    double *y_values = new double[betaValues.size()-thres];
    double *min_constrain = new double[betaValues.size()-thres];
    double *max_constrain = new double[betaValues.size()-thres];

    double min = 0.0;
    double max = betaPerVoxel.at(idx).data.at(thres);

    for (int i=thres; i< betaPerVoxel.at(idx).data.size(); ++i )
    {
        y_values[i-thres]  = betaPerVoxel.at(idx).data.at(i);

        if (betaPerVoxel.at(idx).data.at(i)<min)
            min = betaPerVoxel.at(idx).data.at(i);
        //        if (betaPerVoxel.at(idx).data.at(i)> max)
        //            max = betaPerVoxel.at(idx).data.at(i);
    }

    for (int i=thres; i< betaPerVoxel.at(idx).data.size(); ++i )
    {
        min_constrain[i-thres]  = min;
        max_constrain[i-thres]  = 1.f;
    }

    //    for (int i=0; i<betaValues.size()-thres;i++ )
    //        std::cout<< y_values[i] <<std::endl;

    alglib::real_1d_array y;
    y.attach_to_ptr(size,y_values);

    alglib::real_1d_array _minBd;
    _minBd.attach_to_ptr(size,min_constrain);

    alglib::real_1d_array _maxBd;
    _maxBd.attach_to_ptr(size,max_constrain);

    Viewer::B2B<float> ret;
    ret = actual_fit(x, y, _minBd, _maxBd);

    delete[] x_values;
    delete[] y_values;

    delete[] min_constrain;
    delete[] max_constrain;

    return ret;
}

double IVIM_toolbox::fitSeries(const int& _cX, const int& _cY, int verbosity)
{
    bool ok ;

    int idx = betaPerVoxel.indexOf(Viewer::BSerious(_cX, _cY));
    QString d1  = ui->lineEdit_3->text();
    double ini1 = d1.toDouble(&ok);

    int thres = betaValues.indexOf(ini1);

    if (ok && thres > 1)
    {
        Viewer::B2B<float> b_high = get_b_high(idx, thres);

        if(b_high.status == true)
        {
            if (verbosity == 1)
            {
                QVector<double> hY;
                for (int i = 0; i < betaValues.size();i++ )
                {
                    float y = b_high.x * exp(-b_high.y*betaValues.at(i));
                    hY.append(y);
                }
                plotter->setHighYdata(hY);
                ui->lineEdit->setText(QString::number(b_high.x));
                ui->lineEdit_2->setText(QString::number(b_high.y));
            }
            else
                return b_high.y;
        }

    }
    return 0.0;
}

void IVIM_toolbox::fitCurrent()
{
    int cX = ui->spinXcoord->value();
    int cY = ui->spinYcoord->value();
    fitSeries(cX, cY);
}

void IVIM_toolbox::on_pushButton_2_clicked()
{
    fitCurrent();
}

void IVIM_toolbox::on_pushButton_3_clicked()
{
    //set image data.
    QVector<double>::iterator iter = imageD->get_data_begin();

    for(int cX = minXindx; cX <= maxXindx; ++cX)
        for (int cY = minYindx; cY <= maxYindx; ++cY)
        {
            if (cY == 12 && cX == -87)
                int nikos = 9;
            *iter = fitSeries(cX,cY,0);
            ++iter;
        }

    imageD->set_min_max_values(0.0, 1.0);
    imageD->replot_me();
}
