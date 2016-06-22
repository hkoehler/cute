/***************************************************************************
                          findinfilesdlg.h  -  Find in files dialog using grep
                             -------------------
    begin                : Don, 20 Feb 2003
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

#include <qmultilineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qlineedit.h>

#include "findinfilesdlg.h"


FindInFilesDlg::FindInFilesDlg( QWidget *parent, const char* name)
	: FindInFilesDialog(parent, name)
{
	files = "*.*";
	filesLineEdit->setText(files);
	moreGrepOptionsMultiLineEdit->setText("--binary-files=without-match");
	setWFlags(WShowModal);
	connect(clearButton, SIGNAL(clicked()), SIGNAL(clearList()));
}

void FindInFilesDlg::show()
{
	dir = QDir::currentDirPath();
	dirLineEdit->setText(dir);
	QDialog::show();
}

QString FindInFilesDlg::readLineStdout()
{
	return proc.readLineStdout();
}

bool FindInFilesDlg::canReadLineStdout()
{
	return proc.canReadLineStdout();
}

void FindInFilesDlg::find()
{
	QString args;

	ignoreCase = ignoreCaseCheckBox->isChecked();
	matchWholeLine = matchWholeLineCheckBox->isChecked();
	extendedRegExp = extendedRegExpCheckBox->isChecked();
	recursive = recursiveCheckBox->isChecked();
	pattern = patternLineEdit->text();
	files = filesLineEdit->text();
	dir = dirLineEdit->text();
	moreGrepOptions = moreGrepOptionsMultiLineEdit->text().replace('\n', ' ');

	proc.setWorkingDirectory( dir );
	connect( &proc, SIGNAL(readyReadStdout()), this, SIGNAL(readyReadStdout()));
	proc.clearArguments();
	// create grep command
	proc.addArgument("/bin/sh");
	proc.addArgument("-c");
	
	args += "grep ";
	// print line numbers 
	args += "-n ";
	// print file name
	args += "-H ";
	if(ignoreCase)
		args += "-i ";
	if(matchWholeLine)
		args += "-x ";
	if(extendedRegExp)
		args += "-E ";
	if(recursive)
		args += "-r ";
	args += moreGrepOptions+" ";
	args += "-e ";
	args += "\""+pattern+"\" ";
	args += files;
	proc.addArgument(args);
	proc.start();
	
	emit viewSearch();
}

void FindInFilesDlg::setDirectory()
{
	dir = QFileDialog::getExistingDirectory(QDir::currentDirPath());
	if(dir != QString::null)
		dirLineEdit->setText(dir);
}
