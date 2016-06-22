/***************************************************************************
                          tagsview.cpp  -  view entries of a tags file
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

#include <qmessagebox.h>
#include <qstringlist.h>
#include <qmap.h>
#include "tagsview.h"
 
KindItem::KindItem( QString name, QListView *parent )
	: QListViewItem( parent, name )
{
	setText(0, name);
	setExpandable(true);
}

TagItem::TagItem( QString name, QString file, QString pattern, KindItem *parent )
	: QListViewItem( parent, name )
{
	setText(0, name);
	setText(1, file);
	setExpandable(false);
	this->file = file;
	this->pattern = pattern;
}

TagsView::TagsView(QWidget *parent, const char *name)
	: QListView(parent, name)
{
	addColumn("Name");
	addColumn("File");
	setRootIsDecorated(true);
	
	contextMenu = new QPopupMenu();
	contextMenu->insertItem("Build Tags File", this, SLOT(slotBuildTagsFile()));
}

void TagsView::readTagsFile(tagFile *tags)
{
	QMap<QString, KindItem*> tagKinds;
	tagEntry entry;
	//TagItem *classMember;
	KindItem *classKindItem=0;

	if(!tags){
		QMessageBox::warning(this, "CUTE", "Could not open tags file");
		return;
	}
	if( tagsFirst(tags, &entry) == TagFailure )
		return;
	clear();
	do{
		if( !tagKinds.contains(entry.kind) ){
			KindItem *kind_item = new KindItem(entry.kind, this);
			if( !strcmp(entry.kind, "class") )
				classKindItem = kind_item;
			tagKinds.insert(entry.kind, kind_item);
		}
//		if( !strcmp(entry.kind, "function") ){
//			QString className = tagsField (&entry, "class");
//			if(!classKindItem){
//				classKindItem = new KindItem("class", this);
//				tagKinds.insert("class", classKindItem);
//			}
//			QListViewItem *firstChild = classKindItem->firstChild();
//			if( firstChild ){
//				QListViewItemIterator iter(firstChild);
//				int i;
//				for( i = 0; i < classKindItem->childCount(); i++ ){
//					if( iter.current()->text(0) == className ){
//						new TagItem( entry.name, entry.file, entry.address.pattern, static_cast<KindItem*>(iter.current()) );
//						break;
//					}
//					iter++;
//				}
//				if( i == classKindItem->childCount() ){  // class hasn't been inserted yet
//					TagItem *classItem = new TagItem(className, entry.file, "", classKindItem );
//					new TagItem( entry.name, entry.file, entry.address.pattern, tagKinds["class"] );
//				}
//			}
//		}
//		else if( !strcmp(entry.kind, "member") )
//			;
//		else if( !strcmp(entry.kind, "class") ){
//			QString _namespace = tagsField (&entry, "namespace");
//			if( _namespace )
//				new TagItem( _namespace+"::"+entry.name, entry.file, entry.address.pattern, tagKinds[entry.kind] );
//			else
//				new TagItem( entry.name, entry.file, entry.address.pattern, tagKinds[entry.kind] );
//		}
//		else
			new TagItem( entry.name, entry.file, entry.address.pattern, tagKinds[entry.kind] );
	}while(tagsNext(tags, &entry) == TagSuccess);
//	if( classKindItem && classKindItem->childCount() == 0 )
//		delete classKindItem;
}

void TagsView::slotBuildTagsFile()
{
	emit buildTagsFile();
}

void TagsView::contentsMousePressEvent( QMouseEvent *e )
{
	QPoint p( contentsToViewport( e->pos() ) );
	QListViewItem *i = itemAt( p );
	if ( e->button() == LeftButton && i ) {
		// wether item is a tags item
		if(!i->isExpandable()){
			TagItem *tag = static_cast<TagItem*>(i);
			qWarning(tag->file);
			qWarning(tag->pattern);
			emit jump(tag->file, tag->pattern);
		}
	}
	QListView::contentsMousePressEvent(e);
}

void TagsView::contextMenuEvent( QContextMenuEvent *e )
{
	contextMenu->popup(e->globalPos());
	e->accept();
}
