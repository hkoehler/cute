/***************************************************************************
                          shortcutdlg.h  -  dialog for setting a shortcut
                             -------------------
    begin                : Fre, 11 Apr 2003
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

#include <qkeysequence.h>

#include "ui/shortcutdialog.h"

/** gets primary and alternative shortcut */
class ShortcutDlg : public ShortcutDialog
{
	Q_OBJECT
public:
	ShortcutDlg(QWidget *parent=0, const char *name=0);
	/** sets primary and alternative key in dialog */
	void setKey(QKeySequence key);
protected slots:
	/** clears shortcut label */
	void clearPressed();
	/** hides dialog and emits shortcutChanged signal */
	void okPressed();
	/** records pressed key and sets key labels*/
	void keyPressEvent(QKeyEvent *e);
signals:
	/** emitted when shortcut set */
	void shortcutChanged(QKeySequence key);
private:
	int keys[4];
	int keyIndex;
};
