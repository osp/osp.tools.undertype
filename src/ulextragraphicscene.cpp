//
// C++ Implementation: ulextragraphicscene
//
// Description:
//
//
// Author: Pierre Marchand <pierre@moulindetouvois.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ulextragraphicscene.h"
#include "ulextragraphic.h"
#include "ulpageitem.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QGraphicsTextItem>
#include <QDebug>

ULExtraGraphicScene::ULExtraGraphicScene ( ULPageItem* p )
{
	page = p;
	QFont tf("Helvetica", 13);
	tf.setBold(true);
	QGraphicsTextItem * text = addText("Type [a] to add a graphic to this collection\nType [p] to push the focused item on page", tf);
	text->setZValue(0.0);
	text->setEnabled(false);
	text->setPos(0.0,0.0);
	
	insertPos = text->boundingRect().height();
}


ULExtraGraphicScene::~ULExtraGraphicScene()
{}


void ULExtraGraphicScene::keyPressEvent ( QKeyEvent * keyEvent )
{
	if ( keyEvent->key() == Qt::Key_A )
	{
		add();
	}
	if ( keyEvent->key() == Qt::Key_P )
	{
		push();
	}
}

void ULExtraGraphicScene::add()
{
	QString filename = QFileDialog::getOpenFileName ( 0, "Load graphic file" ) ;
	if ( filename.isEmpty() )
		return;

// 	ULExtraGraphic *it = new ULExtraGraphic ( 0,this,filename );
// 	graphics.insert(filename, it );
// 	it->setPos ( 0,insertPos );
// // 	addItem(it);
// 	insertPos += it->boundingRect().height();
	ULExtraGraphic *it;
	if(!graphics.contains(filename))
	{
		it = new ULExtraGraphic ( 0,this,filename );
		graphics.insert(filename, it);
		it->setPos ( 0,insertPos );
		addItem(it);
		insertPos += it->boundingRect().height();
	}
	else
	{
		QMessageBox::information (0 /* views().first()*/, "Ut Info", "This file is yet in collection,\n it would be a mess to include it twice.\nThen, I won't.");
	}


}
void ULExtraGraphicScene::load(QString file, double x, double y, double z)
{
	if(file.isEmpty())
		return;
	
	ULExtraGraphic *it;
	if(!graphics.contains(file))
	{
		it = new ULExtraGraphic ( 0,this,file );
		graphics.insert(file, it);
		it->setPos ( 0,insertPos );
		insertPos += it->boundingRect().height();
	}
	else
	{
		it = graphics.value(file);
	}
	
	QGraphicsItem * actit = it->clone();
	page->addItem ( actit );
	actit->setFlags (QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable );
	actit->setPos ( x,y );
	actit->setZValue(z);
	
}

QGraphicsItem * ULExtraGraphicScene::push()
{
	if ( selectedItems().isEmpty( ))
		return 0;

	ULExtraGraphic *it = static_cast<ULExtraGraphic*> ( focusItem()->parentItem() );
	if ( it->data(1).toString() == "graphic" &&  it->isAlive() )
	{
		QGraphicsItem * actit = it->clone();
		qDebug() << "clone gave " << actit;
		page->addItem ( actit );
		actit->setPos ( page->cur_pos ); // I wonder where to put the page cursor. I guess it's better to keep it in place.
		actit->setFlags (QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable );
		actit->setZValue(1.0);
		qDebug() << "graphic successfully pushed - " << actit->boundingRect();
		return actit;
	}
	else
	{
		qDebug() << focusItem();
		return 0;
	}
}




