#include "display_screen.h"
#include "ui_display_screen.h"

display_screen::display_screen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::display_screen)
{
    ui->setupUi(this);
}

display_screen::~display_screen()
{
    delete ui;
}
