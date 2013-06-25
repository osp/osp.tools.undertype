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
#ifndef ULGLYPHITEM_H
#define ULGLYPHITEM_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <QGraphicsPathItem>
#include <QPainterPath>

class QKeyEvent;
class ULGlyphCollection;

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/
class ULGlyphItem : public QGraphicsPathItem
{
	public:
		ULGlyphItem ( /*QGraphicsScene *scene*/ );

		~ULGlyphItem();
		
		ULGlyphCollection *collection;
		
		int fromOutline ( FT_Face face, uint index ,double size);
		uint index;
		uint charcode;
		double uniem();
		void setUniem ( double u );
		void makeActive();
		double size;
		void keyForeign(QKeyEvent * event );
		
	
		double m_xadvance;
		double m_yadvance;
		double m_uniem;
		
		bool keyIsSet;
		QString key;
		void attachKey_interactive();
		void attachKey(QString k);
		QGraphicsTextItem * textKey;
		void showText(bool);
		QString toElement();
		void addPDFSubPath(QString s); 
		void setPDF(QString s);
		QString XObject();

		
		
		
		
	protected:
		void keyPressEvent ( QKeyEvent * event );
		void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
		void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
		
		void run();

	private:
		static int ul_moveTo( const FT_Vector*  to, void*   user );
		static int ul_lineTo(const FT_Vector*  to, void*   user );
		static int ul_conicTo(const FT_Vector* control, const FT_Vector*  to, void*   user );
		static int ul_cubicTo(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector*  to, void*   user );
		
//		static ULGlyphItem *mySelf;
		
		bool m_showText;
		QString PDFPath;
};

#endif
