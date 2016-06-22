/***************************************************************************
                          bookmark.h  -  bookmarks
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
 
#ifndef __BOOKMARK_H__
#define __BOOKMARK_H__

#include <qptrlist.h>
#include <qmap.h>
#include <qpopupmenu.h>

class CUTE;
class CUTEView;
class ScintillaBookmark;

/** base class for project and scintilla bookmark */
class Bookmark
{
public:
	virtual int line() = 0;
	virtual QString file() = 0;
	virtual QString name() = 0; 
protected:
	QString __name;
};

/** represents a project bookmark, this bookmark is not in a loaded file */
class ProjectBookmark : public Bookmark
{
public:
	ProjectBookmark( QString name, QString file, int line ) :  __line(line) 
		{ __name = name; __file = file; }
	int line() { return __line; }
	QString file() { return __file; }
	virtual QString name() { return __name; }
	ScintillaBookmark *toScintillaBookmark();
private:
	int __line;
	QString __file;
};

/** represents a scintilla bookmark, this bookmark is in a scintilla view */
class ScintillaBookmark : public Bookmark
{
public:
	ScintillaBookmark( QString name, CUTEView *view, int i, bool isHandle=false ); 
	int line();
	QString file();
	QString name();
	ProjectBookmark* toProjectBookmark();
private:
	CUTEView *__view;
	int handle;
};

/** manages all bookmarks */
class BookmarkManager : public QObject
{
	Q_OBJECT
public:
	BookmarkManager();
	/** construct bookmark menu */
	void createMenu();
	/** retrieves all bookmarks */
	QPtrList<Bookmark> bookmarks();
	/** adds a bookmark of a project */
	void addBookmark( ProjectBookmark *bm );
	/** removes a bookmark of a project */
	void removeBookmark( Bookmark *bm );
	/** maps a bookmark menu ID to a bookmark to jump to it */
	Bookmark *menuBookmark( int id );
	/** updates all entries */
	void updateMenu();
	/** bookmark menu */
	QPopupMenu *bookmarksMenu;
	/** converts all project bookmarks to scintilla bookmarks of loaded file */
	void setBookmarks(CUTEView*);
public slots:
	/** converts all scintilla bookmarks of closed view to project bookmarks */
	void viewClosed(CUTEView *);
private:
	/** maps all bookmarks menu ID to its corresponding bookmark */
	QMap <int, Bookmark*> idBookmarkMap;
	/** project bookmarks */
	QPtrList<ProjectBookmark> projectBookmarks;
	/** main window with child scintilla views */
	CUTE *cute;
};

extern BookmarkManager bookmarkManager;

#endif
