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
#ifndef ULPAGEITEM_H
#define ULPAGEITEM_H

#include <QGraphicsScene>

class QGraphicsItem;
class QGraphicsLineItem;

class ULUnit;


/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/
class ULPageItem : public QGraphicsScene
{
		Q_OBJECT
	public:
		ULPageItem(double w, double h);

		~ULPageItem();
		void clear();


		QPointF cur_pos; //Read only outside
		void hideCursor();
		void showCursor();
		QMap<uint, QPointF> groups();
		QGraphicsItem* groupById(uint id);
		void setPageSize(double w, double h);

	protected:
		virtual void keyPressEvent ( QKeyEvent * event );
// 		virtual void focusInEvent ( QFocusEvent * event );
// 		virtual void focusOutEvent ( QFocusEvent * event );
		virtual void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent );
		virtual void mousePressEvent ( QGraphicsSceneMouseEvent * e );

	public slots:
		void setCur ( double x, double y );
		void setCurX(double x);
		void setCurY(double y);
		void dumpItems();

	private:

		QGraphicsLineItem *cursorItemV;
		QGraphicsLineItem *cursorItemH;
		QGraphicsRectItem *backgroundItem;
//		QGraphicsRectItem *hlRect;
		QGraphicsItem *_focused;
		
		double viewRatio;
	signals:
		void curPosXChanged(double);
		void curPosYChanged(double);
		void wantEditLine(QGraphicsItem*);
		void infoCurrentLine(QString);

};

#endif
