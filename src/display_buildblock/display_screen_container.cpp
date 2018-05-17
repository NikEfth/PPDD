#include "display_screen_container.h"
#include "ui_display_screen_container.h"

#include "stir/is_null_ptr.h"
#include "stir/VoxelsOnCartesianGrid.h"

using namespace stir;

display_screen_container::display_screen_container(QWidget *parent, SimpleVector3D<int>* masterCur) :
    QWidget(parent),
    ui(new Ui::display_screen_container)
{
    ui->setupUi(this);

    currentColorMap = 0;
    currentCur = 0;
    hasSetUp = false;
    setFocusPolicy(Qt::StrongFocus);
    active = false;

    iCur = masterCur;

    initialisePlotArea();

    //connect(new Viewer::FocusWatcher(this), & Viewer::FocusWatcher::focusChanged, this, &display_screen_container::activeScreenContainer);
}

display_screen_container::~display_screen_container()
{
    delete ui;
}

void display_screen_container::initialiseSizes()
{
    BasicCoordinate<3, float> spacing = dynamic_cast<VoxelsOnCartesianGrid<float> *> (density_sptr.get())->get_grid_spacing();

    iMin.set((*density_sptr)[0][0].get_min_index(), (*density_sptr)[0].get_min_index(),
            density_sptr->get_min_index());

    iMax.set((*density_sptr)[0][0].get_max_index(),(*density_sptr)[0].get_max_index(),
            density_sptr->get_max_index());

    iRange.set((*density_sptr)[0][0].get_length(),(*density_sptr)[0].get_length(),
            density_sptr->get_length());

    fSpacing.set(spacing[3], spacing[2], spacing[1]);

    fOrigin.set( (iMin.x*fSpacing.x), (iMin.y*fSpacing.y), (iMin.z*fSpacing.z));

    fRange.set(fOrigin.x + (fSpacing.x*iRange.x), fOrigin.y + (fSpacing.y*iRange.y),
               fOrigin.z + (fSpacing.z*iRange.z));
}

void display_screen_container::setViewOrientation(const Orientation& _o)
{
    currentOrientation = _o;

    if (currentOrientation == Transverse)
    {
        minScrolPos = &iMin.z;
        maxScrolPos = &iMax.z;

        minX = &iMin.x;
        maxX = &iMax.x;

        minY = &iMin.y;
        maxY = &iMax.y;

        numRow = &iRange.y;
        numCol = &iRange.x;

        hintRow = &fSpacing.y;
        hintCol = &fSpacing.x;

        minXAxis = &fOrigin.x;
        maxXAxis = &fRange.x;
        minYAxis = &fOrigin.y;
        maxYAxis = &fRange.y;

        locd = "R";
        rocd = "L";
        tocd = "A";
        bocd = "P";

        iCur->z = iRange.z *0.5;
        currentCur = &iCur->z;

        //numPages = static_cast<int>(static_cast<float>(iRange.z)/numViewports);
        //numPages = *maxScrolPos;

    }
    else if (currentOrientation == Coronal)
    {
        minScrolPos = &iMin.y;
        maxScrolPos = &iMax.y;

        minX = &iMin.x;
        maxX = &iMax.x;

        locd = "A";
        rocd = "P";
        tocd = "I";
        bocd = "S";

        minY = &iMin.z;
        maxY = &iMax.z;

        numRow = &iRange.z;
        numCol = &iRange.x;

        hintRow = &fSpacing.z;
        hintCol = &fSpacing.x;

        minXAxis = &fOrigin.x;
        maxXAxis = &fRange.x;
        minYAxis = &fOrigin.z;
        maxYAxis = &fRange.z;

        iCur->y =  (iMax.y + iMin.y)*0.5;; //iRange.y *0.5;
        currentCur = &iCur->y;

        //numPages = static_cast<int>(static_cast<float>(iRange.y)/numViewports);
        //numPages = maxScrolPos;

    }
    else if (currentOrientation == Sagittal)
    {
        minScrolPos = &iMin.x;
        maxScrolPos = &iMax.x;

        minX = &iMin.y;
        maxX = &iMax.y;

        locd = "R";
        rocd = "L";
        tocd = "I";
        bocd = "S";

        minY = &iMin.z;
        maxY = &iMax.z;

        numRow = &iRange.z;
        numCol = &iRange.y;

        hintRow = &fSpacing.z;
        hintCol = &fSpacing.y;

        minXAxis = &fOrigin.y;
        maxXAxis = &fRange.y;
        minYAxis = &fOrigin.z;
        maxYAxis = &fRange.z;

        iCur->x = (iMax.x + iMin.x)*0.5;// iRange.x * 0.5;
        currentCur = &iCur->x;

        //numPages = static_cast<int>(static_cast<float>(iRange.x)/numViewports);
        //numPages = maxScrolPos;
    }

    numPagesPrint =  " / " + QString::number(*maxScrolPos);
    ui->posss->setText(QString::number(*currentCur) + numPagesPrint);

    setUpControls();
    setUpPlotArea();
}


void display_screen_container::setUpControls()
{
    ui->posSelect_scrbr->blockSignals(true);
    ui->posSelect_scrbr->setMinimum(*minScrolPos);
    ui->posSelect_scrbr->setMaximum(*maxScrolPos);
    ui->posSelect_scrbr->setSliderPosition(*currentCur);
    ui->posSelect_scrbr->blockSignals(false);
}


void display_screen_container::initialisePlotArea()
{
    my_screen.reset(new display_screen_raster(this));

    my_screen->setColorMap(currentColorMap);
    my_screen->setAddressAtParent(0);
    ui->horizontalLayout_3->addWidget(my_screen.get());
    QObject::connect(my_screen.get(), &display_screen_raster::activateScreen,
                     this, &display_screen_container::setActiveScreen);
}

void display_screen_container::setColorMap(const qint16 & _c)
{
    my_screen->setColorMap(_c);
}

void display_screen_container::setUpPlotArea()
{

    //  for(int v = 0; v < numViewports; v++)
    {
        //        std::shared_ptr<display_screen_raster> tmp(new display_screen_raster(ui->displ));

        //        std::shared_ptr<display_screen> &tmp = my_screens[v];

        my_screen->set_sizes(*numCol, *numRow,
                             *minX, *minY,
                             *maxX, *maxY,
                             *hintCol, *hintRow,
                             *minXAxis, *maxXAxis, *minYAxis, *maxYAxis);

        my_screen->setOCD(locd, rocd, bocd, tocd);

        //        QObject::connect(this, &display_screen_container::setColorMap,
        //                         dynamic_cast<display_screen_raster*>(my_screens.last().get()),
        //                         & display_screen_raster::setColorMap);


        //        QObject::connect(my_displays.last().get(), &display_screen_raster::save_this_array,
        //                         this, &Screen_manager::save_as_array);
        //        QObject::connect(my_displays.last().get(), &display_screen::scroll_me_to_slice,
        //                         this, &Screen_manager::remote_scrbr_valueChanged);
        ////        QObject::connect(my_displays.last().get(), &display_screen_raster::scrolledTo,
        ////                         this, &Screen_manager::remote_scrbr_valueChanged);
        //        QObject::connect(my_screens.last().get(), &display_screen_raster::activateScreen,
        //                         this, &display_screen_container::setActiveScreen);
    }
    //ui->displ_frame->updateGeometry();

    //activeScreen =  my_screens.first();
    //activeScreen->set_active(true);
    //            connectCurrentHandTool(my_displays.last().get());

    this->adjustSize();
    hasSetUp = true;
}

void display_screen_container::updatePlotArea()
{
    QVector<double>::iterator iter = my_screen->get_data_begin();

    if(currentOrientation == Transverse)
    {
        BasicCoordinate<3,int> coord = make_coordinate(*currentCur, 0, 0);

        for(coord[2]=*minY ;coord[2]<=*maxY;++coord[2])
            for(coord[3]=*minX;coord[3]<=*maxX;++coord[3])
            {
                *iter = static_cast<double>((*density_sptr)[coord]);
                ++iter;
            }
    }
    else if (currentOrientation == Coronal) //
    {
        BasicCoordinate<3,int> coord = make_coordinate(0, *currentCur, 0);
        for(coord[1]=*minY ;coord[1]<=*maxY;++coord[1])
            for(coord[3]=*minX;coord[3]<=*maxX;++coord[3])
            {
                *iter = static_cast<double>((*density_sptr)[coord]);
                ++iter;
            }
    }
    else if (currentOrientation == Sagittal) //
    {
        BasicCoordinate<3,int> coord = make_coordinate(0, 0,*currentCur);
        for(coord[1]=*minY;coord[1]<=*maxY;++coord[1])
            for(coord[2]=*minX;coord[2]<=*maxX;++coord[2])
            {
                *iter = static_cast<double>((*density_sptr)[coord]);
                ++iter;
            }
    }
    my_screen->setMyTitle(*currentCur);
    my_screen->replot_me();

    emit plotAreaUpdated();
}

void display_screen_container::refreshPlotArea()
{
        my_screen->replot_me();
}

void display_screen_container::setActiveScreen(const qint32& _a)
{
    emit activateScreenContainer(myAddress);
}

void display_screen_container::activeScreenContainer(QWidget* _a)
{
    if (!active)
    {
        emit activateScreenContainer(myAddress);
    }
}

std::shared_ptr<display_screen_raster> display_screen_container::getScreen() const
{
    return my_screen;
}

void display_screen_container::mousePressEvent(QMouseEvent *event)
{
    if (!active)
    {
        emit activateScreenContainer(myAddress);
    }
}

void display_screen_container::on_posSelect_scrbr_valueChanged(int value)
{
    if(!active)
        emit activateScreenContainer(myAddress);

    actuallyScroll(value);

    emit scrolled();
}

void display_screen_container::actuallyScroll(const int& value)
{
    if (value > *minScrolPos && value < *maxScrolPos)
    {
        ui->posss->setText(QString::number(value) + numPagesPrint);
        ui->posSelect_scrbr->blockSignals(true);
        ui->posSelect_scrbr->setSliderPosition(value);
        ui->posSelect_scrbr->blockSignals(false);

        if (currentOrientation == Transverse)
            iCur->z = value;
        else if (currentOrientation == Coronal)
            iCur->y = value;
        else
            iCur->x = value;

        updatePlotArea();
    }
}
