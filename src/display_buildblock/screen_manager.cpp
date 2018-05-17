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
#include "src/display_buildblock/screen_manager.h"
#include "src/display_buildblock/ui_screen_manager.h"

#include <QLayout>
#include <QFileDialog>
#include <QPushButton>
#include <QScrollBar>
#include <QComboBox>

Screen_manager::Screen_manager(Configuration *_curConfig, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Screen_manager)
{
    ui->setupUi(this);

    is_grouped = false;
    curConfig = _curConfig;
    activeScreenContrainer = NULL;
    containersReady = false;
    cursorStatus = false;
    currentCursor = new  SimpleVector3D<int>(0, 0, 0);

    minValueAll = -1;
    maxValueAll = -1;
    this->axisStatus = false;
 }

Screen_manager::~Screen_manager()
{
    delete currentCursor;
    delete curConfig;
    delete ui;
}

void Screen_manager::closeEvent(QCloseEvent *event)
{
    emit aboutToClose();
}

void Screen_manager::set_up_plot_area()
{
}

display_screen_container *Screen_manager::getActiveDisplayScreen() const
{
    return activeScreenContrainer;
}

void Screen_manager::updateAll()
{
    find_max_value(-1);
    find_min_value(-1);

    rePlotPlots();

    emit updatedContainer();
}

void Screen_manager::refreshPlots()
{
    for (std::shared_ptr<display_screen_container> d: my_containers)
        d->refreshPlotArea();
}

void Screen_manager::rePlotPlots()
{
    for (std::shared_ptr<display_screen_container> d: my_containers)
        d->updatePlotArea();
}

void Screen_manager::setUpContainers(int _num)
{
    for (int i = 0; i < _num; i++)
    {
        std::shared_ptr<display_screen_container> dspc(new display_screen_container(this, currentCursor));

        my_containers.append(dspc);

        my_containers.last()->setAddressAtParent(i);

        QObject::connect(this, &Screen_manager::setColorMap,
                         dspc.get(), &display_screen_container::setColorMap);

        QObject::connect(my_containers.last().get(), &display_screen_container::activateScreenContainer,
                         this, &Screen_manager::setActiveScreenContainer);

        QObject::connect(my_containers.last().get(), &display_screen_container::scrolled,
                         this, &Screen_manager::onCursorChanged);

        my_containers.last()->my_screen->set_min_max_values(get_min_value_all(),
                                                            get_max_value_all());

        ui->gridLayout_2->addWidget(my_containers.last().get(),0,i);
    }

    containersReady = true;
}

double Screen_manager::getCurrentValue()
{
    return this->getValueAt(*currentCursor);
}


void Screen_manager::remote_relative_scrbr_valueChanged(qint32 value)
{
//    value += current_id;
//    remote_scrbr_valueChanged(value);
}

void Screen_manager::remote_scrbr_valueChanged(const qint16& value)
{
//    if (value < max_slider_pos && value >= min_slider_pos)
//    {
//        current_id = value;
//        current_view_id = current_id * num_viewports;
//        ui->posss->setText(QString::number(current_id) + " / " +
//                           QString::number(static_cast<int>(static_cast<float>(num_poss)/num_viewports)) + " pages");

//        ui->posSelect_scrbr->blockSignals(true);
//        ui->posSelect_scrbr->setValue(value);
//        ui->posSelect_scrbr->blockSignals(false);

//        if(has_set_up)
//        {
//            draw_plot_area();

//            if(hasHistograms)
//                draw_histograms();
//        }
//    }
}

bool Screen_manager::operator==(const Screen_manager &other) const
{
    return (this->getFullFilePath()==other.getFullFilePath());
}

bool Screen_manager::operator!=(const Screen_manager &other) const
{
    return !(this->getFullFilePath()==other.getFullFilePath());
}

Configuration* Screen_manager::get_configuration() const
{
    return curConfig;
}

void Screen_manager::showAxis(const bool& _s)
{
    if (this->containersReady)
    {
        this->axisStatus = _s;
        if(!my_containers.at(0)->isHidden())
            my_containers.at(0)->getScreen()->showAxis(axisStatus);
        if(!my_containers.at(1)->isHidden())
            my_containers.at(1)->getScreen()->showAxis(axisStatus);
        if(!my_containers.at(2)->isHidden())
            my_containers.at(2)->getScreen()->showAxis(axisStatus);
    }
}

void Screen_manager::setActiveScreenContainer(const qint32& _a)
{
    if(activeScreenContrainer!= NULL){
        activeScreenContrainer->set_active(false);
        disconnect(activeScreenContrainer, &display_screen_container::plotAreaUpdated,this, &Screen_manager::activeScreenUpdated);
    }
    activeScreenContrainer = my_containers.at(_a).get();
    connect(activeScreenContrainer, &display_screen_container::plotAreaUpdated,this, &Screen_manager::activeScreenUpdated);
    activeScreenContrainer->set_active(true);

    onCursorChanged();
    emit activeScreenUpdated();
}
