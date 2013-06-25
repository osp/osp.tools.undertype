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
#include "ulkerningfactory.h"
#include "ulglyphitem.h"
#include "ulglyphcollection.h"


#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QDebug>
#include <QGraphicsView>

#include <math.h>

#define  MAXKERN 100000 

ULKerningFactory::ULKerningFactory(double k_factor)
{
	k = k_factor;
	scene = new QGraphicsScene(0);
	// It's an arbitrary list from GillSans, intended to skip pairs that do not need autokrning
	// Feel free to provide a better list!
	autoPairs << "space#quotedblleft"
			<< "space#quoteleft"
			<< "space#Y"
			<< "space#W"
			<< "space#V"
			<< "space#T"
			<< "space#A"
			<< "comma#quotedblright"
			<< "comma#quoteright"
			<< "comma#space"
			<< "period#quotedblright"
			<< "period#quoteright"
			<< "period#space"
			<< "colon#space"
			<< "semicolon#space"
			<< "A#y"
			<< "A#w"
			<< "A#v"
			<< "A#Y"
			<< "A#W"
			<< "A#V"
			<< "A#U"
			<< "A#T"
			<< "A#Q"
			<< "A#O"
			<< "A#G"
			<< "A#C"
			<< "B#A"
			<< "C#A"
			<< "C#period"
			<< "C#comma"
			<< "D#Y"
			<< "D#W"
			<< "D#V"
			<< "D#A"
			<< "D#period"
			<< "D#comma"
			<< "F#o"
			<< "F#e"
			<< "F#A"
			<< "F#period"
			<< "F#comma"
			<< "J#u"
			<< "J#o"
			<< "J#e"
			<< "K#y"
			<< "K#u"
			<< "K#o"
			<< "K#e"
			<< "K#O"
			<< "L#quotedblright"
			<< "L#quoteright"
			<< "L#y"
			<< "L#Y"
			<< "L#W"
			<< "L#V"
			<< "L#T"
			<< "O#Y"
			<< "O#X"
			<< "O#W"
			<< "O#V"
			<< "O#T"
			<< "O#A"
			<< "O#period"
			<< "O#comma"
			<< "P#o"
			<< "P#e"
			<< "P#A"
			<< "P#period"
			<< "P#comma"
			<< "R#Y"
			<< "R#W"
			<< "R#V"
			<< "R#U"
			<< "R#T"
			<< "R#O"
			<< "T#y"
			<< "T#w"
			<< "T#u"
			<< "T#r"
			<< "T#o"
			<< "T#e"
			<< "T#a"
			<< "T#O"
			<< "T#A"
			<< "T#semicolon"
			<< "T#colon"
			<< "T#period"
			<< "T#hyphen"
			<< "T#comma"
			<< "U#A"
			<< "U#period"
			<< "U#comma"
			<< "V#u"
			<< "V#o"
			<< "V#e"
			<< "V#a"
			<< "V#O"
			<< "V#G"
			<< "V#A"
			<< "V#period"
			<< "V#hyphen"
			<< "V#comma"
			<< "W#y"
			<< "W#u"
			<< "W#o"
			<< "W#e"
			<< "W#a"
			<< "W#O"
			<< "W#A"
			<< "W#period"
			<< "W#hyphen"
			<< "W#comma"
			<< "Y#u"
			<< "Y#o"
			<< "Y#e"
			<< "Y#a"
			<< "Y#S"
			<< "Y#O"
			<< "Y#A"
			<< "Y#semicolon"
			<< "Y#colon"
			<< "Y#period"
			<< "Y#hyphen"
			<< "Y#comma"
			<< "a#y"
			<< "a#w"
			<< "a#v"
			<< "a#p"
			<< "b#y"
			<< "b#period"
			<< "b#comma"
			<< "c#period"
			<< "c#comma"
			<< "d#d"
			<< "e#y"
			<< "e#w"
			<< "e#v"
			<< "e#period"
			<< "e#comma"
			<< "f#quotedblright"
			<< "f#quoteright"
			<< "f#dotlessi"
			<< "f#o"
			<< "f#e"
			<< "g#y"
			<< "g#o"
			<< "h#y"
			<< "k#o"
			<< "k#e"
			<< "l#y"
			<< "l#w"
			<< "m#y"
			<< "m#u"
			<< "n#y"
			<< "n#v"
			<< "n#u"
			<< "o#y"
			<< "o#x"
			<< "o#w"
			<< "o#v"
			<< "o#period"
			<< "o#comma"
			<< "p#y"
			<< "p#period"
			<< "p#comma"
			<< "r#y"
			<< "r#v"
			<< "r#t"
			<< "r#r"
			<< "r#q"
			<< "r#o"
			<< "r#e"
			<< "r#d"
			<< "r#c"
			<< "r#semicolon"
			<< "r#colon"
			<< "r#period"
			<< "r#hyphen"
			<< "r#comma"
			<< "s#w"
			<< "v#o"
			<< "v#e"
			<< "v#period"
			<< "v#comma"
			<< "w#o"
			<< "w#e"
			<< "w#period"
			<< "w#comma"
			<< "x#e"
			<< "y#o"
			<< "y#e"
			<< "y#period"
			<< "y#comma"
			<< "quoteright#quotedblright"
			<< "quoteright#v"
			<< "quoteright#t"
			<< "quoteright#s"
			<< "quoteright#r"
			<< "quoteright#d"
			<< "quotedblleft#quoteleft" ;

}


ULKerningFactory::~ULKerningFactory()
{
	delete scene;
}

double ULKerningFactory::kern(ULGlyphItem *left, ULGlyphItem *right)
{
	double ret = 0.0;
	if(left->collection == right->collection)
	{
		ret = left->collection->kern(left->key, right->key);
	}
	if (ret != 0.0)
		return ret;
	
	// Here is automatic kerning, I don't like it but since it's really a pain
	// to make a kern table from OTF, I keep it until I make up my mind about
	// linking against Harfbuzz (or not).
	
	if(!autoPairs.contains(left->key +"#"+ right->key))
		return 0.0;
	
	double sz = left->uniem();

	QGraphicsPathItem *leftpath = new QGraphicsPathItem();
	QGraphicsPathItem *rightpath = new QGraphicsPathItem();
	
	leftpath->setPath(left->path());
	rightpath->setPath(right->path());
	

	scene->addItem(leftpath);
	leftpath->setPos(0.0, sz);
	scene->addItem(rightpath);
// 	qDebug() << left->m_xadvance << sz << left->collection->fontSize() << left->m_xadvance * sz / left->collection->fontSize();
	rightpath->setPos(left->m_xadvance * sz / left->collection->fontSize() , sz);
// 	qDebug() << leftpath->boundingRect();
// 	qDebug() << rightpath->boundingRect();
	uint stepcounter = 0;
	uint inc = 10;
	bool bing = false;
	while( rightpath->x() > 0.0)
	{
		rightpath->moveBy((double)inc * -1.0,0.0);
		stepcounter += inc;
// 		qDebug() << rightpath->pos() << stepcounter;
		{
		if(leftpath->collidesWithItem(rightpath))
		{
			if(bing)
			{
// 				qDebug() << stepcounter;
// 				qDebug() << powf(2.0,2.0);
				double t = powf((double)stepcounter, k);
// 				qDebug() << t;
				stepcounter = t / 1000.0 ;
				qDebug() << stepcounter ;
				break;
			}
			else
			{
				bing = true;
				rightpath->moveBy((double)inc,0.0);
				stepcounter -= inc;
				inc = 1;
			}
		}
		}
// 		qDebug() << rightpath->x();
	}
	
	
	if( rightpath->x() <= 0.0 || stepcounter < 40.0)
	{
		left->collection->newKern(left->key, right->key, 0.00000001);
		scene->removeItem(leftpath);
		scene->removeItem(rightpath);
		delete leftpath;
		delete rightpath;
		return  0.00000001;
	}
	else
	{
		ret =  (stepcounter * left->collection->fontSize() / sz) * -1 ;
		qDebug() << "ret = " << ret;
		
		left->collection->newKern(left->key, right->key, ret);
		scene->removeItem(leftpath);
		scene->removeItem(rightpath);
		delete leftpath;
		delete rightpath;
		return ret;
	}
}


