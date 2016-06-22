/***************************************************************************
                          projectview.h  -  Projekt docklet
                             -------------------
    begin                : Mon Feb 16 17:53:49 CEST 2004
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
 
#ifndef __PROJECTVIEW_H__
#define __PROJECTVIEW_H__

#include <qlistview.h>
#include <qpopupmenu.h>

class ProjectView;

class ProjectFileItem : public QListViewItem
{
	friend class ProjectView;
public:
	ProjectFileItem( ProjectView *parent, QString file );
private:
	QString fileName;
	ProjectView *view;
};

class ProjectView : public QListView
{
	Q_OBJECT
	friend class ProjectFileItem;
public:
	ProjectView( QWidget *parent, const char *name );
public slots:
	void addFile( QString file );
	void removeFile( QString file );
	void setProjectDir( QString dir ) { projectDir = dir; }
protected slots:
	void slotRemoveFile();
protected:
	void contentsMouseDoubleClickEvent( QMouseEvent *e );
	void contextMenuEvent( QContextMenuEvent *e );
signals:
	void loadFile(QString);
	void removedFile(QString);
private:
	QString projectDir;
	QPopupMenu *contextMenu;
};

#endif
