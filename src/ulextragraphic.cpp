//
// C++ Implementation: ulextragraphic
//
// Description: 
//
//
// Author: Pierre Marchand <pierre@moulindetouvois.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ulextragraphic.h"
#include "pagebuffer.h"
#include "ulpdfdevice.h"

#include <QGraphicsSvgItem>
#include <QImage>
#include <QImageWriter>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QDataStream>
#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QPainter>
#include <QTransform>
#include <QSvgRenderer>



ULExtraGraphic::ULExtraGraphic()
{
	allocated = false;
	instanceN = 0;
}

ULExtraGraphic::ULExtraGraphic(QGraphicsItem* parent, QGraphicsScene* scene, QString filepath): QGraphicsItem(parent, scene)
{
	allocated = false;
	instanceN = 0;
	file = filepath;
	// Raster or vector
	if(file.endsWith("svg"))
	{
		raster = false;
	}
	else
	{
		raster = true;
	}
	qDebug() << "raster = "<< raster;
	if(!raster)
	{
		svgItem = new QGraphicsSvgItem(file, this);
		svgItem->setFlags (ItemIsSelectable | ItemIsFocusable );
		allocated = true;
	}
	else
	{
		QPixmap pixmap(file);
		pixItem = new QGraphicsPixmapItem(pixmap.scaled(pixmap.width()/4.16666, pixmap.height()/4.16666), this); // 300 dpi
// 		pixItem->scale(0.01,0.01);
		pixItem->setFlags (ItemIsSelectable | ItemIsFocusable );
		allocated = true;
	}
	
	setData(1, "graphic");
	
}


ULExtraGraphic::~ULExtraGraphic()
{
}

/* clone child*/
QGraphicsItem * ULExtraGraphic::clone() 
{
	if(!allocated)
	{
		qDebug() << "This ExtraGraphic has no content ["<< this <<"]";
	}
	PageBuffer * pb = PageBuffer::getInstance();
	
	if(raster)
	{
		qDebug() << "clone raster";
		QGraphicsPixmapItem *p = new QGraphicsPixmapItem(pixItem->pixmap());
		p->setData(1, "raster");
		p->setData(2, QString("%1%2").arg(file).arg(instanceN,10,10,QChar('0')));
		if(!pb->graphics.contains(file))
		{
			pb->graphics.insert(file, this);
		}
		qDebug() << "raster cloned with name " << p->data(2).toString();
		++instanceN;
		return p;
	}
	else
	{
		qDebug() << "clone svg";
		QGraphicsSvgItem *s = new QGraphicsSvgItem();
		s->setSharedRenderer(svgItem->renderer());
		s->setData(1,"svg");
		s->setData(2, QString("%1%2").arg(file).arg(instanceN,10,10,QChar('0')));
		if(!pb->graphics.contains(file))
		{
			pb->graphics.insert(file, this);
		}
		qDebug() << "svg cloned with name " << s->data(2).toString() ;
		++instanceN;
		return s;
	} 
	return 0;
}

QByteArray ULExtraGraphic::XObject()
{
	if(!allocated)
		return QByteArray();
	QString ret;
	
	if(raster)
	{
		bool ok ;
		QImage img;
		ok = img.load(file);
		if(!ok)
		{
			qDebug() << "Failed to load " << file;
			return ret.toAscii();
		}
		QRgb *imgdata = (QRgb*) img.bits();
		QByteArray buffer;
		uint numpix = img.width() * img.height();
		buffer.reserve(numpix);
		QDataStream stream(&buffer,QIODevice::WriteOnly );
		for(uint i = 0 ; i < numpix; ++i)
		{
			stream << (quint8) qRed( imgdata[i]);
			stream << (quint8) qGreen( imgdata[i]);
			stream << (quint8) qBlue( imgdata[i]);
		}
		qDebug() << "uncompressed image" << buffer.size();
		QByteArray cbuffer(qCompress (buffer));
		cbuffer = cbuffer.mid(4); //skip Qt header
		qDebug() <<  "compressed image" << cbuffer.size();
		
		
		
		int w = img.width();
		int h = img.height();
		QString colorspace = img.isGrayscale() ? "/DeviceGray" : "/DeviceRGB";
		QString bpc = QString("%1").arg(8);
		int l = cbuffer.size();
		
		ret = QString("<<\n/Type /XObject\n/Subtype /Image\n/Width %1\n/Height %2\n/ColorSpace %3\n/BitsPerComponent %4\n/Length %5\n/Filter /FlateDecode\n>>\nstream\n")
				.arg(w)
				.arg(h)
				.arg(colorspace)
				.arg(bpc)
				.arg(l);
		
		cbuffer.prepend(ret.toAscii());
		cbuffer.append("\nendstream\n");
		
		return cbuffer;
		
		
	}
	else // svg
	{
		QByteArray buf;
		ULPDFDevice dev(&buf);
		QPainter painter(&dev);
		QSvgRenderer * svr = svgItem->renderer();
		svr->render(&painter);
		
		QByteArray retarray(QString("<<\n/Type /XObject\n/Subtype /Form\n/BBox [%1 %2 %3 %4]\n/Matrix [1 0 0 -1 0 0]\n/Length %5\n>>\nstream\n")
				.arg(svgItem->boundingRect().left())
				.arg(svgItem->boundingRect().bottom())
				.arg(svgItem->boundingRect().right())
				.arg(svgItem->boundingRect().top())
				.arg(buf.size())
				.toAscii()
				   );
		retarray.append(buf);
		retarray.append(QString("endstream\n").toAscii());
		return retarray;
	}
}

QString ULExtraGraphic::toElement()
{
	return "plus tard";
}

void ULExtraGraphic::changeProperties()
{
	// Do nothing now
}

QRectF ULExtraGraphic::boundingRect() const
{
	if(!allocated)
		return QRectF();
	if(raster)
		return pixItem->boundingRect();
	else
		return svgItem->boundingRect();
}

void ULExtraGraphic::paint(QPainter * p, const QStyleOptionGraphicsItem * sog, QWidget * w)
{
	if(!allocated)
		return;
	if (raster)
		pixItem->paint(p,sog,w);
	else
		svgItem->paint(p,sog,w);
}

bool ULExtraGraphic::isAlive()
{
	return allocated;
}

QGraphicsItem * ULExtraGraphic::theItem()
{
	if(!allocated)
		return 0;
	if(raster)
	{
		return pixItem;
	}
	else
	{
		return svgItem;
	}
}




