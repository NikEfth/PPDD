#ifndef ANALYSISNEMA_H
#define ANALYSISNEMA_H

#include <QDialog>
#include <QLabel>
#include <QRadioButton>
#include <QSlider>
#include <QHBoxLayout>

#include "src/display_buildblock/screen_manager.h"
#include "src/display_buildblock/screen_manager_imagedata.h"

namespace Ui {
class RangesPage;
}

class AnalysisNema : public QDialog
{
    Q_OBJECT

public:

    explicit AnalysisNema(Screen_manager* data,  QWidget *parent = 0);
    ~AnalysisNema();

private:
    Screen_manager* my_data;
};


#endif // ANALYSISNEMA_H
