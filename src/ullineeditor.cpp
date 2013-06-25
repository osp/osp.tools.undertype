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
#include "ullineeditor.h"
#include "ulpageitem.h"
#include "ulglyphitem.h"
#include "pagebuffer.h"
#include "ulglyphcollection.h"
#include "ulkerningfactory.h"

#include <QGraphicsView>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QGraphicsLineItem>
#include <QGraphicsItemGroup>
#include <QGraphicsItem>
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QDebug>

ULLineEditor::ULLineEditor ( ULPageItem *mainscene )
{
	m_mainScene = mainscene;
	initControl = false;
	editing = false;
	reedit =false;
	ltr = true;

	mouseSelect = false;
	mouseSelectRect = new QGraphicsRectItem ( 0,0,0,0 );
	mouseSelectRect->setPen ( QPen ( Qt::blue ) );
	mouseSelectRect->setBrush ( QBrush ( QColor ( 0,0,255,50 ) ) );
	preview = 0;

	kernF = new ULKerningFactory ( 2 );

}


ULLineEditor::~ULLineEditor()
{}




void ULLineEditor::clear()
{
	if ( initControl )
	{
		removeItem ( lineLeft );
		removeItem ( lineRight );
		removeItem ( lineBase );
		removeItem ( lineCursor );
		delete lineLeft;
		delete lineRight;
		delete lineBase;
		delete lineCursor;
// 		m_curPpos = 0.0;
	}
}

void ULLineEditor::init ( double base, double right )
{
// 	if(initControl)
// 		clear();
	m_baseLine = base;
// 	m_marginLeft = left;
	m_marginRight = right;
	if ( ltr )
	{
		m_curPpos = 0.0;
	}
	else
	{
		m_curPpos = right;
	}
	lineBase = addLine ( QLineF ( 0.0,m_baseLine,m_marginRight,m_baseLine ) );
	lineBase->setEnabled ( false );
	lineLeft = addLine ( QLineF ( 0.0,0.0,0.0,m_baseLine ) );
	lineLeft->setEnabled ( false );
	lineRight = addLine ( QLineF ( m_marginRight,0.0,m_marginRight,m_baseLine ) );
	lineRight->setEnabled ( false );
	lineCursor = addLine ( QLineF ( m_curPpos,0.0,m_curPpos,m_baseLine ) );
	lineCursor->setEnabled ( false );
	lineCursor->setPen ( QPen ( Qt::blue ) );
	initControl = true;
	emit linesChanged ( m_baseLine,m_marginRight,m_curPpos );

}

/*!
    \fn ULLineEditor::validateLine()
 */
void ULLineEditor::validateLine()
{
// 	clearPreview();
	editing = false;
//	QPainterPath  sel;
//	sel.addRect ( sceneRect() );
//	setSelectionArea ( sel );
	QList<QGraphicsItem*> l;
	foreach(QGraphicsItem* gi, glyphs)
	{
		l << gi;
	}

	QGraphicsItemGroup * g = createItemGroup (l);
//	g->setSelected(false);
//	foreach(QGraphicsItem* i, g->childItems())
//	{
//		i->setSelected(false);
//	}

	m_mainScene->addItem ( g );
	g->setFlags ( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
	g->setData ( 1,"group" );
	qDebug() << "set data 3 to "<<m_baseLine;
	g->setData ( 3,QVariant ( m_baseLine ) );
	g->setData ( 4,m_marginRight );
	g->setData ( 5,m_curPpos );
	g->setData( 6, (ltr ? 1 : 0) );
	g->setZValue ( 10.0 );
	g->setHandlesChildEvents ( true );
	if ( reedit )
	{
		g->setPos ( reeditPos );
// 		g->moveBy(0.0,-(g->boundingRect().height()));
		g->setData ( 2, QVariant ( reedit_id ) );
		PageBuffer::getInstance()->insert ( reedit_id,glyphs );
		reedit = false;
	}
	else
	{
		g->setData ( 2, QVariant ( PageBuffer::getInstance()->put ( glyphs ) ) );
		g->setPos ( m_mainScene->cur_pos );
		g->moveBy ( 0.0,-m_baseLine );
		m_mainScene->setCur ( m_mainScene->cur_pos.x(), m_mainScene->cur_pos.y() + m_baseLine );
	}
	m_mainScene->views() [0]->centerOn ( g );
	m_curPpos = 0.0;
	adjust();
	glyphs.clear();
	keyLast.clear();
	emit isValidate();

}

void ULLineEditor::addGlyph ( ULGlyphItem * g, bool atEnd )
{
	if(!g)
		return;
	editing = true;
	addItem ( g );

	if ( atEnd )
	{
		if ( !glyphs.isEmpty() )
		{
// 			qDebug() << glyphs.last()->key << " " <<  g->key << " : " << kernF->kern ( glyphs.last() , g );
			if(ltr)
				m_curPpos += kernF->kern ( glyphs.last() , g );
			else
				m_curPpos -= kernF->kern ( glyphs.last() , g );
		}
		glyphs.append ( g );
		if ( ltr )
		{
			g->setPos ( m_curPpos,m_baseLine );
			m_curPpos += g->m_xadvance;
		}
		else
		{
			m_curPpos -= g->m_xadvance;
			g->setPos ( m_curPpos,m_baseLine );
		}
	}
	else // insert glyph at the beginning
	{
		double toadv;
		if ( !glyphs.isEmpty() )
		{
			toadv =  g->m_xadvance + kernF->kern ( glyphs.first() , g );
		}
		else
			toadv =  g->m_xadvance ;
		
		glyphs.prepend(g);
		
		if(ltr)
		{
			g->setPos ( 0.0,m_baseLine );
			for(uint i= 1;i<glyphs.count();++i)
				glyphs[i]->moveBy(toadv,0.0);
			m_curPpos += toadv;
		}	
		else
		{
			g->setPos(m_marginRight - toadv, m_baseLine);
			for(uint i= 1;i<glyphs.count();++i)
				glyphs[i]->moveBy(-toadv,0.0);
			m_curPpos -= toadv;
		}
	}
	adjust();

	clearFocus();

}

void ULLineEditor::removeLast()
{
	if ( glyphs.isEmpty() )
		return;
	ULGlyphItem * l = glyphs.takeLast();
	if(ltr)
		slotMoveCursor ( m_curPpos - l->m_xadvance );
	else
		slotMoveCursor(m_curPpos + l->m_xadvance );
	removeItem ( l );
}

ULGlyphItem * ULLineEditor::takeLast()
{
	if ( glyphs.isEmpty() )
		return 0;
	ULGlyphItem * l = glyphs.takeLast();
	if(ltr)
		slotMoveCursor ( m_curPpos - l->m_xadvance );
	else
		slotMoveCursor(m_curPpos + l->m_xadvance );
	removeItem ( l );
	return l;
}

ULGlyphItem * ULLineEditor::takeFirst()
{
	if ( glyphs.isEmpty() )
		return 0;
	ULGlyphItem * l = glyphs.takeFirst();
	if(ltr)
	{
		for(uint i = 0; i < glyphs.count(); ++i)
			glyphs[i]->moveBy(-l->m_xadvance,0.0);
		slotMoveCursor ( m_curPpos - l->m_xadvance );
	}
	else
	{
		for(uint i = 0; i < glyphs.count(); ++i)
			glyphs[i]->moveBy(l->m_xadvance,0.0);
		slotMoveCursor(m_curPpos + l->m_xadvance );
	}
	removeItem ( l );
	return l;
}




void ULLineEditor::keyPressEvent ( QKeyEvent * event )
{
	if ( event->key() == Qt::Key_Return )
	{
		validateLine();
		return;
	}
	if ( event->key() == Qt::Key_G && event->modifiers().testFlag ( Qt::ControlModifier ))
	{
		pushLastGlyphAtFirstOfNextLine();
		return;
	}
	if ( event->key() == Qt::Key_P&& event->modifiers().testFlag ( Qt::ControlModifier ) )
	{
		insertAtLastFirstGlyphOfNextLine();
		return;
	}
	if ( event->key() == Qt::Key_G && event->modifiers().testFlag ( Qt::ShiftModifier ))
	{
		insertAtFirstLastGlyphFromPrevLine();
		pushLastGlyphAtFirstOfNextLine();
		return;
	}
	if ( event->key() == Qt::Key_P&& event->modifiers().testFlag ( Qt::ShiftModifier ) )
	{
		pushFirstGlyphAtLastOfPrevLine();
		insertAtLastFirstGlyphOfNextLine();
		return;
	}
	if ( event->key() == Qt::Key_G )
	{
		pushFirstGlyphAtLastOfPrevLine();
		return;
	}
	if ( event->key() == Qt::Key_P )
	{
		insertAtFirstLastGlyphFromPrevLine();
		return;
	}
	

	QList<QGraphicsItem*> it = selectedItems();
	if ( it.isEmpty() && event->modifiers().testFlag ( Qt::ControlModifier )  && event->modifiers().testFlag ( Qt::ShiftModifier ) )
	{
		switch ( event->key() )
		{
			case Qt::Key_Right : slotJustifyItsVeryBadInterletter ( 0.005 );
				break;
			case Qt::Key_Left : slotJustifyItsVeryBadInterletter ( -0.005 );
				break;
			default:break;
		}

	}
	else if ( it.isEmpty() && event->modifiers().testFlag ( Qt::ControlModifier ) )
	{
		switch ( event->key() )
		{
			case Qt::Key_Right : slotJustifyBlanks ( 0.01 );
				break;
			case Qt::Key_Left : slotJustifyBlanks ( -0.01 );
				break;
			default:break;
		}

	}
	else if ( it.isEmpty() )
	{
		QGraphicsView * v = views().first();
		switch ( event->key() )
		{
			case Qt::Key_Up: v->scale ( 1.2,1.2 );
				break;
			case Qt::Key_Down : v->scale ( 1.0/1.2,1.0/1.2 );
				break;
			case Qt::Key_Right : slotJustifyBlanks ( 0.1 );
				break;
			case Qt::Key_Left : slotJustifyBlanks ( -0.1 );
				break;
			default:break;
		}

	}
	else
	{
		if ( event->key() == Qt::Key_Delete )
		{
			for ( uint i = 0; i < it.count();++i )
			{
				removeItem ( it[i] );
				glyphs.removeAll ( static_cast<ULGlyphItem*> ( it[i] ) );
			}
		}
		else if ( event->key() == Qt::Key_E )
		{
			wantEdit ( static_cast<ULGlyphItem*> ( it.first() ) );
		}

		else
		{
			for ( uint i = 0; i < it.count();++i )
				static_cast<ULGlyphItem*> ( it[i] )->keyForeign ( event );

		}
// 		previewLine();
	}
}




void ULLineEditor::focusInEvent ( QFocusEvent * event )
{
	setBackgroundBrush ( Qt::white );
}

void ULLineEditor::focusOutEvent ( QFocusEvent * event )
{
	setBackgroundBrush ( Qt::lightGray );
}

void ULLineEditor::slotChangeBaseLine ( double b )
{
	double delta = b - m_baseLine ;
	m_baseLine = b;
	QList<QGraphicsItem*> g = items();
	for ( uint i=0; i < g.count(); ++i )
		g[i]->moveBy ( 0.0,delta );
	adjust();
}

// void ULLineEditor::slotChangeLeft(double l)
// {
// 	m_marginLeft = l;
// }

void ULLineEditor::slotChangeWidth ( double w )
{
	m_marginRight = w;
	adjust();
}

void ULLineEditor::slotMoveCursor ( double c )
{
	m_curPpos = c;
	adjust();
}

void ULLineEditor::adjust()
{
	clear();
	lineBase = addLine ( QLineF ( 0.0,m_baseLine,m_marginRight,m_baseLine ) );
	lineBase->setEnabled ( false );
	lineLeft = addLine ( QLineF ( 0.0,0.0,0.0,m_baseLine ) );
	lineLeft->setEnabled ( false );
	lineRight = addLine ( QLineF ( m_marginRight,0.0,m_marginRight,m_baseLine ) );
	lineRight->setEnabled ( false );
	lineCursor = addLine ( QLineF ( m_curPpos,0.0,m_curPpos,m_baseLine ) );
	lineCursor->setEnabled ( false );
	lineCursor->setPen ( QPen ( Qt::blue ) );
	emit linesChanged ( m_baseLine,m_marginRight,m_curPpos );
}

void ULLineEditor::slotEditGroup ( QGraphicsItem * git )
{
	if ( editing )
		validateLine();
	editing = true;
	reedit = true;
// 	qDebug() << git->data ( 1 ).toString() << ":" << git->data ( 2 ).toUInt() ;
	QGraphicsItemGroup * gr = static_cast<QGraphicsItemGroup*> ( git );
	QList<QGraphicsItem*> lgr =  gr->children();
	double realwidth = 0;
	reeditPos = gr->scenePos();
	reedit_id = gr->data ( 2 ).toUInt();
	
	slotChangeBaseLine ( gr->data ( 3 ).toDouble() );
	slotChangeWidth ( gr->data ( 4 ).toDouble() );
	slotMoveCursor ( gr->data ( 5 ).toDouble() );
	slotLTR(gr->data( 6 ).toInt() == 1 ? Qt::Checked : Qt::Unchecked);
	
	gr->setPos ( 0.0, 0.0 );
	addItem ( gr );
	destroyItemGroup ( gr );
	
	for ( uint i = 0; i < lgr.count(); ++i )
	{
// 		lgr[i]->clearFocus();
		lgr[i]->setSelected ( false );
	}


// 	adjust();
	glyphs.clear();
	glyphs = PageBuffer::getInstance()->take ( reedit_id, false );
}

void ULLineEditor::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	if ( itemAt ( mouseEvent->scenePos() ) )
	{
		QGraphicsItem * it =itemAt ( mouseEvent->scenePos() );
		it->isSelected() ? it->setSelected ( false ) : it->setSelected ( true );
	}
	else
	{
		mouseSelectBegin = mouseEvent->scenePos();
		mouseSelect = true;
		mouseSelectRect->setPos ( mouseEvent->pos() );
		addItem ( mouseSelectRect );
	}
}

void ULLineEditor::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	if ( mouseSelect )
	{
		QPainterPath p ( mouseSelectBegin );
		p.addRect ( mouseSelectRect->boundingRect() );
		setSelectionArea ( p );
		mouseSelect = false;
		removeItem ( mouseSelectRect );
		mouseSelectRect->setRect ( 0,0,0,0 );
	}
	else if ( !itemAt ( mouseEvent->scenePos() ) )
	{
		clearFocus();
		clearSelection();
	}
}

void ULLineEditor::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	if ( mouseSelect )
	{
		mouseSelectRect->setRect ( QRectF ( mouseSelectBegin,
		                                    QSizeF ( mouseEvent->scenePos().x() - mouseSelectBegin.x(),mouseEvent->scenePos().y() - mouseSelectBegin.y() ) ) );
	}
}

double ULLineEditor::baseLine()
{
	return m_baseLine;
}

void ULLineEditor::slotJustifyBlanks ( double o )
{
	if ( glyphs.isEmpty() )
		return;
// 	qDebug("justif");
	for ( uint i=0;i<glyphs.count();++i )
	{
// 		qDebug(glyphs[i]->key);
		if ( glyphs[i]->data ( 2 ).toString() == "space" )
		{
			for ( uint ii = i+1;ii<glyphs.count();++ii )
			{
				glyphs[ii]->moveBy ( o,0 );

			}
		}
	}
}

void ULLineEditor::slotJustifyItsVeryBadInterletter ( double o )
{
	for ( uint i = 0; i <glyphs.count(); ++i )
	{
		for ( uint ii = i+1 ;ii<glyphs.count() ;  ++ii )
		{
			glyphs[ii]->moveBy ( o,0 );

		}

	}
}

void ULLineEditor::insertAtFirstLastGlyphFromPrevLine()
{
	PageBuffer *PB = PageBuffer::getInstance();

	if ( PB->maxId() == 0 ) //The line we are editing is the first one
		return;

	uint prevlineId;
	int curLineId;
	if ( reedit )
	{
		curLineId = reedit_id;
		prevlineId = reedit_id - 1;
	}
	else
	{
		prevlineId = PB->maxId();
		curLineId = prevlineId + 1;
	}

	slotEditGroup(m_mainScene->groupById(prevlineId));
	ULGlyphItem *it= takeLast();
	slotEditGroup(m_mainScene->groupById(curLineId));
	addGlyph(it, false);


}

void ULLineEditor::pushFirstGlyphAtLastOfPrevLine()
{
	PageBuffer *PB = PageBuffer::getInstance();

	if ( PB->maxId() == 0 )
		return;

	uint prevlineId;
	uint curId;
	if ( reedit )
	{
		prevlineId = reedit_id - 1;
		curId = reedit_id;
	}
	else
	{
		prevlineId = PB->maxId();
		curId = prevlineId + 1;
	}

	ULGlyphItem *it=takeFirst();
	qDebug() << it->key;
	slotEditGroup(m_mainScene->groupById(prevlineId));
	addGlyph(it);
	slotEditGroup(m_mainScene->groupById(curId));

}


void ULLineEditor::pushLastGlyphAtFirstOfNextLine()
{
	PageBuffer *PB = PageBuffer::getInstance();

	if ( !reedit ) //Last line is current
		return;
	if(reedit_id == PB->maxId())//We are the last
		return;

	uint nextlineId = reedit_id + 1;
	uint curId = reedit_id;

	ULGlyphItem *LaGlFrLi = takeLast();
	slotEditGroup(m_mainScene->groupById(nextlineId));
	addGlyph(LaGlFrLi, false);
	slotEditGroup(m_mainScene->groupById(curId));
	
	
}

void ULLineEditor::insertAtLastFirstGlyphOfNextLine()
{
	PageBuffer *PB = PageBuffer::getInstance();

	if ( !reedit ) //Last line is current
		return;
	if(reedit_id == PB->maxId())//We are the last
		return;
	
	uint nextlineId = reedit_id + 1;
	uint curId = reedit_id;
	slotEditGroup(m_mainScene->groupById(nextlineId));
	ULGlyphItem *it = takeFirst();
	slotEditGroup(m_mainScene->groupById(curId));
	addGlyph(it);
	
	
}


void ULLineEditor::slotLTR ( int f )
{
	if ( f == Qt::Checked )
		ltr = true;
	else
		ltr = false;
}




















