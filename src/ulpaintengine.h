//
// C++ Interface: ulpaintengine
//
// Description: 
//
//
// Author: Pierre Marchand <pierre@moulindetouvois.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ULPAINTENGINE_H
#define ULPAINTENGINE_H

#include <QPaintEngine>

class QByteArray;
class QDataStream;

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
	
	This custom QPaintEngine is intended to provide a way to set
	a PDF content stream (CS). Because I do not see any other way to 
	get SVG subpaths (it's the reason why I won't reimplement drawImage(),
	drawPixmap()  and drawTiledPixmap() nor drawTextItem()).
	
*/
class ULPaintEngine : public QPaintEngine
{
public:
	ULPaintEngine(QByteArray *content);

    ~ULPaintEngine();

     bool begin(QPaintDevice* pdev);
     bool end();
     QPoint coordinateOffset() const;
     QPaintEngine::Type type() const;
     void drawEllipse(const QRectF& r);
     void drawEllipse(const QRect& r);
     void drawImage(const QRectF& rect, const QImage& pm, const QRectF& r, Qt::ImageConversionFlags flags);
     void drawLines(const QLineF* lines, int lineCount);
     void drawLines(const QLine* lines, int lineCount);
     void drawPath(const QPainterPath& path);
     void drawPixmap(const QRectF& rect, const QPixmap& pm, const QRectF& r);
     void drawPoints(const QPointF* points, int pointCount);
     void drawPoints(const QPoint* points, int pointCount);
     void drawPolygon(const QPointF* points, int pointCount, PolygonDrawMode mode);
     void drawPolygon(const QPoint* points, int pointCount, PolygonDrawMode mode);
     void drawRects(const QRectF* rects, int rectCount);
     void drawRects(const QRect* rects, int rectCount);
     void drawTextItem(const QPointF& p, const QTextItem& textItem);
     void drawTiledPixmap(const QRectF& r, const QPixmap& pixmap, const QPointF& s);
     void updateState(const QPaintEngineState& s);
	private:
		QByteArray *array;
		QDataStream *stream;
		void write(QByteArray a);
		QPaintDevice *theDevice;

};

#endif
