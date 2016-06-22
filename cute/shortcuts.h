/***************************************************************************
                          shortcuts.h  -  defines standart shortcuts and manage them
                             -------------------
    begin                : Sam, 19 Jul 2003
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

#ifndef __SHORTCUTS_H__
#define __SHORTCUTS_H__

#undef slots
#include <Python.h>
#define slots
#include <qptrlist.h>
#include <qstring.h>
#include <qaction.h>

#include "cute.h"
#include "cutescintilla.h"

/** represent a command, written in python, in the QT frmework */
class PyAction : public QAction
{
	Q_OBJECT
public:
	/** constructs a python action */
	PyAction( PyObject *function, QObject *parent, const char *name );
protected slots:
	/** called when action is activated */
	void execPyFunction();
private:
	/** wrapped python function */
	PyObject *function;
};

/** manages all shortcuts, which are defined in QActions, in the app */
class Shortcuts
{
	public:
		/** global action must have the same shortcut as edit action */
		static void pairActions(QAction *globalAction, QAction *editAction);
		/** synchronize all global with edit actions */
		static void synchronizeActions();
		/** retrieves all global actions*/
		static QPtrList<QAction> *globalActions();
		/** retrieves all global actions without action, which contains an edit shortcut*/
		static QPtrList<QAction> *globalActionsWithoutEditActions();
		/** retrieves all edit actions */
		static QPtrList<QAction> *editActions();
		/** retrieves all actions */
		static QPtrList<QAction> *actions();
	protected:
		/** retrieves a list of global actions, which contains an edit shortcut */
		static QPtrList<QAction> *editGlobalActions();
	private:
		/** stores corresponding global action with edit action */
		struct ActionPair
		{
			/** action defined in main window */
			QAction *globalAction;
			/** action defined in edit views */
			QAction *editAction;
		};
		/** stores all corresponding global action with edit action */
		static QPtrList<ActionPair> actionPairs;
};

#endif
