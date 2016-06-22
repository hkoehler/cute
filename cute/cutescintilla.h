/***************************************************************************
                          cutescintilla.h  -  extension on qscintilla
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

#ifndef __CUTESCINTILLA_H__
#define __CUTESCINTILLA_H__

#include <qextscintilla.h>
#include <qmap.h>
#include <qaction.h>
#include <qpopupmenu.h>

#include "readtags.h"

/** class encapsulating QextScintilla */
class CUTEScintilla : public QextScintilla
{
	Q_OBJECT
public:
	enum AutoCompletion { Document, TagsFile, None };
	CUTEScintilla(QWidget *parent=0, const char *name=0, tagFile *tags=0);
	/** set shortcuts, defined in actions */
	void setActions(QMap<int, QAction*>);
	/** complete current word */
	void autoComplete();
	/** Sets the threshold for the automatic display of 
		the auto-completion list as the user types to thresh.*/
	void setAutoCompletionThreshold(int);
	void setAutoCompletionSource(AutoCompletion ac)
		{ autoCompletionSource = ac; }
	/** set tags file for auto completion */
	void setTagsFile(tagFile *t)
		{ tags = t; }
	/** set word characters for autocompletion */
	void setWordChars( const char* wc )
		{ wordChars = strdup(wc); }
	/** retrieves line at given line number */
	QString line(int);
protected slots:
	void slotCharAdded(int);
protected:
	void autoCompleteFromDocument();
	void autoCompleteFromTagsFile();
	bool eventFilter(QObject *obj, QEvent *e);
	void contextMenu(QContextMenuEvent *e);
	void keyPressEvent( QKeyEvent *e );
	void dragEnterEvent( QDragEnterEvent *e ) { qDebug("dragEnterEvent"); }
	void dropEvent( QDropEvent *e );
private:
	QMap<int, int> keyMap;
	QPopupMenu *menu;
	int threshold;
	AutoCompletion autoCompletionSource;
	tagFile *tags;
	const char *wordChars;
};

#endif
