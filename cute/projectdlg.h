/***************************************************************************
                          projectdlg.h  -  Projekt options dialog
                             -------------------
    begin                : Fri Feb 13 17:53:49 CEST 2004
    copyright            : (C) 2004 by Heiko Köhler
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
 
#ifndef __PROJECTDLG_H__
#define __PROJECTDLG_H__

#include "project.h"
#include "ui/projectdialog.h"

class ProjectDlg : public ProjectDialog
{
	Q_OBJECT
public:
	ProjectDlg( QWidget *parent = 0,  const char *name = 0 );
	void show( Project *pro );
protected slots:
	void addFile();
	void removeFile();
	void addVar();
	void changeVar();
	void deleteVar();
	void addConfig();
	void deleteConfig();
	void updateConfig();
	void accept();
	void updateProject();
	void selectWorkingDir();
signals:
	void removedFile(QString file);
	void addedFile(QString);
private:
	Project *pro;
	/** own copy of original project file for modifications,
	* will be copied back when ok button is pressed */
	Project project;
};

#endif
