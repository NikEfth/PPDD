//
//
/*
   Copyright 2017 Nikos Efthimiou (nikos.efthimiou AT gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "viewmode.h"

#include "stir/is_null_ptr.h"

#include <fstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDir init_dir = QDir::homePath();
    init_path = init_dir.absolutePath();
    this->setWindowIcon(QIcon(":icons/resources/icons8-microbeam-radiation-therapy-80.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initialise_menus()
{

}

void MainWindow::on_actionOpen_sinogram_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Header"), init_path, tr("STIR Interfile Header (*.hs)"));

    if (fileName.size() == 0)
        return;

    // Change the starting point to the previous path for convinience.
    init_path = QFileInfo(fileName).absolutePath()+"/";

    ViewMode *view_msg = new ViewMode(this);
    view_msg->show();
    int view_mode = view_msg->exec();

    if (view_mode == 0)
        return;

    input_proj_data_sptr = ProjData::read_from_file(fileName.toStdString());

    if (is_null_ptr(input_proj_data_sptr))
        QMessageBox::critical(this, tr("My Application"),
                                       tr("Unable to load Projection Data."),
                                       QMessageBox::Ok);

    on_actionactionRefresh_triggered();

    setWindowTitle(QFileInfo(fileName).fileName());

    QString ff = ui->cmb_num_viewports->currentText();
    int num = ff.toInt();

    int cm_index = ui->cmb_colormap->currentIndex();

    my_screen.reset(new Screen_manager(input_proj_data_sptr, num, cm_index, view_mode, this));
    ui->mainFrame_layout->addWidget(my_screen.get());
}

void MainWindow::on_actionactionRefresh_triggered()
{
    ui->lbl_nrings->setText(QString::number(input_proj_data_sptr->get_proj_data_info_sptr()->get_scanner_ptr()->get_num_rings()));
    ui->lbl_ndets->setText(QString::number(input_proj_data_sptr->get_proj_data_info_sptr()->get_scanner_ptr()->get_num_detectors_per_ring()));
    ui->lbl_nsegs->setText(QString::number(input_proj_data_sptr->get_proj_data_info_sptr()->get_num_segments()));
    ui->lbl_nviews->setText(QString::number(input_proj_data_sptr->get_proj_data_info_sptr()->get_num_views()));
    ui->lbl_ntangposs->setText(QString::number(input_proj_data_sptr->get_proj_data_info_sptr()->get_num_tangential_poss()));
}

void MainWindow::on_cmb_num_viewports_currentIndexChanged(int index)
{
    if(is_null_ptr(my_screen))
        return;

    QString ff = ui->cmb_num_viewports->currentText();
    int num = ff.toInt();

    my_screen->set_up_plot_area(num);
    my_screen->set_seg_index(0);
}

void MainWindow::on_cmb_colormap_currentIndexChanged(int index)
{
    my_screen->changeCM(index);
}
