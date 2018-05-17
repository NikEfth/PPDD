#include "mathman.h"
#include "ui_mathman.h"

#include "src/ui_buildblock/myqlistwidgetitem.h"

MathMan::MathMan(QListWidget* _list, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MathMan),
    openedFiles(_list)
{
    ui->setupUi(this);
}

MathMan::~MathMan()
{
    delete ui;
}


void MathMan::updateOpenedFiles()
{
    QStringList w;
    ui->cmb_ImageA->clear();
    ui->cmb_ImageB->clear();
    for (int i = 0; i < openedFiles->count(); i++)
    {
        QListWidgetItem* it = openedFiles->item(i);
        w << it->text();
    }

    ui->cmb_ImageA->insertItems(0,w);
    ui->cmb_ImageB->insertItems(0,w);
}

void MathMan::on_pushButton_3_clicked()
{
    QString expr = ui->lineEdit->text();

    std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > A =
            dynamic_cast<myQListWidgetItem*> (openedFiles->item(ui->cmb_ImageA->currentIndex()))->get_mdiWindow_ptr()->getData_sptr();

    std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > B =
            dynamic_cast<myQListWidgetItem*> (openedFiles->item(ui->cmb_ImageA->currentIndex()))->get_mdiWindow_ptr()->getData_sptr();

   result.reset(B->get_empty_copy());

    // Find operations
   _worker = new Worker_Math(&expr, A, B, result ,this);

   _worker->process();

   const QString * name = dynamic_cast<myQListWidgetItem*> (openedFiles->item(ui->cmb_ImageA->currentIndex()))->get_mdiWindow_ptr()->getFullFilePath_ptr();

   emit dataReady(result.get(), 0, name);

   delete _worker;
}
