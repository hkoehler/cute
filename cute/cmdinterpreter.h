/***************************************************************************
                          cmdinterpreter.h  -  command interpreter for CUTE commands
                             -------------------
    begin                : Thu Feb 3 2003
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

#ifndef __CMDINTERPRETER_H__
#define __CMDINTERPRETER_H__

#include <qobject.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qprocess.h>

/** function type for adding CUTE commands */
typedef bool (*cmd_func)(QString &args);

/** This class represents the CUTE command interpreter, which executes all command typed in the command box of CUTE.
  * New commands  can be added with the @ref registerCmd method. If the typed string is not a CUTE command it is supposed
  * to be a shell command and is executed with a shell in the background.
  @author Heiko Köhler
  @short A class which interprets CUTE and shell commands.
  */

class CmdInterpreter : public QObject
{
	Q_OBJECT
public:
	/** constructs a new interpreter but only one is necessary */
	CmdInterpreter(QObject *parent);
	/** creates new sub process */
	void createNewProcess();
	/** runs a command returns false on failure and true on success */
	bool exec(const QString &cmd);
	/** make interpreter to know a function func which is executed when command starts with :.
	   "name" is the name for the new CUTE function*/
	void registerFunc(QString name, cmd_func func);
	/** registers a new command starting with startToken and implemented in func */
	void registerCmd(QString startToken, cmd_func func);
	/** retrieves all available commands of the interpeter */
	QStringList &commands();
	/** read line from stdout */
	QString readLineStdout();
	/** read line from stderr */
	QString readLineStderr();
	/** can line be read from stdout */
	bool canReadLineStdout();
	/** can line be read from stderr */
	bool canReadLineStderr();
public slots:
	void kill();
signals:
	void readyReadStdout();
	void readyReadStderr();
	void processExited();
private:
	/** all available commands */
	QStringList __commands;
	/** process */
	static QProcess *proc;
};

#endif
