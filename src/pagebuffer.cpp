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
#include "pagebuffer.h"
#include "ulglyphitem.h"

PageBuffer::PageBuffer()
{
	r_id = 0;
}


PageBuffer::~PageBuffer()
{
}




PageBuffer * PageBuffer::getInstance()
{
	if (instance == NULL)
		instance = new PageBuffer;
	return instance;
}

PageBuffer *PageBuffer::instance = NULL;

uint PageBuffer::put(QList< ULGlyphItem * > gl)
{
	++r_id;
	m_map.insert(r_id, gl);
	return r_id;
}



QList< ULGlyphItem * > PageBuffer::take(uint id, bool keep)
{
	if(keep)
		return m_map.value(id);
	
	return m_map.take(id);
}


void PageBuffer::insert(uint id, QList< ULGlyphItem * > ug, bool force)
{
	if(force )
	{
		m_map.insert(id, ug);
	}
	else
	{
		if(!m_map.contains(id))
			m_map.insert(id, ug);
	}
}

void PageBuffer::clear()
{
	m_map.clear();
	graphics.clear();
	r_id = 0;
}