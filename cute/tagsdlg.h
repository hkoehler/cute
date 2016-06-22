/***************************************************************************
                          tagsdlg.h  -  description
                             -------------------
    begin                : Fri Oct 10 23:53:49 CEST 2003
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

#include "ui/tagsdialog.h"

/** dialog for input of ctags options */
class TagsDlg : public TagsDialog
{
	Q_OBJECT
public:
	TagsDlg(QWidget *parent=0, const char *name=0);
	/** retrieves the dir in which tags file should be built */
	QString dir();
	/** wether ctags should recurse into directories */
	bool recursive();
protected slots:
	void slotOkClicked();
	void slotDirButtonClicked();
};
