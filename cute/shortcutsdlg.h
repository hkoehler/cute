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

#ifndef __SHORTCUTSDLG_H__
#define __SHORTCUTSDLG_H__

#include <qlistview.h>

#include "ui/shortcutsdialog.h"
#include "shortcutdlg.h"

/** views all shortcuts and modify them */
class ShortcutsDlg : public ShortcutsDialog
{
	Q_OBJECT
public:
	ShortcutsDlg(QWidget *parent=0, const char *name=0);
public slots:
	/** shows the shortcut dialog */
	void slotDoubleClicked(QListViewItem*);
	/** changes shortcut config and closes the dialog */
	void ok_slot();
	/** changes selected shortcut */
	void slotShortcutChanged(QKeySequence shortcut);
signals:
	void updateShortcuts();
protected:
	/** create list entries when be shown */
	void showEvent(QShowEvent*);
	/** makes sure shortcut is unique */
	bool checkShortcut(QKeySequence);
private:
	/** shortcut dialog */
	ShortcutDlg *shortcutDlg;
	/** view item in list view */
	QListViewItem *view_item, *edit_item, *general_item;
};

#endif
