/***************************************************************************
                          py_buffer.cpp  -  exports scintilla functions to python
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

#include <qapplication.h>
#include "mdi.h"
#include "py_buffer.h"
#include "python_api.h"
#include "config.h"
#include "cuteview.h"
#include "cutescintilla.h"

#define bufferFunc(func) 									\
PyObject *func( PyObject *self, PyObject *args )					\
{														\
	if(!PyArg_ParseTuple( args, "")){							\
		PyErr_SetString(PyExc_TypeError, "function has no args");	\
		return NULL;										\
	}													\
	if(self){												\
		Buffer *buf = (Buffer*)self;							\
		buf->view->func();									\
	}													\
	else{												\
		QextScintilla *view = getCurrentScintilla();			\
		if(view)											\
			view->func();									\
		else												\
			return NULL;									\
	}													\
	Py_INCREF(Py_None);										\
	return Py_None;										\
}

#define retBufferFunc(func, fm_str)							\
PyObject *func( PyObject *self, PyObject *args )					\
{														\
	if(!PyArg_ParseTuple( args, "")){							\
		PyErr_SetString(PyExc_TypeError, "wrong arg");			\
		return NULL;										\
	}													\
	if(self){												\
		Buffer *buf = (Buffer*)self;							\
		return Py_BuildValue( fm_str, buf->view->func());		\
	}													\
	else{												\
		QextScintilla *view = getCurrentScintilla();			\
		if(view)											\
			return Py_BuildValue( fm_str, view->func());		\
		else												\
			return NULL;									\
	}													\
}

#define bufferFuncWith1Arg(func, fm_str, arg_type)				\
PyObject *func( PyObject *self, PyObject *args )					\
{														\
	arg_type arg;											\
	if(!PyArg_ParseTuple( args, fm_str, &arg)){					\
		PyErr_SetString(PyExc_TypeError, "wrong args");			\
		return NULL;										\
	}													\
	if(self){												\
		Buffer *buf = (Buffer*)self;							\
		buf->view->func(arg);								\
	}													\
	else{												\
		QextScintilla *view = getCurrentScintilla();			\
		if(view)											\
			view->func(arg);								\
		else												\
			return NULL;									\
	}													\
	Py_INCREF(Py_None);										\
	return Py_None;										\
}

#define retBufferFuncWith1Arg(func, ret_fm_str, arg_fm_str, arg_type)	\
PyObject *func( PyObject *self, PyObject *args )						\
{															\
	arg_type arg;												\
	if(!PyArg_ParseTuple( args, arg_fm_str, &arg)){					\
		PyErr_SetString(PyExc_TypeError, "wrong args");				\
		return NULL;											\
	}														\
	if(self){													\
		Buffer *buf = (Buffer*)self;								\
		return Py_BuildValue( ret_fm_str, buf->view->func(arg));		\
	}														\
	else{													\
		QextScintilla *view = getCurrentScintilla();				\
		if(view)												\
			return Py_BuildValue( ret_fm_str, view->func(arg));		\
		else													\
			return NULL;										\
	}														\
}

#define bufferFuncWith2Arg(func, arg_fm_str, arg1_type, arg2_type)	\
PyObject *func( PyObject *self, PyObject *args )					\
{														\
	arg1_type arg1;										\
	arg2_type arg2;										\
	if(!PyArg_ParseTuple( args, arg_fm_str, &arg1, &arg2)){		\
		PyErr_SetString(PyExc_TypeError, "wrong args");			\
		return NULL;										\
	}													\
	if(self){												\
		Buffer *buf = (Buffer*)self;							\
		buf->view->func(arg1, arg2);							\
	}													\
	else{												\
		QextScintilla *view = getCurrentScintilla();			\
		if(view)											\
			view->func(arg1, arg2);							\
		else												\
			return NULL;									\
	}													\
	Py_INCREF(Py_None);										\
	return Py_None;										\
}

namespace API {

EnumModule eolModeEnum;
EnumModule mdiModeEnum;
EnumModule foldStyleEnum;
EnumModule braceMatchEnum;
EnumModule autoIndentStyleEnum;
EnumModule autoCompletionEnum;
EnumModule markerSymbolEnum;
EnumModule themeEnum;
EnumModule whitespaceVisibilityEnum;

typedef struct
{
	PyObject_HEAD
	int lineFrom, columnFrom, lineTo, columnTo;
} Selection;

static void selection_del(PyObject *self)
{
	PyObject_Del(self);
}

static PyObject *selection_getattr(PyObject *self, char *attr)
{
	Selection *s = (Selection*)self;
	if(!strcmp(attr, "lineFrom"))
		return Py_BuildValue("i", s->lineFrom);
	if(!strcmp(attr, "lineTo"))
		return Py_BuildValue("i", s->lineTo);
	if(!strcmp(attr, "columnFrom"))
		return Py_BuildValue("i", s->columnFrom);
	if(!strcmp(attr, "columnTo"))
		return Py_BuildValue("i", s->columnTo);
}

static int selection_setattr(PyObject *self, char *attr, PyObject *val)
{
	int i = (int)PyInt_AsLong(val);
	Selection *s = (Selection*)self;
	if(!strcmp(attr, "lineFrom"))
		s->lineFrom = i;
	else if(!strcmp(attr, "lineTo"))
		s->lineTo = i;
	else if(!strcmp(attr, "columnFrom"))
		s->columnFrom = i;
	else if(!strcmp(attr, "columnTo"))
		s->columnTo = i;
	return 0;
}

static PyTypeObject selection_type =
{
	PyObject_HEAD_INIT(NULL)
	0,
	"Selection",
	sizeof(Selection),
	0,
	selection_del,
	0,          /*tp_print*/
    selection_getattr,
    selection_setattr,
    0,          /*tp_compare*/
    0,          /*tp_repr*/
    0,          /*tp_as_number*/
    0,          /*tp_as_sequence*/
    0,          /*tp_as_mapping*/
    0,          /*tp_hash */
};

static PyMethodDef node_methods[] =
{
	{NULL, NULL, 0, NULL }
};

void initEnums(PyObject *module)
{
	eolModeEnum = EnumModule(module, "EolMode");
	eolModeEnum.add("EolUnix", QextScintilla::EolUnix)
			   .add("EolMac", QextScintilla::EolMac)
			   .add("EolWindows", QextScintilla::EolWindows);

	mdiModeEnum = EnumModule(module, "MdiMode");
	mdiModeEnum.add("ToplevelMode", Config::ToplevelMode)
			   .add("ChildframeMode", Config::ChildframeMode)
			   .add("TabPageMode", Config::TabPageMode);

	foldStyleEnum = EnumModule(module, "FoldStyle");
	foldStyleEnum.add("NoFoldStyle", QextScintilla::NoFoldStyle)
				 .add("PlainFoldStyle", QextScintilla::PlainFoldStyle)
				 .add("CircledFoldStyle", QextScintilla::CircledFoldStyle)
				 .add("BoxedFoldStyle", QextScintilla::BoxedFoldStyle)
				 .add("CircledTreeFoldStyle", QextScintilla::CircledTreeFoldStyle)
				 .add("BoxedTreeFoldStyle", QextScintilla::BoxedTreeFoldStyle);

	braceMatchEnum = EnumModule(module, "BraceMatch");
	braceMatchEnum.add("NoBraceMatch", QextScintilla::NoBraceMatch)
				  .add("StrictBraceMatch", QextScintilla::StrictBraceMatch)
				  .add("SloppyBraceMatch", QextScintilla::SloppyBraceMatch);

	autoIndentStyleEnum = EnumModule(module, "AutoIndentStyle");
	autoIndentStyleEnum.add("AiMaintain", QextScintilla::AiMaintain)
					   .add("AiOpening", QextScintilla::AiOpening)
					   .add("AiClosing", QextScintilla::AiClosing);

	autoCompletionEnum = EnumModule(module, "AutoCompletion");
	autoCompletionEnum.add("Document", CUTEScintilla::Document)
					  .add("TagsFile", CUTEScintilla::TagsFile);

	markerSymbolEnum = EnumModule(module, "MarkerSymbol");
	markerSymbolEnum.add("Circle", QextScintilla::Circle)
					.add("Rectangle", QextScintilla::Rectangle )
					.add("RightTriangle", QextScintilla::RightTriangle )
					.add("SmallRectangle", QextScintilla::SmallRectangle )
					.add("RightArrow", QextScintilla::RightArrow )
					.add("Invisible", QextScintilla::Invisible )
					.add("DownTriangle", QextScintilla::DownTriangle )
					.add("Minus", QextScintilla::Minus )
					.add("Plus", QextScintilla::Plus )
					.add("VerticalLine", QextScintilla::VerticalLine )
					.add("BottomLeftCorner", QextScintilla::BottomLeftCorner )
					.add("LeftSideSplitter", QextScintilla::LeftSideSplitter )
					.add("BoxedPlus", QextScintilla::BoxedPlusConnected )
					.add("BoxedMinus", QextScintilla::BoxedMinus )
					.add("BoxedMinusConnected", QextScintilla::BoxedMinusConnected )
					.add("RoundedBottomLeftCorner", QextScintilla::RoundedBottomLeftCorner )
					.add("LeftSideRoundedSplitter", QextScintilla::LeftSideRoundedSplitter )
					.add("CircledPlus", QextScintilla::CircledPlus )
					.add("CircledPlusConnected", QextScintilla::CircledPlusConnected )
					.add("CircledMinus", QextScintilla::CircledMinus )
					.add("CircledMinusConnected", QextScintilla::CircledMinusConnected )
					.add("Background", QextScintilla::Background )
					.add("ThreeDots", QextScintilla::ThreeDots )
					.add("ThreeRightArrows", QextScintilla::ThreeRightArrows );

	whitespaceVisibilityEnum = EnumModule(module, "WhitespaceVisibility");
	whitespaceVisibilityEnum.add("WsInvisible", QextScintilla::WsInvisible)
							.add("WsVisible", QextScintilla::WsVisible)
							.add("WsVisibleAfterIndent", QextScintilla::WsVisibleAfterIndent);

	themeEnum = EnumModule(module, "Theme");
	themeEnum
#ifndef QT_NO_STYLE_MOTIF
		     .add("Motif", Config::Motif )
#endif
#ifndef QT_NO_STYLE_WINDOWS
			 .add("Windows", Config::Windows )
#endif
#ifndef QT_NO_STYLE_MOTIFPLUS
			 .add("MotifPlus", Config::MotifPlus )
#endif
#ifndef QT_NO_STYLE_PLATINUM
			 .add("Platinum", Config::Platinum )
#endif
#ifndef QT_NO_STYLE_SGI
			 .add("SGI", Config::SGI )
#endif
#ifndef QT_NO_STYLE_CDE
			 .add("CDE", Config::CDE )
#endif
			 .add("Default", Config::Default );
}

static PyTypeObject color_type =
{
	PyObject_HEAD_INIT(NULL)
	0,
	"Color",
	sizeof(Color),
	0,
	0,			/*tp_del*/
	0,          /*tp_print*/
    0,			/*tp_getattro*/
    0,			/*tp_setattro*/
    0,          /*tp_compare*/
    0,          /*tp_repr*/
    0,          /*tp_as_number*/
    0,          /*tp_as_sequence*/
    0,          /*tp_as_mapping*/
    0,          /*tp_hash */
};

PyObject *color_new(PyObject *self, PyObject *args)
{
	Color *color;
	int r, g, b;
	if( !PyArg_ParseTuple(args, "iii:color_new", &r, &g, &b))
		return NULL;
	color = PyObject_New( Color, &color_type );
	color->color = QColor(r,g,b);
	Py_INCREF((PyObject*)color);
	return (PyObject*)color;
}

static QextScintilla *getCurrentScintilla()
{
	MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	if(!view)
		return 0;
	return view->scintilla();
}

bufferFunc(copy)
bufferFunc(cut)
bufferFunc(foldAll)
bufferFunc(paste)
bufferFunc(redo)
bufferFunc(undo)
bufferFunc(selectToMatchingBrace)
bufferFunc(zoomIn)
bufferFunc(zoomOut)
bufferFunc(beginUndoAction)
bufferFunc(endUndoAction)

retBufferFunc(autoIndent, "i")
retBufferFunc(backspaceUnindents, "i")
retBufferFunc(braceMatching, "i")
retBufferFunc(eolMode, "i")
retBufferFunc(eolVisibility, "i")
retBufferFunc(findNext, "i")
retBufferFunc(folding, "i")
retBufferFunc(hasSelectedText, "i")
retBufferFunc(indentationGuides, "i")
retBufferFunc(indentationsUseTabs, "i")
retBufferFunc(indentationWidth, "i")
retBufferFunc(isModified, "i")
retBufferFunc(isReadOnly, "i")
retBufferFunc(isRedoAvailable, "i")
retBufferFunc(isUndoAvailable, "i")
retBufferFunc(isUtf8, "i")
retBufferFunc(lines, "i")
retBufferFunc(length, "i")
retBufferFunc(tabIndents, "i")
retBufferFunc(tabWidth, "i")
retBufferFunc(whitespaceVisibility, "i")

bufferFuncWith1Arg(ensureLineVisible, "i", int)
bufferFuncWith1Arg(setAutoIndent, "i", int)
bufferFuncWith1Arg(setBraceMatching, "i", QextScintilla::BraceMatch)
bufferFuncWith1Arg(setBackspaceUnindents, "i", int)
bufferFuncWith1Arg(setEolMode, "i", QextScintilla::EolMode)
bufferFuncWith1Arg(setEolVisibility, "i", int)
bufferFuncWith1Arg(setFolding, "i", QextScintilla::FoldStyle)
bufferFuncWith1Arg(setIndentationGuides, "i", int)
bufferFuncWith1Arg(setIndentationsUseTabs, "i", int)
bufferFuncWith1Arg(setIndentationWidth, "i", int)
bufferFuncWith1Arg(setModified, "i", int)
bufferFuncWith1Arg(setIndentationGuidesBackgroundColor, "O", Color)
bufferFuncWith1Arg(setIndentationGuidesForegroundColor, "O", Color)
bufferFuncWith1Arg(setMarginsBackgroundColor, "O", Color)
bufferFuncWith1Arg(setMarginsForegroundColor, "O", Color)
bufferFuncWith1Arg(setMatchedBraceBackgroundColor, "O", Color)
bufferFuncWith1Arg(setMatchedBraceForegroundColor, "O", Color)
bufferFuncWith1Arg(setUnmatchedBraceBackgroundColor, "O", Color)
bufferFuncWith1Arg(setUnmatchedBraceForegroundColor, "O", Color)
bufferFuncWith1Arg(setReadOnly, "i", int)
bufferFuncWith1Arg(setTabIndents, "i", int)
bufferFuncWith1Arg(setTabWidth, "i", int)
bufferFuncWith1Arg(setUtf8, "i", int)
bufferFuncWith1Arg(setWhitespaceVisibility, "i", QextScintilla::WhitespaceVisibility)
bufferFuncWith1Arg(unindent, "i", int)
bufferFuncWith1Arg(zoomTo, "i", int)
bufferFuncWith1Arg(convertEols, "i", QextScintilla::EolMode)
bufferFuncWith1Arg(markerDeleteHandle, "i", int)
//bufferFuncWith1Arg(zoomIn, "i", int);
//bufferFuncWith1Arg(zoomOut, "i", int);

retBufferFuncWith1Arg(indentation, "i", "i", int)
retBufferFuncWith1Arg(lineLength, "i", "i", int)
retBufferFuncWith1Arg(marginLineNumbers, "i", "i", int)
retBufferFuncWith1Arg(marginSensitivity, "i", "i", int)
retBufferFuncWith1Arg(marginWidth, "i", "i", int)
retBufferFuncWith1Arg(markerDefine, "i", "i", int)
retBufferFuncWith1Arg(markerLine, "i", "i", int)

bufferFuncWith2Arg(setCursorPosition, "ii", int, int)
bufferFuncWith2Arg(setIndentation, "ii", int, int)
bufferFuncWith2Arg(setMarginLineNumbers, "ii", int, int)
bufferFuncWith2Arg(setMarginSensitivity, "ii", int, int)
bufferFuncWith2Arg(setMarginWidth, "ii", int, int)
bufferFuncWith2Arg(setMarkerBackgroundColor, "Oi", Color, int)
bufferFuncWith2Arg(setMarkerForegroundColor, "Oi", Color, int)

PyObject *append( PyObject *self, PyObject *args )
{
	char *str;
	int line=-1, index=-1;
	if( !PyArg_ParseTuple(args, "s|ii", &str, &line, &index ) )
		return NULL;
	MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	if(self){
		if(line==-1)
			reinterpret_cast<QextScintilla*>(self)->getCursorPosition(&line, &index);
		reinterpret_cast<QextScintilla*>(self)->insertAt(str, line, index);
	}
	else if( view ){
		if(line==-1)
			static_cast<CUTEView*>(view)->scintilla()->getCursorPosition(&line, &index);
		static_cast<CUTEView*>(view)->scintilla()->insertAt(str, line, index);
	}
	else
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *find(PyObject *self, PyObject *args)
{
	char *str;
	bool ret;
	int re=false, cs=false, wo=false, wrap=false, forward=true;
	int line=0, index=0;
	if( !PyArg_ParseTuple(args, "s|iiiiiii", &str, &re, &cs, &wo, &wrap, &forward, &line, &index))
		return NULL;
	MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	if(self)
		ret = reinterpret_cast<QextScintilla*>(self)->findFirst( str, re, cs, wo, wrap, forward, line, index );
	else if( view )
		ret = static_cast<CUTEView*>(view)->scintilla()->findFirst( str, re, cs, wo, wrap, forward, line, index );
	else
		return NULL;
	return Py_BuildValue("i", ret);
}

PyObject *currentLine(PyObject *self, PyObject *args)
{
	int line, index;
	if( !PyArg_ParseTuple(args, "" ))
		return NULL;
	QextScintilla *view = getCurrentScintilla();
	if(self)
		reinterpret_cast<QextScintilla*>(self)->getCursorPosition(&line, &index);
	else if(view)
		view->getCursorPosition(&line, &index);
	else
		return NULL;
	return Py_BuildValue( "i", line );
}

PyObject *currentIndex(PyObject *self, PyObject *args)
{
	int line, index;
	if( !PyArg_ParseTuple(args, "" ))
		return NULL;
	QextScintilla *view = getCurrentScintilla();
	if(self)
		reinterpret_cast<QextScintilla*>(self)->getCursorPosition(&line, &index);
	else if(view)
		view->getCursorPosition(&line, &index);
	else
		return NULL;
	return Py_BuildValue( "i", index );
}

PyObject *selection(PyObject *self, PyObject *args)
{
	int lineFrom, lineTo, columnFrom, columnTo;
	if( !PyArg_ParseTuple(args, "" ))
		return NULL;
	QextScintilla *view = getCurrentScintilla();
	Selection *sel =  PyObject_New(Selection, &selection_type);
	if(self)
		reinterpret_cast<QextScintilla*>(self)->getSelection(&lineFrom, &columnFrom, &lineTo, &columnTo);
	else if(view)
		view->getSelection(&lineFrom, &columnFrom, &lineTo, &columnTo);
	else
		return NULL;
	sel->lineFrom = lineFrom;
	sel->lineTo = lineTo;
	sel->columnFrom = columnFrom;
	sel->columnTo = columnTo;
	return (PyObject*)sel;
}

PyObject *selectedText(PyObject *self, PyObject *args)
{
	QString text;
	
	if(!PyArg_ParseTuple( args, "")){
		PyErr_SetString(PyExc_TypeError, "function has no args");
		return NULL;
	}
	QextScintilla *view = getCurrentScintilla();
	if(self)
		text = reinterpret_cast<QextScintilla*>(self)->selectedText();
	else if(view)
		text = view->selectedText();
	else
		return NULL;
	if( text.isEmpty() )
		text = "";
	return Py_BuildValue("s", text.latin1());
}

PyObject *moveCursor(PyObject *self, PyObject *args)
{
	int left, down;
	int line, index;
	QextScintilla *view;
	
	if(!PyArg_ParseTuple( args, "ii", &left, &down))
		return NULL;
	view = getCurrentScintilla();
	if(self)
		view = reinterpret_cast<QextScintilla*>(self);
	if(!view)
		return NULL;
	
	view->getCursorPosition(&line, &index);
	view->setCursorPosition(line+down, index+left);
	
	return Py_None;
}

PyObject *line(PyObject *self, PyObject *args)
{
	int line;
	const char *ret;
	if( !PyArg_ParseTuple(args, "i", &line ))
		return NULL;
	QextScintilla *view = getCurrentScintilla();
	if(self)
		ret = reinterpret_cast<QextScintilla*>(self)->text(line).latin1();
	else if(view)
		ret = view->text(line).latin1();
	else
		return NULL;
	return Py_BuildValue("s", ret);
}

PyObject *setMarginStringWidth(PyObject *self, PyObject *args)
{
	int margin;
	char *str;
	if( !PyArg_ParseTuple(args, "is", &margin, &str) )
		return NULL;
	QextScintilla *view = getCurrentScintilla();
	if(self)
		reinterpret_cast<QextScintilla*>(self)->setMarginWidth(margin, str);
	else if(view)
		view->setMarginWidth(margin, str);
	else
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *setSelection(PyObject *self, PyObject *args)
{
	int lineFrom, columnFrom, lineTo, columnTo;
	if( !PyArg_ParseTuple(args, "iiii", &lineFrom, &columnFrom, &lineTo, &columnTo) )
		return NULL;
	QextScintilla *view = getCurrentScintilla();
	if(self)
		reinterpret_cast<QextScintilla*>(self)->setSelection(lineFrom, columnFrom, lineTo, columnTo);
	else if(view)
		view->setSelection(lineFrom, columnFrom, lineTo, columnTo);
	else
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *insertCommand(PyObject* self, PyObject *args)
{
	char *cmd;
	if( !PyArg_ParseTuple(args, "s", &cmd) )
		return NULL;
	MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	if(self){
		QextScintilla *sc = reinterpret_cast<QextScintilla*>(self);
		reinterpret_cast<CUTEView*>(sc->parent())->insertCommand(cmd);
	}
	else if(view)
		view->insertCommand(cmd);
	else
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *filter(PyObject *self, PyObject *args)
{
	char *cmd;
	if( !PyArg_ParseTuple(args, "s", &cmd) )
		return NULL;
	MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	if(self){
		QextScintilla *sc = reinterpret_cast<QextScintilla*>(self);
		reinterpret_cast<CUTEView*>(sc->parent())->insertCommand(cmd);
	}
	else if(view)
		view->filter(cmd);
	else
		return NULL;
	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *fileName(PyObject *self, PyObject *args)
{
	if( !PyArg_ParseTuple(args, "") )
		return NULL;
	MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
	CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
	if(self){
		QextScintilla *sc = reinterpret_cast<QextScintilla*>(self);
		return Py_BuildValue("s", reinterpret_cast<CUTEView*>(sc->parent())->fileName());
	}
	else if(view)
		return Py_BuildValue("s", view->fileName());
	else
		return NULL;
}

PyObject *clear(PyObject *self, PyObject *args)
{
	if( !PyArg_ParseTuple(args, "") )
		return NULL;

	if(self){
		Buffer *buf = (Buffer*)self;
		static_cast<CUTEView*>(buf->view->parent())->clear();
	}
	else{
		MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
		CUTEView *view = static_cast<CUTEView*>(cute->activeWindow());
		if(view)
			view->clear();
		else
			return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef buffer_methods[] =
{
	BUFFER_METHOD_DEF
	{NULL, NULL, 0, NULL}
};

void buffer_del(PyObject *self)
{
	PyObject_Del(self);
}

PyObject *buffer_getattr(PyObject *self, char *attr)
{
	return Py_FindMethod(buffer_methods, self, attr);
}

int buffer_setattr(PyObject *self, char *attr, PyObject *val)
{
	return -1;
}

PyTypeObject buffer_type =
{
	PyObject_HEAD_INIT(NULL)
	0,
	"Buffer",
	sizeof(Buffer),
	0,
	buffer_del,
	0,          /*tp_print*/
    buffer_getattr,
    buffer_setattr,
    0,          /*tp_compare*/
    0,          /*tp_repr*/
    0,          /*tp_as_number*/
    0,          /*tp_as_sequence*/
    0,          /*tp_as_mapping*/
    0,          /*tp_hash */
};

}	// namespace API
