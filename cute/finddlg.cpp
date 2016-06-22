/***************************************************************************
                          finddlg.cpp  -  description
                             -------------------
    begin                : Fre Nov 8 2002
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

#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qradiobutton.h>
#include <qregexp.h>
#include <qlistbox.h>

#include "finddlg.h"
#include "py_config.h"
#include "cuteview.h"

FindDlg::FindDlg(CUTEView *parent)
	: FindDialog(parent)
{
	stringToFind->setEditable(true);
	stringToFind->setDuplicatesEnabled(false);
}
/** searches in view */
void FindDlg::find()
{
	bool found;
	
	if(firstFound)
		found = view->findNext();
	else{
		firstFound = true;
		findAll();
		found = view->findFirst(	stringToFind->lineEdit()->text(),
							regularExpression->isChecked(),
							caseSensitive->isChecked(),
							wholeWord->isChecked(),
							wrapAround->isChecked(),
							forward->isChecked());
	}
	if(!found){
		QMessageBox::information(this, "CUTE", "String not found");
	}
	hide();
}

void FindDlg::show(CUTEView *view)
{
	this->view = view;
	FindDialog::show();
}

void FindDlg::findAll()
{
	bool found;
	int prevLine=-1, prevIndex=-1;
	int line=-1, index=-1;
	int cursorLine, cursorIndex;
	QString lineStr;

	view->scintilla()->getCursorPosition(&cursorLine, &cursorIndex);
	found = view->findFirst(	stringToFind->lineEdit()->text(),
						regularExpression->isChecked(),
						caseSensitive->isChecked(),
						wholeWord->isChecked(),
						wrapAround->isChecked(),
						true, 0, 0);
	
	while(found){
		view->scintilla()->getCursorPosition(&line, &index);
		lineStr = view->line(line).stripWhiteSpace();
		emit foundString(view->fileName()+QString(":")+QString("%1").arg(line+1)+QString(":")+lineStr);
		qDebug(lineStr);
		if(prevLine == -1){
			prevLine = line;
			prevIndex = index;
		}
		else if( line == prevLine && index == prevIndex )
			break;
		found = view->findNext();
	}
	view->scintilla()->setCursorPosition(cursorLine, cursorIndex);
}

void FindDlg::showEvent(QShowEvent *e)
{
	firstFound = false;
	setFixedWidth(width());
	setFixedHeight(height());
	stringToFind->setEditable(true);
	stringToFind->setFocus();
	QString str = view->scintilla()->selectedText();
	if(view->scintilla()->hasSelectedText()){
		if(str.length() < Config::cute.getInteger("maxSelectedStringLengthInDialog")){
			stringToFind->insertItem(str);
			stringToFind->setCurrentItem( stringToFind->count()-1 );
		}
		else
			stringToFind->clearEdit();
	}
	else
		stringToFind->clearEdit();
	FindDialog::showEvent(e);
}

void FindDlg::hideEvent(QHideEvent *)
{
	if( stringToFind->currentText() != "" )
		if( !stringToFind->listBox()->findItem( stringToFind->currentText(), Qt::ExactMatch ) )
			stringToFind->insertItem( stringToFind->currentText() );
}

/** find previous occurence */
void FindDlg::findPrev(){
	
}
