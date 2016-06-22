// This module implements the QextScintillaCommandSet class.
//
// Copyright (c) 2004
// 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
// 
// This file is part of QScintilla.
// 
// This copy of QScintilla is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option) any
// later version.
// 
// QScintilla is supplied in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// QScintilla; see the file LICENSE.  If not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#include <qsettings.h>

#include "qextscintillacommandset.h"
#include "qextscintillacommand.h"
#include "qextscintilla.h"


// The ctor.
QextScintillaCommandSet::QextScintillaCommandSet(QextScintilla *qs) : qsci(qs)
{
	struct sci_cmd {
		int msg;
		int key;
		int altkey;
		const char *desc;
	};

	// This is based on the default table in src/KeyMap.cxx.
	static struct sci_cmd cmd_table[] = {
		{
			QextScintillaBase::SCI_LINEDOWN,
			Qt::Key_Down,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move down one line")
		},
		{
			QextScintillaBase::SCI_LINEDOWNEXTEND,
			Qt::Key_Down | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection down one line")
		},
		{
			QextScintillaBase::SCI_LINESCROLLDOWN,
			Qt::Key_Down | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Scroll view down one line")
		},
		{
			QextScintillaBase::SCI_LINEDOWNRECTEXTEND,
			Qt::Key_Down | Qt::ALT | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend rectangular selection down one line")
		},
		{
			QextScintillaBase::SCI_LINEUP,
			Qt::Key_Up,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move up one line")
		},
		{
			QextScintillaBase::SCI_LINEUPEXTEND,
			Qt::Key_Up | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection up one line")
		},
		{
			QextScintillaBase::SCI_LINESCROLLUP,
			Qt::Key_Up | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Scroll view up one line")
		},
		{
			QextScintillaBase::SCI_LINEUPRECTEXTEND,
			Qt::Key_Up | Qt::ALT | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend rectangular selection up one line")
		},
		{
			QextScintillaBase::SCI_PARAUP,
			Qt::Key_BracketLeft | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move up one paragraph")
		},
		{
			QextScintillaBase::SCI_PARAUPEXTEND,
			Qt::Key_BracketLeft | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection up one paragraph")
		},
		{
			QextScintillaBase::SCI_PARADOWN,
			Qt::Key_BracketRight | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move down one paragraph")
		},
		{
			QextScintillaBase::SCI_PARADOWNEXTEND,
			Qt::Key_BracketRight | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection down one paragraph")
		},
		{
			QextScintillaBase::SCI_CHARLEFT,
			Qt::Key_Left,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move left one character")
		},
		{
			QextScintillaBase::SCI_CHARLEFTEXTEND,
			Qt::Key_Left | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection left one character")
		},
		{
			QextScintillaBase::SCI_WORDLEFT,
			Qt::Key_Left | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move left one word")
		},
		{
			QextScintillaBase::SCI_WORDLEFTEXTEND,
			Qt::Key_Left | Qt::SHIFT | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection left one word")
		},
		{
			QextScintillaBase::SCI_CHARLEFTRECTEXTEND,
			Qt::Key_Left | Qt::ALT | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend rectangular selection left one character")
		},
		{
			QextScintillaBase::SCI_CHARRIGHT,
			Qt::Key_Right,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move right one character")
		},
		{
			QextScintillaBase::SCI_CHARRIGHTEXTEND,
			Qt::Key_Right | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection right one character")
		},
		{
			QextScintillaBase::SCI_WORDRIGHT,
			Qt::Key_Right | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move right one word")
		},
		{
			QextScintillaBase::SCI_WORDRIGHTEXTEND,
			Qt::Key_Right | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection right one word")
		},
		{
			QextScintillaBase::SCI_CHARRIGHTRECTEXTEND,
			Qt::Key_Right | Qt::ALT | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend rectangular selection right one character")
		},
		{
			QextScintillaBase::SCI_WORDPARTLEFT,
			Qt::Key_Slash | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move left one word part")
		},
		{
			QextScintillaBase::SCI_WORDPARTLEFTEXTEND,
			Qt::Key_Slash | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection left one word part")
		},
		{
			QextScintillaBase::SCI_WORDPARTRIGHT,
			Qt::Key_Backslash | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move right one word part")
		},
		{
			QextScintillaBase::SCI_WORDPARTRIGHTEXTEND,
			Qt::Key_Backslash | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection right one word part")
		},
		{
			QextScintillaBase::SCI_VCHOME,
			Qt::Key_Home,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move to first visible character in line")
		},
		{
			QextScintillaBase::SCI_VCHOMEEXTEND,
			Qt::Key_Home | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection to first visible character in line")
		},
		{
			QextScintillaBase::SCI_DOCUMENTSTART,
			Qt::Key_Home | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move to start of text")
		},
		{
			QextScintillaBase::SCI_DOCUMENTSTARTEXTEND,
			Qt::Key_Home | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection to start of text")
		},
		{
			QextScintillaBase::SCI_HOMEDISPLAY,
			Qt::Key_Home | Qt::ALT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move to start of displayed line")
		},
		{
			QextScintillaBase::SCI_HOMEDISPLAYEXTEND,
			0,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection to start of line")
		},
		{
			QextScintillaBase::SCI_VCHOMERECTEXTEND,
			Qt::Key_Home | Qt::ALT | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend rectangular selection to first visible character in line")
		},
		{
			QextScintillaBase::SCI_LINEEND,
			Qt::Key_End,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move to end of line")
		},
		{
			QextScintillaBase::SCI_LINEENDEXTEND,
			Qt::Key_End | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection to end of line")
		},
		{
			QextScintillaBase::SCI_DOCUMENTEND,
			Qt::Key_End | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move to end of text")
		},
		{
			QextScintillaBase::SCI_DOCUMENTENDEXTEND,
			Qt::Key_End | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection to end of text")
		},
		{
			QextScintillaBase::SCI_LINEENDDISPLAY,
			Qt::Key_End | Qt::ALT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move to end of displayed line")
		},
		{
			QextScintillaBase::SCI_LINEENDDISPLAYEXTEND,
			0,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection to end of displayed line")
		},
		{
			QextScintillaBase::SCI_LINEENDRECTEXTEND,
			Qt::Key_End | Qt::ALT | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend rectangular selection to end of line")
		},
		{
			QextScintillaBase::SCI_PAGEUP,
			Qt::Key_Prior,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move up one page")
		},
		{
			QextScintillaBase::SCI_PAGEUPEXTEND,
			Qt::Key_Prior | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection up one page")
		},
		{
			QextScintillaBase::SCI_PAGEUPRECTEXTEND,
			Qt::Key_Prior | Qt::ALT | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend rectangular selection up one page")
		},
		{
			QextScintillaBase::SCI_PAGEDOWN,
			Qt::Key_Next,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move down one page")
		},
		{
			QextScintillaBase::SCI_PAGEDOWNEXTEND,
			Qt::Key_Next | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend selection down one page")
		},
		{
			QextScintillaBase::SCI_PAGEDOWNRECTEXTEND,
			Qt::Key_Next | Qt::ALT | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Extend rectangular selection down one page")
		},
		{
			QextScintillaBase::SCI_CLEAR,
			Qt::Key_Delete,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Delete current character")
		},
		{
			QextScintillaBase::SCI_CUT,
			Qt::Key_X | Qt::CTRL,
			Qt::Key_Delete | Qt::SHIFT,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Cut selection")
		},
		{
			QextScintillaBase::SCI_DELWORDRIGHT,
			Qt::Key_Delete | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Delete word to right")
		},
		{
			QextScintillaBase::SCI_DELLINERIGHT,
			Qt::Key_Delete | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Delete line to right")
		},
		{
			QextScintillaBase::SCI_EDITTOGGLEOVERTYPE,
			Qt::Key_Insert,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Toggle insert/overtype")
		},
		{
			QextScintillaBase::SCI_PASTE,
			Qt::Key_V | Qt::CTRL,
			Qt::Key_Insert | Qt::SHIFT,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Paste")
		},
		{
			QextScintillaBase::SCI_COPY,
			Qt::Key_C | Qt::CTRL,
			Qt::Key_Insert | Qt::CTRL,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Copy selection")
		},
		{
			QextScintillaBase::SCI_CANCEL,
			Qt::Key_Escape,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Cancel")
		},
		{
			QextScintillaBase::SCI_DELETEBACK,
			Qt::Key_Backspace,
			Qt::Key_Backspace | Qt::SHIFT,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Delete previous character")
		},
		{
			QextScintillaBase::SCI_DELWORDLEFT,
			Qt::Key_Backspace | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Delete word to left")
		},
		{
			QextScintillaBase::SCI_UNDO,
			Qt::Key_Z | Qt::CTRL,
			Qt::Key_Backspace | Qt::ALT,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Undo the last command")
		},
		{
			QextScintillaBase::SCI_DELLINELEFT,
			Qt::Key_Backspace | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Delete line to left")
		},
		{
			QextScintillaBase::SCI_REDO,
			Qt::Key_Y | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Redo last command")
		},
		{
			QextScintillaBase::SCI_SELECTALL,
			Qt::Key_A | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Select all text")
		},
		{
			QextScintillaBase::SCI_TAB,
			Qt::Key_Tab,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Indent one level")
		},
		{
			QextScintillaBase::SCI_BACKTAB,
			Qt::Key_Tab | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Move back one indentation level")
		},
		{
			QextScintillaBase::SCI_NEWLINE,
			Qt::Key_Return,
			Qt::Key_Return | Qt::SHIFT,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Insert new line")
		},
		{
			QextScintillaBase::SCI_ZOOMIN,
			Qt::Key_Plus | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Zoom in")
		},
		{
			QextScintillaBase::SCI_ZOOMOUT,
			Qt::Key_Minus | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Zoom out")
		},
		{
			QextScintillaBase::SCI_SETZOOM,
			0,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Set zoom")
		},
		{
			QextScintillaBase::SCI_FORMFEED,
			0,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Formfeed")
		},
		{
			QextScintillaBase::SCI_LINECUT,
			Qt::Key_L | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Cut current line")
		},
		{
			QextScintillaBase::SCI_LINEDELETE,
			Qt::Key_L | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Delete current line")
		},
		{
			QextScintillaBase::SCI_LINECOPY,
			Qt::Key_T | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Copy current line")
		},
		{
			QextScintillaBase::SCI_LINETRANSPOSE,
			Qt::Key_T | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Swap current and previous lines")
		},
		{
			QextScintillaBase::SCI_LINEDUPLICATE,
			Qt::Key_D | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Duplicate current line")
		},
		{
			QextScintillaBase::SCI_LOWERCASE,
			Qt::Key_U | Qt::CTRL,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Convert selection to lower case")
		},
		{
			QextScintillaBase::SCI_UPPERCASE,
			Qt::Key_U | Qt::CTRL | Qt::SHIFT,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Convert selection to upper case")
		},
		{
			QextScintillaBase::SCI_DELETEBACKNOTLINE,
			0,
			0,
			QT_TRANSLATE_NOOP("QextScintillaCommand",
				"Delete previous character if not at line start")
		},
	};

	cmds.setAutoDelete(TRUE);

	// Clear the default map.
	qsci -> SendScintilla(QextScintillaBase::SCI_CLEARALLCMDKEYS);

	for (int i = 0; i < sizeof (cmd_table) / sizeof (cmd_table[0]); ++i)
		cmds.append(new QextScintillaCommand(qsci,cmd_table[i].msg,cmd_table[i].key,cmd_table[i].altkey,cmd_table[i].desc));
}


// Read the command set from settings.
bool QextScintillaCommandSet::readSettings(QSettings &qs,const char *prefix)
{
	bool rc = TRUE;
	QString skey;

	for (QextScintillaCommand *cmd = cmds.first(); cmd; cmd = cmds.next())
	{
		skey.sprintf("%s/keymap/c%d/",prefix,cmd -> msgId());

		int key;
		bool ok;

		// Read the key.
		key = qs.readNumEntry(skey + "key",0,&ok);

		if (ok)
			cmd -> setKey(key);
		else
			rc = FALSE;

		// Read the alternate key.
		key = qs.readNumEntry(skey + "alt",0,&ok);

		if (ok)
			cmd -> setAlternateKey(key);
		else
			rc = FALSE;
	}

	return rc;
}


// Write the command set to settings.
bool QextScintillaCommandSet::writeSettings(QSettings &qs,const char *prefix)
{
	bool rc = TRUE;
	QString skey;

	for (const QextScintillaCommand *cmd = cmds.first(); cmd; cmd = cmds.next())
	{
		skey.sprintf("%s/keymap/c%d/",prefix,cmd -> msgId());

		// Write the key.
		if (!qs.writeEntry(skey + "key",cmd -> key()))
			rc = FALSE;

		// Write the alternate key.
		if (!qs.writeEntry(skey + "alt",cmd -> alternateKey()))
			rc = FALSE;
	}

	return rc;
}
