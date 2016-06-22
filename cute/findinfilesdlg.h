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

#include <qstring.h>
#include <qprocess.h>

#include "ui/findinfilesdialog.h"

/** dialog for searching in a dir via grep */
class FindInFilesDlg : public FindInFilesDialog
{
	Q_OBJECT
public:
	FindInFilesDlg( QWidget *parent=0, const char *name=0);
	void show();
	/** read line from stdout of grep command */
	QString readLineStdout();
	/** can line be read from stdout of grep command */
	bool canReadLineStdout();
	/** retrieves working dir of grep */
	QString workingDir() { return dir; }
protected slots:
	/** called when find button is pressed */
	void find();
	/** called when dir button is pressed */
	void setDirectory();
signals:
	/** emitted when child process recieved output from stdout */
	void readyReadStdout();
	/** clears the grep list of CUTE */
	void clearList();
	/** emitted when search docklet should be shown */
	void viewSearch();
private:
	QProcess proc;
	QString pattern;
	QString files;
	QString dir;
	QString moreGrepOptions;
	bool ignoreCase;
	bool matchWholeLine;
	bool extendedRegExp;
	bool recursive;
};
