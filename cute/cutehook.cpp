/***************************************************************************
                          cutehook.cpp  -  description
                             -------------------
    begin                : Thusday Apr 29 16:13:53 BST 2004
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
#include "cutehook.h"

CUTEHookTypes::CUTEHookTypes(char *event, QObject *send, char *sig)
{
    hookEventName = event;
    sender = send;
    signal = sig;
}

/** static hook type object list */
QPtrList<CUTEHookTypes> CUTEHook::hookTypes;

CUTEHook::CUTEHook(PyObject *f, QObject *p, const char *n)
    : function(f), parent(p), QObject(p, n)
{
    ;
}

CUTEHook::~CUTEHook()
{
    ;
}

int CUTEHook::attachHook(const QString &hookEventName, QObject *sender)
{
    QPtrListIterator<CUTEHookTypes> it(hookTypes);
    CUTEHookTypes *hookType;
    int match = FALSE;

    while ( (hookType = it.current()) != 0 && !match) {
        if (hookType->sender == sender && hookEventName == hookType->hookEventName) {
            match = TRUE;
        } else {
            ++it;
        }
    }

    if (match) {
        QString slot = hookType->signal;
        slot.remove(0, slot.find('('));
        slot.prepend("1slotCallHook");
        if (connect(hookType->sender, hookType->signal, (const char *)slot)) {
            return 0;
        } else {
            return -1;
        }
    }
    return -1;
}

int CUTEHook::attachHook(const QString &hookEventName)
{
    QPtrListIterator<CUTEHookTypes> it(hookTypes);
    CUTEHookTypes *hookType;
    int match = FALSE;

    while ( (hookType = it.current()) != 0 && !match) {
        if (hookType->hookEventName == NULL || hookEventName == hookType->hookEventName) {
            match = TRUE;
        } else {
            ++it;
        }
    }

    if (match) {
        QString slot = hookType->signal;
        slot.remove(0, slot.find('('));
        slot.prepend("1slotCallHook");
        if (connect(hookType->sender, hookType->signal, (const char *)slot)) {
            return 0;
        } else {
            return -1;
        }
    }
    return -1;
}

/* static */
void CUTEHook::addHookType(char *hookEventName, QObject *sender, char *signal)
{
    CUTEHookTypes *hookType = new CUTEHookTypes(hookEventName, sender, signal);
    hookTypes.append(hookType);
}

void CUTEHook::slotCallHook(const QString param)
{
	if( !PyCallable_Check(function) ) {
		qWarning( "object not callable" );
		return;
	}
	if(!PyObject_CallFunction(function, "s", (const char *)param))
		qWarning("error calling object");
}

void CUTEHook::slotCallHook()
{
	if( !PyCallable_Check(function) ) {
		qWarning( "object not callable" );
		return;
	}
	if(!PyObject_CallFunction(function, ""))
		qWarning("error calling object");
}
