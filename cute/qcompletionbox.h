/***************************************************************************
                          qcompletionbox.h  -  description
                             -------------------
    begin                : Mon Feb 3 2003
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

#ifndef __QCOMPLETIONBOX_H__
#define __QCOMPLETIONBOX_H__

#undef slots
#include <Python.h>
#define slots
#include <qcombobox.h>
#include <qstringlist.h>

class CmdInterpreter;

/** QCommandBox is a combo box for typing commands. All available commands are stored in this class
  * and new can be added by typing new ones 
  * @author Heiko Köhler
  * @short Combo box, which can autocomplete input and make suggestions
*/

/** This combo box is for typing CUTE commands and executes them */
class QCommandBox : public QComboBox
{
	Q_OBJECT
public:
	/** this constructor creates a combo box, which autocompletes his input and executes commands*/
	QCommandBox( CmdInterpreter *cmdInterpreter, QWidget *parent=0, const char *name=0 );
	/** inserts all functions of the python module into history list*/
	void registerPyModule( PyObject *module );
protected slots:
	/** on user input search strings starting with str in list box */
	void autoComplete( const QString & );
	/** called after an item has been selected */
	void itemActivated( const QString & );
	/** executes command with interpreter */
	void exec();
signals:
	void commandExecuted();
private:
	/**all commands stored in the box */
	QStringList commands;
	/** the command interpreter for running commands */
	CmdInterpreter *cmdInterpreter;
};

#endif

