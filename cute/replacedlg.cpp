/***************************************************************************
                          replacedlg.cpp  -  description
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
#include <qlistbox.h>

#include "py_config.h"
#include "replacedlg.h"
#include "cuteview.h"

ReplaceDlg::ReplaceDlg( CUTEView *parent )
	: ReplaceDialog(parent)
{
	firstFound = false;
	view = parent;
}

void ReplaceDlg::show(CUTEView* view)
{
	this->view = view;
	ReplaceDialog::show();
}

void ReplaceDlg::showEvent(QShowEvent *e)
{
	setFixedWidth(width());
	setFixedHeight(height());
	stringToFind->setEditable(true);
	replaceWith->setEditable(true);
	stringToFind->setDuplicatesEnabled(false);
	replaceWith->setDuplicatesEnabled(false);
	stringToFind->setFocus();
	if(view->scintilla()->hasSelectedText()){
		QString str = view->scintilla()->selectedText();
		if(str.length() < Config::cute.getInteger("maxSelectedStringLengthInDialog") ){
			stringToFind->insertItem(str);
			stringToFind->setCurrentItem( stringToFind->count()-1 );
		}
	}
	else
		stringToFind->clearEdit();
}

void ReplaceDlg::hideEvent(QHideEvent *)
{
	if( replaceWith->currentText() != "" )
		if( !replaceWith->listBox()->findItem( replaceWith->currentText(), Qt::ExactMatch ) )
			replaceWith->insertItem( replaceWith->currentText() );
	if( stringToFind->currentText() != "" )
		if( !stringToFind->listBox()->findItem( stringToFind->currentText(), Qt::ExactMatch ) )
			stringToFind->insertItem( stringToFind->currentText() );
}

void ReplaceDlg::replace()
{
	bool found;
	if(firstFound)
		found = view->scintilla()->findNext();
	else{
		firstFound = true;
		found = view->scintilla()->findFirst(stringToFind->lineEdit()->text(),
							regularExpression->isChecked(),
							caseSensitive->isChecked(),
							wholeWord->isChecked(),
							wrapAround->isChecked(),
							forward->isChecked());
	}
	if(!found)
		return;
	hide();
	switch( QMessageBox::information(this, "CUTE", "Replace String?", "Replace", "Omit","Cancel") )
	{
	case 0:
		view->scintilla()->replace(replaceWith->lineEdit()->text());
		replace();
		break;
	case 1:
		replace();
		break;
	case 2:
		return;
	}
}

void ReplaceDlg::replaceAll()
{
	view->scintilla()->beginUndoAction();
	view->replaceAll(stringToFind->lineEdit()->text(),
						replaceWith->lineEdit()->text(),
						regularExpression->isChecked(),
						caseSensitive->isChecked(),
						wholeWord->isChecked(),
						wrapAround->isChecked(),
						forward->isChecked());
	view->scintilla()->endUndoAction();
}

void ReplaceDlg::replaceInSelection()
{
	int lineFrom, lineTo, indexFrom, indexTo;
	int foundLineFrom, foundLineTo, foundIndexFrom, foundIndexTo;
	bool found;
	view->scintilla()->beginUndoAction();
	view->scintilla()->getSelection( &lineFrom, &indexFrom, &lineTo, &indexTo );
	found = view->scintilla()->findFirst(stringToFind->lineEdit()->text(),
						regularExpression->isChecked(),
						caseSensitive->isChecked(),
						wholeWord->isChecked(),
						wrapAround->isChecked(),
						forward->isChecked(), lineFrom, indexFrom);
	while(found){
		view->scintilla()->getSelection( &foundLineFrom, &foundIndexFrom, &foundLineTo, &foundIndexTo );
		if( lineFrom == foundLineFrom && indexFrom > foundIndexFrom )
			break;
		if( lineFrom > foundLineFrom || lineTo < foundLineTo )
			break;
		if( lineTo == foundLineTo && indexTo < foundIndexTo )
			break;
		view->scintilla()->replace(replaceWith->lineEdit()->text());
		found = view->scintilla()->findNext();
	}
	view->scintilla()->selectAll(false);
	view->scintilla()->endUndoAction();
}

void ReplaceDlg::newSearchString(){
	firstFound = false;
}
