#include "screen_manager.h"
#include "ui_screen_manager.h"

Screen_manager::Screen_manager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Screen_manager)
{
    ui->setupUi(this);
}

Screen_manager::~Screen_manager()
{
    delete ui;
}
