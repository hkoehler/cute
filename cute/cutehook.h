/***************************************************************************
                          cutehook.h  -  description
                             -------------------
    begin                : Thusday Apr 29 16:11:52 BST 2004
    copyright            : (C) 2004 by Nick Thompson
    email                : nickthompson@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __CUTEHOOK_H__
#define __CUTEHOOK_H__

#include <qstring.h>
#include <qobject.h>
#include <qptrlist.h> 

#undef slots
#include <Python.h>
#define slots

class CUTEHookTypes
{
public:
    CUTEHookTypes(char *hookEventName, QObject *sender, char *signal);
    
    QString hookEventName;
    QObject *sender;
    QString signal;
};

class CUTEHook : public QObject
{
    Q_OBJECT
public:
    CUTEHook(PyObject *function, QObject *parent, const char *name = 0);
    ~CUTEHook();

    int attachHook(const QString &hookEventName);
    int attachHook(const QString &hookEventName, QObject *sender);
    // int detachHook(const QString &hookEventName);
    static void addHookType(char *hookEventName, QObject *sender, char *signal);
protected slots:
    void slotCallHook(const QString param);
    void slotCallHook();
private:
	/** wrapped python function */
	PyObject *function;
    /** parent */
    QObject *parent;
    /** hook type object list */
    static QPtrList<CUTEHookTypes> hookTypes;
};

#endif
