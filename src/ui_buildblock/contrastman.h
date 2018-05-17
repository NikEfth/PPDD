#ifndef CONTRASTMAN_H
#define CONTRASTMAN_H

#include <QWidget>

#include "src/display_buildblock/screen_manager.h"
#include "src/display_buildblock/barscreenwl.h"

namespace Ui {
class ContrastMan;
}

//!
//! \brief The ContrastMan class
//! The base widget for the display of
//! the Contrast and WL operations.
class ContrastMan : public QWidget
{
    Q_OBJECT

public:
    explicit ContrastMan(QWidget *parent = 0);
    ~ContrastMan();
    //! Set the Screen_manager for the operations
    void setScreen(Screen_manager * _s);
    //! Unset the current Screen_manager.
    void unsetScreen();

private:
    Ui::ContrastMan *ui;

protected:
    //! Pointer to hold the current connected Screen_manager.
    Screen_manager* sc;
    //! Pointer to hold the BarScreenWL.
    BarScreenWL* my_histogram;

private slots:
    void on_spinBox_valueChanged(int arg1);

    void on_doubleSpinBox_valueChanged(double arg1);

    void updateMarkers(const float& _min, const float& _max);

    void updateWL(const float& window, const float& lavel);
    //! Restore the original vis_min and max_viz. Usually this means full contrast range.
    void on_btn_reset_clicked();
    //! Optimisation of the contrast range.
    //! Each click of the button narrows the contrast range 5%,
    //! excluding the bin with the valuw zero.
    void on_btn_auto_clicked();
    //! Set het upper threshold.
    void on_le_max_returnPressed();
    //! Set the lower threshold
    void on_le_min_returnPressed();
    //! Set the image Level.
    void on_le_level_returnPressed();
    //! Set the image window.
    void on_le_win_returnPressed();
};

#endif // CONTRASTMAN_H
