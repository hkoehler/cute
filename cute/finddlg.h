/***************************************************************************
                          finddlg.h  -  description
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
#ifndef __FINDDLG_H__
#define __FINDDLG_H__

#include "ui/finddialog.h"

class CUTEView;
/** dialog for searching in current view */
class FindDlg : public FindDialog
{
	Q_OBJECT
public:
	/** constructs a Find Dialog searching in parent */
	FindDlg(CUTEView *parent);
	/** default ctor does nothing */
	FindDlg() {}
	/** find previous occurence */
	void findPrev();
	/** shows dialog with given view as current view */
	void show(CUTEView *);
protected:
	/** called when dialog is shown */
	void showEvent(QShowEvent*);
	void hideEvent(QHideEvent *);
	/** finds all strings in text in order to insert search results in search docklet */
	void findAll();
private:
	/** current text view */
	CUTEView *view;
	/** set when first string was found */
	bool firstFound;
signals:
	/** emitted when string is found */
	void foundString(QString str);
public slots:
	/** finds a string in the text area*/
	void find();
};
#endif
