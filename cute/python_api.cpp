/***************************************************************************
                          python_api.cpp  -  description
                             -------------------
    begin                : Fre Nov 22 2002
    copyright            : (C) 2002 by Heiko Köhler
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


#undef slots
#include <Python.h>
#define slots
#include <qinputdialog.h>
#include <qdir.h>
#include <qextscintillacommand.h>
#include <qextscintillacommandset.h>
#include "cute.h"
#include "bookmark.h"
#include "config.h"
#include "py_buffer.h"
#include "pyqt_api.h"
#include "qcompletionbox.h"
#include "shortcuts.h"
#include "cutehook.h"

namespace API{   //avoid cluttering the global namespace

static PyMethodDef enum_methods[] =
{
	{NULL, NULL, 0, NULL }
};

EnumModule::EnumModule(PyObject *parent, char* name)
{
	module =Py_InitModule(name, enum_methods);
	Py_INCREF(module);
	PyModule_AddObject(parent, name, module);
}

EnumModule &EnumModule::add(char *name, int value)
{
	PyModule_AddIntConstant(module, name, value);
	return *this;
}

char *EnumModule::itemName(int v)
{
	int pos = 0;
	PyObject *key, *value;
	PyObject *dict = PyModule_GetDict(module);
	Py_INCREF(dict);
	while(PyDict_Next(dict, &pos, &key, &value))
		if( v == (int)PyInt_AsLong(value)){
			Py_DECREF(dict);
			return PyString_AsString(key);
		}
}

static QextScintilla *getCurrentScintilla()
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	if(!view)
		return 0;
	return view->scintilla();
}

class BitMask
{
public:
	BitMask(unsigned int m) : mask(m)
		{}
	unsigned int mask;
};

static PyObject *load( PyObject *self, PyObject *args )
{
	char *str;
	if( !PyArg_ParseTuple(args, "s", &str ) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	static_cast<CUTE*>(qApp->mainWidget())->loadFile(str);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *printOutput( PyObject *self, PyObject *args )
{
	char *str;
	if( !PyArg_ParseTuple(args, "s", &str ) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	static_cast<CUTE*>(qApp->mainWidget())->printOutput(str);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *clearOutput( PyObject *self, PyObject *args )
{
	if( !PyArg_ParseTuple(args, "") ){
		PyErr_SetString(PyExc_TypeError, "no args required");
		return NULL;
	}
	static_cast<CUTE*>(qApp->mainWidget())->clearOutput();
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *inputInteger( PyObject *self, PyObject *args )
{
	char *str;
	int i;
	if( !PyArg_ParseTuple(args, "s", &str) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	i = QInputDialog::getInteger("CUTE", str);
	return Py_BuildValue("i", i);
}

static PyObject *inputText( PyObject *self, PyObject *args )
{
	char *str;
	QString qstr;
	if( !PyArg_ParseTuple(args, "s", &str) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	qstr = QInputDialog::getText("CUTE", str);
	return Py_BuildValue("s", qstr.latin1());
}

static PyObject *message( PyObject *self, PyObject *args )
{
	char *str;
	if( !PyArg_ParseTuple(args, "s", &str) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	QMessageBox::information(qApp->mainWidget(), 0, str);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *question( PyObject *self, PyObject *args )
{
	char *str;
	if( !PyArg_ParseTuple(args, "s", &str) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	if(QMessageBox::information(qApp->mainWidget(), 0, str, QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
		return Py_BuildValue("i", false);
	else
		return Py_BuildValue("i", true);
}

static PyObject *getOpenFileName( PyObject *self, PyObject *args )
{
	char *filter=NULL;
	QString qstr;
	
	if( !PyArg_ParseTuple(args, "|s", &filter) ){
		PyErr_SetString(PyExc_TypeError, "optional string required");
		return NULL;
	}
	if(!filter)
		filter="";
	qstr = QFileDialog::getOpenFileName(QDir::currentDirPath(), QString(filter));
	return Py_BuildValue("s", qstr.latin1());
}

static PyObject *getExistingDirectory( PyObject *self, PyObject *args )
{
	QString qstr;
	
	if( !PyArg_ParseTuple(args, "") ){
		PyErr_SetString(PyExc_TypeError, "no args required");
		return NULL;
	}
	qstr = QFileDialog::getExistingDirectory(QDir::currentDirPath());
	return Py_BuildValue("s", qstr.latin1());
}

static PyObject *addRecentFile(PyObject *self, PyObject *args)
{
	char *file;
	if( !PyArg_ParseTuple(args, "s", &file) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	cute->addRecentFile(file);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *addRecentProject(PyObject *self, PyObject *args)
{
	char *file;
	if( !PyArg_ParseTuple(args, "s", &file) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	cute->addRecentProject(file);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *addBookmark(PyObject *self, PyObject *args)
{
	char *file;
	int line;
	if( !PyArg_ParseTuple(args, "si", &file, &line) ){
		PyErr_SetString(PyExc_TypeError, "string and integer required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	bookmarkManager.addBookmark(new ProjectBookmark("",file, line));
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *addTool(PyObject *self, PyObject *args)
{
	char *name, *cmd;
	
	if( !PyArg_ParseTuple(args, "ss", &name, &cmd) ){
		PyErr_SetString(PyExc_TypeError, "two strings required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	cute->addTool(name, cmd);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *addPythonTool(PyObject *self, PyObject *args)
{
	PyObject *cmd;
	
	if( !PyArg_ParseTuple(args, "O", &cmd) ){
		PyErr_SetString(PyExc_TypeError, "command/cobject required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	cute->addPythonTool((QAction*)PyCObject_AsVoidPtr(cmd));
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *loadSession(PyObject *self, PyObject *args)
{
	char *file;
	
	if( !PyArg_ParseTuple(args, "s", &file) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	cute->loadSession(file);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *output(PyObject *self, PyObject *args)
{
	char *cmd;
	if( !PyArg_ParseTuple(args, "s", &cmd) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	cute->printOutput(cmd);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *setWorkingDirectory(PyObject *self, PyObject *args)
{
	char *arg;
	if( !PyArg_ParseTuple(args, "s", &arg) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	QDir::setCurrent(arg);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *viewList(PyObject *, PyObject *args)
{
	if( !PyArg_ParseTuple(args, "") ){
		PyErr_SetString(PyExc_TypeError, "no args required");
		return NULL;
	}
	
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	PyObject *tuple = PyTuple_New(cute->windowsCount());
	WidgetList list = cute->windows();
	int i = 0;
	for( QWidget* view = list.first(); view; view=list.next() ){
		Buffer *buf = PyObject_New(Buffer, &buffer_type);
		buf->view = static_cast<CUTEView*>(view)->scintilla();
		Py_INCREF((PyObject*)buf);
		PyTuple_SetItem(tuple, i, (PyObject*)buf);
		i++;
	}
	
	return (PyObject*)tuple;
}

PyObject *activateView(PyObject *, PyObject *args)
{
	PyObject *obj;

	if( !PyArg_ParseTuple(args, "O", &obj) ){
		PyErr_SetString(PyExc_TypeError, "Buffer required");
		return NULL;
	}

	QextScintilla *sc = reinterpret_cast<Buffer*>(obj)->view;
	QWidget *view = static_cast<QWidget*>(sc->parent());
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	cute->activateWindow(view);
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *setMainWidgetGeometry(PyObject *, PyObject *args)
{
	int x, y, width, height;
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());

	if( !PyArg_ParseTuple(args, "iiii:setMainWidgetGeometry", &x, &y, &width, &height) ){
		PyErr_SetString(PyExc_TypeError, "x, y, width and height integers required");
		return NULL;
	}
	cute->setGeometry(x, y, width, height);
	Py_INCREF(Py_None);
	return Py_None;
}

/** maps an action to a shortcut */
PyObject *map(PyObject *, PyObject *args)
{
	char *action, *shortcut;
	QPtrList<QAction> *actions = Shortcuts::actions();
	QAction *a;

	if(!PyArg_ParseTuple(args, "ss", &action, &shortcut)){
		PyErr_SetString(PyExc_TypeError, "two strings required");
		return NULL;
	}
	for(a = actions->first(); a; a = actions->next())
		if(a->name() == QString(action)){
			a->setAccel(QKeySequence(shortcut));
			break;
		}
	if(!a)
		qWarning("no command %s found", action);
	
	Py_INCREF(Py_None);
	return Py_None;
}

/** creates a special QAction, which calls a python function/callable */
PyObject *createPythonAction(PyObject *, PyObject *args)
{
	PyObject *action;
	char *name;
	PyAction *py_action;

	if(!PyArg_ParseTuple(args, "Os", &action, &name)){
		PyErr_SetString(PyExc_TypeError, "callable and string required");
		return NULL;
	}
	if(!PyCallable_Check(action)){
		PyErr_SetString(PyExc_TypeError, "first argument not callable");
		return NULL;
	}
	py_action = new PyAction(action, qApp->mainWidget(), name);
	py_action->setText(name);
	py_action->setMenuText(name);
	return PyCObject_FromVoidPtr(py_action, NULL);
}

PyObject *fileExists(PyObject *, PyObject *args)
{
	char *file_name;

	if(!PyArg_ParseTuple(args, "s", &file_name)){
		PyErr_SetString(PyExc_TypeError, "callable and string required");
		return NULL;
	}
	return Py_BuildValue("i", QFile(file_name).exists());
}

static PyObject *loadProject( PyObject *self, PyObject *args )
{
	char *str;
	if( !PyArg_ParseTuple(args, "s", &str ) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	static_cast<CUTE*>(qApp->mainWidget())->slotProjectOpen(str, false);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *addInfoField( PyObject *self, PyObject *args )
{
	char *field;
	char *str;
    if( !PyArg_ParseTuple(args, "ss", &field, &str ) ){
		PyErr_SetString(PyExc_TypeError, "two strings required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
    view->addInfoField(field, str);
	Py_INCREF(Py_None);
	return Py_None;
}    

static PyObject *removeInfoField( PyObject *self, PyObject *args )
{
	char *field;
    if( !PyArg_ParseTuple(args, "s", &field ) ){
		PyErr_SetString(PyExc_TypeError, "string required");
		return NULL;
	}
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
    view->removeInfoField(field);
	Py_INCREF(Py_None);
	return Py_None;
}    

/** creates a CUTEHook, which calls a python function/callable */
static PyObject *createPythonHook(PyObject *self, PyObject *args)
{
	PyObject *cmd;
	char *hookType = NULL;

	if(!PyArg_ParseTuple(args, "O|s", &cmd, &hookType)){
		PyErr_SetString(PyExc_TypeError, "callable required, hook type optional");
		return NULL;
	}
	if(!PyCallable_Check(cmd)){
		PyErr_SetString(PyExc_TypeError, "first argument not callable");
		return NULL;
	}

	CUTEHook *pyHook = new CUTEHook(cmd, qApp->mainWidget(), NULL);
	if (pyHook && hookType) {
		pyHook->attachHook(hookType);
	}
	return PyCObject_FromVoidPtr(pyHook, NULL);
}

/** attaches a CUTEHook to a signal */
static PyObject *attachPythonHook(PyObject *self, PyObject *args)
{
	PyObject *pyHook;
    char *hookType;
	
	if( !PyArg_ParseTuple(args, "Os", &pyHook, &hookType) ){
		PyErr_SetString(PyExc_TypeError, "hook object and hook type required");
		return NULL;
	}
    CUTEHook *hook = (CUTEHook *)PyCObject_AsVoidPtr(pyHook);
	return Py_BuildValue("i", hook->attachHook(hookType));
}

static PyMethodDef cute_methods[] =
{
	{"load", load, METH_VARARGS, "Load a file into the editor."}, 
	{"printOutput", printOutput, METH_VARARGS, "Print text on output view."}, 
	{"clearOutput", clearOutput, METH_VARARGS, "Clear output view."}, 
	{"inputInteger", inputInteger, METH_VARARGS, "Get an interger value from user via a dialog."}, 
	{"inputText", inputText, METH_VARARGS, "Get a string from user via a dialog."}, 
	{"message", message, METH_VARARGS, "Show a message dialog."}, 
	{"question", question, METH_VARARGS, "Show a message dialog asking a question with yes and no button."}, 
	{"getOpenFileName", getOpenFileName, METH_VARARGS, "Show an open file dialog and returns file name: getOpenFileName(filter)."},
	{"getExistingDirectory", getExistingDirectory, METH_VARARGS, "Show an open dir dialog and returns dir name."},
	{"addRecentFile", addRecentFile, METH_VARARGS, "Add an item to recent file menu."},
	{"addRecentProject", addRecentProject, METH_VARARGS, "Add an item to recent project menu."},
	{"addBookmark", addBookmark, METH_VARARGS, "Add an item to bookmark menu."},
	{"addTool", addTool, METH_VARARGS, "Add an item to tools menu."},
	{"addPythonTool", addPythonTool, METH_VARARGS, "Add an python item to tools menu."},
	{"loadSession", loadSession, METH_VARARGS, "Load session."},
	{"output", output, METH_VARARGS, "View text in output view."},
	{"setCurrentDir", setWorkingDirectory, METH_VARARGS, "Set working directory."},
	{"viewList", viewList, METH_VARARGS, "Retrieve list with all views."},
	{"activateView", activateView, METH_VARARGS, "Activate view."},
	{"setMainWidgetGeometry", setMainWidgetGeometry, METH_VARARGS, "Set main window geometry (width, height and "
		"position."},
	{"map", map, METH_VARARGS, "Map an action to a shortcut."},
	{"fileExists", fileExists, METH_VARARGS, "Checks wether a file exists."},
	{"createCommand", createPythonAction, METH_VARARGS, "Creates a new command from a python callable."},
	{"Color", color_new, METH_VARARGS, "Construct new color object: Color(int,int,int)."},
	{"loadProject", loadProject, METH_VARARGS, "Load project."},
	{"addInfoField", addInfoField, METH_VARARGS, "Add an information field to the status bar."},
	{"removeInfoField", removeInfoField, METH_VARARGS, "Remove an information field from the status bar."},
	{"createHook", createPythonHook, METH_VARARGS, "Attaches a function to an editor event."},
	{"attachHook", attachPythonHook, METH_VARARGS, "Attach a hook object to an editor event."},
	// Qt methods are defined in the pyqt_api module
	PYQT_METHOD_DEF
	// Buffer functions are also defined in the cute module
	BUFFER_METHOD_DEF
	{NULL, NULL, 0, NULL }
};

extern "C" void initcute()
{
	PyObject *m, *d;
	
	buffer_type.ob_type = &PyType_Type;
	m = Py_InitModule("cute", cute_methods);
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());
	cute->cmdLine->registerPyModule(m);
	initEnums(m);
	d = PyModule_GetDict (m);
}

}	// namespace API
