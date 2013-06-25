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


#ifndef UNDERLINE_H
#define UNDERLINE_H

#include <QMainWindow>
#include <QCloseEvent>

class QAction;
class QMenu;
class QTextEdit;
class QTabWidget;
class QPushButton;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QDockWidget;
class QDoubleSpinBox;
class QComboBox;
class QLabel;
class QCheckBox;

class ULPageItem;
class QGraphicsScene;
class QGraphicsView;
class ULGlyphCollection;
class ULGlyphScene;
class ULLineEditor;
class ULGlyphItem;
class ULPageView;
class ULExtraGraphicScene;

class undertype:public QMainWindow
{
		Q_OBJECT

	public:
		undertype();
		~undertype();
		
		static undertype* theMainWindow;
		static undertype* getMainWindow(){return theMainWindow;};

	protected:
		void closeEvent ( QCloseEvent *event );
		
	public slots:
		void editGlyph(ULGlyphItem * g);

	private slots:
		void newFile();
		void open();
		bool save();
		bool saveAs();
		void about();
		void documentWasModified();
		void exportPDF();
		void closeDoc();
		
		void newLine();
		void print();
		void pageSetup();
		
		void changeLineWidgetValues(double lh, double w, double cursor);
		void newCasse();
		void saveCasse();
		void loadCasse();
		
		void moveGcursorBegin();
		void moveGcursorEnd();
		
		void switchPAA();
		void switchCMap(bool b);
	private:
		void createActions();
		void createMenus();
		void createToolBars();
		void createStatusBar();
		void readSettings();
		void writeSettings();
		bool maybeSave();
		void loadFile ( const QString &fileName );
		bool saveFile ( const QString &fileName );
		void setCurrentFile ( const QString &fileName );
		QString strippedName ( const QString &fullFileName );
		
		bool pageAntiAlias;



		QString curFile;

		bool m_usecmap;


		QMenu *fileMenu;
		QMenu *editMenu;
		QMenu *helpMenu;
		
        QToolBar *fileToolBar;
		
		QAction *newAct;
		QAction *openAct;
		QAction *saveAct;
		QAction *saveAsAct;
		QAction *closeAct;
		QAction *printAct;
		QAction *exitAct;
		
		QAction *pageAct;
		QAction *pdfAct;

		
		QAction *aboutAct;
		QAction *aboutQtAct;
		

		ULPageItem *pages;
		ULPageView *mainView;
		QGroupBox *pageWidget;
		QHBoxLayout *pageControls;
		QVBoxLayout *pageLayout;
		QDoubleSpinBox *pageCursorX;
		QDoubleSpinBox *pageCursorY;
		QPushButton *pageAA;

		QDockWidget *glyphPalette;
		QTabWidget *tabCasses;
		QPushButton *newCasseBut;
		QPushButton *loadCasseBut;
		QPushButton *saveCasseBut;
		QComboBox *casseSelect;
		QGroupBox *casseBox;
		QGridLayout *casseManagerLayout;
		QList<ULGlyphScene*> glyphScene;
		QList<QGraphicsView*> glyphView;
		QList<ULGlyphCollection*> casses;
		QMap<QString, ULGlyphCollection*> cassesMap;
		QCheckBox *useCMapCheck;
		
		QDockWidget *lineEditor;
		ULLineEditor *lineScene;
		QGraphicsView *lineView;
		QGroupBox *lineWidget;
		QHBoxLayout *lineLayout;
		QGridLayout *lineControls;
		QDoubleSpinBox *lhSpin;
		QDoubleSpinBox *lwSpin;
		QDoubleSpinBox *gcursor;
		QPushButton *gcursorBegin;
		QPushButton *gcursorEnd;
		QCheckBox *gcursorLTR;
		
		
		QDockWidget *glyphEditor;
		
		QLabel *pageDimLabel;
		
		
		QGraphicsView *extraView;
		ULExtraGraphicScene *extraScene;
		
		bool maybeFlag;
	public:
		QLabel *pageRatioLabel;
		QLabel *currentLineLabel;
		bool cMap(){return m_usecmap;};
};

#endif
