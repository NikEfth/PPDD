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


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ui->cmb_colormap->addItems(Viewer::ColorMap::getColormapList());

    connect(ui->mdiArea, &QMdiArea::subWindowActivated,
                this, &MainWindow::updateMenus);

    updateMenus();
    createMoreActions();

    QDir init_dir = QDir::homePath();
    init_path = init_dir.absolutePath();
    this->setWindowIcon(QIcon(":icons/resources/icons8-microbeam-radiation-therapy-80.png"));

    setUnifiedTitleAndToolBarOnMac(true);
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

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::on_actionOpenFile_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Header"), init_path, tr("STIR Interfile Header (*.hs)"));

    if (fileName.size() == 0)
        return;

    // Change the starting point to the previous path for convinience.
    init_path = QFileInfo(fileName).absolutePath()+"/";

    openFile(fileName);
}

bool MainWindow::openFile(const QString fileName)
{
    if (QMdiSubWindow *existing = findMdiChild(fileName)) {
            ui->mdiArea->setActiveSubWindow(existing);
            //existing->installEventFilter()
            return true;
        }
        const bool succeeded = loadFile(fileName);

    Screen_manager *child = createMdiChild();
    ui->mdiArea->addSubWindow(child);

    return child;
}

bool MainWindow::loadFile(const QString &fileName)
{
    Screen_manager *child = createMdiChild();

    const bool succeeded = child->loadFile(fileName);

    if (succeeded)
        child->show();
    else
        child->close();
    MainWindow::prependToRecentFiles(fileName);
    appendToOpenedList(child);
    return succeeded;
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
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
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
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

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
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

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

Screen_manager *MainWindow::createMdiChild()
{
    Screen_manager *child = new Screen_manager(get_current_configuration(), this);
    QObject::connect(child, SIGNAL(closed(QString)), this,SLOT(removeFromOpenedList(QString) ));
    ui->mdiArea->addSubWindow(child);

    return child;
}

Configuration* MainWindow::get_current_configuration()
{
    Configuration* curConfig = new Configuration();
    curConfig->curColorMap_index = ui->cmb_colormap->currentIndex();
    curConfig->numViewPorts = ui->cmb_num_viewports->currentText().toInt();

    return curConfig;
}

void MainWindow::on_cmb_num_viewports_currentIndexChanged(int index)
{
    if(activeMdiChild())
    {
        activeMdiChild()->initialise_plot_area(ui->cmb_num_viewports->currentText().toInt());
        activeMdiChild()->set_up_plot_area();
    }
}

void MainWindow::on_cmb_colormap_currentIndexChanged(int index)
{
    // TODO: Should be applied to all opened files. Not just the active.
    if(activeMdiChild())
        activeMdiChild()->changeCM(index);
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        Screen_manager *mdiChild = qobject_cast<Screen_manager *>(window->widget());
        if (mdiChild->getMyFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (numMdiChild() != 0);
    ui->cmb_colormap->setEnabled(hasMdiChild);
    ui->cmb_num_viewports->setEnabled(hasMdiChild);
    ui->toolBox->setEnabled(hasMdiChild);
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
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog* about = new AboutDialog(this);
    about->exec();
//    QMessageBox::about(this,"Sinogram Viewer", "v.0.1. \n"
//                                               "Nikos Efthimiou (n.efthymiou AT hull.ac.uk");
}

int MainWindow::checkIfRelativeItemAlreadyInList(QListWidget* this_list, QString item)
{
    int reps = 0;
    for (int i = 0; i < this_list->count(); ++i) {
        if (this_list->item(i)->text() == item) {
            reps ++;
        }
    }
    return reps;
}

int MainWindow::checkIfAbsoluteItemAlreadyInList(QListWidget* this_list, QString item)
{
    for (int i = 0; i < this_list->count(); ++i) {
        if (this_list->item(i)->toolTip() == item) {
            return i;
        }
    }
    return -1;
}


void MainWindow::appendToOpenedList(Screen_manager* child)
{
    int reps = checkIfRelativeItemAlreadyInList(ui->listOpenedFiles, child->getMyFileName());
    QString tmp = child->getMyFileName();
    if (reps > 0)
        tmp += " ("+QString::number(reps)+")";

    QListWidgetItem* tmp_itm = new QListWidgetItem;
    tmp_itm->setText(child->getMyFileName());
    tmp_itm->setToolTip(child->getMyFile());
    ui->listOpenedFiles->addItem(tmp_itm);
}

void MainWindow::removeFromOpenedList(QString name)
{
    int tmp_itm = checkIfAbsoluteItemAlreadyInList(ui->listOpenedFiles, name);

    if (tmp_itm >= 0)
    {
        QListWidgetItem *it = ui->listOpenedFiles->takeItem(tmp_itm);
        delete it;
    }

    updateMenus();
}

void MainWindow::on_actionAbout_QT_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_listOpenedFiles_itemDoubleClicked(QListWidgetItem *item)
{
    QString this_label = item->toolTip();
    if (QMdiSubWindow *existing = findMdiChild(this_label)) {
        ui->mdiArea->setActiveSubWindow(existing);
    }
}
