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

#include "src/buildblock/configuration.h"
#include "src/display_buildblock/screen_manager.h"

#include "src/ui_buildblock/analysisnema.h"
#include "src/tools_buildblock/worker.h"
#include "src/tools_buildblock/ToolManager.h"
#include "src/ui_buildblock/contrastman.h"
#include "src/display_buildblock/listroistat.h"
#include "src/ui_buildblock/mathman.h"

#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>
#include <QMdiArea>
#include <QListWidget>
#include <QThread>
#include <QDockWidget>

namespace Ui {
class MainWindow;
}

using namespace Viewer;

//!
//! \brief The MainWindow class
//! The main window class has all the managerial responsibilities in the application.
//! It handles the main GUI and the actions between the displayed objects.
//!
class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread workerThread;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:

    void set_tool(std::shared_ptr<Tool>);
    //! Called when a new data - usually from a worker - are ready to be
    //! displayed
    void dataReady(VoxelsOnCartesianGrid<float> f, int index = 0,
                   const QString *active = 0);
    //! Called when a new data - usually from a worker - are ready to be
    //! displayed
    void dataReady_sptr(std::shared_ptr<VoxelsOnCartesianGrid<float> >f, int index = 0,
                        const QString *active = 0);
    //! Called when a new data - usually from a worker - are ready to be
    //! displayed
    void dataReady_ptr(VoxelsOnCartesianGrid<float>* f, int index = 0,
                       const QString *active = 0);

signals:
    void subWindowReady(QWidget*);

private slots:

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

    void on_actionOpenFile_triggered();

    void on_actionAbout_QT_triggered();

    void updateRecentFileActions();

    void openRecentFile();
    //! Update the state of the interface after every subwindown action
    void updateMenus(QMdiSubWindow *activeSubWindow = NULL);

    void on_listOpenedFiles_itemDoubleClicked(QListWidgetItem *item);
    //! Remove item from the "opened files" list.
    //! Various very important disconnections will take place here.
    void removeFromOpenedList();

    void removeFromGroupedList();

    //!
    //! \brief on_actionSwap_View_mode_triggered
    //! Changes the view order to either Sinograms or Viewgrams
    void on_actionSwap_View_mode_triggered();

    //!
    //! \brief highlightChecked
    //! \param item
    //! This function highlights listed items that have been grouped.
    //! In addition, it performs the grouping, by appending the new
    //! object to the grouped list.
    void highlightChecked(QListWidgetItem *item);

    void grouped_scrolling(qint16 value);

    void grouped_segment_changed(QString value);

    void on_actionExport_as_arrays_triggered();

    void on_actionSettings_triggered();

    void on_actionNEMA_Analysis_triggered();

    void on_actionProfiler_triggered();

    void on_actionSplit_triggered();

    void tileSubWindowsVertically();

    void tileSubWindowsHorizontally();

    void on_showAllViews_clicked();

    void on_showAxialView_clicked();

    void on_showCorView_clicked();

    void on_showSagView_clicked();

    void on_chk_show_cursors_clicked();

    void on_actionIVIM_toolbox_triggered();

    void showListContextMenu(const QPoint &pos);

    void on_actionThreshold_triggered();

    void on_actionContrast_triggered();

    void on_action_Measure_triggered();

    void on_chk_show_axis_stateChanged(int arg1);

    void on_actionImage_calculator_triggered();

private:
    enum { MaxRecentFiles = 10 };

    Ui::MainWindow *ui;

    void readSettings();

    void writeSettings();
    //! Most actions are created and managed in the UI, some depend on the
    //! initializations taking place there and have to run later.
    void createMoreActions();

    bool append_to_workspace(Screen_manager *child, bool prepend_to_recent=true, bool minimized = false);

    bool openFile(const QString fileName);

    bool loadFile(const QString &fileName);

    static bool hasRecentFiles();

    void prependToRecentFiles(const QString &fileName);

    void setRecentFilesVisible(bool visible);

    Configuration *get_current_configuration();

    Screen_manager *createMdiChild(QString *suffix = NULL);

    QMdiSubWindow *findMdiChild(const QString &fileName) const;

    int getCountMdiChild(const QString &fileName) const;

    Screen_manager *findMdiChildInGrouped(const QString &fileName) const;

    bool forceSync(Screen_manager* from, Screen_manager* to);

    Screen_manager *activeMdiChild() const;

    int numMdiChild() const;
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

    QAction *recentFileActs[MaxRecentFiles];

    QAction *recentFileSeparator;

    QAction *recentFileSubMenuAct;

    QAction *tileAct;

    QAction *cascadeAct;

    QAction *closeAllAct;

    QAction *tileVerticalAct;

    QAction *tileHorizontalAct;

    void appendToOpenedList(Screen_manager* child);

    qint16 findQListWidgetItem(QListWidget *this_list, Screen_manager *item);

    QString m_toolSelected;

    QVector<Screen_manager*> groupedItems;

    std::shared_ptr<Tool> currentTool;

    Configuration* defaults;

    unique_ptr<AnalysisNema> nemaWindow;

    Screen_manager* previous_screen;

    QDockWidget* dc_right, *dc_bottom, *dc_contrast, *dc_stats, *dc_math;

    ToolManager* toolMan;

    ContrastMan* contrastMan;

    MathMan* mathMan;

    ListROIStat* statMan;
};

#endif // MAINWINDOW_H
