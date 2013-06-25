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
#include "ulglyphscene.h"
#include "ulglyphitem.h"
#include "ullineeditor.h"
#include "ulglyphcollection.h"
#include "underline.h"

#include <QApplication>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QRegExp>
#include <QGraphicsView>
#include <QClipboard>
#include <QAction>
#include <QDebug>

ULGlyphScene::ULGlyphScene()
{
	keyHelpIsActive = false;
	keyHelper = addText ( "" );
	keyHelper->hide();
	keyHelper->setDefaultTextColor ( Qt::red );
	spaceDiv = 0.0;

    controlMod = false;
    metaMod = false;
/*
    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));
    */

}


ULGlyphScene::~ULGlyphScene()
{}

void ULGlyphScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
	QGraphicsItem * selected(0);/* = itemAt ( mouseEvent->lastScenePos() );*/
	double dist(0xFFFFFFFF);
	double ml(0);
	QPointF ref(mouseEvent->lastScenePos());
	foreach(QGraphicsItem* i, items())
	{
		ml = (ref - i->pos()).manhattanLength();
		if(ml < dist)
		{
			dist = ml;
			selected = i;
		}
	}



	if ( selected && (selected->boundingRect().height() < dist))
	{
// 		if ( mouseEvent->modifiers().testFlag ( Qt::ControlModifier ) )
// 		{
		QString oldkey = static_cast<ULGlyphItem*> ( selected )->key;
		ULGlyphItem* stock = fastKey[oldkey];
		fastKey.remove ( oldkey );
		static_cast<ULGlyphItem*> ( selected )->attachKey_interactive();
		fastKey.insert ( stock->key, stock );


// 		}

// 		else
// 		{
// 			ULGlyphItem *trans = static_cast<ULGlyphItem*> ( selected );
// 			double uem = trans->uniem();
// 			double size = trans->size;
// 			double x_adv = trans->m_xadvance;
// 			ULGlyphItem *item = new ULGlyphItem ( m_mainscene );
// 			item->setPath ( selected->shape() );
// 			item->setBrush ( QBrush ( Qt::SolidPattern ) );
// 			item->scale ( size /uem ,- ( size ) /uem );
// 			item->m_xadvance = x_adv;
// 			item->makeActive();
// 			clearFocus();
// 			m_mainscene->addGlyph ( item );
// 		}

	}
}

void ULGlyphScene::setMainScene ( ULLineEditor * s )
{
	m_mainscene = s;
}

void ULGlyphScene::focusInEvent ( QFocusEvent * event )
{
	setBackgroundBrush ( Qt::white );
}

void ULGlyphScene::focusOutEvent ( QFocusEvent * event )
{
	setBackgroundBrush ( Qt::lightGray );
}


void ULGlyphScene::keyPressEvent ( QKeyEvent * event )
{
    int key(event->key());
    Qt::KeyboardModifiers mods = event->modifiers();

    qDebug(QString("key[%1]\tcontrol[%2]\tmeta[%3]").arg(key).arg(controlMod).arg(metaMod).toAscii());

    if (event->key() == Qt::Key_V
         && (controlMod || metaMod))
    {
        paste();
        return;
    }

	if ( event->key() == Qt::Key_Backspace)
	{
		m_mainscene->removeLast();
		
		setFocus();
		return;
	}
	
	if(undertype::getMainWindow()->cMap())
	{
		keyPressEvent_byChar(event);
	}
	else
	{
		keyPressEvent_byName(event);
	}

    QGraphicsScene::keyPressEvent(event);
	
	setFocus();

    controlMod = mods.testFlag( Qt::ControlModifier );
    metaMod = mods.testFlag(Qt::MetaModifier);
}

void ULGlyphScene::paste()
{
    if(!undertype::getMainWindow()->cMap())
    {
        qDebug("You need to activate the charmap to enable copy and paste");
        return;
    }

    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if(mimeData->hasText())
    {

        QString t(mimeData->text());
        for(int i = 0; i < t.length(); i++)
        {
            insertChar(t.at(i).unicode());
        }
    }
    else
    {
        qDebug("Can not paste something else than plain text");
    }

}

void ULGlyphScene::insertChar(ushort unicode)
{
    ULGlyphItem *selected = m_collection->charcode(unicode);
    ULGlyphItem *item = new ULGlyphItem ( /*m_mainscene*/ );
    item->setPath ( selected->shape() );
    item->setBrush ( QBrush ( Qt::SolidPattern ) );
    item->setData ( 1,"glyph" );
    item->setData ( 2,selected->key );
    item->key = selected->key;
    item->collection = selected->collection;
    item->setPDF(selected->XObject());
    item->m_uniem = selected->uniem();
    item->scale ( selected->size / selected->uniem() ,- ( selected->size ) /selected->uniem() );
    item->m_xadvance = selected->m_xadvance;
    if(spaceDiv > 0.0)
    {
        item->m_xadvance = selected->m_xadvance / spaceDiv;
        spaceDiv = 0.0;
    }
    item->textKey = selected->textKey;
    item->showText ( false );
    item->makeActive();
    clearFocus();
    m_mainscene->addGlyph ( item );
}

void ULGlyphScene::keyPressEvent_byChar ( QKeyEvent * event )
{
	
	if(event->text().isEmpty())
		return;

    insertChar(event->text()[0].unicode());
}

void ULGlyphScene::keyPressEvent_byName ( QKeyEvent * event )
{

	if ( event->key() == Qt::Key_Return && !keyHelpIsActive )
	{
		keyHelpIsActive = true;
		keyHelper->show();
		curKey = "";
		return;
// 		curKey = QInputDialog::getText ( 0,"input","charstring" );

	}
	if ( event->key() == Qt::Key_Return && keyHelpIsActive )
	{
		keyHelpIsActive = false;
		keyHelper->hide();
		keyHelper->setPlainText ( "" );
	}
	else if ( event->key() == Qt::Key_Space  && !keyHelpIsActive  && event->modifiers().testFlag(Qt::ControlModifier))
	{
		curKey = "space";
		spaceDiv = QInputDialog::getDouble(this->views().first(), "alt space", "diviseur de l'espace",2.0, 1.0,10.0);
	}
	else if ( event->key() == Qt::Key_Space  && !keyHelpIsActive )
	{
		curKey = "space";
	}
	else
	{
		if ( keyHelpIsActive )
			curKey += event->text();
		else
			curKey = event->text();
	}
// 	qDebug ( curKey.toAscii() );
complete:
	if ( fastKey.contains ( curKey ) && !keyHelpIsActive )
	{
		ULGlyphItem *selected = fastKey[curKey];
		ULGlyphItem *item = new ULGlyphItem ( /*m_mainscene*/ );
		item->setPath ( selected->shape() );
		item->setBrush ( QBrush ( Qt::SolidPattern ) );
		item->setData ( 1,"glyph" );
		item->setData ( 2,selected->key );
		item->key = selected->key;
		item->collection = selected->collection;
		item->setPDF(selected->XObject());
		item->m_uniem = selected->uniem();
		item->scale ( selected->size / selected->uniem() ,- ( selected->size ) /selected->uniem() );
		item->m_xadvance = selected->m_xadvance;
		if(spaceDiv > 0.0)
		{
			item->m_xadvance = selected->m_xadvance / spaceDiv;
			spaceDiv = 0.0;
		}
		item->textKey = selected->textKey;
		item->showText ( false );
		item->makeActive();
		clearFocus();
		m_mainscene->addGlyph ( item );
		curKey.clear();
	}
	else if ( keyHelpIsActive && !curKey.isEmpty() )
	{


		keyHelper->setPlainText ( "" );
		keyHelper->setPos ( views().first()->mapToScene ( 0,0 ) );
		QStringList kl = keyHelpList.filter ( QRegExp ( QString ( "^%1" ).arg ( curKey ) ) );
		kl.sort();
		if ( kl.count() == 1 )
		{
			curKey = kl[0];
			keyHelpIsActive = false;
			keyHelper->hide();
			keyHelper->setPlainText ( "" );
			goto complete;
		}
		QString res = curKey + "\n\n" + kl.join ( "\n" );
		keyHelper->setPlainText ( res );


	}
}

void ULGlyphScene::mapKeys()
{
	fastKey.clear();


	for ( uint i = 0;i < m_collection->count(); ++i )
	{
		if ( m_collection->at ( i )->keyIsSet )
			fastKey[m_collection->at ( i )->key] = m_collection->at ( i );

	}

	keyHelpList = QStringList ( fastKey.keys() );
}



void ULGlyphScene::setCollection ( ULGlyphCollection * col )
{
	m_collection = col;
}

ULGlyphCollection * ULGlyphScene::collection()
{
	return m_collection;
}

ULGlyphItem * ULGlyphScene::keyItem(QString k)
{
	return fastKey[k];
}

// QString ULGlyphScene::exploreBranch(QString dep)
// {
// 	QStringList kl = keyHelpList.filter ( QRegExp ( QString ( "^%1" ).arg ( dep ) ) );
// 	kl.sort();
// 	QDialog *dial = new QDialog(views().first());
// 	
// }


