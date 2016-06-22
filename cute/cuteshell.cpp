/***************************************************************************
                          cuteshell.cpp  -  description
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
 
#include <stdio.h>
#include <unistd.h> 

#include <qstringlist.h>
#include <qregexp.h>
#include <qcursor.h>
#include <qdir.h>

#include "cuteshell.h"

CUTEShell::CUTEShell( QWidget *parent, const char *name )
	: QTextEdit(parent, name)
{
	curr_cmd = history.end();
	setWordWrap(NoWrap);
	setFont(QFont("courier", 10));
	connect(this, SIGNAL(returnPressed()), this, SLOT(runCommand()));
	connect(this, SIGNAL(clicked(int,int)), this, SLOT(setCursorPos()));
	connect(this, SIGNAL(doubleClicked(int,int)), this, SLOT(setCursorPos()));
	QStringList args = QStringList::split(" ", "/bin/sh -s");
	proc = new QProcess(args, this);
	connect(proc, SIGNAL(readyReadStdout()), this, SLOT(readStdout()));
	connect(proc, SIGNAL(readyReadStderr()), this, SLOT(readStderr()));
	connect(proc, SIGNAL(processExited()), this, SLOT(processExited()));
	proc->start();
	prompt=QDir::current().absPath()+"> ";
	append(prompt);
	setCursorPos();
}

void CUTEShell::readStdout()
{
	QString cwd;
	QString exit_stat;
	char *cmd_end = NULL;
	
	setColor(Qt::blue);
	while(proc->canReadLineStdout()){
		QByteArray bytes = proc->readStdout();
		// parse echo command and find current dir and exit status
		cmd_end = strstr(bytes, "CUTE_END_OF_PROC");
		if(cmd_end) {
			char *tmp = cmd_end;
			strtok(cmd_end, ":");
			cwd = strtok(NULL, ":");
			exit_stat = strtok(NULL, ":");
			*tmp = 0;
			*cmd_end = 0;
		}
		append(bytes);
		// print prompt
		if(cmd_end){
			setColor(Qt::black);
			prompt = cwd+"> ";
			append( prompt );
		}
		setCursorPos();
	}
}

void CUTEShell::readStderr()
{
	QString line;

	setColor(Qt::red);
	while(proc->canReadLineStderr()){
		QByteArray bytes = proc->readStderr();
		line = bytes.data();
		append(bytes);
		setCursorPosition(lines()-1, 0);
	}
	setColor(Qt::black);
}

void CUTEShell::processExited()
{
	setColor(Qt::red);
	insertParagraph("Shell restarted", -1);
	setCursorPosition(lines()-1, 0);
	setColor(Qt::black);
	proc->start();
}

void CUTEShell::runCommand()
{
	QString line;
	
	line = text(lines()-2);
	line = line.mid(prompt.length());
	if(line.stripWhiteSpace() == ""){
		proc->writeToStdin("echo CUTE_END_OF_PROC:$PWD:$?:\n");
		return;
	}
	// add a echo command, writing current dir and exit status
	proc->writeToStdin(line+"; echo CUTE_END_OF_PROC:$PWD:$?:\n");
	history.append(line.stripWhiteSpace());
	curr_cmd = history.end();
}

QPopupMenu *CUTEShell::createPopupMenu( const QPoint & pos )
{
	contextMenu = new QPopupMenu(this);
	contextMenu->insertItem("Clear", this, SLOT(clear()));
	contextMenu->insertItem("Kill", proc, SLOT(kill()));
	return contextMenu;
}

void CUTEShell::setCursorPos( )
{
	setCursorPosition(paragraphs()-1, 1000);
}

void CUTEShell::keyPressEvent( QKeyEvent *e )
{
	int line, index;
	
	setColor(Qt::black);
	getCursorPosition(&line, &index);
	if( e->key() == Qt::Key_Up ){
		curr_cmd--;
		setCursorPosition(line, prompt.length());
		if( index > prompt.length() )
			for( int i = 0; i < index-prompt.length(); i++ )
				del();
		insert(*curr_cmd);
		setCursorPos();
		return;
	}
	if(e->key() == Qt::Key_Down ){
		curr_cmd++;
		setCursorPosition(line, prompt.length());
		if( index > prompt.length() )
			for( int i = 0; i < index-prompt.length(); i++ )
				del();
		insert(*curr_cmd);
		setCursorPos();
		return;
	}
	if( e->key() == Qt::Key_Prior || e->key() == Qt::Key_Next )
		return;
	if( (e->key() == Qt::Key_Left || e->key() == Qt::Key_Backspace) && index < prompt.length()+1 )
		return;
	QTextEdit::keyPressEvent(e);
}
