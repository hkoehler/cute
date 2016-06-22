/***************************************************************************
                          bookmark.cpp  -  bookmarks
                             -------------------
    begin                : Mon Feb 23 22:53:49 CEST 2004
    copyright            : (C) 2004 by Heiko Köhler
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
 
#include <qregexp.h>
#include <qfileinfo.h>
 
#include "bookmark.h"
#include "cute.h"
#include "cuteview.h"

BookmarkManager bookmarkManager;

ScintillaBookmark *ProjectBookmark::toScintillaBookmark()
{
	CUTEView *view;
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	WidgetList list = cute->windows();

	for(QWidget *iter = list.first(); iter; iter = list.next()){
		view = static_cast<CUTEView*>(iter);
		if( QFileInfo(view->fileName()).absFilePath() == QFileInfo(__file).absFilePath() ){
			return new ScintillaBookmark(name(), view, line());
		}
	}
	return 0L;
}

ScintillaBookmark::ScintillaBookmark( QString name, CUTEView *view, int i, bool isHandle )
{
	 __view = view; 
	__name = name;
	if(isHandle)
		handle = i;
	else
		handle = __view->addBookmark(i);
}

int ScintillaBookmark::line()
{
	return __view->scintilla()->markerLine(handle);
}

QString ScintillaBookmark::file()
{
	return __view->fileName();
}

QString ScintillaBookmark::name()
{
	if(__name.isEmpty()){
		__name = __view->line(line());
		__name = __name.replace(QRegExp("\\s"), " ").stripWhiteSpace();
	}
	return __name;
}

ProjectBookmark *ScintillaBookmark::toProjectBookmark()
{
	return new ProjectBookmark(name(), __view->fileName(), line());
}

BookmarkManager::BookmarkManager()
{
}

void BookmarkManager::createMenu()
{
	bookmarksMenu = new QPopupMenu();
}

/** retrieves all bookmarks */
QPtrList<Bookmark> BookmarkManager::bookmarks()
{
	QPtrList<Bookmark> bms;
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view;

	// add scintilla bookmarks
	WidgetList list = cute->windows();
	for(QWidget *iter = list.first(); iter; iter = list.next()){
		view = static_cast<CUTEView*>(iter);
		QValueList<int> handles = view->bookmarks();
		for( QValueList<int>::iterator i = handles.begin(); i != handles.end(); ++i )
			bms.append( new ScintillaBookmark( "", view, *i, true) );
	}
	
	// add project bookmarks
	for( ProjectBookmark *p = projectBookmarks.first(); p; p = projectBookmarks.next() )
		bms.append(p);

	return bms;
}

/** adds a bookmark of a project */
void BookmarkManager::addBookmark( ProjectBookmark *bm )
{
	projectBookmarks.append(bm);
}

/** removes a bookmark of a project */
void BookmarkManager::removeBookmark( Bookmark *bm )
{
	if( dynamic_cast<ProjectBookmark*>(bm) ){
		projectBookmarks.remove((ProjectBookmark*)bm);
	}
}

/** maps a bookmark menu ID to a bookmark to jump to it */
Bookmark *BookmarkManager::menuBookmark( int id )
{
	return idBookmarkMap[id];
}

void BookmarkManager::updateMenu()
{
	Bookmark *bm;
	int i = 0;
	QPtrList<Bookmark> bms = bookmarks();
	
	bookmarksMenu->clear();
	for( bm = bms.first(); bm; bm = bms.next()) {
		QString fileName = QFileInfo(bm->file()).fileName();
		int id = bookmarksMenu->insertItem( fileName + ":" + 
		QString("%1").arg(bm->line()) + " : " + bm->name() );
		idBookmarkMap.insert(id, bm);
		i++;
	}
}

void BookmarkManager::viewClosed( CUTEView *view )
{
	QValueList<int> handles = view->bookmarks();

	for( QValueList<int>::iterator i = handles.begin(); i != handles.end(); ++i ){
		ScintillaBookmark *sciBm = new ScintillaBookmark( "", view, *i, true);
		projectBookmarks.append( sciBm->toProjectBookmark() );
		delete sciBm;
	}
}

void BookmarkManager::setBookmarks( CUTEView *view )
{
	QPtrList<ProjectBookmark> del;

	for( ProjectBookmark *p = projectBookmarks.first(); p; p = projectBookmarks.next() ){
		if(p->toScintillaBookmark())
			del.append(p);
	}
	for( ProjectBookmark *p = del.first(); p; p = del.next() )
		projectBookmarks.remove(p);
}
