/***************************************************************************
                          qcompletionbox.cpp  -  A completion box for CUTE commands
                             -------------------
    begin                : Sam, 15 Feb 2003
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

#include <qlistbox.h>
#include <qlineedit.h>
#include "qcompletionbox.h"
#include "cmdinterpreter.h"

QCommandBox::QCommandBox( CmdInterpreter *ci, QWidget *parent, const char *name )
  : QComboBox(true, parent, name)
{
	setInsertionPolicy(NoInsertion);
	//setAutoCompletion(true);
	commands = ci->commands();
	commands.sort();
	insertStringList(commands);
	clearEdit();
	installEventFilter(lineEdit());
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(autoComplete(const QString&)));
	connect(this, SIGNAL(activated(const QString&)), this, SLOT(itemActivated(const QString&)));
	connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(exec()));
}

void QCommandBox::autoComplete(const QString &str)
{
	QStringList strList;
	for(int i = 0; i < commands.count(); i++)
		if(commands[i].startsWith(str))
			strList.append(commands[i]);
	strList.sort();
	disconnect(this, SIGNAL(textChanged(const QString&)), this, SLOT(autoComplete(const QString&)));
	listBox()->clear();
	listBox()->insertStringList(strList);
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(autoComplete(const QString&)));
}

void QCommandBox::itemActivated( const QString &str )
{
	disconnect(this, SIGNAL(textChanged(const QString&)), this, SLOT(autoComplete(const QString&)));
	listBox()->clear();
	if( commands.findIndex(str) == -1 )
		commands.append(str);
	commands.sort();
	listBox()->insertStringList(commands);
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(autoComplete(const QString&)));
}

void QCommandBox::exec()
{
	cmdInterpreter->exec(currentText());
	emit commandExecuted();
}

void QCommandBox::registerPyModule(PyObject *module )
{
	PyObject *key, *value;
	int pos = 0;
	char *key_str;

	PyObject *dict = PyModule_GetDict(module);
	while(PyDict_Next(dict, &pos, &key, &value)){
		key_str = PyString_AsString(key);
		if(!strcmp(key_str, "__name__"))
			continue;
		if(!strcmp(key_str, "__doc__"))
			continue;
		insertItem(":py " + QString(key_str));
		commands.append(":py " + QString(key_str));
	}
	listBox()->sort();
}


