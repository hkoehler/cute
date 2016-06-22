/***************************************************************************
                          py_buffer.h 
                             -------------------
    begin                : Fri  Feb  7 2003
    copyright       : (C) 2003 by Heiko Köhler
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
 
#ifndef __PYBUFFER_H__
#define __PYBUFFER_H__

#include <qextscintilla.h>
#undef slots
#include <Python.h>
#define slots

#include "python_api.h"

namespace API{

void initEnums(PyObject *module);

/** python type for a CUTE view */
struct Buffer 
{
	PyObject_HEAD
	QextScintilla *view;
};

/** python type for a color */
struct Color
{
	PyObject_HEAD
	QColor color;
	operator QColor() { return color; }
};

extern EnumModule eolModeEnum;
extern EnumModule mdiModeEnum;
extern EnumModule foldStyleEnum;
extern EnumModule braceMatchEnum;
extern EnumModule autoIndentStyleEnum;
extern EnumModule autoCompletionEnum;
extern EnumModule markerSymbolEnum;
extern EnumModule themeEnum;
extern EnumModule whitespaceVisibilityEnum;

extern PyTypeObject buffer_type;

#define BUFFER_METHOD_DEF \
	{"clear", clear, METH_VARARGS,\
	"Deletes all the text in the text edit." }, \
	{"copy", copy, METH_VARARGS, \
	"Copies any selected text to the clipboard." }, \
	{"cut", cut, METH_VARARGS, \
	"Copies any selected text to the clipboard and then deletes the text." }, \
	{"foldAll", foldAll, METH_VARARGS,\
	"If any lines are currently folded then they are all unfolded. Otherwise all lines are folded. This has the same effect as clicking in the fold margin with the shift and control keys pressed." }, \
	{"paste", paste, METH_VARARGS,\
	"Pastes any text from the clipboard into the text edit at the current cursor position." }, \
	{"redo", redo, METH_VARARGS,\
	"Redo the last change or sequence of changes." }, \
	{"selectToMatchingBrace", selectToMatchingBrace, METH_VARARGS,\
	"If the cursor is either side of a brace character then move it to the position of the corresponding brace and select the text between the braces." }, \
	{"undo", undo, METH_VARARGS,\
	"Undo the last change or sequence of changes." }, \
	{"zoomIn", zoomIn, METH_VARARGS, \
	"Zooms in on the text by by making the base font size one point larger and recalculating all font sizes." }, \
	{"zoomOut", zoomOut, METH_VARARGS, \
	"Zooms out on the text by by making the base font size range points smaller and recalculating all font sizes." }, \
	{"beginUndoAction", beginUndoAction, METH_VARARGS, \
	"Mark the beginning of a sequence of actions that can be undone by a single call to undo()." }, \
	{"endUndoAction", endUndoAction, METH_VARARGS, \
	"Mark the end of a sequence of actions that can be undone by a single call to undo()." }, \
	{"autoIndent", autoIndent, METH_VARARGS, \
	"Returns 1 if auto-indentation is enabled." }, \
	{"backspaceUnindents", backspaceUnindents, METH_VARARGS, \
	"Returns 1 if the backspace key unindents a line instead of deleting a character." }, \
	{"braceMatching", braceMatching, METH_VARARGS, \
	"Returns the brace matching mode." }, \
	{"eolMode", eolMode, METH_VARARGS, \
	"Returns the end-of-line mode." }, \
	{"eolVisibility", eolVisibility, METH_VARARGS, \
	"Returns the visibility of end-of-lines." }, \
	{"findNext", findNext, METH_VARARGS, \
	"Find the next occurence of the string found using findFirst()" }, \
	{"folding", folding, METH_VARARGS, \
	"Returns the current folding style." }, \
	{"hasSelectedText", hasSelectedText, METH_VARARGS, \
	"Returns 1 if some text is selected." }, \
	{"indentationGuides", indentationGuides, METH_VARARGS, \
	"Returns 1 if the display of indentation guides is enabled." }, \
	{"indentationsUseTabs", indentationsUseTabs, METH_VARARGS, \
	"Returns 1 if indentations are created using tabs and spaces, rather than just spaces." }, \
	{"indentationWidth", indentationWidth, METH_VARARGS, \
	"Returns the indentation width in characters. The default is 0 which means that the value returned by tabWidth() is actually used." }, \
	{"isModified", isModified, METH_VARARGS, \
	"Returns 1 if the text has been modified." }, \
	{"isReadOnly", isReadOnly, METH_VARARGS, \
	"Returns 1 if the text edit is read-only." }, \
	{"isRedoAvailable", isRedoAvailable, METH_VARARGS, \
	"Returns 1 if there is something that can be redone." }, \
	{"isUndoAvailable", isUndoAvailable, METH_VARARGS, \
	"Returns 1 if there is something that can be undone." }, \
	{"isUtf8", isUtf8, METH_VARARGS, \
	"Returns 1 if text is interpreted as being UTF8 encoded. The default is to interpret the text as Latin1 encoded." }, \
	{"lines", lines, METH_VARARGS, \
	"Returns the number of lines of text." }, \
	{"length", length, METH_VARARGS, \
	"Returns the length of the text edit's text." }, \
	{"tabIndents", tabIndents, METH_VARARGS, \
	"Returns 1 if the tab key indents a line instead of inserting a tab character." }, \
	{"tabWidth", tabWidth, METH_VARARGS, \
	"Returns the tab width in characters." }, \
	{"whitespaceVisibility", whitespaceVisibility, METH_VARARGS, \
	"Returns the visibility of whitespace." }, \
	{"ensureLineVisible", ensureLineVisible, METH_VARARGS, \
	"Ensures that the line number line is visible." }, \
	{"setAutoIndent", setAutoIndent, METH_VARARGS, \
	"If autoindent is 1 then auto-indentation is enabled." }, \
	{"setBackspaceUnindents", setBackspaceUnindents, METH_VARARGS, \
	"If deindent is 1 then the backspace key will unindent a line rather then delete a character." }, \
	{"setEolMode", setEolMode, METH_VARARGS, \
	"Sets the end-of-line mode to mode." }, \
	{"setEolVisibility", setEolVisibility, METH_VARARGS, \
	"If visible is 1 then end-of-lines are made visible." }, \
	{"setFolding", setFolding, METH_VARARGS, \
	"Sets the folding style for margin 2 to fold." }, \
	{"setIndentationGuidesBackgroundColor", setIndentationGuidesBackgroundColor, METH_VARARGS, \
	"Set the background colour of indentation guides." }, \
	{"setIndentationGuidesForegroundColor", setIndentationGuidesForegroundColor, METH_VARARGS, \
	"Set the foreground colour of indentation guides." }, \
	{"setMarginsForegroundColor", setMarginsForegroundColor, METH_VARARGS, \
	"Set the foreground color of all margins" }, \
	{"setMarginsBackgroundColor", setMarginsBackgroundColor, METH_VARARGS, \
	"Set the background color of all margins" }, \
	{"setMarkerBackgroundColor", setMarkerBackgroundColor, METH_VARARGS, \
	"Set the background color of given marker mnr. If mnr is -1 then the color of all markers is set." }, \
	{"setMarkerForegroundColor", setMarkerForegroundColor, METH_VARARGS, \
	"Set the foreground color of given marker mnr. If mnr is -1 then the color of all markers is set." }, \
	{"setMatchedBraceBackgroundColor", setMatchedBraceBackgroundColor, METH_VARARGS, \
	"Set the background color used to display matched braces." }, \
	{"setMatchedBraceForegroundColor", setMatchedBraceForegroundColor, METH_VARARGS, \
	"Set the foreground color used to display matched braces." }, \
	{"setUnmatchedBraceBackgroundColor", setUnmatchedBraceBackgroundColor, METH_VARARGS, \
	"Set the background color used to display unmatched braces." }, \
	{"setUnmatchedBraceForegroundColor", setUnmatchedBraceForegroundColor, METH_VARARGS, \
	"Set the foreground color used to display unmatched braces." }, \
	{"setIndentationGuides", setIndentationGuides, METH_VARARGS, \
	"Enables or disables this display of indentation guides." }, \
	{"setIndentationsUseTabs", setIndentationsUseTabs, METH_VARARGS, \
	"If tabs is 1 then indentations are created using tabs and spaces, rather than just spaces." }, \
	{"setIndentationWidth", setIndentationWidth, METH_VARARGS, \
	"Sets the indentation width. If width is 0 then the value returned by tabWidth() is used." }, \
	{"setModified", setModified, METH_VARARGS, \
	"Sets the modified state of the text edit. Note that it is only possible to clear the modified state. Attempts to set the modified state are ignored." }, \
	{"setReadOnly", setReadOnly, METH_VARARGS, \
	"Sets the read-only state of the text edit" }, \
	{"setTabIndents", setTabIndents, METH_VARARGS, \
	"If indent is 1 then the tab key will indent a line rather then insert a tab character." }, \
	{"setTabWidth", setTabWidth, METH_VARARGS, \
	"Sets the tab width." }, \
	{"setUtf8", setUtf8, METH_VARARGS, \
	"Sets the current text encoding. If cp is 1 then UTF8 is used, otherwise Latin1 is used." }, \
	{"setWhitespaceVisibility", setWhitespaceVisibility, METH_VARARGS, \
	"Sets the visibility of whitespace." }, \
	{"unindent", unindent, METH_VARARGS, \
	"Decreases the indentation of line line." }, \
	{"zoomTo", zoomTo, METH_VARARGS, \
	"Zooms the text by making the base font size given points and recalculating all font sizes." }, \
	{"convertEol", convertEols, METH_VARARGS, \
	"All the lines of the text have their end-of-lines converted to given mode." }, \
	{"markerDeleteHandle", markerDeleteHandle, METH_VARARGS, \
	"Delete the the marker instance with the marker given handle." }, \
	{"zoomIn", zoomIn, METH_VARARGS, \
	"Zooms in on the text by by making the base font size one point larger and recalculating all font sizes." }, \
	{"zoomOut", zoomOut, METH_VARARGS, \
	"Zooms out on the text by by making the base font size range points smaller and recalculating all font sizes." }, \
	{"indentation", indentation, METH_VARARGS, \
	"Returns the number of characters that given line is indented by." }, \
	{"lineLength", lineLength, METH_VARARGS, \
	"Returns the length of given line or -1 if there is no such line." }, \
	{"marginLineNumbers", marginLineNumbers, METH_VARARGS, \
	"Returns 1 if line numbers are enabled for given margin." }, \
	{"marginSensitivity", marginSensitivity, METH_VARARGS, \
	"Returns 1 if given margin is sensitive to mouse clicks." }, \
	{"marginWidth", marginWidth, METH_VARARGS, \
	"Returns the width in pixels of given margin." }, \
	{"markerDefine", markerDefine, METH_VARARGS, \
	"Define a marker using the given symbol with the given marker number. If mnr is -1 then the marker number is automatically allocated." }, \
	{"markerLine", markerLine, METH_VARARGS, \
	"Return the line number that contains the marker instance with the given marker handle." }, \
	{"setCursorPosition", setCursorPosition, METH_VARARGS, \
	"Sets the cursor to the given line at the given position index." }, \
	{"setIndentation", setIndentation, METH_VARARGS, \
	"Sets the indentation of line to given characters." }, \
	{"setMarginLineNumbers", setMarginLineNumbers, METH_VARARGS, \
	"Enables or disables, according to second arg, the display of line numbers in given margin." }, \
	{"setMarginSensitivity", setMarginSensitivity, METH_VARARGS, \
	"Enables or disables, according to second arg, the sensitivity of given margin to mouse clicks." }, \
	{"setMarginWidth", setMarginWidth, METH_VARARGS, \
	"Sets the width of given margin to given pixels. If the width of a margin is 0 then it is not displayed. " },\
	{"insert", append, METH_VARARGS, \
	"inserts text at current position" },\
	{"find", find, METH_VARARGS, \
	"Finds a string in current view: find(string, int regExp, int caseSens, int wholeWords, int wrap, int forward, int line, int index)"},\
	{"currentLine", currentLine, METH_VARARGS, \
	"Returns current line."},\
	{"currentColumn", currentIndex, METH_VARARGS, \
	"Returns current index."},\
	{"selection", selection, METH_VARARGS, \
	"returns current selection"},\
	{"selectedText", selectedText, METH_VARARGS, \
	"returns selected text"},\
	{"moveCursor", moveCursor, METH_VARARGS, \
	"moves cursor, negative values can also be used: moveCursor(int left, int down)"},\
	{"line", line, METH_VARARGS, \
	"Returns line at given line number."},\
	{"setMarginStringWidth", setMarginStringWidth, METH_VARARGS, \
	"Sets margin n width with given string: setMarginStringWidth(int,str)"},\
	{"setSelection", setSelection, METH_VARARGS, \
	"Sets selection: setSelection(int,int,int,int)"},\
	{"insertCommand", insertCommand, METH_VARARGS, \
	"Inserts output of a shell command."},\
	{"filter", filter, METH_VARARGS, \
	"Filters selection with a shell command."},\
	{"fileName", fileName, METH_VARARGS, \
	"Retrieves file name of view."},

PyObject *color_new(PyObject *self, PyObject *args);

PyObject *clear(PyObject *self, PyObject *args);
PyObject *copy(PyObject *self, PyObject *args);
PyObject *cut(PyObject *self, PyObject *args);
PyObject *foldAll(PyObject *self, PyObject *args);
PyObject *paste(PyObject *self, PyObject *args);
PyObject *redo(PyObject *self, PyObject *args);
PyObject *selectToMatchingBrace(PyObject *self, PyObject *args);
PyObject *selectAll(PyObject *self, PyObject *args);
PyObject *undo(PyObject *self, PyObject *args);
PyObject *zoomIn(PyObject *self, PyObject *args);
PyObject *zoomOut(PyObject *self, PyObject *args);
PyObject *beginUndoAction(PyObject *self, PyObject *args);
PyObject *endUndoAction(PyObject *self, PyObject *args);

PyObject *autoIndent(PyObject *self, PyObject *args);
PyObject *backspaceUnindents(PyObject *self, PyObject *args);
PyObject *braceMatching(PyObject *self, PyObject *args);
PyObject *eolMode(PyObject *self, PyObject *args);
PyObject *eolVisibility(PyObject *self, PyObject *args);
PyObject *findNext(PyObject *self, PyObject *args);
PyObject *folding(PyObject *self, PyObject *args);
PyObject *hasSelectedText(PyObject *self, PyObject *args);
PyObject *indentationGuides(PyObject *self, PyObject *args);
PyObject *indentationsUseTabs(PyObject *self, PyObject *args);
PyObject *indentationWidth(PyObject *self, PyObject *args);
PyObject *isModified(PyObject *self, PyObject *args);
PyObject *isReadOnly(PyObject *self, PyObject *args);
PyObject *isRedoAvailable(PyObject *self, PyObject *args);
PyObject *isUndoAvailable(PyObject *self, PyObject *args);
PyObject *isUtf8(PyObject *self, PyObject *args);
PyObject *lines(PyObject *self, PyObject *args);
PyObject *length(PyObject *self, PyObject *args);
PyObject *tabIndents(PyObject *self, PyObject *args);
PyObject *tabWidth(PyObject *self, PyObject *args);
PyObject *whitespaceVisibility(PyObject *self, PyObject *args);

PyObject *ensureLineVisible(PyObject *self, PyObject *args);
PyObject *setAutoIndent(PyObject *self, PyObject *args);
PyObject *setBraceMatching(PyObject *self, PyObject *args);
PyObject *setBackspaceUnindents(PyObject *self, PyObject *args);
PyObject *setEolMode(PyObject *self, PyObject *args);
PyObject *setEolVisibility(PyObject *self, PyObject *args);
PyObject *setFolding(PyObject *self, PyObject *args);
PyObject *setIndentationGuides(PyObject *self, PyObject *args);
PyObject *setIndentationsUseTabs(PyObject *self, PyObject *args);
PyObject *setIndentationWidth(PyObject *self, PyObject *args);
PyObject *setModified(PyObject *self, PyObject *args);
PyObject *setMarginsBackgroundColor(PyObject *self, PyObject *args);
PyObject *setMarginsForegroundColor(PyObject *self, PyObject *args);
PyObject *setMarkerBackgroundColor(PyObject *self, PyObject *args);
PyObject *setMarkerForegroundColor(PyObject *self, PyObject *args);
PyObject *setIndentationGuidesBackgroundColor(PyObject *self, PyObject *args);
PyObject *setIndentationGuidesForegroundColor(PyObject *self, PyObject *args);
PyObject *setMatchedBraceBackgroundColor(PyObject *self, PyObject *args);
PyObject *setMatchedBraceForegroundColor(PyObject *self, PyObject *args);
PyObject *setUnmatchedBraceBackgroundColor(PyObject *self, PyObject *args);
PyObject *setUnmatchedBraceForegroundColor (PyObject *self, PyObject *args);
PyObject *setReadOnly (PyObject *self, PyObject *args);
PyObject *setTabIndents (PyObject *self, PyObject *args);
PyObject *setTabWidth (PyObject *self, PyObject *args);
PyObject *setUtf8 (PyObject *self, PyObject *args);
PyObject *setWhitespaceVisibility (PyObject *self, PyObject *args);
PyObject *unindent (PyObject *self, PyObject *args);
PyObject *zoomTo (PyObject *self, PyObject *args);
PyObject *convertEols (PyObject *self, PyObject *args);
PyObject *markerDeleteHandle (PyObject *self, PyObject *args);
PyObject *zoomIn (PyObject *self, PyObject *args);
PyObject *zoomOut(PyObject *self, PyObject *args);
//PyObject *zoomRangeIn (PyObject *self, PyObject *args);
//PyObject *zoomRangeOut(PyObject *self, PyObject *args);

PyObject *indentation (PyObject *self, PyObject *args);
PyObject *lineLength (PyObject *self, PyObject *args);
PyObject *marginLineNumbers (PyObject *self, PyObject *args);
PyObject *marginSensitivity (PyObject *self, PyObject *args);
PyObject *marginWidth (PyObject *self, PyObject *args);
PyObject *markerDefine (PyObject *self, PyObject *args);
PyObject *markerLine (PyObject *self, PyObject *args);

PyObject *setCursorPosition (PyObject *self, PyObject *args);
PyObject *setIndentation (PyObject *self, PyObject *args);
PyObject *setMarginLineNumbers (PyObject *self, PyObject *args);
PyObject *setMarginSensitivity (PyObject *self, PyObject *args);
PyObject *setMarginWidth (PyObject *self, PyObject *args);

PyObject *append(PyObject *self, PyObject *args);
PyObject *find(PyObject *self, PyObject *args);
PyObject *currentLine(PyObject *self, PyObject *args);
PyObject *currentIndex(PyObject *self, PyObject *args);
PyObject *selection(PyObject *self, PyObject *args);
PyObject *selectedText(PyObject *self, PyObject *args);
PyObject *moveCursor(PyObject *self, PyObject *args);
PyObject *line(PyObject *self, PyObject *args);
PyObject *setMarginStringWidth(PyObject *self, PyObject *args);
PyObject *setSelection(PyObject *self, PyObject *args);
PyObject *insertCommand(PyObject *self, PyObject *args);
PyObject *filter(PyObject *self, PyObject *args);
PyObject *fileName(PyObject *self, PyObject *args);

}	// namespace API

#endif
