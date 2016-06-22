/***************************************************************************
                          py_config.h
                             -------------------
    begin                : Mon Feb 10 2003
    copyright          : (C) 2003 by Heiko Köhler
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
 
#ifndef __PY_CONFIG_H__
#define __PY_CONFIG_H__
 
#undef slots
#include <Python.h>
#define slots
#include <map>
#include <string>

namespace API{
	class EnumModule;
}

namespace Config
{
	/** python module wrapper for saving settings */
	class ConfigModule
	{
	public:
		ConfigModule() {}
		/** creates a ConfigModule as child or parent with given name */
		ConfigModule( PyObject *parent, char *name );
		/** adds a python object and return itself */
		ConfigModule &add( char *name, PyObject *obj);
		/** adds a string option and return itself */
		ConfigModule &add( char *name, const char *str);
		/** adds an integer option which is a field of _enum */
		ConfigModule &add( char *name, int value, API::EnumModule *_enum=0);
		/** retrieves integer from config module */
		int getInteger(char *name);
		/** retrieves string from config module */
		char *getString(char *name);
		/** sets integer in config module */
		int setInteger(char *name, int value);
		/** sets string in config module */
		int setString(char *name, const char *str);
		/** returns dictionary of wrapped python module */
		PyObject *dict();
		/** returns name of config module */
		char *name();
		/** casts config module to python object */
		operator PyObject*() { return module; }
		/** returns enum module of given option */
		API::EnumModule *getEnum(std::string name) { return enum_map[name]; }
	private:
		PyObject *module;
		std::map<std::string, API::EnumModule*> enum_map;
	};
	
	extern ConfigModule edit, view, cute, lang;
	
	extern "C" void initconfig();
}

#endif
