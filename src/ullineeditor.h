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
#ifndef ULLINEEDITOR_H
#define ULLINEEDITOR_H

#include <QGraphicsScene>

class ULPageItem;
class ULGlyphItem;
class ULKerningFactory;

class QGraphicsLineItem;
class QGraphicsRectItem;
class QGraphicsPixmapItem;
/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/
class ULLineEditor : public QGraphicsScene
{
		Q_OBJECT

	public:


		ULLineEditor ( ULPageItem *mainscene );

		~ULLineEditor();
		void clear();
		void init ( double base, double right );
		void adjust();
		double baseLine();
	private:
		void insertAtFirstLastGlyphFromPrevLine();
		void pushFirstGlyphAtLastOfPrevLine();
		void pushLastGlyphAtFirstOfNextLine();
		void insertAtLastFirstGlyphOfNextLine();
	public slots:
		void validateLine();
// 		void previewLine();
// 		void clearPreview();
		void addGlyph ( ULGlyphItem* g , bool atEnd = true);
		void removeLast();
		ULGlyphItem * takeLast();
		ULGlyphItem * takeFirst();
		void slotChangeBaseLine ( double b );
// 		void slotChangeLeft(double l);
		void slotChangeWidth ( double w );
		void slotMoveCursor ( double c );
		void slotJustifyBlanks ( double o );
		void slotJustifyItsVeryBadInterletter ( double o );
		void slotLTR ( int f );

		void slotEditGroup ( QGraphicsItem* git );
	private:
		QList<ULGlyphItem*> glyphs;
		bool initControl;
		bool editing;
		bool reedit;
		uint reedit_id;
		bool ltr;
		QPointF reeditPos;
		QString keyLast;
		ULKerningFactory * kernF;

		bool mouseSelect;
		QPointF mouseSelectBegin;
		QGraphicsRectItem *mouseSelectRect;

		ULPageItem *m_mainScene;

		double m_baseLine;
		double m_marginRight;
		double m_curPpos;

		QGraphicsLineItem *lineLeft;
		QGraphicsLineItem *lineBase;
		QGraphicsLineItem *lineRight;
		QGraphicsLineItem *lineCursor;

		QGraphicsPixmapItem *preview;

	protected:
		virtual void keyPressEvent ( QKeyEvent * event );
		virtual void focusInEvent ( QFocusEvent * event );
		virtual void focusOutEvent ( QFocusEvent * event );
		virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent ) ;
		virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
		virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent ) ;
	signals:
		void isValidate();
		void wantEdit ( ULGlyphItem* );
		void linesChanged ( double,double,double );


};

#endif
