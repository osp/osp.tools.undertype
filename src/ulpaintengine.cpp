//
// C++ Implementation: ulpaintengine
//
// Description: 
//
//
// Author: Pierre Marchand <pierre@moulindetouvois.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ulpaintengine.h"

#include <QDataStream>
#include <QPainterPath>
#include <QPaintEngineState>
#include <QDebug>
/**
	Implementing drawPath() & expect base class to work for us as the documentation states :
	"The default implementation of this function will try to use drawPath()
	if the engine supports the feature QPaintEngine::PainterPaths [...]"
*/

ULPaintEngine::ULPaintEngine(QByteArray* content)
	:QPaintEngine(QPaintEngine::AllFeatures)
{
	qDebug() << "MyPaintEngine : " << this << "hasFeature(PainterPaths) = " << hasFeature(QPaintEngine::PainterPaths);
	array = content;
	
}


ULPaintEngine::~ULPaintEngine()
{
}


bool ULPaintEngine::begin(QPaintDevice* pdev)
{
	qDebug() << "MyPaintEngine (begin) : device ==" << pdev << ", Me == " << this;
	stream = new QDataStream(array,QIODevice::WriteOnly);
	if(stream)
		return true;
	setActive(true);
	setPaintDevice(pdev);
	
	
	return false;
}

bool ULPaintEngine::end()
{
	qDebug() << "MyPaintEngine (end)";

	if(stream)
		delete stream;
	
	setActive(false);
	return true;
}

QPoint ULPaintEngine::coordinateOffset() const
{
    return QPaintEngine::coordinateOffset();
}

QPaintEngine::Type ULPaintEngine::type() const
{
	return QPaintEngine::User;
}

void ULPaintEngine::drawEllipse(const QRectF& r)
{
    QPaintEngine::drawEllipse(r);
}

void ULPaintEngine::drawEllipse(const QRect& r)
{
    QPaintEngine::drawEllipse(r);
}

void ULPaintEngine::drawImage(const QRectF& rect, const QImage& pm, const QRectF& r, Qt::ImageConversionFlags flags)
{
//     QPaintEngine::drawImage(r, pm, r, flags);
	qDebug("This engine does not want to deal with pictures");
}

void ULPaintEngine::drawLines(const QLineF* lines, int lineCount)
{
    QPaintEngine::drawLines(lines, lineCount);
// 	QByteArray a;
// 	QLineF l;
// 	for(int i = 0 ; i < lineCount; ++i)
// 	{
// 		l = lines[i];
// 		QPointF p(l.x1(), l.y1());
// 		if(p == (*point))
// 		{
// 			a = QString("%1 %2 l\n").arg(l.x2()).arg(l.y2()).toAscii();
// 			point->setX(l.x2());
// 			point->setY(l.y2());
// 			write(a);
// 		}
// 		else
// 		{
// 			a = QString("%1 %2 m\n%3 % 4 l\n").arg(l.x1()).arg(l.y1()).arg(l.x2()).arg(l.y2()).toAscii();
// 			point->setX(l.x2());
// 			point->setY(l.y2());
// 			write(a);
// 		}
// 	}
}

void ULPaintEngine::drawLines(const QLine* lines, int lineCount)
{
    drawLines(lines, lineCount);
}

void ULPaintEngine::drawPath(const QPainterPath& path)
{
//     QPaintEngine::drawPath(path);
	for(int i = 0 ; i < path.elementCount(); ++i)
	{
		const QPainterPath::Element& elem = path.elementAt(i);
		switch(elem.type)
		{
			case QPainterPath::MoveToElement:
				write(QString("%1 %2 m\n").arg(elem.x).arg(elem.y).toAscii());
				break;
			case QPainterPath::LineToElement:
				write(QString("%1 %2 l\n").arg(elem.x).arg(elem.y).toAscii());
				break;
			case QPainterPath::CurveToElement:
			{
				const QPainterPath::Element& c1 = path.elementAt(i + 1);
				const QPainterPath::Element& c2 = path.elementAt(i + 2);
				write(QString("%1 %2 %3 %4 %5 %6 c\n")
						.arg(elem.x)
						.arg(elem.y)
						.arg(c1.x)
						.arg(c1.y)
						.arg(c2.x)
						.arg(c2.y)
						.toAscii());
				break;
			}
			default:qDebug() << "DrawPath: can't handle -> "<< elem.type;
			
		}
		
	}
	Qt::FillRule fr = path.fillRule();
	QByteArray fill((fr == Qt::WindingFill) ? "f\n" : "f*\n");
	write(fill);
}

void ULPaintEngine::drawPixmap(const QRectF& r1, const QPixmap& pm, const QRectF& r)
{
	qDebug("This engine does not want to deal with pictures");
}

void ULPaintEngine::drawPoints(const QPointF* points, int pointCount)
{
    QPaintEngine::drawPoints(points, pointCount);
}

void ULPaintEngine::drawPoints(const QPoint* points, int pointCount)
{
    QPaintEngine::drawPoints(points, pointCount);
}

void ULPaintEngine::drawPolygon(const QPointF* points, int pointCount, PolygonDrawMode mode)
{
    QPaintEngine::drawPolygon(points, pointCount, mode);
}

void ULPaintEngine::drawPolygon(const QPoint* points, int pointCount, PolygonDrawMode mode)
{
    QPaintEngine::drawPolygon(points, pointCount, mode);
}

void ULPaintEngine::drawRects(const QRectF* rects, int rectCount)
{
    QPaintEngine::drawRects(rects, rectCount);
}

void ULPaintEngine::drawRects(const QRect* rects, int rectCount)
{
    QPaintEngine::drawRects(rects, rectCount);
}

void ULPaintEngine::drawTextItem(const QPointF& p, const QTextItem& textItem)
{
    QPaintEngine::drawTextItem(p, textItem);
}

void ULPaintEngine::drawTiledPixmap(const QRectF& r, const QPixmap& pixmap, const QPointF& s)
{
//     QPaintEngine::drawTiledPixmap(r, pixmap, s);
	qDebug("This engine does not want to deal with pictures");
}

void ULPaintEngine::updateState(const QPaintEngineState& s)
{
	;
}

void ULPaintEngine::write(QByteArray a)
{
	stream->writeRawData(a.constData(), a.size());
}

