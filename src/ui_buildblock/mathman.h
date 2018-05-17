#ifndef MATHMAN_H
#define MATHMAN_H

#include <QWidget>
#include <QListWidget>

#include "stir/DiscretisedDensity.h"
#include "stir/VoxelsOnCartesianGrid.h"

#include "src/tools_buildblock/worker_math.h"

namespace Ui {
class MathMan;
}

class MathMan : public QWidget
{
    Q_OBJECT

public:
    explicit MathMan(QListWidget* , QWidget *parent = 0);
    ~MathMan();

    void updateOpenedFiles();
private slots:
    void on_pushButton_3_clicked();

signals:
    void dataReady(stir::VoxelsOnCartesianGrid<float>*, int , const QString* );

private:

    Ui::MathMan *ui;
    //! List of the opened files, constantly in-sync with the main
    //! window
    QListWidget* openedFiles;

    std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > result;

    Worker_Math* _worker;
};

#endif // MATHMAN_H
