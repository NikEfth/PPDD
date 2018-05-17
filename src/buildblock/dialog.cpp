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
#include "dialog.h"
#include "ui_dialog.h"

#include <QLabel>
#include <QPushButton>


Dialog::Dialog(QString space, QWidget *parent) :
    QDialog(parent,Qt::FramelessWindowHint | Qt::Dialog),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //    this->setStyleSheet("border-top-left-radius: 10px;"
    //                        "border-top-right-radius: 10px;"
    //                        "border-bottom-left-radius: 10px;"
    //                        "border-bottom-right-radius: 10px;"
    //                        "border: 1px solid black;"
    //                        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #56a, stop: 0.1 #016);");

//    my_color = background;

    buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->setCenterButtons(true);

    if(space == "projSpace")
    {
        btn_sino = new QPushButton("As Sinogram", this);
        btn_sino->setDefault(true);
        btn_sino->setProperty("ActionRole", 1);

        btn_view = new QPushButton("As Viewgram", this);
        btn_view->setCheckable(true);
        btn_view->setAutoDefault(false);
        btn_view->setProperty("ActionRole", 2);

        buttonBox->addButton(btn_sino, QDialogButtonBox::ActionRole);
        buttonBox->addButton(btn_view, QDialogButtonBox::ActionRole);
        connect(buttonBox,SIGNAL(clicked(QAbstractButton*)), this, SLOT(on_buttonBox_clicked(QAbstractButton*)) );

    }
    else if(space == "imageSpace")
    {
        btn_trans = new QPushButton("Transverse", this);
        btn_trans->setDefault(true);
        btn_trans->setProperty("ActionRole", 3);
//        btn_trans->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);");

        btn_coron = new QPushButton("Coronal", this);
        btn_coron->setAutoDefault(false);
        btn_coron->setProperty("ActionRole", 4);
//        btn_coron->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);");

        btn_sag = new QPushButton("Sagittal", this);
        btn_sag->setAutoDefault(false);
        btn_sag->setProperty("ActionRole", 5);
//        btn_sag->setStyleSheet("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #f6f7fa, stop: 1 #dadbde);");

        buttonBox->addButton(btn_trans, QDialogButtonBox::ActionRole);
        buttonBox->addButton(btn_coron, QDialogButtonBox::ActionRole);
        buttonBox->addButton(btn_sag, QDialogButtonBox::ActionRole);
        connect(buttonBox,SIGNAL(clicked(QAbstractButton*)), this, SLOT(on_buttonBox_clicked(QAbstractButton*)) );


    }
    else if (space == "split")
    {
        QLabel *msg = new QLabel("Split every ___ slices, along the current viewing orientation", this);
        edit = new QLineEdit("",this);
        chkBox = new QCheckBox("Keep oginal?", this);
        ui->gridLayout->addWidget(msg);
        ui->gridLayout->addWidget(edit,1, 0);
        ui->gridLayout->addWidget(chkBox,1,1);
        buttonBox->addButton(QDialogButtonBox::Ok);
        ret = 1;
        connect(buttonBox,SIGNAL(clicked(QAbstractButton*)), this, SLOT(on_buttonBox_clicked(QAbstractButton*)) );

    }
    else
    {
        QLabel *msg = new QLabel(space, this);
        ui->gridLayout->addWidget(msg);
        buttonBox->addButton(QDialogButtonBox::Ok);
        buttonBox->addButton(QDialogButtonBox::Cancel);
    }

    ui->gridLayout->addWidget(buttonBox);


}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_clicked(QAbstractButton *button)
{
    bool ok;
    int in;
    if (ret == 1)
    {
        in =  edit->text().toInt(&ok);
        ok ? done(in) : done(0);
    }
    else
    {
        in = button->property("ActionRole").toInt(&ok)-1;
        ok ? done(in) : done(0);
    }
}
