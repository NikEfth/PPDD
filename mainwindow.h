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
#include "configuration.h"
#include "common.h"

#include <QMainWindow>
#include <QToolBar>
#include <QToolButton>
#include <QMdiArea>
#include <QListWidget>


namespace Ui {
class MainWindow;
}

using namespace Viewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;


signals:
//    //!
//    //! \brief projdata_loaded
//    //! Signals that a new ProjData has been succeessfully loaded.
//    //! In addition, sends a pointer of that data set to the connected slots.
//    //! \param new_projdata
//    //!
//    void projdata_loaded(std::shared_ptr<ProjData> new_projdata);

//    //!
//    //! \brief projdata_info_loaded
//    //! Signals a new ProjDataInfo and sends a pointer of it.
//    //! \param new_projdata_info
//    //!
//    void projdata_info_loaded(std::shared_ptr<ProjDataInfo> new_projdata_info);

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

    void on_listOpenedFiles_itemDoubleClicked(QListWidgetItem *item);

    void removeFromOpenedList(QString name);

private:
    enum { MaxRecentFiles = 5 };

    Ui::MainWindow *ui;

    void readSettings();

    void writeSettings();

    void updateMenus();

    void createMoreActions();

    bool openFile(const QString fileName);

    bool loadFile(const QString &fileName);

    static bool hasRecentFiles();

    void prependToRecentFiles(const QString &fileName);

    void setRecentFilesVisible(bool visible);

    Configuration *get_current_configuration();

    Screen_manager *createMdiChild();

    QMdiSubWindow *findMdiChild(const QString &fileName) const;

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

    void appendToOpenedList(Screen_manager* child);

    int checkIfRelativeItemAlreadyInList(QListWidget* this_list, QString item);

    int checkIfAbsoluteItemAlreadyInList(QListWidget* this_list, QString item);
};

#endif // MAINWINDOW_H
