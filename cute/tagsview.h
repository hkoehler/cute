/***************************************************************************
                          tagsview.h  -  view entries of a tags file
                             -------------------
    begin                : Tue Sep 16 12:23:49 CEST 2003
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

#ifndef __TAGSVIEW_H__
#define __TAGSVIEW_H__

#include "readtags.h"
#include <qlistview.h>
#include <qpopupmenu.h>

// forward declaration for friend declaration in TagItem
class TagsView;

/** contains all tags items with same kind */
class KindItem : public QListViewItem
{
public:
	KindItem( QString name, QListView *parent );
	QString className() { return "KindItem"; }
};

/** represents a tag file entry */
class TagItem : public QListViewItem
{
	friend class TagsView;
public:
	TagItem( QString name, QString file, QString pattern, KindItem *parent );
	QString className() { return "TagItem"; }
private:
	QString name;
	QString file;
	QString pattern;
};

class TagsView : public QListView
{
	Q_OBJECT
public:
	TagsView(QWidget *parent=0, const char *name=0);
	/** builds tree with given tag file */
	void readTagsFile(tagFile *tags);
signals:
	/* * emitted when jumping to tag in source is requested */
	void jump(QString fileName, QString pattern);
	/** emitted when tags file should be re/loaded */
	void buildTagsFile();
protected slots:
	void slotBuildTagsFile();
protected:
	/** jump to tag of pressed tag item */
	void contentsMousePressEvent( QMouseEvent *e );
	/** show context menu */
	void contextMenuEvent( QContextMenuEvent *e );
private:
	/** the context menu */
	QPopupMenu *contextMenu;
};

#endif
