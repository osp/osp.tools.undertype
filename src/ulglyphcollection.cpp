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
#include "ulglyphcollection.h"
#include "ulglyphscene.h"
#include "ulglyphitem.h"
#include "pagebuffer.h"

#include <QDebug>
#include <QFileDialog>

#define DONTKERN 0.00000001

ULGlyphCollection::ULGlyphCollection ( QGraphicsScene *scene, double size )
		:m_scene ( scene )
{
	sz = size;
	hasDesignKern = false;
}


ULGlyphCollection::~ULGlyphCollection()
{}

void ULGlyphCollection::autoFeed()
{

	fontfile = QFileDialog::getOpenFileName ( 0, "Open font file", "",  "Font files (*.pfb *.ttf *.otf)" );
	FT_Error error;
	FT_Library ftlib;
	error = FT_Init_FreeType ( &ftlib );
	if ( error )
	{
		qDebug ( "Unable to  init freetype" );
		return;
	}
	FT_Face face;
	error = FT_New_Face ( ftlib, fontfile.toUtf8(), 0, &face );
	if ( error )
	{
		qDebug ( "Unable to create new face" );
		return;
	}

	m_name = face->family_name;
	m_name += face->style_name;
	m_name += QString ( " (%1)" ).arg ( sz );

	if ( fontfile.endsWith ( "pfb" ) )
	{
		QString s ( fontfile );
		FT_Attach_File ( face, s.replace ( ".pfb",".afm" ).toUtf8() );
		qDebug() << "loading " << s;
	}

	{
		FT_ULong  charcode;
		FT_UInt   gindex;
		charcode = FT_Get_First_Char ( face, &gindex );
		while ( gindex != 0 )
		{
			reversedCMap[gindex] = charcode;
// 			cMap[charcode] = gindex;
			charcode = FT_Get_Next_Char ( face, charcode, &gindex );
		}
	}

	ULGlyphItem *tmpitem;
	for ( uint index = 1; index <= face->num_glyphs ;++index )
	{
		tmpitem = new ULGlyphItem();

		if ( tmpitem->fromOutline ( face, index, sz ) )
		{
			tmpitem->charcode = reversedCMap[index];
			cMap[tmpitem->charcode] = size();
			append ( tmpitem );
			m_scene->addItem ( last() );
			last()->collection = this;
			last()->index = index;
// 			qDebug() << last()->data(1);
		}
		else
		{
			qDebug() << index;
			delete tmpitem;
		}
	}



	if ( FT_HAS_KERNING ( face ) )
	{
		qDebug ( "BEGIN KERN" );
		hasDesignKern = true;

		FT_Vector k;
		for ( iterator it=begin(); it != end(); ++it )
		{
			for ( iterator it2=begin(); it2 != end(); ++it2 )
			{
				if ( !FT_Get_Kerning ( face,
				                       ( *it )->index,
				                       ( *it2 )->index,
				                       FT_KERNING_UNSCALED,
				                       &k ) )
				{
					if ( k.x )
					{
						kernMap.insert ( ( *it )->key + "#" + ( *it2 )->key, ( double ) k.x / ( double ) face->units_per_EM * sz );
// 					qDebug() << ( *it )->key << " "<< ( *it2 )->key << " : " <<  k.x ;
					}
				}
			}
		}
		qDebug() << m_name << " contains " << kernMap.count() << " kern pairs";
		qDebug ( "END OF KERN" );
	}
	else
	{
		qDebug() << m_name <<" has no kern table";
	}

	double sizeBox = sz * 2.0;
	double inArow = 24.0;

	QList<ULGlyphItem*>::iterator it = begin();
	for ( uint i = 0; i <= ( count() / inArow ) ; ++i )
	{
		for ( uint j = 0; j <  inArow ; ++j )
		{
			double px = j * sizeBox;
			double py = i * sizeBox;
			if ( it != end() )
			{
				( *it )->setPos ( px,py );
				++it;
			}

		}

	}

	FT_Done_Face ( face );
	FT_Done_FreeType ( ftlib );
	static_cast<ULGlyphScene*> ( m_scene )->mapKeys();

	ULGlyphCollectionRegister::Register(this);

}

void ULGlyphCollection::userFeed ( QString fpath, QString fname, QMap<uint,QString> glyames )
{
	qDebug() << "Loading " + fname;
	PageBuffer::getInstance()->fontCollection.append ( fname );
	fontfile = fpath;
	FT_Error error;
	FT_Library ftlib;
	error = FT_Init_FreeType ( &ftlib );
	if ( error )
	{
		qDebug ( "Unable to  init freetype" );
		return;
	}
	FT_Face face;
	error = FT_New_Face ( ftlib, fontfile.toUtf8(), 0, &face );
	if ( error )
	{
		qDebug() << fontfile.toUtf8() << fpath << fname << error;
		qDebug ( "Unable to create new face" );
		return;
	}
	if ( fontfile.endsWith ( "pfb" ) )
	{
		QString s ( fontfile );
		FT_Attach_File ( face, s.replace ( ".pfb",".afm" ).toUtf8() );
		qDebug() << "loading " << s;
	}
	m_name = fname;
	
	{
		FT_ULong  charcode;
		FT_UInt   gindex;
		charcode = FT_Get_First_Char ( face, &gindex );
		while ( gindex != 0 )
		{
			reversedCMap[gindex] = charcode;
// 			cMap[charcode] = gindex;
			charcode = FT_Get_Next_Char ( face, charcode, &gindex );
		}
	}
	
	ULGlyphItem *tmpitem;
	QList<uint> keylist = glyames.keys();
	for ( uint index = 0; index < keylist.count() ;++index )
	{
// 		qDebug() << index << "  :  " << keylist[index];
        uint faceIndex = keylist[index];
		tmpitem = new ULGlyphItem();
        if ( tmpitem->fromOutline ( face, faceIndex , sz ) )
		{
            tmpitem->charcode = reversedCMap[faceIndex];
			cMap[tmpitem->charcode] = size();
			append ( tmpitem );
			m_scene->addItem ( last() );
			last()->collection = this;
            last()->index =  faceIndex;
            // tmpitem->attachKey ( QString("<div style=\"background-color:white\">%1</div>").arg(glyames[keylist[index]]) );
            tmpitem->attachKey(glyames[faceIndex]);
// 			qDebug() << last()->XObject();
		}
		else
		{

			delete tmpitem;
		}
	}

	if ( FT_HAS_KERNING ( face ) )
	{
		qDebug ( "BEGIN KERN" );
		hasDesignKern = true;

		FT_Vector k;
		for ( iterator it=begin(); it != end(); ++it )
		{
			for ( iterator it2=begin(); it2 != end(); ++it2 )
			{
				if ( !FT_Get_Kerning ( face,
				                       ( *it )->index,
				                       ( *it2 )->index,
				                       FT_KERNING_UNSCALED,
				                       &k ) )
				{
					if ( k.x )
					{
						kernMap.insert ( ( *it )->key + "#" + ( *it2 )->key, ( double ) k.x / ( double ) face->units_per_EM * sz );
// 					qDebug() << ( *it )->key << " "<< ( *it2 )->key << " : " <<  k.x ;
					}
				}
			}
		}
		qDebug() << m_name << " contains " << kernMap.count() << " kern pairs";
		qDebug ( "END OF KERN" );
	}
	else
	{
		qDebug() << m_name <<" has no kern table";
	}

	double sizeBox = sz * 2.0;
	double inArow = 24.0;

	QList<ULGlyphItem*>::iterator it = begin();
	for ( uint i = 0; i <= ( count() / inArow ) ; ++i )
	{
		for ( uint j = 0; j <  inArow ; ++j )
		{
			double px = j * sizeBox;
			double py = i * sizeBox;
			if ( it != end() )
			{
				( *it )->setPos ( px,py );
				++it;
			}

		}

	}
	static_cast<ULGlyphScene*> ( m_scene )->mapKeys();

	FT_Done_Face ( face );
	FT_Done_FreeType ( ftlib );
	ULGlyphCollectionRegister::Register(this);
}

QString ULGlyphCollection::name()
{
	return m_name;
}

QString ULGlyphCollection::fontPath()
{
	return fontfile;
}

double ULGlyphCollection::fontSize()
{
	return sz;
}

QString ULGlyphCollection::toElement()
{
	QString ret;
	ret = "<collection>\n";
	ret += "<fontfile>" + fontfile + "</fontfile>\n";
	ret += "<name>" + m_name + "</name>\n";
	ret += "<size>" + QString::number ( sz ) + "</size>\n";
	for ( uint i = 0; i < count(); ++i )
	{
		ret += "\t<index>" + QString::number ( at ( i )->index ) + "</index>";
		ret += "\t<key>" + at ( i )->key + "</key>\n";
	}
	ret += "</collection>\n";
	return ret;
}

ULGlyphScene * ULGlyphCollection::scene()
{
	return static_cast<ULGlyphScene *> ( m_scene );
}

double ULGlyphCollection::kern ( QString gl, QString gr )
{
	if ( kernMap.contains ( gl+"#"+gr ) )
		return kernMap[gl+"#"+gr];
	else
	{
		if ( hasDesignKern )
			return DONTKERN;
		else
			return 0.0;
	}
}

void ULGlyphCollection::newKern ( QString gl, QString gr, double v )
{
	kernMap.insert ( gl+"#"+gr, v );
}

ULGlyphCollectionRegister* ULGlyphCollectionRegister::instance = 0;
ULGlyphCollectionRegister* ULGlyphCollectionRegister::that()
{
	if(instance == 0)
		instance = new ULGlyphCollectionRegister;
	return instance;
}

void ULGlyphCollectionRegister::Register(ULGlyphCollection * uc)
{
	that()->map.insert(uc->name(), uc);
}

ULGlyphCollection* ULGlyphCollectionRegister::Get(const QString &name)
{
	ULGlyphCollection * ret(0);
	if(that()->map.contains(name))
		return that()->map.value(name);
	return ret;
}
