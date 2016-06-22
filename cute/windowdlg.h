/***************************************************************************
                          windowdlg.h  -  description
                             -------------------
    begin                : Son Nov 24 2002
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

#include <qlistbox.h>
#include <qpushbutton.h>
#include "ui/windowdialog.h"

/** views all windows and set current one */
class WindowDlg : public WindowDialog
{ 
	Q_OBJECT
public:
	WindowDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	~WindowDlg();
public slots:
	/** closes selected window */
	void close_slot();
	/** creates new window */
	void new_slot();
	/** makes selected window being the current and closes the dialog */
	void ok_slot();
	/** switches to given window */
	void switchWindow(const QString&);
protected:
	/** creates all list entries */
	void updateListView();
private:
	QMap<QString,QWidget*> viewMap;
};
