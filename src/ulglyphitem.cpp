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
#include "ulglyphitem.h"
#include "ulglyphcollection.h"
#include "pagebuffer.h"

#include <QString>
#include <QBrush>
#include <QKeyEvent>
#include <QInputDialog>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QDebug>
#include <QPainter>
#include <QPaintDevice>
#include <QSizeF>
#include <QDebug>

#include FT_OUTLINE_H
#include FT_GLYPH_H 

/** functions set for decomposition
*/
//ULGlyphItem* ULGlyphItem::mySelf=0;

int ULGlyphItem::ul_moveTo ( const FT_Vector*  to, void*   user )
{
	ULGlyphItem * gi(reinterpret_cast<ULGlyphItem*>(user));
	QPainterPath path(gi->path());
	path.moveTo ( to->x, to->y );
	gi->setPath(path);
	gi->addPDFSubPath ( QString().number ( to->x ) + " " +QString().number ( to->y ) + " m\n" );
	return 0;
}
int ULGlyphItem::ul_lineTo ( const FT_Vector*  to, void*   user )
{
	ULGlyphItem * gi(reinterpret_cast<ULGlyphItem*>(user));
	QPainterPath path(gi->path());
	path.lineTo ( to->x, to->y );
	gi->setPath(path);
	gi->addPDFSubPath ( QString().number ( to->x ) + " " + QString().number ( to->y ) + " l\n" );
	return  0;
}
int ULGlyphItem::ul_conicTo ( const FT_Vector* control, const FT_Vector*  to, void*   user )
{
	ULGlyphItem * gi(reinterpret_cast<ULGlyphItem*>(user));
	QPainterPath path(gi->path());
	path.quadTo ( control->x,control->y,to->x,to->y );
	gi->setPath(path);
	gi->addPDFSubPath ( QString().number ( control->x ) + " "
	                        + QString().number ( control->y ) + " "
	                        + QString().number ( control->x ) + " "
	                        + QString().number ( control->y ) + " "
	                        + QString().number ( to->x ) + " "
	                        + QString().number ( to->y ) + " c\n" );
	return 0;
}
int ULGlyphItem::ul_cubicTo ( const FT_Vector* control1, const FT_Vector* control2, const FT_Vector*  to, void*   user )
{
	ULGlyphItem * gi(reinterpret_cast<ULGlyphItem*>(user));
	QPainterPath path(gi->path());
	path.cubicTo ( control1->x,control1->y,control2->x,control2->y,to->x,to->y );
	gi->setPath(path);
	gi->addPDFSubPath ( QString().number ( control1->x ) + " "
	                        + QString().number ( control1->y ) + " "
	                        + QString().number ( control2->x ) + " "
	                        + QString().number ( control2->y ) + " "
	                        + QString().number ( to->x ) + " "
	                        + QString().number ( to->y ) + " c\n" );
	return 0;
}
/****************************************************/

ULGlyphItem::ULGlyphItem()
		:QGraphicsPathItem ( 0 )
{
	keyIsSet = false;
	setAcceptsHoverEvents ( true );
	m_showText = true;
	setData ( 1,"glyph" );
	setBrush ( QBrush ( Qt::SolidPattern ) );
	setPen(Qt::NoPen);
}


ULGlyphItem::~ULGlyphItem()
{
	if ( keyIsSet )
		delete textKey;
}

int ULGlyphItem::fromOutline ( FT_Face face, uint index, double sz )
{
//	mySelf = this;
	if ( FT_Load_Glyph ( face, index, FT_LOAD_NO_SCALE ) )
	{
		qDebug ( ) << "Unable to load glyph at index "  << index ;
		return 0;
	}
	size = sz;
	FT_Outline_Funcs funcs;
	funcs.move_to = ul_moveTo;
	funcs.line_to = ULGlyphItem::ul_lineTo;
	funcs.conic_to = ULGlyphItem::ul_conicTo;
	funcs.cubic_to = ULGlyphItem::ul_cubicTo;
	funcs.shift = 0;
	funcs.delta = 0;
	FT_Outline *outline;
	m_uniem = face->units_per_EM;
	m_xadvance = face->glyph->metrics.horiAdvance / m_uniem * size;
	outline = &face->glyph->outline;
//	QPainterPath glyph ( QPointF ( 0.0,0.0 ) );
	FT_Outline_Decompose ( outline, &funcs, this );
	PDFPath += "f\n";
	prepareGeometryChange();
	scale ( size/m_uniem, -size/m_uniem );

	if ( FT_HAS_GLYPH_NAMES ( face ) )
	{
		char buf[256];
		FT_Get_Glyph_Name ( face, index, buf, 255 );
		if ( buf[0] != 0 )
		{
			key = buf;
			keyIsSet = true;
			textKey = new QGraphicsTextItem ( key );
			textKey->setDefaultTextColor ( QColor ( 0,0,0 ) );
		}
	}
	else
	{
		key = "noname" + index;
		keyIsSet = true;
		textKey = new QGraphicsTextItem ( key );
		textKey->setDefaultTextColor ( QColor ( 0,0,0 ) );
	}

	
	uint len = PDFPath.length();
	QRectF bbox = path().controlPointRect();

	QString mat ( QString("/Matrix [%1 0 0 %1 0 0]\n" ).arg ( sz/uniem() )) ;
	QString bbs (QString( "/BBox [%1 %2 %3 %4]\n" ).arg ( bbox.left()).arg ( bbox.bottom() ).arg ( bbox.right() ).arg ( bbox.top() ));
	QString lens ( QString("/Length %1\n" ).arg ( len ));
	QString ptmp ( PDFPath );

	PDFPath = "<< /Type /XObject\n/Subtype /Form\n/FormType 1\n";
	PDFPath += bbs + mat + lens;
	PDFPath += ">>\nstream\n";
	PDFPath += ptmp;
	PDFPath += "endstream\n";
	
	return 1;



}

double ULGlyphItem::uniem()
{
	prepareGeometryChange();
	return m_uniem;
}

void ULGlyphItem::setUniem ( double u )
{
	m_uniem = u;
}

void ULGlyphItem::makeActive()
{
	setFlags ( ItemIsMovable | ItemIsSelectable | ItemIsFocusable );
}

void ULGlyphItem::keyPressEvent ( QKeyEvent * event )
{
	if ( event->modifiers().testFlag ( Qt::ControlModifier ) )
	{
		switch ( event->key() )
		{
			case Qt::Key_Left: moveBy ( -0.1,0.0 );
				break;
			case Qt::Key_Right: moveBy ( 0.1,0.0 );
				break;
			case Qt::Key_Up: moveBy ( 0.0,-0.1 );
				break;
			case Qt::Key_Down : moveBy ( 0.0,0.1 );
				break;
			default : break;
		}
	}
	else
	{
		switch ( event->key() )
		{
			case Qt::Key_Left: moveBy ( -1.0,0.0 );
				break;
			case Qt::Key_Right: moveBy ( 1.0,0.0 );
				break;
			case Qt::Key_Up: moveBy ( 0.0,-1.0 );
				break;
			case Qt::Key_Down : moveBy ( 0.0,1.0 );
				break;
			default : break;
		}
	}
}

void ULGlyphItem::keyForeign ( QKeyEvent * event )
{
	keyPressEvent ( event );
}

void ULGlyphItem::attachKey_interactive()
{
	QString rm = QInputDialog::getText ( 0,"hum","key",QLineEdit::Normal, key );
	if ( !rm.isEmpty() )
	{
		key = rm;
		if ( textKey )
			delete textKey;
		textKey = new QGraphicsTextItem ( key );
		keyIsSet = true;
	}
}

void ULGlyphItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
	if ( m_showText )
	{
		textKey->setPos ( event->scenePos().x(), event->scenePos().y() - 30.0 );
		scene()->addItem ( textKey );
	}

}

void ULGlyphItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
	if ( m_showText )
		scene()->removeItem ( textKey );

}

void ULGlyphItem::showText ( bool b )
{
	m_showText=b;
}

// QRectF ULGlyphItem::boundingRect() const
// {
// 	return QRectF(pos().x(),pos().y(), bbox.width(),bbox.height());
// }
//

void ULGlyphItem::attachKey ( QString k )
{
	key = k;
	if ( textKey )
		delete textKey;
	textKey = new QGraphicsTextItem ( key );
	keyIsSet = true;
}

QString ULGlyphItem::toElement()
{
	QString ret;
	ret = "<gitem>\n";
	ret += "<xpos>" +QString::number ( x() ) + "</xpos>\n";
	ret += "<ypos>" +QString::number ( y() ) + "</ypos>\n";
	ret += "<key>" + key + "</key>\n";
	ret += "<from>" + collection->name() + "</from>\n";
// 	ret += "<xadvance>" +QString::number (  m_xadvance )+ "</xadvance>\n";
// 	QByteArray buf;
// 	QDataStream stream(&buf,QIODevice::WriteOnly);
// 	stream << path();
// 	ret += "<path>" + QString(buf) +  "</path>\n";
	ret += "</gitem>\n";
	return ret;
}

void ULGlyphItem::addPDFSubPath ( QString s )
{
	PDFPath += s;
// 	qDebug() << s;
}

QString ULGlyphItem::XObject()
{
	return PDFPath;
}

void ULGlyphItem::setPDF(QString s)
{
// 	qDebug() << s;
	PDFPath = s;
}




