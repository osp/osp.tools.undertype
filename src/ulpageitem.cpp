/***************************************************************************
 *   Copyright (C) 2007 by Pierre Marchand   *
 *   pierre@moulindetouvois.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "ulpageitem.h"
#include "ulglyphitem.h"
#include "pagebuffer.h"
#include "ulunit.h"
#include "underline.h"

#include <QDialog>
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLineItem>
#include <QDebug>

ULPageItem::ULPageItem ( double w, double h )
{
	setSceneRect ( 0.0,0.0,w,h );
	setBackgroundBrush ( Qt::lightGray );

	backgroundItem = addRect ( sceneRect() );
	backgroundItem->setEnabled ( false );
	backgroundItem->setBrush ( Qt::white );
	backgroundItem->setData ( 1,"noprint" );
	backgroundItem->setData ( 2,"background" );

	cursorItemV = addLine ( QLineF ( 0.0,0.0,0.0,h ) );
	cursorItemV->setEnabled ( false );
	cursorItemV->setPen ( QPen ( Qt::blue ) );
	cursorItemV->setPos ( 0.0 ,0.0 );
	cursorItemV->setData ( 1,"noprint" );
	cursorItemV->setData ( 2,"cursor" );

	cursorItemH = addLine ( QLineF ( 0.0,0.0,w,0.0 ) );
	cursorItemH->setEnabled ( false );
	cursorItemH->setPen ( QPen ( Qt::blue ) );
	cursorItemH->setPos ( 0.0 ,0.0 );
	cursorItemH->setData ( 1,"noprint" );
	cursorItemH->setData ( 2,"cursor" );

//	hlRect = addRect ( 0.0,0.0,0.0,0.0 );
//	hlRect->setData ( 1,"noprint" );
//	hlRect->setData ( 2,"highlight" );
//	hlRect->setBrush ( QColor ( 0,255,0,80 ) ) ;
//	hlRect->setPen ( QPen ( QColor ( 0,188,0 ) ) );
//	hlRect->setEnabled ( false );
//	hlRect->setZValue ( 0.0 );
//	hlRect->hide();

	_focused = 0;
	viewRatio = 1;
}


ULPageItem::~ULPageItem()
{}

void ULPageItem::setPageSize ( double w, double h )
{
	setSceneRect ( 0,0,w,h );
	backgroundItem->setRect ( 0,0,w,h );
	cursorItemH->setLine ( QLineF ( 0.0,0.0,w,0.0 ) );
	cursorItemV->setLine ( QLineF ( 0.0,0.0,0.0,h ) );
}


void ULPageItem::mousePressEvent ( QGraphicsSceneMouseEvent * e )
{
	qDebug ( "mouse press event in main scene" );
//	hlRect->hide();
	QPointF sp = e->scenePos();
	QGraphicsItem *it = itemAt ( sp );
// 	QGraphicsItemGroup *git;
	if ( it )
	{
		bool ok = false;
		qDebug() << it->data ( 1 ).toString() << ":" << it->data ( 2 ).toString();
		if ( it->data ( 1 ).toString() == "raster" )
		{
			ok = true;
		}
		if ( it->data ( 1 ).toString() == "svg" )
		{
			ok = true;
		}
		if ( !ok && it->data ( 1 ).toString() != "group" )
		{
			it = it->parentItem();
			if ( it->data ( 1 ).toString() != "group" )
				return;
		}
		setFocusItem ( it );
		_focused = it;
//		hlRect->setRect ( it->boundingRect() );
//		hlRect->setPos ( it->pos() );
//		hlRect->show();
		emit infoCurrentLine(QString("LineInfo : pos( %1 , %2 )").arg(it->pos().x()).arg(it->pos().y() + it->data(3).toDouble()));

	}
//	else
//	{
//		hlRect->hide();
//	}
	QGraphicsScene::mousePressEvent(e);
}
void ULPageItem::keyPressEvent ( QKeyEvent * event )
{
	bool realSel= false;

	if ( event->modifiers().testFlag ( Qt::ControlModifier ) )
	{
		qDebug ( "modifier Control" );
		QGraphicsItem * sel;
		if ( focusItem() )
		{
			sel = focusItem();
			if ( sel->data ( 1 ).toString() == "glyph" || sel->data ( 1 ).toString() == "white" )
				sel = sel->parentItem();
			qDebug ( "item got focus" );

		}
		else
		{
			qDebug ( "No item got focus" );
			return;
		}
		switch ( event->key() )
		{
			case Qt::Key_Return : emit wantEditLine ( sel );
				break;
			case Qt::Key_Up: sel->moveBy ( 0.0,-0.1 );
//				hlRect->moveBy ( 0.0,-0.1 );
				break;
			case Qt::Key_Down: sel->moveBy ( 0.0,0.1 );
//				hlRect->moveBy ( 0.0,0.1 );
				break;
			case Qt::Key_Left: sel->moveBy ( -0.1,0.0 );
//				hlRect->moveBy ( -0.1,0.0 );
				break;
			case Qt::Key_Right: sel->moveBy ( 0.1,0.0 );
//				hlRect->moveBy ( 0.1,0.0 );
				break;
			default:break;
		}
		return;
	}
	QGraphicsItem * sel;
	if ( focusItem() )
	{
		sel = focusItem();
		if ( sel->data ( 1 ).toString() == "glyph" || sel->data ( 1 ).toString() == "white" )
			sel = sel->parentItem();

		realSel = true;

	}
	else
	{
		qDebug ( "No item got focus: try _focused" );
		if ( _focused )
		{
			sel=_focused;
			realSel = true;
		}

// 		return;
	}
	QString type = sel->data ( 1 ).toString();
	PageBuffer *pb = PageBuffer::getInstance();
	switch ( event->key() )
	{
		case Qt::Key_Return : if ( realSel ) emit wantEditLine ( sel ) ;
//			hlRect->hide();
			break;
		case Qt::Key_PageUp : views().first()->scale ( 1.1,1.1 );
			viewRatio *= 1.1;
			undertype::getMainWindow()->pageRatioLabel->setText ( QString ( "%1 \%" ).arg ( viewRatio * 100 ) );
			break;
		case Qt::Key_PageDown : views().first()->scale ( 1.0/1.1,1.0/1.1 );
			viewRatio *= 1.0/1.1;
			undertype::getMainWindow()->pageRatioLabel->setText ( QString ( "%1 \%" ).arg ( viewRatio * 100 ) );
			break;
		case Qt::Key_Up: if ( realSel )
			{
				sel->moveBy ( 0.0,-1 );
//				hlRect->moveBy ( 0.0,-1 );
			}
			break;
		case Qt::Key_Down: if ( realSel )
			{
				sel->moveBy ( 0.0,1 );
//				hlRect->moveBy ( 0.0,1 );
			}
			break;
		case Qt::Key_Left:if ( realSel )
			{
				sel->moveBy ( -1,0.0 );

//				hlRect->moveBy ( -1,0.0 );
			}
			break;
		case Qt::Key_Right: if ( realSel )
			{
				sel->moveBy ( 1,0.0 );
//				hlRect->moveBy ( 1,0.0 );
			}
			break;
		case Qt::Key_Delete : if ( type == "raster" || type == "svg" )
			{
				removeItem ( sel );
//				hlRect->hide();
				pb->graphics.remove ( sel->data ( 2 ).toString() );
				delete sel;

			}
		default:break;
	}

}
// void ULPageItem::focusInEvent(QFocusEvent * event)
// {
// 	setBackgroundBrush(Qt::white);
// }
//
// void ULPageItem::focusOutEvent(QFocusEvent * event)
// {
// 	setBackgroundBrush(Qt::lightGray);
// }

void ULPageItem::setCur ( double x, double y )
{
	cur_pos.setX ( x );
	cur_pos.setY ( y );
	cursorItemV->setPos ( cur_pos.x(),0.0 );
	cursorItemH->setPos ( 0.0, cur_pos.y() );
	if ( !sender() )
	{
		emit curPosXChanged ( x );
		emit curPosYChanged ( y );
	}
}

void ULPageItem::setCurX ( double x )
{
	cur_pos.setX ( x );
	cursorItemV->setPos ( cur_pos.x(),0.0 );
	if ( !sender() )
	{
		emit curPosXChanged ( x );
	}
}

void ULPageItem::setCurY ( double y )
{
	cur_pos.setY ( y );
	cursorItemH->setPos ( 0.0, cur_pos.y() );
	if ( !sender() )
	{
		emit curPosYChanged ( y );
	}

}

void ULPageItem::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	if ( focusItem() )
	{
		QGraphicsItem * sel = focusItem();
		if ( !sel )
		{
			sel = _focused;
		}
		if ( sel->data ( 1 ).toString() == "glyph" || sel->data ( 1 ).toString() == "white" )
		{
			sel = sel->parentItem();
			emit wantEditLine ( sel );
//			hlRect->hide();
		}
		else if ( sel->data ( 1 ).toString() == "raster" || sel->data ( 1 ).toString() == "svg" )
		{
			double zz = sel->zValue();
			QDialog *dial = new QDialog ( views().first() );

			QGridLayout *lay = new QGridLayout ( dial );
			QLabel *pz = new QLabel ( "Z" );
			QDoubleSpinBox *sz = new QDoubleSpinBox();
			sz->setRange ( 0.0,10000.0 );
			sz->setValue ( zz );

			QPushButton *ok = new QPushButton ( "OK" );
			lay->addWidget ( pz,0,0 );
			lay->addWidget ( sz,0,1 );
			lay->addWidget ( ok,1,1 );

			connect ( ok,SIGNAL ( released() ),dial,SLOT ( close() ) );
			dial->exec();

			sel->setZValue ( sz->value() );
			delete sz;
			delete pz;
			delete lay;
			delete dial;
		}


	}
}

void ULPageItem::hideCursor()
{
	removeItem ( cursorItemH );
	removeItem ( cursorItemV );
	removeItem ( backgroundItem );
	setBackgroundBrush ( Qt::white );
}

void ULPageItem::showCursor()
{
	addItem ( cursorItemH );
	addItem ( cursorItemV );
	addItem ( backgroundItem );
	setBackgroundBrush ( Qt::lightGray );
}

void ULPageItem::dumpItems()
{
	for ( uint i = 0; i < items().count(); ++i )
		qDebug() << items() [i]->data ( 1 ).toString() << " : " <<  items() [i]->data ( 2 ).toString();
}

QMap< uint, QPointF> ULPageItem::groups()
{
	QMap< uint, QPointF > ret;
	for ( uint i = 0; i < items().count(); ++i )
	{
		if ( items() [i]->data ( 1 ).toString() == "group" )
		{
			ret.insert ( items() [i]->data ( 2 ).toUInt(), items() [i]->scenePos() );
		}
	}
	return ret;
}

QGraphicsItem * ULPageItem::groupById ( uint id )
{
	if ( PageBuffer::getInstance()->maxId() < id )
		return 0;

	for ( uint i = 0; i < items().count(); ++i )
	{
		if ( items() [i]->data ( 1 ).toString() == "group" )
		{
			//qDebug() << "grId :"<<items() [i]->data ( 2 ).toUInt();
			if ( items() [i]->data ( 2 ).toUInt() == id )
			{
				return items() [i];
			}
		}
	}
	return 0;
}





void ULPageItem::clear()
{
	QGraphicsItem * it;
	QList<QGraphicsItem*> lit = items();
	for ( int i = 0; i < lit.count(); ++i )
	{
		
		it = lit[i];
		if ( it->data ( 1 ).toString() != "noprint" && it->data(1).toString() != "group")
		{
			if ( it->scene() == this )
			{
				removeItem ( it );
				delete it;
			}
			else
			{
				qDebug() << "Something weird happens, " << it << " is not mine";
			}
		}
	}
	lit = items();
	for ( int i = 0; i < lit.count(); ++i )
	{
		
		it = lit[i];
		if ( it->data ( 1 ).toString() != "noprint")
		{
			if ( it->scene() == this )
			{
				removeItem ( it );
				delete it;
			}
			else
			{
				qDebug() << "Something weird happens, " << it << " is not mine";
			}
		}
	}

}
