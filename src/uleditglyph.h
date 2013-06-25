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
#ifndef ULEDITGLYPH_H
#define ULEDITGLYPH_H

#include <QGraphicsScene>

class ULGlyphItem;
class QGraphicsRectItem;

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/
class ULEditGlyph : public QGraphicsScene
{
	Q_OBJECT
public:
    ULEditGlyph();

    ~ULEditGlyph();
    void putGlyph(QGraphicsScene *s,ULGlyphItem * g);
	private:
		ULGlyphItem *theGlyph;
		QGraphicsRectItem *theBox;
		QGraphicsScene *theScene;
		QPointF fromPos;
		double lastScale;
		double lastRotate;
		double lastTransX;
		double lastTransY;
		double lastAdvance;
	public slots:
		void slotScale(double s);
		void slotRotate(double r);
		void slotTransX(double x);
		void slotTransY(double y);
		void slotAdvance(double a);
	signals:
		void endEdit();
		
	protected:
		virtual void keyPressEvent ( QKeyEvent * event );
};

#endif
