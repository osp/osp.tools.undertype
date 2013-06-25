//
// C++ Implementation: ulpdfdevice
//
// Description: 
//
//
// Author: Pierre Marchand <pierre@moulindetouvois.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ulpdfdevice.h"
#include "ulpaintengine.h"

#include <QByteArray>
#include <QDebug>

ULPDFDevice::ULPDFDevice(QByteArray *content)
 : QPaintDevice()
{
	qDebug() << "creating MyPaintDevice";
	contentstream = content;
	
	pdfengine = new ULPaintEngine(contentstream);
}


ULPDFDevice::~ULPDFDevice()
{
	qDebug() << "deleting MyPaintDevice";
	if(pdfengine)
		delete pdfengine;
}


QPaintEngine* ULPDFDevice::paintEngine() const
{
	qDebug() << "MyPaintDevice (paintEngine) : Me == "<< this <<", engine == " << pdfengine;
	return pdfengine;
}

int ULPDFDevice::metric(QPaintDevice::PaintDeviceMetric metric) const
{
	//Test
	return 100000000;
	
}

int ULPDFDevice::devType() const
{
	return 12;
}


