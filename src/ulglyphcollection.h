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
#ifndef ULGLYPHCOLLECTION_H
#define ULGLYPHCOLLECTION_H




#include <QList>
#include <QMap>
#include <QGraphicsScene>

class ULGlyphScene;
class ULGlyphItem;

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
*/


class ULGlyphCollection : public QList<ULGlyphItem*>
{
public:
	ULGlyphCollection(QGraphicsScene *scene, double size);

    ~ULGlyphCollection();
    void autoFeed();//load all glyphs
    void userFeed(QString fpath,QString fname, QMap<uint,QString> glyames);
    QString name();
    QString fontPath();
    double fontSize();
    QString toElement();
    ULGlyphScene *scene();
    double kern(QString gl, QString gr);
    void newKern(QString gl, QString gr, double v);
    ULGlyphItem* charcode(int c){return at(cMap[c]);};
//     QMap<QString, QPixmap*> pixGlyph;
private:
	QGraphicsScene *m_scene;
	QString m_name;
	QString fontfile;
	double sz;
	QMap<QString, double> kernMap;
	bool hasDesignKern;
	QMap<uint, uint> reversedCMap;
	QMap<uint, uint> cMap;
};

class ULGlyphCollectionRegister
{
	QMap<QString, ULGlyphCollection*> map;
	static ULGlyphCollectionRegister * instance;
	static ULGlyphCollectionRegister * that();

	ULGlyphCollectionRegister(){}


public:
	static void Register(ULGlyphCollection*);
	static ULGlyphCollection* Get(const QString& name);

};


#endif
