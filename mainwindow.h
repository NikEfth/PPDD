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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "screen_manager.h"

#include "stir/IO/ExamData.h"
#include "stir/ProjData.h"
#include "stir/ProjDataFromStream.h"
#include "stir/ProjDataInMemory.h"
#include "stir/ProjDataInterfile.h"
#include "stir/IO/interfile.h"
#include "stir/IO/read_from_file.h"
#include "stir/ProjDataInfoCylindrical.h"
#include "stir/is_null_ptr.h"

#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    //!
    //! \brief projdata_loaded
    //! Signals that a new ProjData has been succeessfully loaded.
    //! In addition, sends a pointer of that data set to the connected slots.
    //! \param new_projdata
    //!
    void projdata_loaded(std::shared_ptr<ProjData> new_projdata);

    //!
    //! \brief projdata_info_loaded
    //! Signals a new ProjDataInfo and sends a pointer of it.
    //! \param new_projdata_info
    //!
    void projdata_info_loaded(std::shared_ptr<ProjDataInfo> new_projdata_info);

private slots:
    void on_actionOpen_sinogram_triggered();

    //!
    //! \brief on_actionactionRefresh_triggered
    //! Currently this function only refreshes the Info side bar.
    //!
    void on_actionactionRefresh_triggered();

    //!
    //! \brief on_cmb_num_viewports_currentIndexChanged
    //! \param index
    //! Actions to be taken when the number of viewport is changed.
    void on_cmb_num_viewports_currentIndexChanged(int index);

    //!
    //! \brief on_cmb_colormap_currentIndexChanged
    //! \param index
    //!
    void on_cmb_colormap_currentIndexChanged(int index);

    //!
    //! \brief on_actionAbout_triggered
    //!
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;

    //!
    //! \brief init_path
    //! Hold the previous path of the data.
    QString init_path;

    //!
    //! \brief my_screen
    //! This member holds the cetran widget.
    std::unique_ptr<Screen_manager> my_screen;

    //!
    //! \brief toolBar
    //!
    QToolBar *toolBar;

    //!
    //! \brief input_proj_data_sptr
    //! As, in the future, the ProjData might be used by different classes,
    //! the a pointer is shared in here.
    std::shared_ptr<ProjData> input_proj_data_sptr;


};

#endif // MAINWINDOW_H
