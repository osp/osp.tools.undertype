//
// C++ Interface: ulextragraphicscene
//
// Description: 
//
//
// Author: Pierre Marchand <pierre@moulindetouvois.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ULEXTRAGRAPHICSCENE_H
#define ULEXTRAGRAPHICSCENE_H

#include <QGraphicsScene>
#include <QMap>

class ULPageItem;
class ULExtraGraphic;
class QPointF;

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/
class ULExtraGraphicScene : public QGraphicsScene
{
public:
	ULExtraGraphicScene(ULPageItem* p);

    ~ULExtraGraphicScene();
	private:
		ULPageItem *page;
		QMap<QString, ULExtraGraphic*> graphics;
		double insertPos;
	public:
		QGraphicsItem * push();
		void add();
		void load(QString file, double x, double y, double z);
	protected:
		virtual void keyPressEvent ( QKeyEvent * keyEvent ) ;

};

#endif
