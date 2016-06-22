/***************************************************************************
                          helpviewer.cpp  -  HTML view for help
                             -------------------
    begin                : Fri Sep 05 23:53:49 CEST 2003
    copyright            : (C) 2003 by Heiko Köhler
    email                : heicom@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "helpviewer.h"
#include <qstatusbar.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstylesheet.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobjectlist.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <qsimplerichtext.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

#include "icons/left.xpm"
#include "icons/right.xpm"
#include "icons/gohome.xpm"

#include <ctype.h>

HelpViewer::HelpViewer( const QString& home_, const QString& _path,
			QWidget* parent, const char *name )
	: QMainWindow( parent, name, WDestructiveClose ),
	  pathCombo( 0 )
{
	readHistory();
	//readBookmarks();  caused crash 
	
	browser = new QTextBrowser( this );
	
	browser->mimeSourceFactory()->setFilePath( _path );
	browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
	connect( browser, SIGNAL( sourceChanged(const QString& ) ),
		 this, SLOT( sourceChanged( const QString&) ) );

	setCentralWidget( browser );

	if ( !home_.isEmpty() )
		browser->setSource( home_ );

	connect( browser, SIGNAL( highlighted( const QString&) ),
		statusBar(), SLOT( message( const QString&)) );

	resize( 800, 600 );

	QPopupMenu* file = new QPopupMenu( this );
	file->insertItem( tr("&New Window"), this, SLOT( newWindow() ), CTRL+Key_N );
	file->insertItem( tr("&Open File"), this, SLOT( openFile() ), CTRL+Key_O );
	file->insertItem( tr("&Print"), this, SLOT( print() ), CTRL+Key_P );
	file->insertSeparator();
	file->insertItem( tr("&Close"), this, SLOT( close() ), CTRL+Key_Q );

	// The same three icons are used twice each.
	QIconSet icon_back( QPixmap("back.xpm") );
	QIconSet icon_forward( QPixmap("forward.xpm") );
	QIconSet icon_home( QPixmap("home.xpm") );

	QPopupMenu* go = new QPopupMenu( this );
	backwardId = go->insertItem( icon_back,
				 tr("&Backward"), browser, SLOT( backward() ),
				 CTRL+Key_Left );
	forwardId = go->insertItem( icon_forward,
				tr("&Forward"), browser, SLOT( forward() ),
				CTRL+Key_Right );
	go->insertItem( icon_home, tr("&Home"), browser, SLOT( home() ) );

	hist = new QPopupMenu( this );
	QStringList::Iterator it = history.begin();
	for ( ; it != history.end(); ++it )
	mHistory[ hist->insertItem( *it ) ] = *it;
	connect( hist, SIGNAL( activated( int ) ),
		this, SLOT( histChosen( int ) ) );

	bookm = new QPopupMenu( this );
	bookm->insertItem( tr( "Add Bookmark" ), this, SLOT( addBookmark() ) );
	bookm->insertSeparator();

	QStringList::Iterator it2 = bookmarks.begin();
	for ( ; it2 != bookmarks.end(); ++it2 )
		mBookmarks[ bookm->insertItem( *it2 ) ] = *it2;
	connect( bookm, SIGNAL( activated( int ) ),
		this, SLOT( bookmChosen( int ) ) );

	menuBar()->insertItem( tr("&File"), file );
	menuBar()->insertItem( tr("&Go"), go );
	menuBar()->insertItem( tr( "History" ), hist );
	menuBar()->insertItem( tr( "Bookmarks" ), bookm );

	menuBar()->setItemEnabled( forwardId, FALSE);
	menuBar()->setItemEnabled( backwardId, FALSE);
	connect( browser, SIGNAL( backwardAvailable( bool ) ),
		this, SLOT( setBackwardAvailable( bool ) ) );
	connect( browser, SIGNAL( forwardAvailable( bool ) ),
		this, SLOT( setForwardAvailable( bool ) ) );


	QToolBar* toolbar = new QToolBar( this );
	addToolBar( toolbar, "Toolbar");
	QToolButton* button;

	button = new QToolButton( QIconSet(QPixmap((const char**)left_xpm)), tr("Backward"), "", browser, 
		SLOT(backward()), toolbar );
	connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
	button->setEnabled( FALSE );
	button = new QToolButton( QIconSet(QPixmap((const char**)right_xpm)), tr("Forward"), "", browser, 
		SLOT(forward()), toolbar );
	connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
	button->setEnabled( FALSE );
	button = new QToolButton( QIconSet(QPixmap((const char**)gohome)), tr("Home"), "", browser, 
		SLOT(home()), toolbar );

	toolbar->addSeparator();

	pathCombo = new QComboBox( TRUE, toolbar );
	connect( pathCombo, SIGNAL( activated( const QString & ) ),
		this, SLOT( pathSelected( const QString & ) ) );
	toolbar->setStretchableWidget( pathCombo );
	setRightJustification( TRUE );
	setDockEnabled( DockLeft, FALSE );
	setDockEnabled( DockRight, FALSE );

	pathCombo->insertItem( home_ );
	browser->setFocus();
}


void HelpViewer::setBackwardAvailable( bool b)
{
	menuBar()->setItemEnabled( backwardId, b);
}

void HelpViewer::setForwardAvailable( bool b)
{
	menuBar()->setItemEnabled( forwardId, b);
}

void HelpViewer::sourceChanged( const QString& url )
{
	if ( browser->documentTitle().isNull() )
		setCaption( "CUTE Help - " + url.stripWhiteSpace() );
	else
		setCaption( "CUTE Help - " + browser->documentTitle().stripWhiteSpace() );

	if ( !url.isEmpty() && pathCombo ) {
		bool exists = FALSE;
		int i;
		for ( i = 0; i < pathCombo->count(); ++i ) {
			if ( pathCombo->text( i ) == url ) {
				exists = TRUE;
				break;
			}
		}
		if ( !exists ) {
			pathCombo->insertItem( url, 0 );
			pathCombo->setCurrentItem( 0 );
			mHistory[ hist->insertItem( url ) ] = url;
		}else
			pathCombo->setCurrentItem( i );
	}
}

HelpViewer::~HelpViewer()
{
	history =  mHistory.values();

	QFile f( QDir::currentDirPath() + "/.history" );
	f.open( IO_WriteOnly );
	QDataStream s( &f );
	s << history;
	f.close();

	bookmarks = mBookmarks.values();

	QFile f2( QDir::currentDirPath() + "/.bookmarks" );
	f2.open( IO_WriteOnly );
	QDataStream s2( &f2 );
	s2 << bookmarks;
	f2.close();
}

void HelpViewer::openFile()
{
#ifndef QT_NO_FILEDIALOG
	QString fn = QFileDialog::getOpenFileName( QString::null, QString::null, this );
	if ( !fn.isEmpty() )
		browser->setSource( fn );
#endif
}

void HelpViewer::newWindow()
{
	( new HelpViewer(browser->source(), "qbrowser") )->show();
}

void HelpViewer::print()
{
#ifndef QT_NO_PRINTER
	QPrinter printer;
	printer.setFullPage(TRUE);
	if ( printer.setup( this ) ) {
		QPainter p( &printer );
		QPaintDeviceMetrics metrics(p.device());
		int dpix = metrics.logicalDpiX();
		int dpiy = metrics.logicalDpiY();
		const int margin = 72; // pt
		QRect body(margin*dpix/72, margin*dpiy/72,
		metrics.width()-margin*dpix/72*2,
		metrics.height()-margin*dpiy/72*2 );
		QSimpleRichText richText( browser->text(), QFont(), browser->context(), browser->styleSheet(),
				browser->mimeSourceFactory(), body.height() );
		richText.setWidth( &p, body.width() );
		QRect view( body );
		int page = 1;
		do {
			richText.draw( &p, body.left(), body.top(), view, colorGroup() );
			view.moveBy( 0, body.height() );
			p.translate( 0 , -body.height() );
			p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
			if ( view.top()  >= richText.height() )
				break;
			printer.newPage();
			page++;
		} while (TRUE);
	}
#endif
}

void HelpViewer::pathSelected( const QString &_path )
{
	browser->setSource( _path );
	if ( mHistory.values().contains(_path) )
		mHistory[ hist->insertItem( _path ) ] = _path;
}

void HelpViewer::readHistory()
{
	if ( QFile::exists( QDir::currentDirPath() + "/.history" ) ) {
		QFile f( QDir::currentDirPath() + "/.history" );
		f.open( IO_ReadOnly );
		QDataStream s( &f );
		s >> history;
		f.close();
		while ( history.count() > 20 )
			history.remove( history.begin() );
	}
}

void HelpViewer::readBookmarks()
{
	if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) {
		QFile f( QDir::currentDirPath() + "/.bookmarks" );
		f.open( IO_ReadOnly );
		QDataStream s( &f );
		s >> bookmarks;
		f.close();
	}
}

void HelpViewer::histChosen( int i )
{
	if ( mHistory.contains( i ) )
		browser->setSource( mHistory[ i ] );
}

void HelpViewer::bookmChosen( int i )
{
	if ( mBookmarks.contains( i ) )
		browser->setSource( mBookmarks[ i ] );
}

void HelpViewer::addBookmark()
{
	mBookmarks[ bookm->insertItem( caption() ) ] = browser->context();
}
