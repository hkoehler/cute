/***************************************************************************
                          shortcutsdlg.cpp  -  implementation of shortcut dialog
						  -------------------
    begin                : Fri Apr 04 22:02:33 CEST 2003
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
 
#include <qaction.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qkeysequence.h>
#include <qextscintilla.h>
#include <qextscintillacommand.h>
#include <qextscintillacommandset.h> 

#include "shortcuts.h"
#include "config.h"
#include "shortcutsdlg.h"

ShortcutsDlg::ShortcutsDlg(QWidget *parent, const char *name) :
	ShortcutsDialog(parent, name)
{
	general_item = new QListViewItem(ShortcutListView, "General");
	edit_item = new QListViewItem(ShortcutListView, "Basic Editing");
	//view_item = new QListViewItem(ShortcutListView, "View");
	setWFlags(WShowModal);
	shortcutDlg = new ShortcutDlg();
	ShortcutListView->setRootIsDecorated(true);

	connect(shortcutDlg, SIGNAL(shortcutChanged(QKeySequence)), this, SLOT(slotShortcutChanged(QKeySequence)));
}

void ShortcutsDlg::slotDoubleClicked(QListViewItem *item)
{
	int key;
	
	if(item->firstChild())
		return;
	
	shortcutDlg->setKey(QKeySequence(item->text(1)));
	shortcutDlg->show();
}

void ShortcutsDlg::ok_slot()
{
	QListViewItem *item;
	QPtrList<QAction> *actions;
	QAction *action;
	
	actions = Shortcuts::editActions();
	
	for( item = edit_item->firstChild(); item; item = item->nextSibling() )
		for( action = actions->first(); action; action = actions->next() )
			if( item->text(0) == action->text()){
				action->setAccel(QKeySequence(item->text(1)));
				break;
			}

	delete actions;
	actions = Shortcuts::globalActionsWithoutEditActions();

	for( item = general_item->firstChild(); item; item = item->nextSibling() )
		for( action = actions->first(); action; action = actions->next() )
			if( item->text(0) == action->text()){
				action->setAccel(QKeySequence(item->text(1)));
				break;
			}
	delete actions;

	Shortcuts::synchronizeActions();
	emit updateShortcuts();
	hide();
}

void ShortcutsDlg::slotShortcutChanged(QKeySequence key)
{
	if(!checkShortcut(key))
		return;

	ShortcutListView->currentItem()->setText(1, (QString)key);
}

void ShortcutsDlg::showEvent(QShowEvent *)
{
	QAction *action;
	QPtrList<QAction> *editActions, *globalActions;

	delete general_item;
	delete edit_item;
	//delete view_item;

	general_item = new QListViewItem(ShortcutListView, "General");
	edit_item = new QListViewItem(ShortcutListView, "Basic Editing");
	//view_item = new QListViewItem(ShortcutListView, "View");

	globalActions = Shortcuts::globalActionsWithoutEditActions();
	for( action = globalActions->first(); action; action = globalActions->next() ){
		QString text = action->text();
		QString key = (QString)action->accel();
		general_item->insertItem(new QListViewItem(general_item, text, key));
	}
	delete globalActions;

	editActions = Shortcuts::editActions();
	for( action = editActions->first(); action; action = editActions->next() ){
		QString text = action->text();
		QString key = (QString)action->accel();
		edit_item->insertItem(new QListViewItem(edit_item, text, key));
	}
	delete editActions;
}

bool ShortcutsDlg::checkShortcut(QKeySequence key)
{
	QString msg, key_str;
	QListViewItem *item;
	QListViewItem *items[] = { general_item, edit_item };

	if(key.isEmpty())
		return true;
	
	key_str = (QString)QKeySequence(key);
	for( int i = 0; i < 2; i++ )
		for( item = items[i]->firstChild(); item; item = item->nextSibling() ){
			// edit actions must have only one key as shortcut
			if( items[i] == edit_item )
				if( key.count() > 1 ) {
					msg = QString("Edit action \"%1\" must have only on key").arg(item->text(0));
					QMessageBox::warning(this, "CUTE", msg);
					return false;
				}
			if( item != ShortcutListView->currentItem() ){
				if( key.matches(QKeySequence(item->text(1))) == Qt::Identical ){
					msg = QString(	"%1 has already been allocated to the \"%2\" action. \n"
									"Choose an unique key combination." ).arg(key_str).arg(item->text(0));
					QMessageBox::warning(this, "CUTE", msg);
					return false;
				}
				if( key.matches(QKeySequence(item->text(1))) == Qt::PartialMatch ){
					msg = QString(	"\"%1\" is part of some shortcuts for example \"%2\" action. \n"
									"Choose an unique key combination." ).arg(key_str).arg(item->text(0));
					QMessageBox::warning(this, "CUTE", msg);
					return false;
				}
				if( QKeySequence(item->text(1)).matches(key) == Qt::PartialMatch && item->text(1).length() ){
					msg = QString(	"\"%1\" is part of another shortcut \n"
									"Choose an unique key combination!" ).arg(item->text(0));
					QMessageBox::warning(this, "CUTE", msg);
					return false;
				}
			}
		}

	return true;
}

