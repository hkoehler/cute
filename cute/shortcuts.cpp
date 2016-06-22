/***************************************************************************
                          shortcuts.cpp  -  defines standart shortcuts and manage them
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

#include <qobjectlist.h>

#include "shortcuts.h"

PyAction::PyAction( PyObject *f, QObject *parent, const char *name ) 
	: function(f), QAction(parent, name)
{
	connect(this, SIGNAL(activated()), this, SLOT(execPyFunction()));
}

void PyAction::execPyFunction()
{
	if( !PyCallable_Check(function) ){
		qWarning( "object not callable" );
		return;
	}
	if(!PyObject_CallObject(function, NULL))
		qWarning("error calling object");
}

/** stores all corresponding global action with edit action */
QPtrList<Shortcuts::ActionPair> Shortcuts::actionPairs;

/** global action must have the same shortcat as edit action */
void Shortcuts::pairActions(QAction *globalAction, QAction *editAction)
{
	ActionPair *pair = new ActionPair;

	pair->globalAction = globalAction;
	pair->editAction = editAction;
	actionPairs.append(pair);
}

/** synchronize all global actions with edit actions */
void Shortcuts::synchronizeActions()
{
	// each global shortcut is equal with its (when exists) corresponding edit shortcut
	ActionPair *ap;
	for(ap = actionPairs.first(); ap; ap = actionPairs.next() )
		ap->globalAction->setAccel(ap->editAction->accel());
}

/** retrieves all global actions */
QPtrList<QAction> *Shortcuts::globalActions()
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	QPtrList<QAction> *actions = new QPtrList<QAction>;
	const QObjectList *list = cute->queryList("QAction");
	QObject *obj;

	QObjectListIterator it(*list);
	while( (obj = it.current()) != 0 ){
		actions->append(static_cast<QAction*>(it.current()));
		++it;
	}
	delete it;
	return actions;
}

/** retrieves all edit actions */
QPtrList<QAction> *Shortcuts::editActions()
{
	QPtrList<QAction> *actionList = new QPtrList<QAction>();
	QMap<int, QAction*> *editActions = &static_cast<CUTE*>(qApp->mainWidget())->editActions;

	for( QMap<int, QAction*>::iterator i = editActions->begin(); i != editActions->end(); ++i )
		actionList->append(i.data());
	
	return actionList;
}

/** retrieves a list of global actions, which contains an edit shortcut */
QPtrList<QAction> *Shortcuts::editGlobalActions()
{
	ActionPair *actionPair;
	QPtrList<QAction> *list = new QPtrList<QAction>;
	
	for(actionPair = actionPairs.first(); actionPair; actionPair = actionPairs.next())
		list->append(actionPair->globalAction);
	
	return list;
}

/** retrieves all global actions without action, which contains an edit shortcut*/
QPtrList<QAction> *Shortcuts::globalActionsWithoutEditActions()
{
	QPtrList<QAction> *global_actions = globalActions();
	QPtrList<QAction> *edit_global_actions = editGlobalActions();
	QPtrList<QAction> *list = new QPtrList<QAction>;
	QAction *action;
	
	for( action = global_actions->first(); action; action = global_actions->next() )
		if( !edit_global_actions->contains(action) )  // when action edit action then leave it out
			list->append(action);
			
	return list;
}

/** retrieves all actions */
QPtrList<QAction> *Shortcuts::actions()
{
	QPtrList<QAction> *edit_actions = editActions();
	QPtrList<QAction> *global_actions = globalActionsWithoutEditActions();
	QPtrList<QAction> *list = new QPtrList<QAction>;

	QAction *action;
	for( action = global_actions->first(); action; action = global_actions->next() )
		list->append(action);
	for( action = edit_actions->first(); action; action = edit_actions->next() )
		list->append(action);
	
	return list;
}
