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
#include "ulpageview.h"
#include "ullineeditor.h"
#include "ulpageitem.h"

#include <QPainter>

#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsRectItem>
#include <QColor>
#include <QDesktopWidget>
#include <QDebug>

ULPageView::ULPageView(ULPageItem* pit)
	:QGraphicsView(pit)
{
	main = pit;
	setRenderHints(QPainter::Antialiasing);
// 	set a roughly good view of actual size on screen
	int dpx = physicalDpiX();
	int dpy = physicalDpiY();
	double xratio = (double)dpx / 72.0;
	double yratio = (double)dpy / 72.0;
	qDebug() << dpx << dpy << xratio << yratio;
	scale(xratio, yratio);
	

}


ULPageView::~ULPageView()
{
}

// void ULPageView::drawForeground(QPainter * painter, const QRectF & rect)
// {
// 	preview->render(painter);
// 	painter->translate(main->cur_pos);
// 	painter->translate(0.0,preview->baseLine());
// 	
// }




