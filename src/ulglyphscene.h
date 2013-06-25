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
#ifndef ULGLYPHSCENE_H
#define ULGLYPHSCENE_H

#include "ulpageitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QMap>

class ULGlyphItem;
class ULLineEditor;
class ULGlyphCollection;
class QGraphicsTextItem;
class QAction;

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/
class ULGlyphScene : public QGraphicsScene
{
public:
    ULGlyphScene();

    ~ULGlyphScene();

    void setMainScene(ULLineEditor *s);
    void mapKeys();
    void setCollection(ULGlyphCollection* col);
    ULGlyphCollection* collection();
    ULGlyphItem* keyItem(QString k);
	private:
		ULLineEditor *m_mainscene;
		ULGlyphCollection *m_collection;
		QString curKey;
		QMap<QString,ULGlyphItem*> fastKey;
		QGraphicsTextItem *keyHelper;
        QAction *pasteAct;
		bool keyHelpIsActive;
		QStringList keyHelpList;
		double spaceDiv;
		void keyPressEvent_byName ( QKeyEvent * event );
		void keyPressEvent_byChar ( QKeyEvent * event );
        void paste();
        void insertChar(ushort unicode);
	protected:
		void focusInEvent ( QFocusEvent * event );
		void focusOutEvent ( QFocusEvent * event ); 
		void keyPressEvent ( QKeyEvent * event );
		void mousePressEvent( QGraphicsSceneMouseEvent * mouseEvent );
// 		virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
};

#endif
