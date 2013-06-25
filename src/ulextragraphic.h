//
// C++ Interface: ulextragraphic
//
// Description: 
//
//
// Author: Pierre Marchand <pierre@moulindetouvois.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ULEXTRAGRAPHIC_H
#define ULEXTRAGRAPHIC_H

#include <QGraphicsItem>

class QPixmap;
class QGraphicsPixmapItem;
class QGraphicsSvgItem;
class QImage;

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
	
	A wrapper for both images and vector graphics objects.
*/
class ULExtraGraphic : public QGraphicsItem
{
public:
    ULExtraGraphic(QGraphicsItem* parent, QGraphicsScene* scene, QString filepath);
ULExtraGraphic();
    ~ULExtraGraphic();
	private:
		QString file;
// 		QPixmap pixmap; 
		QGraphicsPixmapItem *pixItem;
		QGraphicsSvgItem *svgItem;
		bool raster;
		
		bool allocated;
		uint instanceN;
	public:
		bool isAlive();
		QGraphicsItem* clone();
		QGraphicsItem* theItem();
		
		virtual QRectF boundingRect() const;
		virtual void paint(QPainter* p, const QStyleOptionGraphicsItem* sog, QWidget* w);
		
	public slots:
		QByteArray XObject();
		QString toElement();
		void changeProperties();
		
	public: // but private use ;-)
		void setRaster(bool r){raster = r;};
		void setAllocated(bool a){allocated = a;};
		QGraphicsPixmapItem * thePix(){return pixItem;};
		QGraphicsSvgItem * theSvg(){return svgItem;};
	
};

#endif
