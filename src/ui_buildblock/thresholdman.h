#ifndef THRESHOLDMAN_H
#define THRESHOLDMAN_H

#include <QWidget>

#include "src/display_buildblock/screen_manager.h"
#include "src/display_buildblock/barscreenhistogram.h"

#include "src/tools_buildblock/worker_thresholder.h"

namespace Ui {
class ThresholdMan;
}

class ThresholdMan : public QWidget
{
    Q_OBJECT

public:
    explicit ThresholdMan(Screen_manager*_p, QWidget *parent = 0);
    ~ThresholdMan();
    //! Depending on wether manual or Otsu operation this
    //! function will return the appropriate threshold value.
    float getThresholdValue();
    //! Set a QVector of data for batch processing.
    void batch(QVector<Screen_manager*>*);

signals:
    void doubleValueChanged(double value);

    void intValueChanged(int value);

    void dataReady(stir::VoxelsOnCartesianGrid<float>*, int , const QString* );

public slots:
    void notifyValueChanged(int value) {
        double doubleValue = sc->get_max_value_all() * value/100;
        preview_thresholder->setThresValue(doubleValue);
        emit doubleValueChanged(doubleValue);
        sc->rePlotPlots();
    }

    void notifyIntValueChanged(double value) {
        int intValue = static_cast<int>(value);
        emit intValueChanged(intValue);
    }

protected:
    //! Main dataset to preview and perform a signle operation
    Screen_manager* sc;

    QVector<Screen_manager*> *stackSc;
    //! The histogram to display the selection
    BarScreenHistogram* my_histogram;
    //! The Worker that performs the operation
    WorkerThresholder *preview_thresholder;

private slots:
    //! Apply the threshold on a copy of the original data.
    //! The original data will remain unchanged.
    void on_pushOK_clicked();
    //! Cancel and return to full contrast.
    void on_pushCancel_clicked();
    //! Apply will perform the thresholding on the existing data,
    //! modifing them directly.
    void on_pushApply_clicked();
    //! Sets the number of bins used in the histogram.
    void on_spinBox_valueChanged(int arg1);
    //! Sets the percentage of the zero bin that will be displayed on the Y axis
    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_chkMask_clicked(bool checked);

    void on_actionThresholdMethodChanged_triggered();

    void on_chkGlobal_clicked(bool);

private:
    Ui::ThresholdMan *ui;
    //! If checked the pixels that are turned to zero will be set to 1s.
    bool createMask;

    bool globalHistogram;
    //! Return the threshold level as estimated by the triangle method.
    float getThresLevelTriangle();
    //! this implements the fast Otsu method (comparison of the between class variance).
    float getThresLevelOtsu();

    float getThresLevelOtsuSP();
    //! If true the threshold will be applied on the whole stack.
    bool applyToStack;

    void localThresGlobalAppApply(bool copyToNew = false);

    void localThresLocalAppApply(bool recrusive = false, bool copyToNew = false);

    //void globalThresGlobalAppApply();

    //void globalThresGlobalAppApply();
};

#endif // THRESHOLDMAN_H
