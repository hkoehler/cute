/***************************************************************************
                          tagsdlg.cpp  -  description
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

#include <qfiledialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include "tagsdlg.h"

TagsDlg::TagsDlg(QWidget *parent, const char *name) :
	TagsDialog(parent, name)
{
	dirLineEdit->setText(QDir::currentDirPath());
}

QString TagsDlg::dir()
{
	return dirLineEdit->text();
}

bool TagsDlg::recursive()
{
	return recursiveCheckBox->isOn();
}

void TagsDlg::slotOkClicked()
{
	accept();
}

void TagsDlg::slotDirButtonClicked()
{
	QString filename = QFileDialog::getExistingDirectory();
	dirLineEdit->setText(filename);
}
