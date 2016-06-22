/***************************************************************************
                          cuteshell.h  -  description
                             -------------------
    begin                : Sam Aug 10 2:5:49 CEST 2002
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

#ifndef __CUTESHELL_H__
#define __CUTESHELL_H__

#include <qtextedit.h>
#include <qprocess.h>
#include <qpopupmenu.h>
#include <qstringlist.h>

class CUTEShell : public QTextEdit
{
	Q_OBJECT
public:
	CUTEShell( QWidget *parent = 0, const char *name = 0 );
protected:
	QPopupMenu *createPopupMenu( const QPoint & pos );
	/** disables some keys */
	void keyPressEvent( QKeyEvent *e );
protected slots:
	void readStdout();
	void readStderr();
	void processExited();
	/** execute current line */
	void runCommand();
	/** set cursor position to end of text */
	void setCursorPos();
private:
	/** process of shell */
	QProcess *proc;
	/** context menu */
	QPopupMenu *contextMenu;
	/** command history */
	QStringList history;
	/** current command in history */
	QStringList::iterator curr_cmd;
	/** prompt, created of last command */
	QString prompt;
};

#endif

