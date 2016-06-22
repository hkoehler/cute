/***************************************************************************
                          dirview.cpp  -  Directory View
                             -------------------
    begin                : Sam, 15 Feb 2003
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

#include "dirview.h"

#include <stdio.h>

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qpixmap.h>
#include <qevent.h>
#include <qpoint.h>
#include <qmessagebox.h>
#include <qdragobject.h>
#include <qmime.h>
#include <qstrlist.h>
#include <qstringlist.h>
#include <qapplication.h>
#include <qheader.h>

static const char* folder_closed_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "a c #cccccc",
    "c c #ffffff",
    "................",
    "................",
    "..*****.........",
    ".*ababa*........",
    "*abababa******..",
    "*cccccccccccc*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "*cabababababa*d.",
    "*cbababababab*d.",
    "**************d.",
    ".dddddddddddddd.",
    "................"};

static const char* folder_open_xpm[]={
    "16 16 6 1",
    ". c None",
    "b c #ffff00",
    "d c #000000",
    "* c #999999",
    "c c #cccccc",
    "a c #ffffff",
    "................",
    "................",
    "...*****........",
    "..*aaaaa*.......",
    ".*abcbcba******.",
    ".*acbcbcaaaaaa*d",
    ".*abcbcbcbcbcb*d",
    "*************b*d",
    "*aaaaaaaaaa**c*d",
    "*abcbcbcbcbbd**d",
    ".*abcbcbcbcbcd*d",
    ".*acbcbcbcbcbd*d",
    "..*acbcbcbcbb*dd",
    "..*************d",
    "...ddddddddddddd",
    "................"};

static const char * pix_file []={
    "16 16 5 1",
    ". c #7f7f7f",
    "# c None",
    "c c #000000",
    "b c #bfbfbf",
    "a c #ffffff",
    "################",
    "..........######",
    ".aaaaaaaab.#####",
    ".aaaaaaaaba.####",
    ".aaaaaaaacccc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".aaaaaaaaaabc###",
    ".bbbbbbbbbbbc###",
    "ccccccccccccc###"};

QPixmap *folderClosed = 0;
QPixmap *folderOpen = 0;
QPixmap *fileNormal = 0;

/*****************************************************************************
 *
 * Class Directory
 *
 *****************************************************************************/

Directory::Directory( Directory * parent, const QString& filename )
	: QListViewItem( parent ), f(filename),
	  showDirsOnly( parent->showDirsOnly ),
	  pix( 0 )
{
	p = parent;
	readable = QDir( fullName() ).isReadable();
	setPixmap( folderClosed );
}

Directory::Directory( QListView * parent, const QString& filename )
	: QListViewItem( parent ), f(filename),
	  showDirsOnly( ( (DirectoryView*)parent )->showDirsOnly() ),
	  pix( 0 )
{
	p = 0;
	readable = QDir( fullName() ).isReadable();
}

void Directory::setPixmap( QPixmap *px )
{
	pix = px;
	setup();
	widthChanged( 0 );
	invalidateHeight();
	repaint();
}

const QPixmap *Directory::pixmap( int i ) const
{
	if ( i )
	return 0;
	return pix;
}

void Directory::setOpen( bool o )
{
	if ( o )
		setPixmap( folderOpen );
	else
		setPixmap( folderClosed );

	if ( o && !childCount() ) {
		QString s( fullName() );
		QDir thisDir( s );
		if ( !thisDir.isReadable() ) {
			readable = FALSE;
			setExpandable( FALSE );
			return;
		}

		listView()->setUpdatesEnabled( FALSE );
		const QFileInfoList * files = thisDir.entryInfoList();
		if ( files ) {
			QFileInfoListIterator it( *files );
			QFileInfo * fi;
			it.toLast();
			while( (fi=it.current()) != 0 ) {
				--it;
				if ( fi->fileName() == "." || fi->fileName() == ".." )
					; // nothing
				else if ( fi->isDir() )
					; // nothing
				else if ( !showDirsOnly ) {
					FileItem *item = new FileItem( this, fi->fileName(),
								 fi->isFile()?"File":"Special" );
					item->setPixmap( fileNormal );
				}
			}
			it = QFileInfoListIterator( *files );
			it.toLast();
			while( (fi=it.current()) != 0 ) {
				--it;
				if ( fi->fileName() == "." || fi->fileName() == ".." )
					; // nothing
				else if ( fi->isDir() )
					(void)new Directory( this, fi->fileName() );
			}
		}
		listView()->setUpdatesEnabled( TRUE );
	}
	QListViewItem::setOpen( o );
}

void Directory::setup()
{
	setExpandable( TRUE );
	QListViewItem::setup();
}

QString Directory::fullName()
{
	QString s;
	if ( p ) {
		s = p->fullName();
		s.append( f.name() );
		s.append( "/" );
	} else {
		s = f.name();
	}
	return s;
}

QString Directory::text( int column ) const
{
	if ( column == 0 )
		return f.name();
	else if ( readable )
		return "Directory";
	else
		return "Unreadable Directory";
}

/*****************************************************************************
 *
 * Class DirectoryView
 *
 *****************************************************************************/

DirectoryView::DirectoryView( QWidget *parent, const char*name, QString topDir, bool sdo )
	: QListView( parent, name ), dirsOnly( sdo ), oldCurrent( 0 ),
	  dropItem( 0 ), mousePressed( FALSE )
{
	autoopen_timer = new QTimer( this );
	folderClosed = new QPixmap( folder_closed_xpm );
	folderOpen = new QPixmap( folder_open_xpm );
	fileNormal = new QPixmap( pix_file );
	
	addColumn( "Name" );
	setTreeStepSize( 20 );
	Directory * root = new Directory( this, topDir );
	setSorting(-1);
	root->setOpen( true );
	
	connect( this, SIGNAL( doubleClicked( QListViewItem * ) ),
		 this, SLOT( slotFolderSelected( QListViewItem * ) ) );
	connect( this, SIGNAL( returnPressed( QListViewItem * ) ),
		 this, SLOT( slotFolderSelected( QListViewItem * ) ) );
	
	//setAcceptDrops( TRUE );
	//viewport()->setAcceptDrops( TRUE );
	
	connect( autoopen_timer, SIGNAL( timeout() ),
		 this, SLOT( openFolder() ) );
}

void DirectoryView::slotFolderSelected( QListViewItem *i )
{
    if ( !i || !showDirsOnly() )
		return;

    Directory *dir = (Directory*)i;
    emit folderSelected( dir->fullName() );
}

void DirectoryView::openFolder()
{
    autoopen_timer->stop();
    if ( dropItem && !dropItem->isOpen() ) {
		dropItem->setOpen( TRUE );
		dropItem->repaint();
    }
}

static const int autoopenTime = 750;

QString DirectoryView::fullPath(QListViewItem* item)
{
    QString fullpath = item->text(0);
    while ( (item=item->parent()) ) {
		if ( item->parent() )
			fullpath = item->text(0) + "/" + fullpath;
		else
			fullpath = item->text(0) + fullpath;
	}
    return fullpath;
}

void DirectoryView::contentsMousePressEvent( QMouseEvent* e )
{
    QListView::contentsMousePressEvent(e);
    QPoint p( contentsToViewport( e->pos() ) );
    QListViewItem *i = itemAt( p );
    if ( i ) {
		// if the user clicked into the root decoration of the item, don't try to start a drag!
		if ( p.x() > header()->cellPos( header()->mapToActual( 0 ) ) +
			treeStepSize() * ( i->depth() + ( rootIsDecorated() ? 1 : 0) ) + itemMargin() ||
			p.x() < header()->cellPos( header()->mapToActual( 0 ) ) ) {
			presspos = e->pos();
			mousePressed = TRUE;
		}
    }
	e->accept();
}

void DirectoryView::contentsMouseReleaseEvent( QMouseEvent * )
{
	mousePressed = FALSE;
}

void DirectoryView::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
	QListViewItem *item = itemAt( contentsToViewport(e->pos()) );
	QListView::contentsMouseDoubleClickEvent( e );
	if(item){
		QString source = fullPath(item);
		FILE *f;
		if( !item->isExpandable() )
			emit fileSelected( source );
	}
}

void DirectoryView::setDir( const QString &s )
{
	QListViewItemIterator it( this );
	++it;
	for ( ; it.current(); ++it ) {
		it.current()->setOpen( FALSE );
	}
	
	QStringList lst( QStringList::split( "/", s ) );
	QListViewItem *item = firstChild();
	QStringList::Iterator it2 = lst.begin();
	for ( ; it2 != lst.end(); ++it2 ) {
		while ( item ) {
			if ( item->text( 0 ) == *it2 ) {
			item->setOpen( TRUE );
			break;
			}
			item = item->itemBelow();
		}
	}
	
	if ( item )
	setCurrentItem( item );
}

void FileItem::setPixmap( QPixmap *p )
{
	pix = p;
	setup();
	widthChanged( 0 );
	invalidateHeight();
	repaint();
}

const QPixmap *FileItem::pixmap( int i ) const
{
	if ( i )
		return 0;
	return pix;
}
