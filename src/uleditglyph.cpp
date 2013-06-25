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
#include "uleditglyph.h"
#include "ulglyphitem.h"

#include <QGraphicsRectItem>
#include <QKeyEvent>

ULEditGlyph::ULEditGlyph()
{
	lastScale = 1.0;
}


ULEditGlyph::~ULEditGlyph()
{
}

void ULEditGlyph::putGlyph(QGraphicsScene *s,ULGlyphItem * g)
{
	theScene = s;
	theGlyph = g;
	fromPos = g->scenePos();
	addItem(g);
	
	double h = height();
	g->setPos(0,h);
	g->setZValue(10.0);
	
	theBox = addRect(QRectF(0,0,g->m_xadvance,2*h));
	theBox->setEnabled(false);
	theBox->setBrush(Qt::green);
	
	
}

void ULEditGlyph::slotScale(double s)
{
	double diff = s - lastScale;
	double ds = diff > 0.0 ? 1.0 + diff : 1.0/(1.0 - diff);
	lastScale = s;
	theGlyph->scale(ds,ds);
	qDebug(QString("s = %1, ls = %2, ds = %3").arg(s).arg(lastScale).arg(ds).toAscii());
}

void ULEditGlyph::slotRotate(double r)
{
	double rr = r - lastRotate;
	lastRotate = r;
	theGlyph->rotate(rr);
}

void ULEditGlyph::slotTransX(double x)
{
	double xx = x - lastTransX;
	lastTransX = x;
	theGlyph->translate(xx,0.0);
}

void ULEditGlyph::slotTransY(double y)
{
	
	double yy = y - lastTransY;
	lastTransY = y;
	theGlyph->translate(0.0,yy);
}

void ULEditGlyph::slotAdvance(double a)
{
	theGlyph->m_xadvance = a;
	theBox->setRect(theBox->rect().x(),theBox->rect().y(),theGlyph->m_xadvance,theBox->rect().height());
}

void ULEditGlyph::keyPressEvent(QKeyEvent * event)
{
	if(event->key() == Qt::Key_Return)
	{
		theScene->addItem(theGlyph);
		theGlyph->setPos(fromPos);
		emit endEdit();
	}
}


