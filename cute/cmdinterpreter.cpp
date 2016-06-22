/***************************************************************************
                          cmdinterpreter.cpp  -  executes CUTE commands
                             -------------------
    begin                : Don, 13 Feb 2003
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

#undef slots
#include <Python.h>
#define slots
#include <stdlib.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qregexp.h>
#include <qstringlist.h>

#include "cute.h"
#include "cmdinterpreter.h"

/** type representing a CUTE command */
class Cmd
{
public:
	cmd_func func;
	QString startToken;
};

class Func
{
public:
	cmd_func func;
	QString name;
};

/** jums to given line*/
static bool goto_cmd( QString &cmd );
/** executes a python command */
static bool python_cmd( QString &cmd );
/** filters selection with a shell command */
static bool filter_cmd( QString &cmd );
/** inserts output of a shell command */
static bool insertCommand_cmd( QString &cmd );
/** searches and/or replaces text */
static bool search_cmd(QString &cmd, bool fwd);
/** searches backward and/or replaces text */
static bool search_foreward_cmd(QString &cmd);
/** searches backward and/or replaces text */
static bool search_backward_cmd(QString &cmd);
/** searches and/or replaces text (sed style)*/
static bool sed_search_cmd(QString &cmd);
/**quits app*/
static bool quit_cmd(QString &cmd);
/** saves current file */
static bool write_cmd(QString &cmd);
/** loads file */
static bool edit_cmd(QString &cmd);
/** writes all modified files and exits*/
static bool write_quit_cmd(QString &cmd);
/** closes current window */
static bool close_cmd(QString &cmd);
/** changes working dir */
static bool cd_cmd(QString &cmd);
/** inserts character with given char code */
static bool insert_char_cmd(QString &cmd);

/**  list containing all CUTE command implementations */
QList<Cmd> cmdList;
/** list containing all CUTE function implementations */
QList<Func> funcList;
/** process */
QProcess *CmdInterpreter::proc;

static bool goto_cmd( QString &cmd )
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	int line;
	bool ok;

	if(!view)
		return false;
	line = cmd.toInt(&ok);
	if(ok)
		view->sendEditor(QextScintillaBase::SCI_GOTOLINE, line-1);
	return ok;
}

static bool python_cmd( QString &cmd )
{
	int ret;
	char *cmd_str = new char[cmd.length()+2];
	
	cmd = cmd.stripWhiteSpace()+"\n";
	strcpy( cmd_str, cmd.latin1());
	if( (ret = PyRun_SimpleString(cmd_str)) == -1){
		PyRun_SimpleString("output(\"Exception raised\")\n");
	}
	return ret;
}

static bool filter_cmd( QString &cmd )
{
	QString str = "filter(r\"" + cmd + "\")";
	return python_cmd(str);
}

static bool insertCommand_cmd( QString &cmd )
{
	QString str = "insertCommand(r\"" + cmd + "\")";
	return python_cmd(str);
}

static bool search_cmd(QString &cmd, bool fwd)
{
	bool re=true, cs=true, wo=false, wrap=false;
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	QString str;
	if(!view)
		return false;

	QStringList stringList = QStringList::split(QRegExp("\\s"), cmd);
	QStringList replaceList = QStringList::split(QRegExp("/"), cmd);

	re = !stringList.contains("!re");
	cs = !stringList.contains("!cs");
	wo = stringList.contains("word") || stringList.contains("wo");
	wrap = stringList.contains("wrap");

	if(replaceList.count() >= 2){
		QString replaceStr;
		int line=0, col=0;

		str = replaceList[0];
		replaceStr = QStringList::split(QRegExp("\\s"), replaceList[1])[0];
		qDebug("replace: %s, %s", str.latin1(), replaceStr.latin1());
		view->scintilla()->beginUndoAction();
		while(view->findFirst(str, re, cs, wo, wrap, fwd, line, col )){
			view->scintilla()->replace(replaceStr);
			view->scintilla()->getCursorPosition(&line, &col); 
		}
		view->scintilla()->endUndoAction();
		view->scintilla()->selectAll(false);
		return true;
	}

	str = stringList[0];
	qDebug("findFirst: %s, %d, %d, %d", str.latin1(), wo, wrap, fwd );
	return view->findFirst(str, re, cs, wo, wrap, fwd );
}

static bool search_backward_cmd(QString &cmd)
{
	return search_cmd(cmd, false);
}

static bool search_foreward_cmd(QString &cmd)
{
	return search_cmd(cmd, true);
}

static bool sed_search_cmd(QString &cmd)
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	QString replaceStr, str, optStr;
	QStringList replaceStrList;
	bool global=false, confirm=false;
	int line, col;

	if(!view)
		return false;
	replaceStrList = QStringList::split("/", cmd);
	if(replaceStrList.size() < 2)
		return false;
	
	str = replaceStrList[0];
	qDebug("str: %s", str.latin1());
	replaceStr = replaceStrList[1];
	qDebug("replaceStr: %s", replaceStr.latin1());
	if(replaceStrList.size() > 2){
		optStr = replaceStrList[2];
		qDebug("optStr: %s", optStr.latin1());
		if(optStr == "g")
			global = true;
		else if(optStr == "gc"){
			global = true;
			confirm = true;
		}
	}

	view->scintilla()->beginUndoAction();
	if( !global ){
		int prev_line;
		view->scintilla()->getCursorPosition(&line, &col);
		prev_line = line;
		view->findFirst(str, true, true, false, true, true, line, 0 );
		if( prev_line == line)
			view->scintilla()->replace(replaceStr);
		else
			view->scintilla()->setCursorPosition(line,col);
	}
	else{
		line = col = 0;
		view->replaceAll(str, replaceStr, true, true, false, true, true, confirm );
	}
	view->scintilla()->endUndoAction();
	if(view->scintilla()->hasSelectedText())
		view->scintilla()->selectAll(false);

	return true;
}

static bool quit_cmd(QString &cmd)
{
	qApp->quit();
	return true;
}

static bool write_cmd(QString &cmd)
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());

	if(!view)
		return false;
	if(cmd != QString::null)
		return view->saveFile( cmd );
	else
		return view->saveFile();
}

static bool edit_cmd(QString &cmd)
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());

	cmd = cmd.stripWhiteSpace();
	cute->loadFile(cmd);
	return true;
}

static bool write_quit_cmd(QString &cmd)
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());

	cute->slotFileSaveAll();
	qApp->quit();
	return true;
}

static bool close_cmd(QString &cmd)
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	QStringList strList = QStringList::split(QRegExp("\\s"), cmd);
	if(strList.count() > 0){
		if( strList[0] == "all" )
			cute->closeAllViews();
	}
	else
		cute->slotFileClose();
	return true;
}

static bool cd_cmd(QString &cmd)
{
	bool ret;

	cmd = cmd.stripWhiteSpace();
	if(!(ret = QDir::setCurrent(cmd)))
		QMessageBox::warning(qApp->mainWidget(), "CUTE", "Change of directory unsuccessfull");
	return ret;
}

static bool insert_char_cmd(QString &cmd)
{
	int line, index, ch;
	bool ok;
	QString str;
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());

	if(!view)
		return false;
	cmd = cmd.stripWhiteSpace();
	if(cmd == QString::null)
		return false;
	ch = cmd.toInt(&ok);
	if(!ok)
		return false;
	str = (char)ch;
	view->scintilla()->getCursorPosition(&line, &index);
	view->scintilla()->insertAt(str, line, index);

	return true;
}

CmdInterpreter::CmdInterpreter(QObject *parent)
	: QObject(parent)
{
	registerCmd("!", filter_cmd);
	registerCmd(">", insertCommand_cmd);
	registerCmd("/", search_foreward_cmd);
	registerCmd("?", search_backward_cmd);
	registerCmd(":", goto_cmd);
	registerCmd(":py", python_cmd);
	registerCmd(":python", python_cmd);
	registerCmd(":s", sed_search_cmd);
	registerCmd(":q", quit_cmd);
	registerCmd(":quit", quit_cmd);
	registerCmd(":w", write_cmd);
	registerCmd(":write", write_cmd);
	registerCmd(":e", edit_cmd);
	registerCmd(":edit", edit_cmd);
	registerCmd(":wq", write_quit_cmd);
	registerCmd(":x", write_quit_cmd);
	registerCmd(":close", close_cmd);
	registerCmd(":cd", cd_cmd);
	registerCmd(":char", insert_char_cmd);

	createNewProcess();
	__commands.sort();
}

void CmdInterpreter::createNewProcess()
{
	proc = new QProcess();
	connect(proc, SIGNAL(readyReadStdout()), this, SIGNAL(readyReadStdout()));
	connect(proc, SIGNAL(readyReadStderr()), this, SIGNAL(readyReadStderr()));
	connect(proc, SIGNAL(processExited()), this, SIGNAL(processExited()));
}

bool CmdInterpreter::exec(const QString &c)
{
	QString cmd = c, fileName;
	int pos;
	
	MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	if(view)
		fileName = view->fileName();
	else
		fileName = "";
	// replaces a % in command with file name, like vi ;-) 
	cmd.replace(QRegExp("\%"), fileName);

	// !: forces a shell comand
	if(cmd.startsWith(":!"))
		cmd.remove(0, 2);
	else
		for( Cmd *pCmd = cmdList.last(); pCmd != 0; pCmd = cmdList.prev() ){
			if(cmd.startsWith(pCmd->startToken)){
				cmd.remove(0, pCmd->startToken.length());
				return pCmd->func(cmd);
			}
		}

	if(proc->isRunning()){
		int i = QMessageBox::warning(0, "CUTE", "There is an application already running.",
											"Kill", "Start", "Cancel");
		switch(i){
		case 0:
			proc->kill();
			break;
		case 1:
			break;
		case 2:
			return false;
		}
	}
	proc->clearArguments();
	proc->addArgument("/bin/sh");
	proc->addArgument("-c");
	proc->addArgument(cmd);
	proc->setWorkingDirectory( QDir::currentDirPath() );
	proc->start();
	return true;
}

void CmdInterpreter::registerFunc(QString name, cmd_func func)
{
	Func *f = new Func;
	f->name = name;
	f->func = func;
	funcList.append(f);
	__commands.append(":"+name);
}

void CmdInterpreter::registerCmd(QString startToken, cmd_func func)
{
	Cmd *cmd = new Cmd;
	cmd->startToken = startToken;
	cmd->func = func;
	cmdList.append(cmd);
	__commands.append(startToken);
}

QStringList &CmdInterpreter::commands()
{
	return __commands;
}

QString CmdInterpreter::readLineStdout()
{
	return proc->readLineStdout();
}

QString CmdInterpreter::readLineStderr()
{
	return proc->readLineStderr();
}

void CmdInterpreter::kill()
{
	proc->tryTerminate();
}

bool CmdInterpreter::canReadLineStdout()
{
	return proc->canReadLineStdout();
}

bool CmdInterpreter::canReadLineStderr()
{
	return proc->canReadLineStderr();
}
