#ifndef IVIM_TOOLBOX_H
#define IVIM_TOOLBOX_H

#include <QDialog>
#include "src/buildblock/common.h"
#include "src/display_buildblock/screen_manager.h"

#include "src/display_buildblock/display_screen_curve.h"

#include "src/display_buildblock/display_screen_raster.h"

#include "linalg.h"

namespace Ui {
class IVIM_toolbox;
}

class IVIM_toolbox : public QDialog
{
    Q_OBJECT

public:
    explicit IVIM_toolbox(const QVector<Screen_manager *> & _scList, QWidget *parent = 0);
    explicit IVIM_toolbox(QWidget *parent = 0);
    ~IVIM_toolbox();

    inline void setImageList(QVector<Screen_manager*> _list)
    {imagesList_orig = _list;}

    bool initialize();
private slots:
    void on_pushButton_clicked();

    void on_cmb_sliceToProcess_currentIndexChanged(int index);

    void on_spinXcoord_valueChanged(int arg1);

    void on_spinYcoord_valueChanged(int arg1);

    void on_cmb_sliceToProcess_currentIndexChanged(const QString &arg1);

    void on_sld_browser_sliderMoved(int position);

    void on_checkBox_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::IVIM_toolbox *ui;

    void plotSeries(const int&, const int&);

    void plotCurrentSeries();

    double fitSeries(const int& _cX, const int& _cY, int verbosity = 1);

    Viewer::B2B<float> actual_fit(const alglib::real_2d_array& _x,
                                  const alglib::real_1d_array& _y,
                                  const alglib::real_1d_array &_min_bd,
                                  const alglib::real_1d_array &_max_bd);

    void fitCurrent();

    Viewer::B2B<float>  get_b_high(const int&, const int&);

    bool loadList();

    bool loadSlices();

    bool checkListItems();

    int getBetaValues();

    QVector<Screen_manager*> imagesList;

    QVector<Screen_manager*> imagesList_orig;

    display_screen_curve* plotter;

    QVector<double> betaValues;

    //QVector<alglib::real_1d_array> betaPerVoxel;

    QVector< BSerious > betaPerVoxel;

    std::shared_ptr<display_screen_raster> imageD;

    int currentSlice;

    int numBetaValues;

    int dispXcoord;

    int dispYcoord;

    int minYindx;

    int minXindx;

    int maxYindx;

    int maxXindx;
};

#endif // IVIM_TOOLBOX_H
