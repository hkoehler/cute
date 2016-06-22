/***************************************************************************
                          shortcutdlg.cpp  -  dialog for setting a shortcut
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
#include <qlabel.h>
#include <qradiobutton.h>
#include "shortcutdlg.h"

ShortcutDlg::ShortcutDlg(QWidget *parent, const char *name)
	: ShortcutDialog(parent, name)
{
	keyIndex = 0;
	memset(keys, 0, sizeof(keys));
	setFocusPolicy(ClickFocus);
	
	setWFlags(WShowModal);
}

void ShortcutDlg::setKey(QKeySequence key)
{
	keyIndex = 0;
	memset(keys, 0, sizeof(keys));
	shortcutLabel->setText((QString)key);
}

void ShortcutDlg::clearPressed()
{
	keyIndex = 0;
	memset(keys, 0, sizeof(keys));
	shortcutLabel->setText("");
}

void ShortcutDlg::okPressed()
{
	hide();
	emit shortcutChanged( QKeySequence(shortcutLabel->text()) );
}

void ShortcutDlg::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Key_Control)
		return;
	if(e->key() == Key_Meta)
		return;
	if(e->key() == Key_Shift)
		return;
	if(e->key() == Key_Alt)
		return;
	if(e->key() == Key_Menu)
		return;

	if(keyIndex == 4){
		keyIndex = 0;
		memset(keys, 0, sizeof(keys));
	}

	keys[keyIndex] = e->key();
	
	if( e->state() & ControlButton )
		keys[keyIndex] += CTRL;
	if( e->state() & AltButton )
		keys[keyIndex] += ALT;
	if( e->state() & MetaButton )
		keys[keyIndex] += META;
	if( e->state() & ShiftButton )
		keys[keyIndex] += SHIFT;
	
	keyIndex++;
	
	if( keyIndex == 1 )
		shortcutLabel->setText((QString)QKeySequence(keys[0]));
	else if( keyIndex == 2 )
		shortcutLabel->setText((QString)QKeySequence(keys[0], keys[1]));
	else if( keyIndex == 3 )
		shortcutLabel->setText((QString)QKeySequence(keys[0], keys[1], keys[2]));
	else if( keyIndex == 4 )
		shortcutLabel->setText((QString)QKeySequence(keys[0], keys[1], keys[2], keys[3]));
		
	e->accept();
}
