//
// C++ Interface: ulpdfdevice
//
// Description:
//
//
// Author: Pierre Marchand <pierre@moulindetouvois.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ULPDFDEVICE_H
#define ULPDFDEVICE_H

#include <QPaintDevice>
#include <QByteArray>


class ULPaintEngine;

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/
class ULPDFDevice : public QPaintDevice
{
	public:
		ULPDFDevice ( QByteArray *content );

		~ULPDFDevice();
	
		int devType() const;

	private:
		ULPaintEngine *pdfengine;
		QByteArray *contentstream;
	public:
// 		QByteArray *getContent(){return contentstream;};
		QByteArray getContentArray();
	protected:
		virtual QPaintEngine* paintEngine() const;
		int metric ( QPaintDevice::PaintDeviceMetric metric ) const ;
// 	private:
// 		friend class ULExtraGraphic;
};

#endif
