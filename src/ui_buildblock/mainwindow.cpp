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
#include <QMdiSubWindow>
#include <QComboBox>
#include <QSettings>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QMdiSubWindow>
#include <QVector3D>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/tools_buildblock/worker_splitter.h"
#include "src/tools_buildblock/ivim_toolbox.h"
#include "src/buildblock/aboutdialog.h"
#include "src/ui_buildblock/thresholdman.h"
#include "myqlistwidgetitem.h"

#include "src/display_buildblock/screen_manager_projdata.h"
#include "src/display_buildblock/screen_manager_imagedata.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    readSettings();

    ui->setupUi(this);

    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->cmb_colormap->addItems(Viewer::ColorMap::getColormapList());

    connect(ui->mdiArea, &QMdiArea::subWindowActivated,
            this, &MainWindow::updateMenus);

    connect(ui->listOpenedFiles, SIGNAL(itemChanged(QListWidgetItem*)),
            this, SLOT(highlightChecked(QListWidgetItem*)));

    QDir init_dir = QDir::homePath();
    init_path = init_dir.absolutePath();
    this->setWindowIcon(QIcon(":resources/icons8-microbeam-radiation-therapy-80.png"));

    dc_right = new QDockWidget("Tools and Options", this);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,dc_right);

    //    dc_bottom = new QDockWidget(this);
    //    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,dc_bottom);
    //    dc_bottom->setHidden(true);

    toolMan = new ToolManager(dc_right);
    dc_right->setWidget(toolMan);

    connect(ui->mdiArea, &QMdiArea::subWindowActivated,
            toolMan, &ToolManager::updateMenus);

    connect(toolMan, &ToolManager::tool_selected, this, &MainWindow::set_tool );

    currentTool = toolMan->getCurrentTool();

    //    workerMan = new WorkerManager(this);
    //    workerMan->show();

    dc_contrast = new QDockWidget("Contrast and WL",this);
    contrastMan = new ContrastMan(this);
    dc_contrast->setWidget(contrastMan);
    this->addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea,dc_contrast);
    dc_contrast->setFloating(true);
    dc_contrast->setVisible(false);

    dc_stats = new QDockWidget("Measurements and Stats", this);
    statMan = new ListROIStat(this);
    dc_stats->setWidget(statMan);
    this->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea,dc_stats);
    //    dc_contrast->setVisible(false);

    dc_math = new QDockWidget("Image Operations", this);
    mathMan = new MathMan(ui->listOpenedFiles, this);
    dc_math->setWidget(mathMan);
    this->addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,dc_math);
    dc_math->setVisible(false);
    connect(mathMan, &MathMan::dataReady, this, &MainWindow::dataReady_ptr);

    setUnifiedTitleAndToolBarOnMac(true);
    previous_screen = NULL;

    createMoreActions();
    updateMenus();

    connect(ui->listOpenedFiles, &QListWidget::customContextMenuRequested,this, &MainWindow::showListContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if (ui->mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::showListContextMenu(const QPoint &pos)
{
    bool hasMdiChild = (numMdiChild() != 0);
    if(hasMdiChild)
    {
        QMenu myMenu;
        QAction *actSelectAll = myMenu.addAction("Select All");
        //    actSelect->setIcon(QIcon::fromTheme("gtk-add.png"));

        QObject::connect(actSelectAll, &QAction::triggered, [=]()
        {
            for(int i = 0; i < ui->listOpenedFiles->count(); ++i)
            {
                QListWidgetItem* item = ui->listOpenedFiles->item(i);
                item->setCheckState(Qt::Checked);
            }
        });

        QAction *actDeselectAll = myMenu.addAction("Deselect All");
        //    actSelect->setIcon(QIcon::fromTheme("gtk-add.png"));

        QObject::connect(actDeselectAll, &QAction::triggered, [=]()
        {
            for(int i = 0; i < ui->listOpenedFiles->count(); ++i)
            {
                QListWidgetItem* item = ui->listOpenedFiles->item(i);
                item->setCheckState(Qt::Unchecked);
            }
        });

        QAction *actCloseAll = myMenu.addAction("Close Selected");
        //    actClose->setIcon(QIcon::fromTheme("gimp-clsoe.png"));

        QObject::connect(actCloseAll, &QAction::triggered, [=]()
        {
            while(groupedItems.size() > 0)
            {
                Screen_manager* item =  groupedItems.first();
                QMdiSubWindow *existing = findMdiChild(item->getFullFilePath());
                existing->close();
            }
        });

        myMenu.exec(this->mapToGlobal(pos));
    }
}


void MainWindow::readSettings()
{
    QSettings settings;

    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }

    settings.beginGroup("Configuration");
    defaults = new Configuration(this);
    defaults->curColorMap_index = static_cast<qint8>(settings.value("default_colorMapIndex").toInt());
    defaults->numViewPorts =static_cast<qint8>( settings.value("default_numViewports").toInt());
    defaults->showHistogram = static_cast<qint8>(settings.value("default_showHistogram").toBool());
    defaults->nBins = static_cast<qint8>(settings.value("default_N_bins_Histograms").toInt());
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.beginGroup("Configuration");
    settings.setValue("default_colorMapIndex", defaults->curColorMap_index);
    settings.setValue("default_numViewports", defaults->numViewPorts);
    settings.setValue("default_showHistogram", defaults->showHistogram);
    settings.setValue("default_N_bins_Histograms", defaults->nBins);
}

void MainWindow::on_actionOpenFile_triggered()
{
    const QStringList fileNames =
            QFileDialog::getOpenFileNames(this,
                                          tr("Open Header"), init_path, tr("STIR Interfile Header (*.hs);;"
                                                                           "STIR Image Header (*.hv);;"
                                                                           "Dicom Images (*.dicom);;"
                                                                           "All Files (*.*)"));

    if (fileNames.size() == 0)
        return;

    for (QString fileName : fileNames)
    {
        // Some basic checks
        QFileInfo fi(fileName);
        if (!(fi.suffix() == "hs" ||
              fi.suffix() == "hv" ||
              fi.suffix() == "dicom"||
              fi.suffix() == "dcm"))
            return;

        // Change the starting point to the previous path for convinience.
        init_path = fi.absolutePath();

        openFile(fileName);
    }
}

bool MainWindow::openFile(const QString fileName)
{
    if (QMdiSubWindow *existing = findMdiChild(fileName)) {
        ui->mdiArea->setActiveSubWindow(existing);
        //existing->installEventFilter()
        return true;
    }
    const bool succeeded = loadFile(fileName);
    return succeeded;
}

bool MainWindow::loadFile(const QString &fileName)
{
    if (QFileInfo::exists(fileName))
    {
        QString suffix = QFileInfo(fileName).suffix();

        Screen_manager *child = createMdiChild(&suffix);

        if (!child)
            return false;

        const bool succeeded = child->loadFile(fileName);

        if (succeeded)
        {
            append_to_workspace(child);
        }
        else
            child->close();

        return succeeded;
    }

    return false;
}

bool MainWindow::append_to_workspace(Screen_manager *child, bool prepend_to_recent, bool minimized)
{
    QObject::connect(child, SIGNAL(aboutToClose()), this,SLOT(removeFromOpenedList()));
    QObject::connect(child, SIGNAL(aboutToClose()), this,SLOT(removeFromGroupedList()));
    QObject::connect(child, SIGNAL(updatedContainer()), this,SLOT(updateMenus()));

    ui->mdiArea->addSubWindow(child);
    ui->mdiArea->setFocusPolicy(Qt::StrongFocus);

    if (!minimized)
        child->show();
    else
        child->showMinimized();

    appendToOpenedList(child);

    if(prepend_to_recent)
        MainWindow::prependToRecentFiles(child->getFullFilePath());
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings;
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings;

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

Screen_manager *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<Screen_manager *>(activeSubWindow->widget());
    return 0;
}

int MainWindow::numMdiChild() const
{
    return ui->mdiArea->subWindowList().size();
}

Screen_manager *MainWindow::createMdiChild(QString* suffix)
{
    Configuration* curConfig = new Configuration();
    Screen_manager *child;
    if(is_null_ptr(suffix))
        return NULL;

    if(*suffix == "hs")
        child = new screen_manager_ProjData(curConfig, this);
    else if (*suffix == "hv" || *suffix == "dcm")
    {
        child = new Screen_manager_ImageData(curConfig, this);
    }
    else
    {
        Dialog* msg = new Dialog("Unknow file type!",this);
        msg->exec();
        return 0;
    }

    return child;
}

Configuration* MainWindow::get_current_configuration()
{
    Configuration* curConfig = new Configuration();
    curConfig->curColorMap_index = ui->cmb_colormap->currentIndex();
    //    curConfig->showHistogram = ui->chk_show_histrogram->checkState();

    return curConfig;
}

void MainWindow::on_cmb_num_viewports_currentIndexChanged(int index)
{
    //    if(activeMdiChild())
    //    {
    //        Screen_manager* current =  activeMdiChild();
    //        if(groupedItems.count()>0)
    //        {
    //            if(findMdiChildInGrouped(current->getFullFilePath()))
    //            {
    //                for (Screen_manager* s: groupedItems)
    //                {
    //                    s->set_num_viewports(ui->cmb_num_viewports->currentText().toShort());
    //                }
    //            }
    //            else
    //                current->set_num_viewports(ui->cmb_num_viewports->currentText().toShort());
    //        }
    //        else
    //        {

    //            activeMdiChild()->set_num_viewports(ui->cmb_num_viewports->currentText().toInt());
    //        }
    //    }
}

void MainWindow::on_cmb_colormap_currentIndexChanged(int index)
{
    if(activeMdiChild())
        activeMdiChild()->setColorMap(index);
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    //QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    for(QMdiSubWindow *window: ui->mdiArea->subWindowList()) {
        Screen_manager *mdiChild = qobject_cast<Screen_manager *>(window->widget());
        if (mdiChild->getFullFilePath() == fileName)
            return window;
    }
    return 0;
}

int MainWindow::getCountMdiChild(const QString &fileName) const
{
    int ret = 0;
    QRegularExpression f("<\\d\\d\\d>");

    for(QMdiSubWindow *window: ui->mdiArea->subWindowList()) {
        Screen_manager *mdiChild = qobject_cast<Screen_manager *>(window->widget());
        if (mdiChild->getFullFilePath() == fileName)
        {
           QRegularExpressionMatch match = f.match(fileName);
            if (match.hasMatch())
                return match.captured().toInt();

            ret++;
        }
    }
    return ret;
}



Screen_manager *MainWindow::findMdiChildInGrouped(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    for(Screen_manager *mdiChild: groupedItems) {
        if (mdiChild->getFullFilePath() == canonicalFilePath)
            return mdiChild;
    }
    return 0;
}

void MainWindow::updateMenus(QMdiSubWindow * activeSubWindow)
{
    bool hasMdiChild = (numMdiChild() != 0);

    //* Update the interface Actions / Buttons
    ui->cmb_colormap->setEnabled(hasMdiChild);
    ui->actionSwap_View_mode->setEnabled(hasMdiChild);
    ui->actionProfiler->setEnabled(hasMdiChild);
    ui->actionIVIM_toolbox->setEnabled(groupedItems.size()>1?true:false);
    ui->showAllViews->setEnabled(hasMdiChild);
    ui->showAxialView->setEnabled(hasMdiChild);
    ui->showCorView->setEnabled(hasMdiChild);
    ui->showSagView->setEnabled(hasMdiChild);
    ui->actionShow_all_views->setEnabled(hasMdiChild);
    ui->actionTransverse_Only->setEnabled(hasMdiChild);
    ui->actionCoronal_only->setEnabled(hasMdiChild);
    ui->actionSagital_only->setEnabled(hasMdiChild);
    ui->actionNEMA_Analysis->setEnabled(hasMdiChild);
    ui->actionSplit->setEnabled(hasMdiChild);
    ui->actionThreshold->setEnabled(hasMdiChild);
    ui->actionContrast->setEnabled(hasMdiChild);
    ui->action_Measure->setEnabled(hasMdiChild);
    ui->actionImage_calculator->setEnabled(hasMdiChild);
    ui->chk_show_axis->setEnabled(hasMdiChild);
    ui->chk_show_cursors->setEnabled(hasMdiChild);
    contrastMan->setEnabled(hasMdiChild);
    toolMan->setEnabled(hasMdiChild);
    //* Update

    if (activeSubWindow != NULL) // If there is an active subWindow
    {
        Screen_manager * my_screen =
                qobject_cast<Screen_manager *>(activeSubWindow->widget());

        if(my_screen != previous_screen)
        {

            ui->cmb_colormap->blockSignals(true);
            ui->cmb_colormap->setCurrentIndex(my_screen->getColorMapIndex());
            ui->cmb_colormap->blockSignals(false);

            ui->chk_show_axis->setChecked(my_screen->getAxisStatus());

            ui->chk_show_cursors->setChecked(my_screen->getCursorStatus());

            if (!dc_contrast->isHidden())
            {
                contrastMan->unsetScreen();
                contrastMan->setScreen(my_screen);
            }

            // Disconnect previous
            toolMan->unsetScreen();
            // tool -> setScreen
            toolMan->setScreen(my_screen);

            // Store in previous to keep history and avoid updating
            // on simple clicks in the same window.
            previous_screen = my_screen;

            QString dataType = my_screen->getDataType();

            if (dataType == "imageData")
            {
                ui->actionNEMA_Analysis->setEnabled(true);
                ui->actionSplit->setEnabled(true);
                const Orientation *u = my_screen->getDisplayLayout();

                if(*u == All)
                    ui->showAllViews->setChecked(true);
                else if (*u == Transverse)
                    ui->showAxialView->setChecked(true);
                else if (*u == Coronal)
                    ui->showCorView->setChecked(true);
                else
                    ui->showSagView->setChecked(true);
            }
            else
            {
                ui->actionNEMA_Analysis->setEnabled(false);
                ui->actionSplit->setEnabled(false);
            }
        }
    }

}

void MainWindow::createMoreActions()
{
    ui->menuFile->addSeparator();

    QMenu *recentMenu = ui->menuFile->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = ui->menuFile->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
    ui->menuWindow->addAction(tileAct);

    tileVerticalAct = new QAction(tr("Tile Vertically"), this);
    tileVerticalAct->setStatusTip(tr("Tile the windows vertically"));
    connect(tileVerticalAct, SIGNAL(triggered()), this, SLOT(tileSubWindowsVertically()));
    ui->menuWindow->addAction(tileVerticalAct);

    tileHorizontalAct = new QAction(tr("Tile Horizontally"), this);
    tileHorizontalAct->setStatusTip(tr("Tile the windows horizontally"));
    connect(tileHorizontalAct, SIGNAL(triggered()), this, SLOT(tileSubWindowsHorizontally()));
    ui->menuWindow->addAction(tileHorizontalAct);

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
    ui->menuWindow->addAction(cascadeAct);

    ui->menuWindow->addSeparator();

    closeAllAct = new QAction(tr("&Close All"), this);
    closeAllAct->setStatusTip(tr("Close all windows"));
    connect(closeAllAct, SIGNAL(triggered()), ui->mdiArea, SLOT(closeAllSubWindows()));
    ui->menuWindow->addAction(closeAllAct);
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog* about = new AboutDialog(this);
    about->exec();
    //    QMessageBox::about(this,"Sinogram Viewer", "v.0.1. \n"
    //                                               "Nikos Efthimiou (n.efthymiou AT hull.ac.uk");
}

qint16 MainWindow::findQListWidgetItem(QListWidget* this_list, Screen_manager* item)
{
    for (qint16 i = 0; i < this_list->count(); i++)
        if (dynamic_cast<myQListWidgetItem *>(this_list->item(i))->get_mdiWindow_ptr() == item)
            return i;

    return -1;
}


void MainWindow::appendToOpenedList(Screen_manager* child)
{

    myQListWidgetItem* tmp_itm =
            new myQListWidgetItem();

    tmp_itm->set_mdiWindow_ptr(child);

    tmp_itm->setFlags(tmp_itm->flags() | Qt::ItemIsUserCheckable);
    tmp_itm->setCheckState(Qt::Unchecked);

    ui->listOpenedFiles->addItem(tmp_itm);
    ui->listOpenedFiles->setCurrentItem(tmp_itm);

    mathMan->updateOpenedFiles();
}

void MainWindow::removeFromOpenedList()
{
    // Find who send this
    Screen_manager* src = qobject_cast<Screen_manager *>(sender());
    previous_screen = NULL;

    // Get sender's row number
    qint16 rowNum = findQListWidgetItem(ui->listOpenedFiles, src);

    if (rowNum >= 0)
    {
        ui->listOpenedFiles->takeItem(rowNum); //row
        // Should delete ... but it is getting destroyed soon.
        // Disconnect from tool
        toolMan->unsetScreen();
        // Disconnect from contrast window
        contrastMan->unsetScreen();
        // The Math Manager must have an up-to-date list of the opened
        // files.
        mathMan->updateOpenedFiles();
        // If another similar type of connection exist in the future
        // it has to be disconnected here.
    }
    // Activate the window which is before the closed. - Feels cool.
    if(ui->listOpenedFiles->count()>0)
    {
        QString this_label = ui->listOpenedFiles->currentItem()->toolTip();

        if (QMdiSubWindow *existing = findMdiChild(this_label)) {
            ui->mdiArea->setActiveSubWindow(existing);
        }
    }
}

void MainWindow::removeFromGroupedList()
{
    Screen_manager* src = qobject_cast<Screen_manager *>(sender());

    // If any groups exists
    if(groupedItems.count()>0)
    {
        for (Screen_manager* i: groupedItems)
        {
            if (*i == *src)
            {
                groupedItems.removeOne(i);
                i->setGrouped(false);
                disconnect(i, SIGNAL(scrolledTo(qint16)), this, SLOT(grouped_scrolling(qint16)));
                if(screen_manager_ProjData* i_p = dynamic_cast<screen_manager_ProjData*>(i))
                    disconnect(i_p, SIGNAL(segmentChangedTo(QString)),
                               this, SLOT(grouped_segment_changed(QString)));
            }
        }
    }
}

void MainWindow::on_actionAbout_QT_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_listOpenedFiles_itemDoubleClicked(QListWidgetItem *item)
{
    if (QMdiSubWindow *existing = findMdiChild(item->toolTip())) {
        ui->mdiArea->setActiveSubWindow(existing);
    }
}

void MainWindow::on_actionSwap_View_mode_triggered()
{
    Screen_manager* sc = activeMdiChild();
    sc->setOrientation();

    if(findMdiChildInGrouped(sc->getFullFilePath()))
    {
        for (Screen_manager* s: groupedItems)
            if(*s != *sc)
                s->setOrientation();
    }
}

void MainWindow::highlightChecked(QListWidgetItem *item)
{
    if(item->checkState() == Qt::Checked)
    {
        myQListWidgetItem *tmp_itm = reinterpret_cast <myQListWidgetItem *>(item);

        Screen_manager *mdiChild = tmp_itm->get_mdiWindow_ptr();
        QString dataType = mdiChild->getDataType();

        if(groupedItems.count()>0)
        {
            if( dataType != groupedItems.at(0)->getDataType())
            {
                ui->listOpenedFiles->blockSignals(true);
                item->setCheckState(Qt::CheckState::Unchecked);
                ui->listOpenedFiles->blockSignals(false);
                return;
            }

            //            if(!forceSync(groupedItems.at(0), mdiChild))
            //            {
            //                ui->listOpenedFiles->blockSignals(true);
            //                item->setCheckState(Qt::CheckState::Unchecked);
            //                ui->listOpenedFiles->blockSignals(false);
            //                return;
            //            }
        }

        groupedItems.append(mdiChild);
        mdiChild->setGrouped(true);

        //connect(mdiChild, SIGNAL(scrolledTo(qint16)), this, SLOT(grouped_scrolling(qint16)));

        //if(dataType == "projData" )
        //    connect(mdiChild, SIGNAL(segmentChangedTo(QString)), this, SLOT(grouped_segment_changed(QString)));

        ui->listOpenedFiles->blockSignals(true);
        item->setBackgroundColor(QColor("#ffffb2"));
        ui->listOpenedFiles->blockSignals(false);

    }
    else
    {
        ui->listOpenedFiles->blockSignals(true);
        item->setBackgroundColor(QColor("#ffffff"));
        ui->listOpenedFiles->blockSignals(false);

        myQListWidgetItem *tmp_itm = reinterpret_cast <myQListWidgetItem *>(item);

        Screen_manager *mdiChild = tmp_itm->get_mdiWindow_ptr();

        groupedItems.removeOne(mdiChild);
        mdiChild->setGrouped(false);
        //        disconnect(mdiChild, SIGNAL(scrolledTo(qint16)), this, SLOT(grouped_scrolling(qint16)));
        //        if(mdiChild->getDataType() == "projData" )
        //            disconnect(mdiChild, SIGNAL(segmentChangedTo(QString)), this, SLOT(grouped_segment_changed(QString)));
    }
    ui->actionIVIM_toolbox->setEnabled(groupedItems.size()>1?true:false);
}

bool MainWindow::forceSync(Screen_manager* from, Screen_manager* to)
{
    //    screen_manager_ProjData* fromProj = dynamic_cast<screen_manager_ProjData* > (from);
    //    screen_manager_ProjData* toProj = dynamic_cast<screen_manager_ProjData* > (to);

    //    if (!is_null_ptr(fromProj) && !is_null_ptr(toProj))
    //        return toProj->setState(fromProj->igetDisplayLayout(),
    //                                fromProj->getCurrentCursor(),
    //                                fromProj->getCurrentSegmentNumber(),
    //                                fromProj->getViewportsIndex());

    //    Screen_manager_ImageData* fromIm = dynamic_cast<Screen_manager_ImageData* > (from);
    //    Screen_manager_ImageData* toIm = dynamic_cast<Screen_manager_ImageData* > (to);

    //    if (!is_null_ptr(fromIm) && !is_null_ptr(toIm))
    //        return toIm->setState(fromIm->igetDisplayLayout(),
    //                              fromIm->getCurrentCursor(),
    //                              fromIm->getViewportsIndex());

}

void MainWindow::grouped_scrolling(qint16 value)
{
    Screen_manager* src = qobject_cast<Screen_manager *>(sender());
    for (Screen_manager* s: groupedItems)
    {
        if (*src != *s)
            s->remote_scrbr_valueChanged(value);
    }
}

void MainWindow::grouped_segment_changed(QString value)
{
    if (screen_manager_ProjData* src = qobject_cast<screen_manager_ProjData *>(sender()))
    {
        for (Screen_manager* s: groupedItems)
        {
            screen_manager_ProjData* s_p = qobject_cast<screen_manager_ProjData *>(s);
            if (*src != *s_p)
                s_p->remote_segment_change(value);
        }
    }
}


void MainWindow::on_actionExport_as_arrays_triggered()
{
    if (groupedItems.size() > 0)
    {
        QString filePath = QFileDialog::getExistingDirectory(this, tr("Save to path:"));

        if (filePath.size() == 0 )
            return;

        for (Screen_manager* s: groupedItems)
        {
            s->writeToDisk(filePath);
        }
    }
    else
    {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save array"), "",
                                                        tr("STIR Interfile Image (*.hv)"));

        if (fileName.size() == 0 )
            return;

        Screen_manager* sc = activeMdiChild();
        sc->writeToDisk(fileName);
    }
}

void MainWindow::on_actionSettings_triggered()
{

}

void MainWindow::on_actionNEMA_Analysis_triggered()
{
    if(Screen_manager *active = activeMdiChild())
    {
        QString dataType = active->getDataType();

        if (dataType == "imageData")
        {
            //on_toolButtonLinePointer_clicked();
            nemaWindow.reset(new AnalysisNema(active,this));
            nemaWindow->show();
        }

    }
}

void MainWindow::set_tool(std::shared_ptr<Tool> _t)
{
    currentTool = _t;
    if(Screen_manager *active = activeMdiChild())
    {
        // Disconnect previous
        toolMan->unsetScreen();
        // tool -> setScreen
        toolMan->setScreen(active);
    }
}

void MainWindow::on_actionProfiler_triggered()
{
    if(Screen_manager *active = activeMdiChild())
    {
        //        QString dataType = active->getDataType();

        //        if (dataType == "imageData")
        //        {
        //            on_toolButtonLinePointer_clicked();
        //            nemaWindow.reset(new AnalysisNema(active,this));
        //            nemaWindow->show();
        //        }

    }
}

void MainWindow::on_actionSplit_triggered()
{
    QVector<VoxelsOnCartesianGrid<float> > split_list;

    if(Screen_manager *active = activeMdiChild())
    {
        QString dataType = active->getDataType();

        if (dataType == "imageData")
        {
            Dialog *msg = new Dialog("split", this);
            int split_slide = msg->exec();
            if (split_slide>0)
            {
                Worker_Splitter worker(this);
                worker.setSpitSlide(split_slide);
                worker.setData(dynamic_cast<Screen_manager_ImageData* >(active)->getData_ptr());
                worker.process();

                //                worker->moveToThread(&workerThread);
                //                connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);

                //                connect(&workerThread, &QThread::started, worker, &Worker::process);
                //                connect(worker, &Worker::resultReady, this, &MainWindow::on_actionSplit_finished);
                //                workerThread.start();
                //dynamic_cast<Screen_manager_ImageData* >(active)->split(split_slide, split_list);


                //                dynamic_cast<Screen_manager_ImageData* >(active)->set_process_to_perform(
                //                        dynamic_cast<Screen_manager_ImageData* >(active)->split(split_slide, split_list));

                split_list = worker.split_list;

                int i = 1;
                for (VoxelsOnCartesianGrid<float> f : split_list)
                {
                    dataReady(f, i, active->getFullFilePath_ptr());
                    i++;
                }
            }
        }
    }
}

//! \bug New array numbering is inconsistent
void MainWindow::dataReady(VoxelsOnCartesianGrid<float> f, int index,
                           const QString *active)
{
    Configuration* curConfig = new Configuration();
    Screen_manager_ImageData *tmp = new Screen_manager_ImageData(&f, curConfig,this);
    {
        QFileInfo fi(*active);
        int count = getCountMdiChild(*active);
        QString fn;

        if (count == 0)
        {
            QString number =  fi.baseName()+QString("<%1>").arg(count+1, 3, 10, QChar('0'));
            fn = fi.absolutePath() + QDir::separator()+ fn +"_"+number+"."+fi.suffix();
        }
        else
        {
            QRegularExpression f("<\\d\\d\\d>");
            QString number = QString("%1").arg(count, 3, 10, QChar('0'));
            fn = fi.baseName().replace(f, number);
            fn = fi.absolutePath() + QDir::separator()+ fn +"."+fi.suffix();
        }
        tmp->setFullFilePath(fn);
    }
    append_to_workspace(tmp, false, true);
}

//! \bug New array numbering is inconsistent
void MainWindow::dataReady_sptr(std::shared_ptr<VoxelsOnCartesianGrid<float> > f, int index,
                                const QString *active)
{
    Configuration* curConfig = new Configuration();
    Screen_manager_ImageData *tmp = new Screen_manager_ImageData(f.get(), curConfig, this);
    {
        QFileInfo fi(*active);
        int count = getCountMdiChild(*active);
        QString fn;

        if (count == 0)
        {
            QString number =  fi.baseName()+QString("<%1>").arg(count+1, 3, 10, QChar('0'));
            fn = fi.absolutePath() + QDir::separator()+ fn +"_"+number+"."+fi.suffix();
        }
        else
        {
            QRegularExpression f("<\\d\\d\\d>");
            QString number = QString("%1").arg(count, 3, 10, QChar('0'));
            fn = fi.baseName().replace(f, number);
            fn = fi.absolutePath() + QDir::separator()+ fn +"."+fi.suffix();
        }
        tmp->setFullFilePath(fn);
    }
    append_to_workspace(tmp, false, true);
}

//! \bug New array numbering is inconsistent
void MainWindow::dataReady_ptr(VoxelsOnCartesianGrid<float>* f, int index,
                               const QString *active)
{
    Configuration* curConfig = new Configuration();
    Screen_manager_ImageData *tmp = new Screen_manager_ImageData(f, curConfig,this);
    {
        QFileInfo fi(*active);
        int count = getCountMdiChild(*active);
        QString fn;

        if (count == 1)
        {
            QString number =  fi.baseName()+QString("<%1>").arg(count, 3, 10, QChar('0'));
            fn = fi.absolutePath() + QDir::separator()+ number+"."+fi.suffix();
        }
        else
        {
            QRegularExpression f("<\\d\\d\\d>");
            QString number = QString("<%1>").arg(count+1, 3, 10, QChar('0'));
            fn = fi.baseName().replace(f, number);
            fn = fi.absolutePath() + QDir::separator()+ fn +"."+fi.suffix();
        }
        tmp->setFullFilePath(fn);
    }
    append_to_workspace(tmp, false, true);
}

void MainWindow::tileSubWindowsVertically()
{
    if (ui->mdiArea->subWindowList().isEmpty())
        return;

    QPoint position(0, 0);

    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        QRect rect(0, 0, ui->mdiArea->width(), ui->mdiArea->height() / ui->mdiArea->subWindowList().count());
        window->setGeometry(rect);
        window->move(position);
        position.setY(position.y() + window->height());
    }
}

void MainWindow::tileSubWindowsHorizontally()
{
    if (ui->mdiArea->subWindowList().isEmpty())
        return;

    QPoint position(0, 0);

    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        QRect rect(0, 0, ui->mdiArea->width() / ui->mdiArea->subWindowList().count(), ui->mdiArea->height());
        window->setGeometry(rect);
        window->move(position);
        position.setX(position.x() + window->width());
    }
}

void MainWindow::on_showAllViews_clicked()
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
    {
        QSize q;
        QPoint p(0,0);
        QRect r = activeSubWindow->geometry();
        if(Screen_manager *active = qobject_cast<Screen_manager *>(activeSubWindow->widget()))
        {
            q = active->setOrientation(All);
        }
        r.setWidth(q.width());
        activeSubWindow->setGeometry(r);
    }
}

void MainWindow::on_showAxialView_clicked()
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
    {
        QSize q;
        QPoint p(0,0);
        QRect r = activeSubWindow->geometry();
        if(Screen_manager *active = qobject_cast<Screen_manager *>(activeSubWindow->widget()))
        {
            q = active->setOrientation(Transverse);
        }
        r.setWidth(q.width());
        activeSubWindow->setGeometry(r);
    }
}

void MainWindow::on_showCorView_clicked()
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
    {
        QSize q;
        QPoint p(0,0);
        QRect r = activeSubWindow->geometry();
        if(Screen_manager *active = qobject_cast<Screen_manager *>(activeSubWindow->widget()))
        {
            q = active->setOrientation(Coronal);
        }
        r.setWidth(q.width());
        activeSubWindow->setGeometry(r);
    }
}

void MainWindow::on_showSagView_clicked()
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
    {
        QSize q;
        QPoint p(0,0);
        QRect r = activeSubWindow->geometry();
        if(Screen_manager *active = qobject_cast<Screen_manager *>(activeSubWindow->widget()))
        {
            q = active->setOrientation(Sagittal);
        }
        r.setWidth(q.width());
        activeSubWindow->setGeometry(r);
    }
}

void MainWindow::on_chk_show_cursors_clicked()
{
    for(QMdiSubWindow *window: ui->mdiArea->subWindowList())
    {
        Screen_manager *mdiChild = qobject_cast<Screen_manager *>(window->widget());

        mdiChild->showCursor(ui->chk_show_cursors->isChecked());
    }
}

void MainWindow::on_actionIVIM_toolbox_triggered()
{
    if(groupedItems.size() > 1)
    {
        IVIM_toolbox* ivim = new IVIM_toolbox(this->groupedItems, this);
        if (!ivim->initialize())
        {
            delete ivim;
            return;
        }
        ivim->exec();
    }
}

void MainWindow::on_actionThreshold_triggered()
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
    {
        if(Screen_manager *active = qobject_cast<Screen_manager *>(activeSubWindow->widget()))
        {
            ThresholdMan* tMan = new ThresholdMan(active, this);
            if (groupedItems.size()>1)
            {
                tMan->batch(&groupedItems);
            }
            tMan->show();
            connect(tMan, &ThresholdMan::dataReady, this, &MainWindow::dataReady_ptr);
            connect(active, &Screen_manager::aboutToClose, tMan, &ThresholdMan::close);
        }
    }
}

void MainWindow::on_actionContrast_triggered()
{
    if(Screen_manager *active = activeMdiChild())
    {
        if(dc_contrast->isHidden())
        {
            dc_contrast->show();
        }
        else
        {
            dc_contrast->hide();
        }
        if (!dc_contrast->isHidden())
        {
            contrastMan->unsetScreen();
            contrastMan->setScreen(active);
            //disconnect(active, &Screen_manager::activeScreenUpdated, my_histogram, &BarScreen::replot_me);
            //my_histogram->setScreen(active);
            //connect(active, &Screen_manager::activeScreenUpdated, my_histogram, &BarScreen::replot_me);
        }
    }
}


/*

QVector<QVector<float> > split_list;

if(Screen_manager *active = activeMdiChild())
{
    QString dataType = active->getDataType();

    if (dataType == "imageData")
    {
        Dialog *msg = new Dialog("split", this);
        int split_slide = msg->exec();
        if (split_slide>0)
        {
            VoxelsOnCartesianGrid<float>* data = dynamic_cast<Screen_manager_ImageData* >(active)->getData_ptr();
            QVector<float> d =  Mapper::from(*data);
            Worker_Splitter worker(this);
            worker.setSliceLocation(split_slide);
            int sliceSize = 175*175;
            worker.setSliceSize(sliceSize);
            worker.setData(d);
            worker.process();

            /*                worker->moveToThread(&workerThread);
            //                connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);

            //                connect(&workerThread, &QThread::started, worker, &Worker::process);
            //                connect(worker, &Worker::resultReady, this, &MainWindow::on_actionSplit_finished);
            //                workerThread.start();
            //dynamic_cast<Screen_manager_ImageData* >(active)->split(split_slide, split_list);


            //                dynamic_cast<Screen_manager_ImageData* >(active)->set_process_to_perform(
            //                        dynamic_cast<Screen_manager_ImageData* >(active)->split(split_slide, split_list));

            split_list = worker.getResult();

            int i = 1;
            const BasicCoordinate<3,int> new_min = make_coordinate(data->get_min_z(), data->get_min_y(), data->get_min_x());
            const BasicCoordinate<3,int> new_max = make_coordinate(data->get_max_z(), data->get_max_y(), data->get_max_x());

            const IndexRange<3> nrange(new_min, new_max);
            for (QVector<float> f : split_list)
            {
                Array<3, float> fi = Mapper::from<3>(f, nrange);
                VoxelsOnCartesianGrid<float> g(fi,data->get_origin(),data->get_voxel_size());
                Configuration* curConfig = new Configuration();
                Screen_manager_ImageData *tmp = new Screen_manager_ImageData(&g, curConfig,this);
                {
                    QFileInfo ffi(active->getFullFilePath());

                    QFileInfo fi(active->getFileName());

                    QString fn = fi.baseName();
                    QString number = QString("%1").arg(i, 3, 10, QChar('0'));

                    fn = ffi.absolutePath() + QDir::separator()+ fn +"_"+number+"."+fi.suffix();
                    tmp->setFullFilePath(fn);
                }
                append_to_workspace(tmp, false, true);

                i++;
            }
        }
    }
*/

void MainWindow::on_action_Measure_triggered()
{
    if (dc_stats->isHidden())
        dc_stats->show();

    statMan->analyseAll(toolMan->getSelection());
}

void MainWindow::on_chk_show_axis_stateChanged(int arg1)
{
    if(Screen_manager *active = activeMdiChild())
    {
        bool _s = arg1 == 0 ? false : true;
        active->showAxis(_s);
    }
}

void MainWindow::on_actionImage_calculator_triggered()
{
    if(dc_math->isHidden())
    {
        dc_math->show();
        mathMan->updateOpenedFiles();
    }
    else
    {
        dc_math->hide();
    }
}
