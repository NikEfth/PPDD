#include "options_panel.h"
#include "ui_options_panel.h"

Options_panel::Options_panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Options_panel)
{
    ui->setupUi(this);
}

Options_panel::~Options_panel()
{
    delete ui;
}
