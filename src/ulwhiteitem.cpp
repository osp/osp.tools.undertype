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
#include "ulwhiteitem.h"
#include "ullineeditor.h"

#include <QKeyEvent>

ULWhiteItem::ULWhiteItem(double x, double w, double lineW,double lineH)
	:QGraphicsRectItem(0)
{
	leftX = x;
	rightX = leftX + w;
	endX = lineW;
	allY = lineH;
	setData(1,QVariant("white"));
}


ULWhiteItem::~ULWhiteItem()
{
}

void ULWhiteItem::setPreview(bool p)
{
	if(p)
		setBrush(Qt::lightGray);
	else
		setBrush(Qt::transparent);
}

void ULWhiteItem::keyPressEvent(QKeyEvent * event)
{
	if(event->modifiers().testFlag(Qt::ControlModifier))
	{
		switch(event->key())
		{
			case Qt::Key_Left: moveBy(-0.1,0.0);
			break;
			case Qt::Key_Right: moveBy(0.1,0.0);
			break;
			case Qt::Key_Up: moveBy(0.0,-0.1);
			break;
			case Qt::Key_Down : moveBy(0.0,0.1);
			break;
			default : break;
		}
	}
	else
	{
		switch(event->key())
		{
			case Qt::Key_Left: moveBy(-1.0,0.0);
			break;
			case Qt::Key_Right: moveBy(1.0,0.0);
			break;
			case Qt::Key_Up: moveBy(0.0,-1.0);
			break;
			case Qt::Key_Down : moveBy(0.0,1.0);
			break;
			default : break;
		}
	}
}

void ULWhiteItem::adjust()
{
	leftRect.setRect(0,0,leftX,allY);
	rightRect.setRect(rightX,0,endX,allY);
}


