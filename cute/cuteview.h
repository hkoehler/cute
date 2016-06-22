/***************************************************************************
                          cuteview.h  -  description
                             -------------------
    begin                : Sam Sep 28 23:53:49 CEST 2002
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

#ifndef __CUTEVIEW_H__
#define __CUTEVIEW_H__

#include <qextscintillamacro.h>
#include <qcolor.h>
#include <qfile.h>
#include <qaction.h>
#include <qmap.h>
#include <qprocess.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <list>

#include "cutescintilla.h"

namespace Config{ class Lexer; }

/** action class for storing of macros */
class ScintillaAction
{
public:
	ScintillaAction(unsigned int msg, unsigned long wParam, long lParam);
	ScintillaAction(const ScintillaAction& a);
	~ScintillaAction() {}
	unsigned int msg;
	unsigned long wParam;
	long lParam;
	ScintillaAction &operator=(const ScintillaAction& a);
};

/**
 * view in mulit document interface
 */
class CUTEView : public QWidget
{
  Q_OBJECT
public:
	enum { CUTE_MARGIN = 0, CUTE_LINENUMBER_MARGIN = 1, CUTE_FOLD_MARGIN = 2 };
	CUTEView(QWidget *parent=0, QString title = "Document", tagFile *tags=0);
	~CUTEView();
	/** sets all settings of view from global config */
	void configure();
	/** sends scintilla command */
	void sendEditor(unsigned int msg, unsigned long wParam=0, long lParam=0);
	/** loads file from disk */
	bool loadFile(const char *file);
	/** rereads fiel from disk */
	bool reloadFile();
	/** saves text of view to file, if forced then an empty file will also be created */
	bool saveFile(const char *file=0, bool force=false);
	/** toggles bookmark at given line */
	void toggleBookmark(int line);
	/** displays an error in a line */
	void addError(int line );
	/** displays a bookmark in a line */
	int addBookmark(int line );
	/** deletes all errors in view */
	void deleteAllErrors();
	/** jumps to next found bookmark */
	void findNextBookmark();
	/** jumps to previous found bookmark */
	void findPrevBookmark();
	/** inserts output of a command at current cursor pos */
	void insertCommand(QString);
	/** filters selection and insert output of filter at current cursor pos */
	void filter(QString);
	/** starts macro recording if none is be recorded */
	void startMacroRecording();
	/** stops macro recording */
	void stopMacroRecording();
	/** runs last recorded or loaded macro */
	void runMacro();
	/** loads macro from file */
	void loadMacro(QString &file);
	/** save macro to file */
	void saveMacro(QString &file);
	/** sets the macro in QString to be the current one */
	bool setMacro(QString);
	/** retrieves the current macro as a string */
	QString macroString();
	/** finds first occurence of string with some options */
	bool findFirst(const QString &expr, bool re=false, bool cs=false, bool wo=false, bool wrap=true, bool forward=TRUE, int line=-1, int index=-1);
	/** finds next occurence */
	bool findNext();
	/** find previous occurence */
	bool findPrev();
	/** choose lexer by a file name */
	void setupLexer(const QString&);
	/** replaces all occurence of string in view */
	void replaceAll(const QString &expr, const QString &replaceStr, bool re, bool cs, bool wo, bool wrap, bool forward=TRUE, bool confirm=false);
	/** weather text has been modified */
	bool edited();
	/** if undo function is available */
	bool canUndo();
	/** if redo function is available */
	bool canRedo();
	/** undo last action */
	void undo();
	/** redo last action */
	void redo();
	/** retrieves a line */
	QString line(int l) { return view->line(l); }
	/** returns wrapped QScintilla text widget */
	CUTEScintilla* scintilla();
	/** set shortcuts, defined in QActions */
	void setActions(QMap<int, QAction*>);
	/** retrieves file name */
	char *fileName() { return filename; }
	/** returns chosen lexer of view */
	Config::Lexer *lexer() { return _lexer; }
	/** sets lexer for highlighting and folding */
	void setLexer( Config::Lexer *l ) { _lexer = l; }
	/** get the current line/index of cursor */
	void getCursorPosition( int &line, int &index );
	/** get the line/index of selected text */
	void getSelection(int &lineFrom,int &indexFrom, int &lineTo,int &indexTo);
	/** clear text and all markers */
	void clear();
	/** return all bookmarks handles */
	QValueList<int> bookmarks();
	/** deletes all Bookmarks in view */
	void removeAllBookmarks();
	/** return language name */
	QString langName();
	/** the scintilla widget has the focus */
	void setFocus() { view->setFocus(); }
    /** return the information string for the view */
    QString CUTEView::infoString();
    /** set an information field to the info string list */
    void CUTEView::addInfoField(QString name, QString info);
    /** remove an information field from the info string list */
    void CUTEView::removeInfoField(QString name);
protected:
	/** called when view is about to be closed */
	void closeEvent(QCloseEvent *e);
	/** called when view is shown */
	void showEvent(QShowEvent *e);
	/** called when view is hidden */
	void hideEvent(QHideEvent *e);
protected slots:
	/** called when text has been changed */
	void slotDocumentChanged();
	/** sets markers */
	void slotMarginClicked(int margin, int line, Qt::ButtonState state);
	/** called from insertCommand and filter in order to insert program output */
	void insertStdout();
	/** called when child process has finished */
	void processExited();
	/** handle macro recording */
	void recordMacro(unsigned int, unsigned long, long);
	/** called when char has been typed */
	void slotCharAdded(int);
	/** when file has changed outside the editor, the user can reload it */
	void fileChangedOutside();
signals: 
	void save();
	void closed(CUTEView *view=0);
	void setTabCaption(QWidget *view, QString name);
	void infoStringChanged(QWidget *view);
private:
	CUTEScintilla *view;
	Config::Lexer *_lexer;
	/** process of filter command */
	QProcess *proc;
	char *filename;
	QDateTime lastModified;
	int bookmark, error;
	static bool recording;
	static std::list<ScintillaAction> macro;
	QString searchString;
	bool searchRegExp;
	bool searchCaseSensitive;
	bool searchWholeWord;
	bool searchLineWrap;
	bool searchDirection;
	bool searching;
	QValueList<int> handles;
    QStringList infoFields;
	/** timer for fileChangedOutside(), checking wether file has changed outside */
	QTimer *timer;
};

#endif
