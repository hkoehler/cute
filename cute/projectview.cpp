/***************************************************************************
                          projectview.cpp  -  Projekt docklet
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

#include <qfileinfo.h>
#include <qdir.h>

#include "project.h"
#include "projectview.h"

ProjectFileItem::ProjectFileItem( ProjectView *parent, QString f )
	: QListViewItem( parent, f )
{
	view = parent;
	if(QFileInfo(f).isRelative())
		fileName = f;
	else
		fileName = QFileInfo(f).absFilePath().remove(view->projectDir+QDir::separator());
	setText(0, fileName);
}

ProjectView::ProjectView( QWidget *parent, const char *name )
	: QListView(parent, name)
{
	addColumn("File");
	contextMenu = new QPopupMenu(this);
	contextMenu->insertItem( "Remove from Project", this, SLOT(slotRemoveFile()) );
}

void ProjectView::addFile( QString file )
{
	QListViewItem *item = new ProjectFileItem(this, file);
	insertItem(item);
}

void ProjectView::removeFile( QString name )
{
	QListViewItem *i = findItem( name, 0, Qt::ExactMatch );
	takeItem(i);
}

void ProjectView::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
	if(e->button() == LeftButton){
		QPoint p( contentsToViewport( e->pos() ) );
		QListViewItem *i = itemAt( p );
		if(i){
			emit loadFile(static_cast<ProjectFileItem*>(i)->fileName);
		}
	}
	QListView::contentsMousePressEvent(e);
}

void ProjectView::contextMenuEvent( QContextMenuEvent *e )
{
	contextMenu->popup(e->globalPos());
	e->accept();
}

void ProjectView::slotRemoveFile()
{
	if( !currentItem() )
		return;
	QString fileName = static_cast<ProjectFileItem*>(currentItem())->fileName;
	qDebug("slotRemoveFile: "+fileName);
	takeItem(currentItem());
	emit removedFile(fileName);
}
