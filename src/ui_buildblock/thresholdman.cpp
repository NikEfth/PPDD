#include "thresholdman.h"
#include "ui_thresholdman.h"

#include <QMessageBox>

ThresholdMan::ThresholdMan(Screen_manager *_p, QWidget *parent) :
    sc(_p), QWidget(parent, Qt::Window),
    ui(new Ui::ThresholdMan)
{
    ui->setupUi(this);

    globalHistogram = false;
    createMask = false;
    applyToStack = false;

    my_histogram = new BarScreenHistogram(ui->groupBox);
    ui->gridLayout_4->addWidget(my_histogram);
    my_histogram->setScreen(sc);

    my_histogram->setNumBin(ui->spinBox->value());
    my_histogram->setCutOff(ui->doubleSpinBox_2->value());

    ui->horizontalSlider->blockSignals(true);
    ui->doubleSpinBox->blockSignals(true);
    connect(sc, &Screen_manager::activeScreenUpdated, my_histogram, &BarScreen::replot_me);

    preview_thresholder = new WorkerThresholder(this);
    sc->applyFunctionToAllContainers(preview_thresholder);
    //    sc->refreshPlots();

    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(100);

    ui->doubleSpinBox->setMinimum(sc->get_min_value_all());
    ui->doubleSpinBox->setMaximum(sc->get_max_value_all());

    connect(ui->horizontalSlider, &QSlider::sliderMoved, this,
            &ThresholdMan::notifyValueChanged);

    connect(this, &ThresholdMan::doubleValueChanged,
            ui->doubleSpinBox, &QDoubleSpinBox::setValue);

    //    connect(ui->doubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
    //            this, &ThresholdMan::notifyIntValueChanged);

    connect(this, &ThresholdMan::intValueChanged,
            ui->horizontalSlider, &QSlider::setSliderPosition);

    connect(this, &ThresholdMan::doubleValueChanged, my_histogram,
            &BarScreenHistogram::setThreshold);

    //    connect(ui->doubleSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), my_histogram, &BarScreenHistogram::setThreshold);

    ui->horizontalSlider->blockSignals(false);
    ui->doubleSpinBox->blockSignals(false);

}

void ThresholdMan::batch(QVector<Screen_manager*>* _stack)
{
    stackSc = _stack;
    applyToStack = true;
}

ThresholdMan::~ThresholdMan()
{
    delete ui;
}

void ThresholdMan::on_pushOK_clicked()// save to a new image
{
    int appMethod = ui->appMethod->currentIndex();

    switch (appMethod) {
    case 0:
        localThresGlobalAppApply(true);
        break;
    case 1:
        localThresLocalAppApply(false, false);
        break;
    case 2:
        //globalThresGlobalAppApply();
        break;
    case 3:
        //globalThresGlobalAppApply();
        break;
    default:
        break;
    }

    this->close();
}

void ThresholdMan::on_pushCancel_clicked()// cancel
{
    //my_histogram->resetCOG(0);
    sc->removeFunctionToAllContainers();
    close();
}

float ThresholdMan::getThresholdValue()
{
    if (ui->radioManualThres)
        return my_histogram->getMinMarker();
    else if(ui->radioTriangleThres)
    {
        return getThresLevelTriangle();
    }
    else if (ui->radioOtsuThres)
    {
       return getThresLevelOtsu();
    }
    else
        return 0.0;
}

void ThresholdMan::on_pushApply_clicked() // apply to this.
{
    int appMethod = ui->appMethod->currentIndex();

    switch (appMethod) {
    case 0:
        localThresGlobalAppApply();
        break;
    case 1:
        localThresLocalAppApply(false, false);
        break;
    case 2:
          localThresLocalAppApply(true, false);
        break;
    case 3:
        //globalThresGlobalAppApply();
        break;
    default:
        break;
    }

    this->close();
}

void ThresholdMan::localThresGlobalAppApply(bool copyToNew)
{
    float thresVal = getThresholdValue();

    if(applyToStack)
    {
        if(stackSc->size() == 0)  // No selected files.
        {
            return;
        }

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Selected Files", "Apply to All selected files?",
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            QVector<Screen_manager* >::iterator it = stackSc->begin();
            for (; it != stackSc->end(); ++it)
            {
                VoxelsOnCartesianGrid<float>* old_data = (*it)->getData_ptr();
                if (!copyToNew)
                {
                    WorkerThresholder* _thresholder = new WorkerThresholder(old_data, thresVal, this);
                    _thresholder->process();
                    (*it)->updatedContainer();
                    (*it)->removeFunctionToAllContainers();
                    delete _thresholder;
                }
                else
                {
                    VoxelsOnCartesianGrid<float>* new_data =
                            (*it)->getData_ptr()->clone();

                    WorkerThresholder* _thresholder =
                            new WorkerThresholder(new_data, thresVal, this);
                    _thresholder->process();

                    (*it)->removeFunctionToAllContainers();
                    const QString * name = sc->getFullFilePath_ptr();
                    emit dataReady(new_data,0,name);
                    delete _thresholder;
                }

            }
        }
    }
    else
    {
        VoxelsOnCartesianGrid<float>* old_data = sc->getData_ptr();
        if(!copyToNew)
        {
            WorkerThresholder* _thresholder = new WorkerThresholder(old_data, thresVal, this);
            _thresholder->process();
            sc->updatedContainer();
            sc->removeFunctionToAllContainers();
            delete _thresholder;
        }
        else
        {
            VoxelsOnCartesianGrid<float>* new_data =
                    sc->getData_ptr()->clone();

            WorkerThresholder* _thresholder = new WorkerThresholder(new_data, thresVal, this);
            _thresholder->process();

            sc->removeFunctionToAllContainers();
            const QString * name = sc->getFullFilePath_ptr();
            emit dataReady(new_data,0,name);
            delete _thresholder;
        }
    }
}


void ThresholdMan::localThresLocalAppApply(bool recrusive, bool copyToNew)
{
    float thresVal = 0.0;

    if (!recrusive)
       thresVal = getThresholdValue();

    if(applyToStack)
    {
        if(stackSc->size() == 0)  // No selected files.
        {
            return;
        }

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Selected Files", "Apply to All selected files?",
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
            QVector<Screen_manager* >::iterator it = stackSc->begin();

            for (; it != stackSc->end(); ++it)
            {
                VoxelsOnCartesianGrid<float>* old_data = (*it)->getData_ptr();
                if (!copyToNew)
                {
                    WorkerThresholder* _thresholder = new WorkerThresholder(old_data, thresVal, this);
                    _thresholder->process();
                    (*it)->updatedContainer();
                    (*it)->removeFunctionToAllContainers();
                    delete _thresholder;
                }
                else
                {
                    VoxelsOnCartesianGrid<float>* new_data =
                            (*it)->getData_ptr()->clone();

                    WorkerThresholder* _thresholder =
                            new WorkerThresholder(new_data, thresVal, this);
                    _thresholder->process();

                    (*it)->removeFunctionToAllContainers();
                    const QString * name = sc->getFullFilePath_ptr();
                    emit dataReady(new_data,0,name);
                    delete _thresholder;
                }

            }
        }
    }
    else
    {
        VoxelsOnCartesianGrid<float>* old_data = sc->getData_ptr();

        int num_slides = sc->getNumOfArrays();
        int start_slide = 0;

        for (int islide = start_slide; islide <= num_slides; ++islide)
        {
            VoxelsOnCartesianGrid<float> old_slide = sc->getSlice(islide);

            if (recrusive)
               thresVal = getThresholdValue();

            if(!copyToNew)
            {
                WorkerThresholder* _thresholder = new WorkerThresholder(&old_slide, thresVal, this);
                _thresholder->process();

                sc->updatedContainer();
                sc->removeFunctionToAllContainers();
                delete _thresholder;
            }
            else
            {
                VoxelsOnCartesianGrid<float>* new_data =
                        sc->getData_ptr()->clone();

                WorkerThresholder* _thresholder = new WorkerThresholder(new_data, thresVal, this);
                _thresholder->process();

                sc->removeFunctionToAllContainers();
                const QString * name = sc->getFullFilePath_ptr();
                emit dataReady(new_data,0,name);
                delete _thresholder;
            }
        }
    }
}


void ThresholdMan::on_spinBox_valueChanged(int arg1)
{
    my_histogram->setNumBin(arg1);
}

void ThresholdMan::on_doubleSpinBox_2_valueChanged(double arg1)
{
    my_histogram->setCutOff(static_cast<float>(arg1));
}

void ThresholdMan::on_chkMask_clicked(bool checked)
{
    createMask = checked;
    preview_thresholder->setMask(createMask);
    on_actionThresholdMethodChanged_triggered();
}

void ThresholdMan::on_chkGlobal_clicked(bool _b)
{
    my_histogram->setScreen(sc,_b);
}


float ThresholdMan::getThresLevelTriangle()
{

    QPointF pointA = my_histogram->getMaxFrequencyPoint();
    QPointF pointB = my_histogram->getLastBinPoint();

    QVector2D BA(pointB - pointA);

    //t = [(p-v) . (w-v)] / |w-v|^2
    int numBins = ui->spinBox->value();
    double dist = 0.0;
    int max_i = 0;

    float ret = 0.0;

    for(int i = 1; i < numBins-1; i++)
    {
        QPointF pointC = my_histogram->getPointofBin(i);
        QVector2D vC(pointC);

        double _dist = BA.distanceToPoint(vC);
        if (_dist > dist)
        {
            dist = _dist;
            ret = pointB.x() - pointC.x();
            //            max_i = i;
        }
    }
    return ret;
}

float ThresholdMan::getThresLevelOtsu()
{
    double ret = 0.0;
    // Between class variance
    double bcv = 0.0;
    int numBins = ui->spinBox->value();
    double _w_b = 0.0;
    double _w_f = 0.0;
    double _sum_b = 0.0;

    int total = sc->getArray2D(0).get_length() * sc->getArray2D(0)[0].get_length();

    double _sum = 0.0;
    for(int i = 0; i < numBins; i++)
        _sum += i * my_histogram->getBinValue(i);

    for(int i = 0; i < numBins; i++)
    {
        _w_b += my_histogram->getBinValue(i);
        if (_w_b == 0)
            continue;

        _w_f = total - _w_b;
        if (_w_f == 0)
            break;

        _sum_b += static_cast<double>(i) * my_histogram->getBinValue(i);

        double _mu_b = _sum_b / _w_b;
        double _mu_f = (_sum - _sum_b)/_w_f;

        double _bcv = (_mu_b - _mu_f)*(_mu_b - _mu_f)*_w_b * _w_f;
        //       std::cout<< bcv << std::endl;

        if(_bcv > bcv)
        {
            bcv = _bcv;
            QPointF pointC = my_histogram->getPointofBin(i);
            ret = pointC.x();
        }
    }

    return ret;
}

float ThresholdMan::getThresLevelOtsuSP()
{
    double ret = 0.0;
    // Between class variance
    double bcv = 0.0;
    int numBins = ui->spinBox->value();
    double _w_b = 0.0;
    double _w_f = 0.0;
    double _sum_b = 0.0;

    int total = sc->getArray2D(0).get_length() * sc->getArray2D(0)[0].get_length();

    double _sum = 0.0;
    for(int i = 0; i < numBins; i++)
        _sum += i * my_histogram->getBinValue(i);

    for(int i = 0; i < numBins; i++)
    {
        _w_b += my_histogram->getBinValue(i);
        if (_w_b == 0)
            continue;

        _w_f = total - _w_b;
        if (_w_f == 0)
            break;

        _sum_b += static_cast<double>(i) * my_histogram->getBinValue(i);

        double _mu_b = _sum_b / _w_b;
        double _mu_f = (_sum - _sum_b)/_w_f;

        double _bcv = (_mu_b - _mu_f)*(_mu_b - _mu_f)*_w_b * _w_f;
        //       std::cout<< bcv << std::endl;

        if(_bcv > bcv)
        {
            bcv = _bcv;
            QPointF pointC = my_histogram->getPointofBin(i);
            ret = pointC.x();
        }
    }

    return ret;
}

void ThresholdMan::on_actionThresholdMethodChanged_triggered()
{
    ui->horizontalSlider->setEnabled(ui->radioManualThres->isChecked());
    ui->doubleSpinBox->setEnabled(ui->radioManualThres->isChecked());

    double thresVal = 0.0;

    preview_thresholder->setThresValue(thresVal);
    emit doubleValueChanged(thresVal);
    sc->rePlotPlots();

    if(ui->radioManualThres->isChecked())
    {
        thresVal = sc->get_max_value_all() * ui->horizontalSlider->value()/100;
    }
    else if(ui->radioTriangleThres->isChecked())
    {
        thresVal = getThresLevelTriangle();
    }
    else if(ui->radioOtsuThres->isChecked())
    {
        thresVal = getThresLevelOtsu();
    }
    else if(ui->radioOtsuSPThres->isChecked())
    {
        thresVal = getThresLevelOtsu();
    }

    preview_thresholder->setThresValue(thresVal);
    emit doubleValueChanged(thresVal);
    sc->refreshPlots();
}

