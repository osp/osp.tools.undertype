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
#ifndef PAGEBUFFER_H
#define PAGEBUFFER_H

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/

#include <QMap>
#include <QList>
#include <QPointF>
#include <QPixmap>

class ULGlyphItem;
class ULExtraGraphic;

class PageBuffer
{
	private:
		PageBuffer();

		~PageBuffer();

		static PageBuffer *instance;
		QMap< uint, QList<ULGlyphItem*> > m_map;
		uint r_id;
		
	public:
		static PageBuffer *getInstance();
		uint put ( QList<ULGlyphItem*> );
		QList<ULGlyphItem*> take ( uint id, bool keep = true);
		void insert(uint id, QList<ULGlyphItem*> ug, bool force = false);
		
		uint maxId(){return r_id;};
		QMap< QString, ULExtraGraphic*> graphics;
		QList<QString> fontCollection;
		
		void clear();

};

#endif
