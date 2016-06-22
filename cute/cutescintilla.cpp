/***************************************************************************
                          cutescintilla.cpp  -  extension on qscintilla
                             -------------------
    begin                : Sam Jul 19 23:53:49 CEST 2003
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
#include <qobjectlist.h>
#include <qdragobject.h>

#include "cutescintilla.h"
#include "cute.h"
#include "bookmark.h"

CUTEScintilla::CUTEScintilla(QWidget *parent, const char *name, tagFile *t) : 
	QextScintilla(parent, name), tags(t)
{
	CUTE *cute = static_cast<CUTE*>(qApp->mainWidget());

	threshold = -1;
	wordChars = 0;
	autoCompletionSource = Document;
	// disable QScintillas auto completion
	QextScintilla::setAutoCompletionThreshold(-1);
	connect(this, SIGNAL(SCN_CHARADDED(int)), this, SLOT(slotCharAdded(int)));
	menu = new QPopupMenu(this);
	QObjectList *childs = queryList();
	for( QObject *obj = childs->first(); obj; obj = childs->next() ){
		if(!strcmp(obj->className(), "QWidget")){
			obj->installEventFilter(this);
			//static_cast<QWidget*>(obj)->setBackgroundColor(Qt::gray);
		}
	}
	
	cute->editUndo->addTo(menu);
	cute->editRedo->addTo(menu);
	menu->insertSeparator();
	cute->editCut->addTo(menu);
	cute->editCopy->addTo(menu);
	cute->editPaste->addTo(menu);
	cute->editDelete->addTo(menu);
	menu->insertSeparator();
	cute->editSelectAll->addTo(menu);
	menu->insertSeparator();
	menu->insertItem("Bookmarks", bookmarkManager.bookmarksMenu );
	cute->searchFind->addTo(menu);
	cute->searchFindSelection->addTo(menu);
	cute->searchJumpToTag->addTo(menu);
	menu->insertSeparator();
	cute->fileClose->addTo(menu);

	setAcceptDrops(true);
}

bool CUTEScintilla::eventFilter(QObject *o, QEvent *e)
{
	//Show context menu on right click.
	if(e->type() == QEvent::ContextMenu){
		contextMenu(static_cast<QContextMenuEvent *>(e));
		return true;
	}
	return QextScintilla::eventFilter(o,e);
}

void CUTEScintilla::contextMenu(QContextMenuEvent *e)
{
	static_cast<CUTE*>(qApp->mainWidget())->slotUpdateBookmarksMenu();
	menu->popup(QPoint(e->globalX(), e->globalY()));
}

void CUTEScintilla::setActions(QMap<int, QAction*> map)
{
	int sci_accel;

	SendScintilla(SCI_CLEARALLCMDKEYS);
	// Assign all given shorcuts to their corresponding scintilla command.
	for( QMap<int, QAction*>::iterator i = map.begin(); i != map.end(); ++i ){
		if(i.data()->accel().count() > 0){
			int key;
			key = i.data()->accel()[0];
			keyMap.insert( key, i.key() );
			//sci_accel = convert(key);
			//keyMap.insert( key, sci_accel );
			//SendScintilla(SCI_ASSIGNCMDKEY, i.key(), sci_accel);
		}
	}
}

void CUTEScintilla::autoComplete()
{
	if(autoCompletionSource == Document)
		autoCompleteFromDocument();
	else if(autoCompletionSource == TagsFile)
		autoCompleteFromTagsFile();
}

void CUTEScintilla::autoCompleteFromDocument()
{
	long wend, wstart;
	QString acWordList;
	bool single = false;

	// See how long a word has been entered so far.
	wend = SendScintilla(SCI_GETCURRENTPOS);
	wstart = SendScintilla(SCI_WORDSTARTPOSITION,wend,TRUE);

	int wlen = wend - wstart;

	if(wlen == 0)
		return;

	// Get the word entered so far.
	char *word = new char[wlen + 1];
	char *cp = word;

	for (long i = wstart; i < wend; ++i)
		*cp++ = SendScintilla(SCI_GETCHARAT,i);

	*cp = '\0';

	// Generate the string representing the valid words to select from.
	QStringList wlist;
	bool cs = !SendScintilla(SCI_AUTOCGETIGNORECASE);

	SendScintilla(SCI_SETSEARCHFLAGS,SCFIND_WORDSTART | (cs ? SCFIND_MATCHCASE : 0));

	long pos = 0;
	long dlen = SendScintilla(SCI_GETLENGTH);
	QString root(word);

	for (;;)
	{
		long fstart;

		SendScintilla(SCI_SETTARGETSTART,pos);
		SendScintilla(SCI_SETTARGETEND,dlen);

		if ((fstart = SendScintilla(SCI_SEARCHINTARGET,wlen,word)) < 0)
			break;

		// Move past the root part.
		pos = fstart + wlen;

		// Skip if this is the word we are auto-completing.
		if (fstart == wstart)
			continue;

		// Get the rest of this word.
		QString w(root);

		while (pos < dlen)
		{
			char ch = SendScintilla(SCI_GETCHARAT,pos);
			// Check if character ch can be in a word.
			if (strchr(wordChars,ch) == NULL)
				break;

			w += ch;

			++pos;
		}

		// Add the word if it isn't already there.
		if (wlist.findIndex(w) < 0)
			wlist.append(w);
	}

	wlist.sort();

	delete []word;

	if (wlist.isEmpty())
		return;

	char sep = SendScintilla(SCI_AUTOCGETSEPARATOR);
	acWordList = wlist.join(QChar(sep));

	SendScintilla(SCI_AUTOCSETCHOOSESINGLE,single);
	SendScintilla(SCI_AUTOCSHOW,wlen,acWordList.latin1());
}

void CUTEScintilla::autoCompleteFromTagsFile()
{
	long wend, wstart;
	QString acWordList;
	bool single = false;
	static bool noTagsFile = false;

	// See how long a word has been entered so far.
	wend = SendScintilla(SCI_GETCURRENTPOS);
	wstart = SendScintilla(SCI_WORDSTARTPOSITION,wend,TRUE);

	int wlen = wend - wstart;

	if(wlen == 0)
		return;

	// Get the word entered so far.
	char *word = new char[wlen + 1];
	char *cp = word;

	for (long i = wstart; i < wend; ++i)
		*cp++ = SendScintilla(SCI_GETCHARAT,i);

	*cp = '\0';

	// Generate the string representing the valid words to select from.
	QStringList wlist;
	bool cs = !SendScintilla(SCI_AUTOCGETIGNORECASE);

	QString root(word);
	
	// find word in tags file
	tagEntry entry;

	if(!tags){
		if(!noTagsFile){
			QMessageBox::warning(this, "CUTE", "You must load a tags file to "
				"use auto completion from tags file");
			noTagsFile = true;
		}
		return;
	}
	if( tagsFirst(tags, &entry) == TagFailure )
		return;
	do{
		if( strlen(entry.name) < wlen )
			continue;
		if( strncmp( entry.name, word, wlen ) )
			continue;
		// Add the word if it isn't already there.
		if (wlist.findIndex(entry.name) < 0)
			wlist.append(entry.name);
	}while(tagsNext(tags, &entry) == TagSuccess);

	wlist.sort();

	delete []word;

	if (wlist.isEmpty())
		return;

	char sep = SendScintilla(SCI_AUTOCGETSEPARATOR);
	acWordList = wlist.join(QChar(sep));

	SendScintilla(SCI_AUTOCSETCHOOSESINGLE,single);
	SendScintilla(SCI_AUTOCSHOW,wlen,acWordList.latin1());
}

void CUTEScintilla::slotCharAdded(int c)
{
	int wend, wstart;
	static int lastLine= 0;
	
	if(threshold != -1){
		// See how long a word has been entered so far.
		wend = SendScintilla(SCI_GETCURRENTPOS);
		wstart = SendScintilla(SCI_WORDSTARTPOSITION,wend,TRUE);

		int wlen = wend - wstart;

		if(wlen >= threshold)
			autoComplete();
	}
	// in/undent one level if a brace was added or deleted in previous line
	if( autoIndent() && c == '\n' ){
		int _line, _index;
		int line_len;
		getCursorPosition(&_line, &_index);
		line_len = line(_line).length();
		if( line(_line-1).stripWhiteSpace().endsWith("{") ){
			indent(_line);
			setCursorPosition(_line, _index+line(_line).length()-line_len);
		}
		else if( lastLine == _line-1 ){
			if( line(_line-1).stripWhiteSpace().right(1) == "}" ){
				unindent(_line-1);
				unindent(_line);
				setCursorPosition(_line, _index+line(_line).length()-line_len);
			}
		}
		lastLine = _line;
	}
}

QString CUTEScintilla::line(int line)
{
	QString qline;
	int len = SendScintilla(QextScintilla::SCI_LINELENGTH, line);
	char *_line = (char*)malloc(len+1);
	memset(_line, 0, len+1);
	SendScintilla(QextScintilla::SCI_GETLINE, line, _line);
	qline = _line;
	qline.remove('\n');
	return qline;
}

void CUTEScintilla::setAutoCompletionThreshold(int t)
{
	threshold = t;
}

void CUTEScintilla::keyPressEvent( QKeyEvent *e )
{
	int key = e->key();
	
	if( e->state() & ControlButton )
		key += CTRL;
	if( e->state() & AltButton )
		key += ALT;
	if( e->state() & MetaButton )
		key += META;
	if( e->state() & ShiftButton )
		key += SHIFT;

	int cmd = keyMap[key];
	if( cmd )
		SendScintilla(cmd);
	else
		QextScintilla::keyPressEvent( e );
}

void CUTEScintilla::dropEvent( QDropEvent *e )
{
	int line, index;
	QString text;
	
	if(QTextDrag::decode(e, text)){
		getCursorPosition( &line, &index );
		insertAt( text, line, index );
	}
}
