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

#include "display_screen.h"

#include <qwt_symbol.h>

#include <QAction>
#include <QFileDialog>
#include <QMenu>

#include "stir/is_null_ptr.h"

#if __has_include("Board.h")
#include "Board.h"
#include "board/Color.h"
#endif

#include <qwt_color_map.h>

display_screen::display_screen(QWidget *parent) :
    QwtPlot(parent)
{

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(const QPoint &  )),this,SLOT(popUpMenu(const QPoint &)));

    setFocusPolicy(Qt::StrongFocus);
    canvas()->setFocusPolicy(Qt::StrongFocus);

    font.setBold( true );
    title.setFont( font );
    title.setRenderFlags( Qt::AlignRight | Qt::AlignBottom );

    lOCD.setFont(font);
    lOCD.setRenderFlags( Qt::AlignLeft );

    rOCD.setFont(font);
    rOCD.setRenderFlags( Qt::AlignRight );

    tOCD.setFont(font);
    tOCD.setRenderFlags( Qt::AlignTop  );

    bOCD.setFont(font);
    bOCD.setRenderFlags( Qt::AlignBottom );

    titleItem = new QwtPlotTextLabel();
    lOCDItem = new QwtPlotTextLabel();
    rOCDItem = new QwtPlotTextLabel();
    bOCDItem = new QwtPlotTextLabel();
    tOCDItem = new QwtPlotTextLabel();

    cursorStatus = false;

    filter = 0;
}

display_screen::~display_screen()
{
    delete titleItem;
    delete lOCDItem;
    delete rOCDItem;
    delete bOCDItem;
    delete tOCDItem;
}

int
display_screen::get_data_size()
{
    return  my_data.size();
}

void display_screen::popUpMenu(const QPoint &pos)
{
    QMenu myMenu;
    QAction *act_saveAs = myMenu.addAction("Save as array");
    act_saveAs->setIcon(QIcon::fromTheme("document-save-as.png"));

    QAction * act_saveAsImage = myMenu.addAction("Save as image");
    act_saveAsImage->setIcon(QIcon::fromTheme("image-x-generic.png"));

    QAction * act_saveAsPDF = myMenu.addAction("Save as PDF");
    act_saveAsPDF->setIcon(QIcon::fromTheme("viewpdf.png"));

#if __has_include("Board.h")
    QAction * act_saveAsVector = myMenu.addAction("Save as Vector");
    act_saveAsVector->setIcon(QIcon::fromTheme("im.vector.svg"));
#endif
    myMenu.addSeparator();

    QAction * act_popOut = myMenu.addAction("Copy");
    act_popOut->setIcon(QIcon(":/resources/gimp-pro/images/gtk-copy.png"));

    myMenu.exec(this->mapToGlobal(pos));
}

void display_screen::mousePressEvent(QMouseEvent *event)
{
    emit activateScreen(myAddress);
}

void display_screen::keyPressEvent( QKeyEvent * event )
{
    if( event->key() == Qt::Key_Delete)
    {
        // do your stuff here
    }
}

void display_screen::clearCursors()
{
    for(QwtPlotItem* d  : this->itemList(QwtPlotItem::Rtti_PlotCurve))
    {
        d->detach();
    }
    this->replot();
}
