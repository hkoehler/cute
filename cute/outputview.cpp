/***************************************************************************
                          outputview.cpp  -  views program's  output in a list box
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

#include <qlayout.h>
#include <qstringlist.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include "outputview.h"


BasicOutputView::BasicOutputView( QWidget *parent, const char *name )
	: QListBox(parent, name)
{
}

void BasicOutputView::append( QString text )
{
	insertItem(text);
	setCurrentItem(count()-1);
}

Message BasicOutputView::message(QString errorLine)
{
	bool ok;
	QString num;
	Message msg;
	QStringList str_list;

	// GNU style
	str_list = QStringList::split(":", errorLine);
	msg.file = str_list[0];
	num = str_list[1];
	msg.error = str_list[2];
	msg.line = num.toUInt(&ok);
	if(!ok) {
		// try another style
		// "filename", line 12: Error: Code: Message
		errorLine.remove('"');
		str_list = QStringList::split(",", errorLine);
		msg.file = str_list[0];
		num = str_list[1].remove(0, 6);
		str_list = QStringList::split(":", num);
		num = str_list[0];
		msg.error = str_list[3];
		msg.line = num.toUInt(&ok);
		if(!ok) {
			// give up
			msg.line = -1;
		}
	}
	
	return msg;
}

void BasicOutputView::slotDoubleClicked(QListBoxItem *item)
{
	Message msg = message(item->text());
	
	if(msg.line != -1)
		emit jump(msg.file, msg.line);
}

void BasicOutputView::slotNextMessage()
{
	Message msg;
	QListBoxItem *item;
	static bool first = false;

	if(!count())
		return;

	if(!selectedItem()){
		setCurrentItem(0);
		first = false;
		if(selectedItem())
			slotNextMessage();
		return;
	}
	
	if( currentItem() == 0 && first == false ){
		first = true;
		item = selectedItem();
	}
	else
		item = selectedItem()->next();
	if(item){
		setCurrentItem(item);
		msg = message(item->text());
		if( msg.line == -1 )
			slotNextMessage();
		else
			emit jump(msg.file, msg.line);
	}
}

void BasicOutputView::slotPrevMessage()
{
	Message msg;
	QListBoxItem *item;
	static bool last = false;

	if(!count())
		return;

	if(!selectedItem()){
		setCurrentItem(count()-1);
		last = false;
		if(selectedItem())
			slotPrevMessage();
		return;
	}
	
	if( currentItem() == count()-1 && last == false ){
		last = true;
		item = selectedItem();
	}
	else
		item = selectedItem()->prev();
	if(item){
		setCurrentItem(item);
		msg = message(item->text());
		if( msg.line == -1 )
			slotPrevMessage();
		else
			emit jump(msg.file, msg.line);
	}
}

void BasicOutputView::slotClearAllOutputViews()
{
	emit clearAllOutputViews();
}

void BasicOutputView::mousePressEvent(QMouseEvent *e)
{
	if(e->button()==Qt::RightButton)
		contextMenu->popup(e->globalPos());
	QListBox::mousePressEvent(e);
}

void BasicOutputView::insertString(QString str)
{
	insertItem(str);
}

OutputView::OutputView( QWidget *parent, const char *name )
	: BasicOutputView(parent, name)
{
	contextMenu = new QPopupMenu(this);
	contextMenu->insertItem("Clear", this, SLOT(clear()));
	contextMenu->insertItem("Clear Program Output", this, SLOT(slotClearAllOutputViews()));
	connect(this, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(slotDoubleClicked(QListBoxItem*)));
	connect(this, SIGNAL(returnPressed(QListBoxItem*)), this, SLOT(slotDoubleClicked(QListBoxItem*)));
}

GrepOutputView::GrepOutputView( QWidget *parent, const char *name )
	: BasicOutputView(parent, name)
{
	contextMenu = new QPopupMenu(this);
	contextMenu->insertItem("Clear", this, SLOT(clear()));
	QAction *grepAction = new QAction("New Search", "New Search", 0, this);
	connect(grepAction, SIGNAL(activated()), this, SIGNAL(showGrepDialog()));
	grepAction->addTo(contextMenu);
	connect(this, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(slotDoubleClicked(QListBoxItem*)));
	connect(this, SIGNAL(returnPressed(QListBoxItem*)), this, SLOT(slotDoubleClicked(QListBoxItem*)));
}

TagOutputView::TagOutputView( QWidget *parent, const char *name )
	: BasicOutputView(parent, name)
{
	contextMenu = new QPopupMenu(this);
	contextMenu->insertItem("Clear", this, SLOT(clear()));
	connect(this, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(slotDoubleClicked(QListBoxItem*)));
	connect(this, SIGNAL(returnPressed(QListBoxItem*)), this, SLOT(slotDoubleClicked(QListBoxItem*)));
}

