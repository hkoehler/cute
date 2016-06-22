/***************************************************************************
                          py_config.cpp  -  exports configs in form of modules to python
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

#include "py_config.h"
#include "python_api.h"
#include "py_buffer.h"
#include "config.h"
#include "lexer.h"
#include "shortcuts.h"

namespace Config{

ConfigModule edit, view, cute, lang;
static ConfigModule configRoot;

static PyMethodDef configModule_methods[] =
{
	{NULL, NULL, 0, NULL }
};

ConfigModule::ConfigModule(PyObject *parent, char* name)
{
	module =Py_InitModule(name, configModule_methods);
	if(parent){
		PyModule_AddObject(parent, name, module);
	}
}

ConfigModule &ConfigModule::add(char *name, PyObject *obj)
{
	Py_INCREF(obj);
	PyModule_AddObject(module, name, obj);
	return *this;
}

ConfigModule &ConfigModule::add(char *name, const char* str)
{
	PyObject *obj = PyString_FromString(str);
	Py_INCREF(obj);
	PyModule_AddObject(module, name, obj);
	return *this;
}

ConfigModule &ConfigModule::add(char *name, int value, API::EnumModule *_enum)
{
	PyObject *obj = PyInt_FromLong(value);
	Py_INCREF(obj);
	PyModule_AddObject(module, name, obj);
	enum_map[std::string(name)] = _enum;
	return *this;
}

int ConfigModule::getInteger(char *name)
{
	int i;

	PyObject *dict = PyModule_GetDict(module);
	Py_INCREF(dict);
	PyObject *item = PyDict_GetItemString(dict, name);
	Py_DECREF(dict);
	if(item){
		i = PyInt_AsLong(item);
		return i;
	}
	else{
		qWarning("no integer %s found\n", name);
		return 0;
	}
}

char *ConfigModule::getString(char *name)
{
	PyObject *dict = PyModule_GetDict(module);
	Py_INCREF(dict);
	PyObject *item = PyDict_GetItemString(dict, name);
	Py_DECREF(dict);
	if(item){
		return PyString_AsString(item);
	}
	else{
		qWarning("no string %s found\n", name);
		return 0;
	}
}

int ConfigModule::setInteger(char *name, int value)
{
	PyObject *dict = PyModule_GetDict(module);
	Py_INCREF(dict);
	PyObject *item = PyInt_FromLong(value);
	Py_DECREF(dict);
	return PyDict_SetItemString(dict, name, item);
}

int ConfigModule::setString(char *name, const char *str)
{
	PyObject *dict = PyModule_GetDict(module);
	Py_INCREF(dict);
	PyObject *item = PyString_FromString(str);
	Py_DECREF(dict);
	return PyDict_SetItemString(dict, name, item);
}

PyObject *ConfigModule::dict()
{
	PyObject *dict = PyModule_GetDict(module);
	Py_INCREF(dict);
	return dict;
}

char *ConfigModule::name()
{
	return PyModule_GetName(module);
}

void initconfig()
{
	configRoot = ConfigModule(0, "config");

	cute = ConfigModule(configRoot, "general");
	cute.add("maxRecentFiles", 20)
		.add("theme", Config::Default, &API::themeEnum)
		.add("maxSelectedStringLengthInDialog", 100)
		.add("mdiMode", Config::ChildframeMode, &API::mdiModeEnum)
		.add("showMessagePanel", true)
		.add("showSidePanel", true)
		.add("docPath", DOC_PATH);
	edit = ConfigModule(configRoot, "edit");
	edit.add("backspaceUnindents", false)
		.add("indentationsUseTabs", true)
		.add("tabWidth", 4)
		.add("lineFeed", QextScintilla::EolUnix, &API::eolModeEnum)
		.add("tabIndents", true)
		.add("autoCompletionThreshold", 5)
		.add("autoCompletionSource", CUTEScintilla::Document, &API::autoCompletionEnum)
		.add("autoCompletionCaseSensitivity", true)
		.add("autoCompletionReplaceWord", true)
		.add("useUtf8Encoding", false)
		.add("autoIndent", true);
		//.add("autoIndentStyle", AiMaintain, &API::autoIndentStyleEnum);
	view = ConfigModule(configRoot, "view");
	view.add("folding", QextScintilla::PlainFoldStyle, &API::foldStyleEnum)
		.add("eolVisibility", false)
		.add("whitespaces", QextScintilla::WsVisible, &API::whitespaceVisibilityEnum)
		.add("margin", true)
		.add("foldMargin", true)
		.add("toggleAllFolds", true)
		.add("lineNumbers", false)
		.add("cmdToolbar", true)
		.add("toolbar", true)
		.add("statusbar", true)
		.add("taskbar", true)
		.add("braceMatching", QextScintilla::StrictBraceMatch, &API::braceMatchEnum)
		.add("wrapLines", false)
		.add("monospacedFont", "Courier,10,-1,5,50,0,0,0,0,0")
		.add("defaultFont", "Courier,10,-1,5,50,0,0,0,0,0")
		.add("zoomLevel", 0)
		.add("fullScreenShowToolBar", true)
		.add("fullScreenShowMenuBar", true)
		.add("fullScreenShowCommandBox", true)
		.add("indentationsGuide", true);
	lang = ConfigModule(configRoot, "lang");
	LexerManager::initLexers();
}

}	// namespace Config
