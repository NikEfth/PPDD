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
#include "viewmode.h"
#include "ui_viewmode.h"

#include <QPushButton>

enum { Sinogram = 1, Viewgram = 2 };

ViewMode::ViewMode(QWidget *parent) :
    QDialog(parent,Qt::FramelessWindowHint | Qt::Dialog),
    ui(new Ui::ViewMode)
{
    ui->setupUi(this);

    btn_sino = new QPushButton("As Sinogram", this);
    btn_sino->setDefault(true);
    btn_sino->setProperty("ActionRole", Sinogram);

    btn_view = new QPushButton("As Viewgram", this);
    btn_view->setCheckable(true);
    btn_view->setAutoDefault(false);
    btn_view->setProperty("ActionRole", Viewgram);

    buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->setCenterButtons(true);
    buttonBox->addButton(btn_sino, QDialogButtonBox::ActionRole);
    buttonBox->addButton(btn_view, QDialogButtonBox::ActionRole);

    connect(buttonBox,SIGNAL(clicked(QAbstractButton*)), this, SLOT(on_buttonBox_clicked(QAbstractButton*)) );
    ui->gridLayout->addWidget(buttonBox);
}

ViewMode::~ViewMode()
{
    delete ui;
}

void ViewMode::on_buttonBox_clicked(QAbstractButton *button)
{

    if(button->property("ActionRole") == 1 ){
//       HERE GOES MY CODE
        done(1);
    }
    else if(button->property("ActionRole") == 2 ){
//       HERE GOES MY CODE
        done(2);
    }
    else
    {
        done(0);
    }
}
