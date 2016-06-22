/***************************************************************************
                          outputview.h  -  views output of programs in a list box
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

#ifndef __OUTPUTVIEW_H__
#define __OUTPUTVIEW_H__

#include <qlistbox.h>
#include <qpopupmenu.h>
#include "ui/outputviewbase.h"

/** an error message for error in given file at given line */
struct Message
{
	QString file;
	int line;
	QString error;
};

/** generic view for program output */
class BasicOutputView : public QListBox
{
	Q_OBJECT
public:
	BasicOutputView( QWidget *parent = 0, const char *name = 0);
	/** appends text as new line at the end */
	void append( QString text );
public slots:
	/** selects next message item */
	void slotNextMessage();
	/** selects next message item */
	void slotPrevMessage();
signals:
	/** emitted when double clicked on a view item */
	void jump( QString fileName, unsigned int lineno );
	/** emitted when all output vies should be cleared */
	void clearAllOutputViews();
protected slots:
	/** called when double clicked on item */
	void slotDoubleClicked(QListBoxItem*);
	/** emits @li clearAllOutputViews() signal */
	void slotClearAllOutputViews();
	/** insert string */
	void insertString(QString);
protected:
	/** shows context menu */
	void mousePressEvent(QMouseEvent*);
	/** retrieves line number of message */
	Message message(QString msg);
	QPopupMenu *contextMenu;
};

/** view for program output */
class OutputView : public BasicOutputView
{
	Q_OBJECT
public:
	OutputView( QWidget *parent = 0, const char *name = 0);
};

/** view for grep output */
class GrepOutputView : public BasicOutputView
{
	Q_OBJECT
public:
	GrepOutputView( QWidget *parent = 0, const char *name = 0);
signals:
	/** emited when "New Search" in context menu is clicked */
	void showGrepDialog();
};

/** view for tag output */
class TagOutputView : public BasicOutputView
{
	Q_OBJECT
public:
	TagOutputView( QWidget *parent = 0, const char *name = 0);
};

#endif
