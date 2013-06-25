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


#include <QtGui>
#include "underline.h"
#include "ulpageitem.h"
#include "ulglyphcollection.h"
#include "ulglyphscene.h"
#include "ullineeditor.h"
#include "uleditglyph.h"
#include "ulpageview.h"
#include "pagebuffer.h"
#include "ulglyphitem.h"
#include "ulunit.h"
#include "ulextragraphic.h"
#include "ulextragraphicscene.h"


#include <QTextEdit>
#include <QTextStream>
#include <QDataStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDockWidget>
#include <QTabWidget>
#include <QPushButton>
#include <QInputDialog>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDomDocument>
#include <QDomNodeList>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QMapIterator>
#include <QDebug>

undertype* undertype::theMainWindow = 0;

undertype::undertype()
{
	maybeFlag = true;
	m_usecmap = false;
	
	theMainWindow = this;

	pages = new ULPageItem ( 595.276,841.89 ); // a4
	mainView = new ULPageView ( pages );
	pageWidget = new QGroupBox ( this );
	pageLayout = new QVBoxLayout();
	pageControls = new QHBoxLayout();
	pageControls->addWidget ( new QLabel ( "cursor (X)" ) );
	pageCursorX = new QDoubleSpinBox();
	pageCursorX->setRange ( 0.0,10000.0 );
	pageControls->addWidget ( pageCursorX );
	pageControls->addWidget ( new QLabel ( "cursor (Y)" ) );
	pageCursorY = new QDoubleSpinBox();
	pageCursorY->setRange ( 0.0,10000.0 );
	pageControls->addWidget ( pageCursorY );
	pageAA = new QPushButton ( "switch AA",0 );
	pageControls->addWidget ( pageAA );
	pageControls->addStretch();
	currentLineLabel = new QLabel("");
	pageControls->addWidget (currentLineLabel);
	
	
	pageDimLabel = new QLabel ( "page : " + QString().number ( ULUnit ( 595.276 ).mm() ) +" mm x "+QString().number ( ULUnit ( 841.89 ).mm() ) + " mm" );
	pageDimLabel->setFont ( QFont ( "sans", 7 ) );
	pageRatioLabel = new QLabel("100 %");
	pageRatioLabel->setFont ( QFont ( "sans", 7 ,QFont::Bold) );
	
	statusBar()->addPermanentWidget ( pageDimLabel );
	statusBar()->addPermanentWidget ( pageRatioLabel );
	pageLayout->addLayout ( pageControls );
	pageLayout->addWidget ( mainView );
	pageWidget->setLayout ( pageLayout );
	setCentralWidget ( pageWidget );



	glyphPalette = new QDockWidget ( tr ( "Casses" ), this );
	tabCasses = new QTabWidget ( glyphPalette );
	newCasseBut = new QPushButton ( "New" );
	saveCasseBut = new QPushButton ( "Save" );
	loadCasseBut = new QPushButton ( "Load" );
	useCMapCheck = new QCheckBox ("Use Cmap");
	casseSelect = new QComboBox();
	casseBox = new QGroupBox ( tabCasses );
	casseManagerLayout = new QGridLayout();
	casseManagerLayout->addWidget ( newCasseBut, 0,0 );
	casseManagerLayout->addWidget ( loadCasseBut,0,1 );
	casseManagerLayout->addWidget ( casseSelect,1,0 );
	casseManagerLayout->addWidget ( saveCasseBut,1,1 );
	casseManagerLayout->addWidget ( useCMapCheck,2,1);
	casseBox->setLayout ( casseManagerLayout );
	tabCasses->addTab ( casseBox, "manager" );
	glyphPalette->setWidget ( tabCasses );
	addDockWidget ( Qt::RightDockWidgetArea, glyphPalette );

	extraScene = new ULExtraGraphicScene ( pages );
	extraView = new QGraphicsView ( extraScene );
	extraView->scale((double)physicalDpiX() / 72.0, (double)physicalDpiY() / 72.0);
	tabCasses->addTab ( extraView, "Graphics" );

	lineEditor = new QDockWidget ( tr ( "Composteur" ), this );
	lineWidget = new QGroupBox ( lineEditor );
	lineLayout = new QHBoxLayout();
	lineControls = new QGridLayout();

	lineControls->addWidget ( new QLabel ( "Line height" ),0,0 );
	lhSpin = new QDoubleSpinBox ( lineWidget );
	lhSpin->setSingleStep ( 0.1 );
	lhSpin->setRange ( -1000.0,1000.0 );
	lineControls->addWidget ( lhSpin,0,1 );

	lineControls->addWidget ( new QLabel ( "Line width" ),1,0 );
	lwSpin = new QDoubleSpinBox ( lineWidget );
	lwSpin->setSingleStep ( 0.1 );
	lwSpin->setRange ( -1000.0,1000.0 );
	lineControls->addWidget ( lwSpin,1,1 );

	lineControls->addWidget ( new QLabel ( "cursor(x)" ),2,0 );
	gcursor = new QDoubleSpinBox ( lineWidget );
	gcursor->setSingleStep ( 0.1 );
	gcursor->setRange ( -1000.0,1000.0 );
	lineControls->addWidget ( gcursor,2,1 );

	gcursorBegin = new QPushButton ( "to begin", 0 );
	gcursorEnd = new QPushButton ( "to end",0 );
	lineControls->addWidget ( gcursorBegin, 3,0 );
	lineControls->addWidget ( gcursorEnd, 3,1 );

	gcursorLTR = new QCheckBox ( "Left to Right",0 );
	gcursorLTR->setCheckState ( Qt::Checked );
	lineControls->addWidget ( gcursorLTR,4,0 );


	lineScene = new ULLineEditor ( pages );

	lineView = new QGraphicsView ( lineScene );
	lineView->setRenderHints ( QPainter::Antialiasing );
	pageAntiAlias = true;
// 	lineView->setViewport(new QGLWidget);
	lineView->move ( 100.0,10.0 );

	lineLayout->addLayout ( lineControls );
	lineLayout->addWidget ( lineView );
	lineWidget->setLayout ( lineLayout );
	lineEditor->setWidget ( lineWidget );
	addDockWidget ( Qt::BottomDockWidgetArea, lineEditor );




	connect ( newCasseBut,SIGNAL ( released() ),this,SLOT ( newCasse() ) );
	connect ( saveCasseBut,SIGNAL ( released() ),this,SLOT ( saveCasse() ) );
	connect ( loadCasseBut,SIGNAL ( released() ),this,SLOT ( loadCasse() ) );
	connect(useCMapCheck,SIGNAL(toggled( bool )),this,SLOT(switchCMap(bool)));

	connect ( lineScene,SIGNAL ( wantEdit ( ULGlyphItem* ) ),this,SLOT ( editGlyph ( ULGlyphItem* ) ) );

	connect ( lhSpin,SIGNAL ( valueChanged ( double ) ),lineScene,SLOT ( slotChangeBaseLine ( double ) ) );
	connect ( lwSpin,SIGNAL ( valueChanged ( double ) ),lineScene,SLOT ( slotChangeWidth ( double ) ) );
	connect ( gcursor,SIGNAL ( valueChanged ( double ) ),lineScene,SLOT ( slotMoveCursor ( double ) ) );
	connect ( lineScene,SIGNAL ( linesChanged ( double, double, double ) ),this,SLOT ( changeLineWidgetValues ( double, double, double ) ) );
	connect ( gcursorBegin, SIGNAL ( released() ), this,SLOT ( moveGcursorBegin() ) );
	connect ( gcursorEnd,SIGNAL ( released() ), this, SLOT ( moveGcursorEnd() ) );
	connect ( gcursorLTR, SIGNAL ( stateChanged ( int ) ), lineScene, SLOT ( slotLTR ( int ) ) );

	connect ( pageCursorX,SIGNAL ( valueChanged ( double ) ),pages,SLOT ( setCurX ( double ) ) );
	connect ( pageCursorY,SIGNAL ( valueChanged ( double ) ),pages,SLOT ( setCurY ( double ) ) );
	connect ( pages,SIGNAL ( curPosXChanged ( double ) ),pageCursorX,SLOT ( setValue ( double ) ) );
	connect ( pages,SIGNAL ( curPosYChanged ( double ) ),pageCursorY,SLOT ( setValue ( double ) ) );
	connect ( pageAA, SIGNAL ( released() ),this, SLOT ( switchPAA() ) );

	connect ( pages,SIGNAL ( wantEditLine ( QGraphicsItem* ) ),lineScene,SLOT ( slotEditGroup ( QGraphicsItem* ) ) );
	
	connect ( pages, SIGNAL(infoCurrentLine(QString)), currentLineLabel, SLOT(setText(QString)));
	newLine();

	pageCursorY->setValue ( 40.0 );
	pageCursorX->setValue ( 40.0 );
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();

	readSettings();



	setCurrentFile ( "" );
}

void undertype::closeEvent ( QCloseEvent *event )
{
//       if (maybeSave()) {
//             writeSettings();
//             event->accept();
//       } else {
//             event->ignore();
//       }
}

void undertype::newFile()
{
	if ( maybeSave() )
	{
		maybeFlag =false;
		closeDoc();
		maybeFlag = true;
		setCurrentFile ( "" );
	}
}

void undertype::open()
{
	if ( maybeSave() )
	{
		QString fileName = QFileDialog::getOpenFileName ( this );
		if ( !fileName.isEmpty() )
		{
			maybeFlag = false;
			closeDoc();
			maybeFlag = true;
			loadFile ( fileName );
		}
	}

}

bool undertype::save()
{
	if ( curFile.isEmpty() )
	{
		return saveAs();
	}
	else
	{
		return saveFile ( curFile );
	}
}

bool undertype::saveAs()
{
	QString fileName = QFileDialog::getSaveFileName ( this );
	if ( fileName.isEmpty() )
		return false;

	return saveFile ( fileName );
}

void undertype::about()
{
	QMessageBox::about ( this, tr ( "About UnderType" ),
	                     QString ( "Un projet pédagogique de formation de l'imprimé" ) );
}

void undertype::documentWasModified()
{
	setWindowModified ( true );
}

void undertype::createActions()
{
	newAct = new QAction ( QIcon ( ":/filenew.xpm" ), tr ( "&New" ), this );
	newAct->setShortcut ( tr ( "Ctrl+N" ) );
	newAct->setStatusTip ( tr ( "Create a new file" ) );
	connect ( newAct, SIGNAL ( triggered() ), this, SLOT ( newFile() ) );

	openAct = new QAction ( QIcon ( ":/fileopen.xpm" ), tr ( "&Open..." ), this );
	openAct->setShortcut ( tr ( "Ctrl+O" ) );
	openAct->setStatusTip ( tr ( "Open an existing file" ) );
	connect ( openAct, SIGNAL ( triggered() ), this, SLOT ( open() ) );

	saveAct = new QAction ( QIcon ( ":/filesave.xpm" ), tr ( "&Save" ), this );
	saveAct->setShortcut ( tr ( "Ctrl+S" ) );
	saveAct->setStatusTip ( tr ( "Save the document to disk" ) );
	connect ( saveAct, SIGNAL ( triggered() ), this, SLOT ( save() ) );

	saveAsAct = new QAction ( tr ( "Save &As..." ), this );
	saveAsAct->setStatusTip ( tr ( "Save the document under a new name" ) );
	connect ( saveAsAct, SIGNAL ( triggered() ), this, SLOT ( saveAs() ) );
	
	closeAct = new QAction(tr("Close doc"),this);
	connect ( closeAct, SIGNAL(triggered( bool )),this,SLOT(closeDoc()));

	exitAct = new QAction ( tr ( "E&xit" ), this );
	exitAct->setShortcut ( tr ( "Ctrl+Q" ) );
	exitAct->setStatusTip ( tr ( "Exit the application" ) );
	connect ( exitAct, SIGNAL ( triggered() ), this, SLOT ( close() ) );

	pageAct = new QAction ( tr ( "Page Setup..." ),this );
	pageAct->setStatusTip ( tr ( "Set up page dim." ) );
	connect ( pageAct, SIGNAL ( triggered() ),this, SLOT ( pageSetup() ) );

	pdfAct = new QAction ( tr ( "Export to PDF..." ),this );
	connect ( pdfAct, SIGNAL ( triggered() ),this, SLOT ( exportPDF() ) );

	cutAct = new QAction ( QIcon ( ":/editcut.xpm" ), tr ( "Cu&t" ), this );
	cutAct->setShortcut ( tr ( "Ctrl+X" ) );
	cutAct->setStatusTip ( tr ( "Cut the current selection's contents to the "
	                            "clipboard" ) );

	copyAct = new QAction ( QIcon ( ":/editcopy.xpm" ), tr ( "&Copy" ), this );
	copyAct->setShortcut ( tr ( "Ctrl+C" ) );
	copyAct->setStatusTip ( tr ( "Copy the current selection's contents to the "
	                             "clipboard" ) );

	pasteAct = new QAction ( QIcon ( ":/editpaste.xpm" ), tr ( "&Paste" ), this );
	pasteAct->setShortcut ( tr ( "Ctrl+V" ) );
	pasteAct->setStatusTip ( tr ( "Paste the clipboard's contents into the current "
	                              "selection" ) );

	aboutAct = new QAction ( tr ( "&About" ), this );
	aboutAct->setStatusTip ( tr ( "Show the application's About box" ) );
	connect ( aboutAct, SIGNAL ( triggered() ), this, SLOT ( about() ) );

	aboutQtAct = new QAction ( tr ( "About &Qt" ), this );
	aboutQtAct->setStatusTip ( tr ( "Show the Qt library's About box" ) );
	connect ( aboutQtAct, SIGNAL ( triggered() ), qApp, SLOT ( aboutQt() ) );

	printAct = new QAction ( tr ( "&Print" ),this );
	connect ( printAct, SIGNAL ( triggered() ), this, SLOT ( print() ) );

	cutAct->setEnabled ( false );
	copyAct->setEnabled ( false );
}

void undertype::createMenus()
{
	fileMenu = menuBar()->addMenu ( tr ( "&File" ) );
	fileMenu->addAction ( newAct );
	fileMenu->addAction ( openAct );
	fileMenu->addAction ( saveAct );
	fileMenu->addAction ( saveAsAct );
	fileMenu->addSeparator();
	fileMenu->addAction(closeAct);
	fileMenu->addSeparator();
	fileMenu->addAction ( printAct );
	fileMenu->addSeparator();
	fileMenu->addAction ( exitAct );

	editMenu = menuBar()->addMenu ( tr ( "&Page" ) );
	editMenu->addAction ( pageAct );
	editMenu->addAction ( pdfAct );
// 	editMenu->addAction ( cutAct );
// 	editMenu->addAction ( copyAct );
// 	editMenu->addAction ( pasteAct );

	menuBar()->addSeparator();

	helpMenu = menuBar()->addMenu ( tr ( "&Help" ) );
	helpMenu->addAction ( aboutAct );
// 	helpMenu->addAction ( aboutQtAct );
}

void undertype::createToolBars()
{
	fileToolBar = addToolBar ( tr ( "File" ) );
	fileToolBar->addAction ( newAct );
	fileToolBar->addAction ( openAct );
	fileToolBar->addAction ( saveAct );

	editToolBar = addToolBar ( tr ( "Edit" ) );
	editToolBar->addAction ( cutAct );
	editToolBar->addAction ( copyAct );
	editToolBar->addAction ( pasteAct );
}

void undertype::createStatusBar()
{
	statusBar()->showMessage ( tr ( "Ready" ) );
}

void undertype::readSettings()
{
	QSettings settings ( "Undertype" );
	QPoint pos = settings.value ( "pos", QPoint ( 200, 200 ) ).toPoint();
	QSize size = settings.value ( "size", QSize ( 400, 400 ) ).toSize();
	resize ( size );
	move ( pos );
}

void undertype::writeSettings()
{
	QSettings settings ( "Undertype" );
	settings.setValue ( "pos", pos() );
	settings.setValue ( "size", size() );
}

bool undertype::maybeSave()
{
	if ( maybeFlag)
	{
		int ret = QMessageBox::warning ( this, tr ( "Application" ),
		                                 tr ( "The document has been modified.\n"
		                                      "Do you want to save your changes?" ),
		                                 QMessageBox::Yes | QMessageBox::Default,
		                                 QMessageBox::No,
		                                 QMessageBox::Cancel | QMessageBox::Escape );
		if ( ret == QMessageBox::Yes )
			return save();
		else if ( ret == QMessageBox::Cancel )
			return false;
	}
	return true;
}

void undertype::loadFile ( const QString &fileName )
{
	QDomDocument doc ( "udl" );
	QFile file ( fileName );
	if ( !file.open ( QFile::ReadOnly ) )
	{
		QMessageBox::warning ( this, tr ( "Application" ),
		                       tr ( "Cannot read file %1:\n%2." )
		                       .arg ( fileName )
		                       .arg ( file.errorString() ) );
		return;
	}
	if ( !doc.setContent ( &file ) )
	{
		file.close();
		statusBar()->showMessage ( tr ( "ERROR loading xml for %1" ).arg ( fileName ),3000 );
		return;
	}
	file.close();
	statusBar()->showMessage ( tr ( "loading %1" ).arg ( fileName ) );

	//loading page
	QDomNodeList pagList = doc.elementsByTagName ( "page" );
	if ( pagList.length() == 0 )
	{
		statusBar()->showMessage ( tr ( "ERROR loading page for %1" ).arg ( fileName ),3000 );
		return;
	}
	QDomNode pag = pagList.item ( 0 );
	double pw = pag.attributes().namedItem ( "width" ).toAttr().value().toDouble();
	double ph = pag.attributes().namedItem ( "height" ).toAttr().value().toDouble();
	pages->setPageSize ( pw,ph );
	pageDimLabel->setText ( "page : " + QString().number ( ULUnit ( pw ).mm() ) +" mm x "+QString().number ( ULUnit ( ph ).mm() ) + " mm" );

	//loading casses
	QDomNodeList colList = doc.elementsByTagName ( "collection" );
	if ( colList.length() == 0 )
	{
		statusBar()->showMessage ( tr ( "WARNING: no casse in %1" ).arg ( fileName ),3000 );
// 		return;
	}
	qDebug() << colList.length();
	for ( uint i = 0; i < colList.length(); ++i )
	{
		QDomNode col = colList.item ( i );
		QString fp = col.namedItem ( "fontfile" ).toElement().text();
		QString fn = col.namedItem ( "name" ).toElement().text();
		double fs = QString ( col.namedItem ( "size" ).toElement().text() ).toDouble();
		QMap<uint,QString> indname;
		QDomNode gnode = col.firstChildElement ( "index" );
		
		if(PageBuffer::getInstance()->fontCollection.contains(fn))
			continue;
		
		while ( !gnode.isNull() )
		{
			QDomElement inode = gnode.toElement();
			gnode = gnode.nextSiblingElement();
			QDomElement nnode = gnode.toElement();
			indname.insert ( QString ( inode.text() ).toDouble(), nnode.text() );
			gnode = gnode.nextSiblingElement ( "index" );
		}
		qDebug() << indname.count();

		ULGlyphScene *scene = new ULGlyphScene();
		scene->setBackgroundBrush ( Qt::lightGray );
		scene->setMainScene ( lineScene );
		glyphScene.append ( scene );
		QGraphicsView *view = new QGraphicsView ( scene );
		view->setRenderHints ( QPainter::Antialiasing );
		glyphView.append ( view );
		ULGlyphCollection * coll = new ULGlyphCollection ( scene,fs );
		scene->setCollection ( coll );
		coll->userFeed ( fp, fn, indname );
		casses.append ( coll );
		cassesMap.insert ( fn, coll );
		casseSelect->addItem ( coll->name(), QVariant ( coll->name() ) );
		tabCasses->setCurrentIndex ( tabCasses->addTab ( view, coll->name() ) );

	}

	// loading glyphs
	QDomNodeList grList = doc.elementsByTagName ( "group" );
	for ( uint i = 0; i < grList.length(); ++i )
	{
		QGraphicsItemGroup * g = new QGraphicsItemGroup;
		QDomNode gr = grList.item ( i );
		double gxpos = QString ( gr.attributes().namedItem ( "xpos" ).toAttr().value() ).toDouble();
		double gypos = QString ( gr.attributes().namedItem ( "ypos" ).toAttr().value() ).toDouble();
		double bali = QString ( gr.attributes().namedItem ( "baseline" ).toAttr().value() ).toDouble();
		double lihe = QString ( gr.attributes().namedItem ( "lineheight" ).toAttr().value() ).toDouble();
		double cupo = QString ( gr.attributes().namedItem ( "curpos" ).toAttr().value() ).toDouble();
		int xltr = QString ( gr.attributes().namedItem ( "ltr" ).toAttr().value() ).toInt();
		QList<ULGlyphItem*> glist;
		QDomNode gnode = gr.firstChildElement ( "gitem" );
		while ( !gnode.isNull() )
		{
			double xpos = QString ( gnode.namedItem ( "xpos" ).toElement().text() ).toDouble();
			double ypos = QString ( gnode.namedItem ( "ypos" ).toElement().text() ).toDouble();
// 			qDebug() << gnode.namedItem("key").toElement().text();
            QString from(gnode.namedItem ( "from" ).toElement().text() );
            QString key(gnode.namedItem ( "key" ).toElement().text());
            ULGlyphCollection* gCollection = cassesMap[from];
            ULGlyphScene * cScene = gCollection->scene();

            ULGlyphItem *selected = cScene->keyItem(key);
			ULGlyphItem *item = new ULGlyphItem ();

			item->setPath ( selected->shape() );
			item->setBrush ( QBrush ( Qt::SolidPattern ) );
			item->setData ( 1,"glyph" );
			item->setData ( 2,selected->key );
			item->key = selected->key;
			item->collection = selected->collection;
			item->setPDF ( selected->XObject() );
			item->scale ( selected->size / selected->uniem() ,- ( selected->size ) /selected->uniem() );
			item->m_xadvance = selected->m_xadvance;
			item->textKey = selected->textKey;
			item->showText ( false );
			item->makeActive();
			item->setPos ( xpos,ypos );
			glist.append ( item );
			g->addToGroup ( item );
			gnode = gnode.nextSiblingElement ( "gitem" );
		}
		uint gid = PageBuffer::getInstance()->put ( glist );

		pages->addItem ( g );
		g->setFlags ( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );
		g->setData ( 1,"group" );
		g->setZValue ( 10.0 );
		g->setHandlesChildEvents ( true );
		g->setData ( 2, gid );
		g->setData ( 3, bali );
		g->setData ( 4, lihe );
		g->setData ( 5, cupo );
		g->setData ( 6, xltr );
		g->setPos ( gxpos, gypos );

	}

	QDomNodeList ra = doc.elementsByTagName ( "raster" );
	for ( uint i = 0;i < ra.count(); ++i )
	{
		double xra = QString ( ra.item ( i ).attributes().namedItem ( "xpos" ).toAttr().value() ).toDouble();
		double yra = QString ( ra.item ( i ).attributes().namedItem ( "ypos" ).toAttr().value() ).toDouble();
		double zra = QString ( ra.item ( i ).attributes().namedItem ( "zindex" ).toAttr().value() ).toDouble();
		QString fra = ra.item ( i ).attributes().namedItem ( "file" ).toAttr().value();
		qDebug() << "loading raster " << fra;
		extraScene->load ( fra, xra, yra , zra);
	}

	QDomNodeList svg = doc.elementsByTagName ( "svg" );
	for ( uint i = 0;i < svg.count(); ++i )
	{
		double xra = QString ( svg.item ( i ).attributes().namedItem ( "xpos" ).toAttr().value() ).toDouble();
		double yra = QString ( svg.item ( i ).attributes().namedItem ( "ypos" ).toAttr().value() ).toDouble();
		double zra = QString ( svg.item ( i ).attributes().namedItem ( "zindex" ).toAttr().value() ).toDouble();
		QString fra = svg.item ( i ).attributes().namedItem ( "file" ).toAttr().value();
		qDebug() << "loading svg " << fra;
		extraScene->load ( fra, xra, yra , zra);
	}

	QDomNodeList cu = doc.elementsByTagName ( "cursor" );
	double cux = QString ( cu.item ( 0 ).attributes().namedItem ( "xpos" ).toAttr().value() ).toDouble();
	double cuy = QString ( cu.item ( 0 ).attributes().namedItem ( "ypos" ).toAttr().value() ).toDouble();
// 	qDebug() << cux << " " << cuy;
	pageCursorX->setValue ( cux );
	pageCursorY->setValue ( cuy );

	QDomNodeList lh = doc.elementsByTagName ( "lineh" );
	double slh = QString ( lh.item ( 0 ).toElement().text() ).toDouble();
	QDomNodeList lw = doc.elementsByTagName ( "linew" );
	double slw = QString ( lw.item ( 0 ).toElement().text() ).toDouble();
	lhSpin->setValue ( slh );
	lwSpin->setValue ( slw );

	setCurrentFile ( fileName );
	statusBar()->showMessage ( tr ( "%1 loaded" ).arg ( fileName ), 2000 );
}

bool undertype::saveFile ( const QString &fileName )
{
	QFile file ( fileName );
	if ( !file.open ( QFile::WriteOnly ) )
	{
		QMessageBox::warning ( this, tr ( "Application" ),
		                       tr ( "Cannot write file %1:\n%2." )
		                       .arg ( fileName )
		                       .arg ( file.errorString() ) );
		return false;
	}
	// my own code
	QTextStream ts ( &file );
	ts << "<undertype>\n";
	ts << QString ( "<page width=\"%1\" height=\"%2\"/>\n" ).arg ( pages->width() ).arg ( pages->height() );
	QMap<uint, QPointF> gr = pages->groups();
	QList<uint> k = gr.keys();
	QList<ULGlyphItem*> gl;
	for ( uint i = 0; i < glyphScene.count(); ++i )
	{
		ts << glyphScene[i]->collection()->toElement();
	}
	for ( uint i = 0; i < k.count(); ++i )
	{
		ts << "<group id=\""<< k[i]
		<< "\" xpos=\"" << gr[k[i]].x()
		<< "\" ypos=\"" << gr[k[i]].y()
		<< "\" baseline=\"" << pages->groupById ( k[i] )->data ( 3 ).toDouble()
		<< "\" lineheight=\"" << pages->groupById ( k[i] )->data ( 4 ).toDouble()
		<< "\" curpos=\"" << pages->groupById ( k[i] )->data ( 5 ).toDouble()
		<< "\" ltr=\"" << pages->groupById ( k[i] )->data ( 6 ).toInt()
		<<  "\">\n";
		gl.clear();
		gl = PageBuffer::getInstance()->take ( k[i] );
		for ( uint j=0;j<gl.count();++j )
		{
			ts << gl[j]->toElement();
		}
		ts << "</group>\n";
	}

	QList<QGraphicsItem*> grlist = pages->items();
	for ( uint i = 0; i <grlist.count(); ++i )
	{
		if ( grlist[i]->data ( 1 ).toString() == "raster" )
		{
			QString rf = grlist[i]->data ( 2 ).toString();
			rf.chop ( 10 );
			ts << QString ( "<raster file=\"%1\" xpos=\"%2\" ypos=\"%3\" zindex=\"%4\"/>\n" )
					.arg ( rf )
					.arg ( grlist[i]->scenePos().x() )
					.arg ( grlist[i]->scenePos().y() )
					.arg(grlist[i]->zValue()) ;
		}
		if ( grlist[i]->data ( 1 ).toString() == "svg" )
		{
			QString rf = grlist[i]->data ( 2 ).toString();
			rf.chop ( 10 );
			ts << QString ( "<svg file=\"%1\" xpos=\"%2\" ypos=\"%3\" zindex=\"%4\"/>\n" )
					.arg ( rf )
					.arg ( grlist[i]->scenePos().x() )
					.arg ( grlist[i]->scenePos().y() )
					.arg(grlist[i]->zValue()) ;
		}
	}

	ts << "<cursor xpos=\""<< QString::number ( pageCursorX->value() ) <<"\" ypos=\""<<  QString::number ( pageCursorY->value() ) <<"\"/>";
	ts << "<lineh>" <<  QString::number ( lhSpin->value() ) << "</lineh>";
	ts << "<linew>" << QString::number ( lwSpin->value() ) << "</linew>";
	ts << "\n</undertype>";
	file.close();
	// end of my own code
	setCurrentFile ( fileName );
	statusBar()->showMessage ( tr ( "%1 saved" ).arg ( fileName ), 2000 );
	return true;
}

void undertype::setCurrentFile ( const QString &fileName )
{
	curFile = fileName;
	setWindowModified ( false );

	QString shownName;
	if ( curFile.isEmpty() )
		shownName = "untitled.udl";
	else
		shownName = strippedName ( curFile );

	setWindowTitle ( tr ( "%1[*] - %2" ).arg ( shownName ).arg ( tr ( "UnderType" ) ) );
}

QString undertype::strippedName ( const QString &fullFileName )
{
	return QFileInfo ( fullFileName ).fileName();
}

undertype::~undertype()
{}

void undertype::newCasse()
{
	ULGlyphScene *scene = new ULGlyphScene();
	scene->setBackgroundBrush ( Qt::lightGray );
	scene->setMainScene ( lineScene );
	glyphScene.append ( scene );
	QGraphicsView *view = new QGraphicsView ( scene );
	view->setRenderHints ( QPainter::Antialiasing );
	glyphView.append ( view );
	double size = QInputDialog::getDouble ( this, "Size","Size", 12.0,4.0,200.0,1 );
	ULGlyphCollection * col = new ULGlyphCollection ( scene,size );
	scene->setCollection ( col );
	col->autoFeed();
	casses.append ( col );
	casseSelect->addItem ( col->name(), QVariant ( col->name() ) );
	tabCasses->setCurrentIndex ( tabCasses->addTab ( view, col->name() ) );
}

void undertype::loadCasse()
{
	QStringList cassePath = QFileDialog::getOpenFileNames ( this );
	if ( cassePath.isEmpty() )
	{
		return;
	}
	QString control = "UNDERTYPE_CASSE";
	for ( uint f = 0; f < cassePath.count(); ++f )
	{
		QFile fontfile ( cassePath[f] );
		if ( !fontfile.open ( QIODevice::ReadOnly | QIODevice::Text ) )
			continue;

		QTextStream fontstream ( &fontfile );
		if ( fontstream.readLine() == control )
		{
			QString fname;
			QString fpath;
			double fsize;
			QMap<uint,QString> gmap;

			fpath = fontstream.readLine();
			qDebug ( fpath.toAscii() );
			fsize = fontstream.readLine().toDouble();
			qDebug ( QString::number ( fsize ).toAscii() );
			fname = fontstream.readLine();
			qDebug ( fname.toAscii() );

			while ( !fontstream.atEnd() )
			{
				QString it = fontstream.readLine();
				qDebug ( it.toAscii() );
				QStringList lit = it.split ( ' ' );
				if ( lit.count() == 2 )
					gmap.insert ( lit[0].toUInt(),lit[1] );
			}

			ULGlyphScene *scene = new ULGlyphScene();
			scene->setBackgroundBrush ( Qt::lightGray );
			scene->setMainScene ( lineScene );
			glyphScene.append ( scene );
			QGraphicsView *view = new QGraphicsView ( scene );
			view->setRenderHints ( QPainter::Antialiasing );
			glyphView.append ( view );
			ULGlyphCollection * col = new ULGlyphCollection ( scene,fsize );
			scene->setCollection ( col );
			col->userFeed ( fpath,fname,gmap );
			casses.append ( col );
			casseSelect->addItem ( col->name(), QVariant ( col->name() ) );
			tabCasses->setCurrentIndex ( tabCasses->addTab ( view, col->name() ) );
		}
		fontfile.close();
	}

}

void undertype::print()
{
	QPrinter thePrinter ( QPrinter::HighResolution );
	QPrintDialog printDialog ( &thePrinter, this );

	if ( printDialog.exec() == QDialog::Accepted )
	{
		thePrinter.setFullPage ( true );
		QPainter painter ( &thePrinter );
		painter.setRenderHint ( QPainter::Antialiasing );
		painter.setBackgroundMode ( Qt::TransparentMode );
// 		painter.setViewport(pages->sceneRect().toAlignedRect());
		pages->hideCursor();
		pages->render ( &painter );
		pages->showCursor();
	}
}

void undertype::newLine()
{
	//Setting default line
// // 	double b = QInputDialog::getDouble(this,"line","set line height");
// // 	double w = QInputDialog::getDouble(this,"line","set line width");
	lineScene->init ( 20.0,250.0 );
}

void undertype::editGlyph ( ULGlyphItem * g )
{
	glyphEditor = new QDockWidget ( tr ( "Établi" ), this );
	QGroupBox *frame= new QGroupBox ( this );


	// inputs
	typedef  QPair<QLabel*,QDoubleSpinBox*> InputPair;

	QMap<QString, InputPair > inputs;
	inputs.insert ( "scale", InputPair ( new QLabel ( "Scale",glyphEditor ),new QDoubleSpinBox() ) );
	inputs.insert ( "rotate",InputPair ( new QLabel ( "Rotate",glyphEditor ),new QDoubleSpinBox() ) );
	inputs.insert ( "transH",InputPair ( new QLabel ( "Translate Hor.",glyphEditor ),new QDoubleSpinBox() ) );
	inputs.insert ( "transV",InputPair ( new QLabel ( "Translate Vert.",glyphEditor ),new QDoubleSpinBox() ) );
	inputs.insert ( "advance",InputPair ( new QLabel ( "Advance",glyphEditor ),new QDoubleSpinBox() ) );

	inputs["scale"].second->setRange ( 0.1,10.0 );
	inputs["scale"].second->setSingleStep ( 0.1 );
	inputs["scale"].second->setValue ( 1.0 );

	inputs["rotate"].second->setRange ( -180.0,180.0 );

	inputs["transH"].second->setRange ( -100.0,100.0 );
	inputs["transV"].second->setRange ( -100.0,100.0 );

	inputs["advance"].second->setRange ( -100.0,100.0 );
	inputs["advance"].second->setSingleStep ( 0.1 );
	inputs["advance"].second->setValue ( g->m_xadvance );

	// glyph slot
	ULEditGlyph *gscene = new ULEditGlyph();
	QGraphicsView *gview = new QGraphicsView ( gscene );

	gscene->putGlyph ( lineScene,g );

	gview->scale ( 5.0,5.0 );
	// layout
	QHBoxLayout *vbox = new QHBoxLayout();
	QGridLayout *glay = new QGridLayout();
	QVBoxLayout *v = new QVBoxLayout();
	glay->addWidget ( inputs["scale"].first, 0,0 );
	glay->addWidget ( inputs["scale"].second, 0,1 );
	glay->addWidget ( inputs["rotate"].first, 1,0 );
	glay->addWidget ( inputs["rotate"].second, 1,1 );
	glay->addWidget ( inputs["transH"].first, 2,0 );
	glay->addWidget ( inputs["transH"].second, 2,1 );
	glay->addWidget ( inputs["transV"].first, 3,0 );
	glay->addWidget ( inputs["transV"].second, 3,1 );
	glay->addWidget ( inputs["advance"].first, 4,0 );
	glay->addWidget ( inputs["advance"].second, 4,1 );

	v->addWidget ( gview );
	v->addWidget ( new QLabel ( "The Glyph" ) );
	vbox->addLayout ( v );
	vbox->addLayout ( glay );


	frame->setLayout ( vbox );
	glyphEditor->setWidget ( frame );
	addDockWidget ( Qt::BottomDockWidgetArea, glyphEditor );


	connect ( inputs["scale"].second, SIGNAL ( valueChanged ( double ) ),gscene,SLOT ( slotScale ( double ) ) );
	connect ( inputs["rotate"].second, SIGNAL ( valueChanged ( double ) ),gscene,SLOT ( slotRotate ( double ) ) );
	connect ( inputs["transH"].second, SIGNAL ( valueChanged ( double ) ),gscene,SLOT ( slotTransX ( double ) ) );
	connect ( inputs["transV"].second, SIGNAL ( valueChanged ( double ) ),gscene,SLOT ( slotTransY ( double ) ) );
	connect ( inputs["advance"].second, SIGNAL ( valueChanged ( double ) ),gscene,SLOT ( slotAdvance ( double ) ) );

	connect ( gscene,SIGNAL ( endEdit() ),glyphEditor,SLOT ( close() ) );
}

void undertype::changeLineWidgetValues ( double lh, double w, double cursor )
{
	lhSpin->setValue ( lh );
	lwSpin->setValue ( w );
	gcursor->setValue ( cursor );
}

void undertype::saveCasse()
{
	QString control = "UNDERTYPE_CASSE";
	QString sel = casseSelect->currentText();
	ULGlyphCollection *les;
	for ( uint i = 0; i < glyphScene.count(); ++i )
	{
		if ( glyphScene[i]->collection()->name() == sel )
			les = glyphScene[i]->collection();
	}

// 	QFileDialog dialog(this);
// // 	dialog.setFileMode(QFileDialog::AnyFile);
// // 	dialog.setAcceptMode(QFileDialog::AcceptSave);
	QString filepath = QFileDialog::getSaveFileName();
	QFile file ( filepath );
	if ( !file.open ( QIODevice::WriteOnly | QIODevice::Text ) )
		return;
	QTextStream out ( &file );
	out << control << "\n";
	out << les->fontPath() << "\n";
	out << les->fontSize() << "\n";
	out << les->name() << "\n";
	for ( uint i = 0; i < les->count(); ++i )
	{
		out << ( i + 1 ) << " " << ( *les ) [i]->key << "\n";
	}
	file.close();

}

void undertype::moveGcursorBegin()
{
	gcursor->setValue ( 0.0 );
}

void undertype::moveGcursorEnd()
{
	gcursor->setValue ( lwSpin->value() );
}

void undertype::switchPAA()
{
	mainView->setRenderHint ( QPainter::Antialiasing, !pageAntiAlias );
	pageAntiAlias =  !pageAntiAlias;
}

void undertype::pageSetup()
{
	QDialog *dial = new QDialog ( this );

	QGridLayout *lay = new QGridLayout ( dial );

	QLabel *pw = new QLabel ( "Width" );
	QLabel *ph = new QLabel ( "Height" );

	QDoubleSpinBox *sw = new QDoubleSpinBox();
	sw->setRange ( 1.0,10000.0 );
// 	sw->setSuffix(" pt");
	sw->setValue ( pages->width() );
	QDoubleSpinBox *sh = new QDoubleSpinBox();
	sh->setRange ( 1.0,10000.0 );
// 	sh->setSuffix(" pt");
	sh->setValue ( pages->height() );

	QButtonGroup * unit = new QButtonGroup();
	QRadioButton * pt = new QRadioButton ( "pt" );
	QRadioButton * mm = new QRadioButton ( "mm" );

	unit->addButton ( pt );
	unit->addButton ( mm );
	pt->setChecked ( true );

	QPushButton *ok = new QPushButton ( "OK" );

	lay->addWidget ( pw,0,0 );
	lay->addWidget ( ph,1,0 );
	lay->addWidget ( sw,0,1 );
	lay->addWidget ( sh,1,1 );
	lay->addWidget ( pt,2,0 );
	lay->addWidget ( mm,2,1 );
	lay->addWidget ( ok,3,1 );

	connect ( ok,SIGNAL ( released() ),dial,SLOT ( close() ) );



	dial->exec();

	QString u = unit->checkedButton()->text();


	ULUnit w ( sw->value(),u );
	ULUnit h ( sh->value(),u );
	pages->setPageSize ( w.pt(), h.pt() );
	pageDimLabel->setText ( "page : " + QString().number ( w.mm() ) +" mm x "+QString().number ( h.mm() ) + " mm" );

	delete sw;
	delete sh;
	delete pw;
	delete ph;
	delete lay;
	delete dial;
}

void undertype::exportPDF()
{
	qDebug ( "exporting pdf" );
	QString fileName = QFileDialog::getSaveFileName ( this, tr ( "Export to PDF" ), curFile + ".pdf", tr ( "Portable etc. (*.pdf)" ) );
	QFile file ( fileName );
	if ( !file.open ( QIODevice::WriteOnly ) )
		return;

	QByteArray buffer;
	QDataStream pdf ( &buffer, QIODevice::WriteOnly );



	QString xref;
	xref = "0000000000 65535 f \n";
	QByteArray tmp = QString ( "%PDF-1.3\n" ).toAscii();
	pdf.writeRawData ( tmp,tmp.size() );

	qDebug() << buffer;
	QString xDic;

	QByteArray content;
	QMap<double, QByteArray > stackedcontent;

	uint obj = 1;
	QMap<ULGlyphItem*, uint> objMap;
	PageBuffer *pb = PageBuffer::getInstance();
	uint nit = pb->maxId() + 1;

	double gZindex = 10.0;
	for ( uint i = 1; i <  nit ; ++i )
	{
		QList<ULGlyphItem*> glist = pb->take ( i );
		for ( uint g=0;g<glist.count();++g )
		{
			xref.append ( QString ( "%1 00000 n \n" ).arg ( buffer.size()-1,10,10,QChar ( '0' ) ) );
			QByteArray tmp;
			tmp.append ( QString ( "%1 0 obj\n" ).arg ( obj ).toAscii() );
			tmp.append ( glist[g]->XObject().toAscii() );
			tmp.append ( "endobj\n" );
			pdf.writeRawData ( tmp.constData(), tmp.size() );
			objMap.insert ( glist[g], obj );
			xDic.append ( QString ( "/gl%1 %1 0 R\n" ).arg ( obj ) );
			stackedcontent.insert(gZindex,  QString ( "q\n1 0 0 1 %1 %2 cm\n/gl%3 Do\nQ\n" ).arg ( glist[g]->scenePos().x() ).arg ( pages->height() - glist[g]->scenePos().y() ).arg ( obj ).toAscii() );
			++obj;
			gZindex += 0.0000001;

		}
	}

	QMap<QString, uint> gobjs;
	QMap<QString, QRectF> grect;
	QMap<QString, bool> grast;
	QMap<QString, double> gstack;
	
	QList<QGraphicsItem*> allI = pages->items();
	
	for ( uint i=0; i < allI.count(); ++i )
	{
		if ( allI[i]->data ( 1 ).toString() == "raster" )
		{
			grect.insert ( allI[i]->data ( 2 ).toString(), allI[i]->sceneBoundingRect() );
			grast.insert ( allI[i]->data ( 2 ).toString(), true );
			gstack.insert ( allI[i]->data ( 2 ).toString(), allI[i]->zValue() );
		}
		if ( allI[i]->data ( 1 ).toString() == "svg" )
		{
			grect.insert ( allI[i]->data ( 2 ).toString(), allI[i]->sceneBoundingRect() );
			grast.insert ( allI[i]->data ( 2 ).toString(), false );
			gstack.insert ( allI[i]->data ( 2 ).toString(), allI[i]->zValue() );
		}
	}
	QStringList imgs = grect.keys();
	
	for ( uint i = 0; i < imgs.count(); ++i )
	{

		QString name = imgs[i];
		QString realname = name;
		realname.chop ( 10 );
		if ( !gobjs.contains ( realname ) )
		{
			double zz = gstack[name];
			while ( stackedcontent.contains ( zz ) )
			{
				zz += 0.0000001;
			}
			xref.append ( QString ( "%1 00000 n \n" ).arg ( buffer.size()-1,10,10,QChar ( '0' ) ) );
			bool rast = grast[name];
			gobjs.insert ( realname, obj );
			QByteArray tmp ( QString ( "%1 0 obj\n" ).arg ( obj ).toAscii() );
			tmp.append ( pb->graphics[realname]->XObject() );
			tmp.append ( "endobj\n" );
			pdf.writeRawData ( tmp.constData(), tmp.size() );
			xDic.append ( QString ( "/graphic%1 %1 0 R\n" ).arg ( obj ) );
			stackedcontent.insert ( zz, QString ( "q\n%1 0 0 %2 %3 %4 cm\n/graphic%5 Do\nQ\n" )
			                        .arg ( rast ? grect[name].width() : 1 )
			                        .arg ( rast ? grect[name].height() : 1 )
			                        .arg ( grect[name].x() )
			                        .arg ( rast ? pages->height() - grect[name].bottom() : pages->height() - grect[name].y() )
			                        .arg ( obj )
						.toAscii()
			                      );
			++obj;
		}
		else
		{
			double zz = gstack[name];
			while ( stackedcontent.contains ( zz ) )
			{
				zz += 0.0000001;
			}
			uint tmpobj = gobjs[realname];
			bool rast = grast[name];
			stackedcontent.insert ( zz, QString ( "q\n%1 0 0 %2 %3 %4 cm\n/graphic%5 Do\nQ\n" )
			                        .arg ( rast ? grect[name].width() : 1 )
			                        .arg ( rast ? grect[name].height() : 1 )
			                        .arg ( grect[name].x() )
			                        .arg ( rast ?  pages->height() - grect[name].bottom() : pages->height() - grect[name].y() )
			                        .arg ( tmpobj )
						.toAscii()
			                      );

		}

	}
	
	QMapIterator<double, QByteArray> i(stackedcontent);
	while (i.hasNext()) {
		i.next();
		content.append(i.value());
	}

	xref.append ( QString ( "%1 00000 n \n" ).arg ( buffer.size()-1,10,10,QChar ( '0' ) ) );
	tmp = QString ( "%1 0 obj\n" ).arg ( obj ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size()	);
	uint root = obj;
	++obj;
	tmp = QString ( "<<\n/Type /Catalog\n/Pages %1 0 R\n>>\nendobj\n" ).arg ( obj ).toAscii();
	pdf.writeRawData (	tmp.constData(), tmp.size()	);

	xref.append ( QString ( "%1 00000 n \n" ).arg ( buffer.size()-1,10,10,QChar ( '0' ) ) );
	tmp = QString ( "%1 0 obj\n" ).arg ( obj ).toAscii();
	pdf.writeRawData (	tmp.constData(), tmp.size() );
	++obj;
	tmp = QString ( "<<\n/Type /Pages\n/Kids [%1 0 R]\n/Count 1\n>>\nendobj\n" ).arg ( obj ).toAscii();
	pdf.writeRawData (	tmp.constData(), tmp.size() );

	xref.append ( QString ( "%1 00000 n \n" ).arg ( buffer.size()-1,10,10,QChar ( '0' ) ) );
	tmp = QString ( "%1 0 obj\n" ).arg ( obj ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );
	++obj;
	tmp =QString ( "<<\n/Type /Page\n/Parent %1 0 R\n" ).arg ( obj - 2 ).toAscii() ;
	pdf.writeRawData ( tmp.constData(), tmp.size() );
	tmp =  QString ( "/MediaBox [0 0 %1 %2]\n" ).arg ( pages->width() ).arg ( pages->height() ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );
	tmp =  QString ( "/Contents %1 0 R\n" ).arg ( obj + 1 ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );
	tmp = QString ( "/Resources << /XObject %1 0 R >>\n>>\nendobj\n" ).arg ( obj ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );

	xref.append ( QString ( "%1 00000 n \n" ).arg ( buffer.size()-1,10,10,QChar ( '0' ) ) );
	xDic.prepend ( QString ( "%1 0 obj\n<<\n" ).arg ( obj ) );
	tmp =  QString ( xDic + ">>\nendobj\n" ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );
	++obj;

	uint sizecontent = content.length();
	content.prepend ( QString ( "%2 0 obj\n << /Length %1 >>\nstream\n" ).arg ( sizecontent ).arg ( obj ).toAscii() );
	content.append ( "endstream\nendobj\n" );

	xref.append ( QString ( "%1 00000 n \n" ).arg ( buffer.size()-1,10,10,QChar ( '0' ) ) );
	pdf.writeRawData ( content, content.size() );

	uint xrefref = buffer.size();
	xref.prepend ( QString ( "xref\n0 %1\n" ).arg ( obj + 1 ) );
	tmp = xref.toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );

	tmp=QString ( "trailer\n<< /Size %1 \n" ).arg ( obj + 1 ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );
	tmp= QString ( "/Root %1 0 R>>\nstartxref\n" ).arg ( root ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );
	tmp= QString ( "%1\n%%EOF" ).arg ( xrefref ).toAscii();
	pdf.writeRawData ( tmp.constData(), tmp.size() );

	//control
// 	QStringList xl = xref.split('\n');
// 	for(uint i = 1 ;i < xl.count(); ++i)
// 	{
// 		uint seek = xl[i].left(10).toLongLong();
// 		qDebug() << xl[i] << seek << buffer.mid(seek, 10) ;
// 	}
	file.seek ( 0 );
	QDataStream out ( &file );
	out.writeRawData ( buffer.constData(),buffer.size() );
	file.close();
	statusBar()->showMessage ( QString ( "%1 saved" ).arg ( fileName ),3000 );
}

void undertype::closeDoc()
{
	if(maybeSave())
	{
		PageBuffer::getInstance()->clear();
		pages->clear();
	}
	pages->setCur(40,40);
}

void undertype::switchCMap(bool b)
{
	m_usecmap = b;
}

