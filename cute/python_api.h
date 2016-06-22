/***************************************************************************
                          python_api.h  -  main file of python API
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

#ifndef __PYTHON_API_H__
#define __PYTHON_API_H__

#undef slots
#include <Python.h>
#define slots

/** namespace for python API */
namespace API
{
	/** wraps a python module which is used as enum */
	class EnumModule
	{
	public:
		EnumModule() {}
		EnumModule( PyObject *parent, char *name );
		/** add const with given name and value */
		EnumModule &add( char *name, int value);
		/** retrieves name of item */
		char *itemName(int value);
		/** retrieves name */
		char *name() { return PyModule_GetName(module); }
		/** cast operator to PyObject */
		operator PyObject*() { return module; }
	private:
		PyObject *module;
	};

	PyObject *activateView(PyObject *, PyObject *args);
	PyObject *viewList(PyObject *, PyObject *args);
	extern "C" void initcute();
}

#endif 
