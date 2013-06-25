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
#ifndef ULWHITEITEM_H
#define ULWHITEITEM_H

#include <QGraphicsRectItem>
#include <QPainterPath>

class ULLineEditor;
class QKeyEvent;
/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/
class ULWhiteItem : public QGraphicsRectItem
{
	public:
		ULWhiteItem (  double x, double w, double lineW ,double lineH);

		~ULWhiteItem();

		void setPreview ( bool p );
		
	private:
		void adjust();
		
		ULLineEditor *theScene;
		double rightX;
		double leftX;
		double endX;
		double allY;
		bool preview;
		QRectF leftRect;
		QRectF rightRect;
	protected:
		virtual void keyPressEvent ( QKeyEvent * event );


};

#endif
