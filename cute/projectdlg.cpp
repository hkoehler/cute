/***************************************************************************
                          projectdlg.cpp  -  Projekt options dialog
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

#include <qlineedit.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qdir.h>
#include <qinputdialog.h>
#include <qgroupbox.h>

#include "projectdlg.h"
#include "project.h"

ProjectDlg::ProjectDlg( QWidget *parent, const char *name )
{
}

void ProjectDlg::show( Project *p )
{
	pro = p;
	// copy original project to own one
	project = *p;
	// clear all config items
	config->clear();
	files->clear();
	// set working dir
	workingDir->setText(project.currentDirPath());
	// current config
	config->insertStringList(project.configs());
	if( !project.currentConfig().isEmpty() ){
		config->setCurrentText(project.currentConfig());
	}
	// set files
	QPtrList<ProjectFile> fileList = project.files();
	for( ProjectFile *i = fileList.first(); i; i = fileList.next())
		files->insertItem(i->fileName);
	updateConfig();
	QDialog::show();
}

void ProjectDlg::updateConfig()
{
	bool enable;

	// if no config is chosen, disable config specific items
	if(project.currentConfig().isEmpty())
		enable = false;
	else
		enable = true;
	envBox->setEnabled(enable);
	compileCommand->setEnabled(enable);
	buildCommand->setEnabled(enable);
	goCommand->setEnabled(enable);
	// clear all configuration specific items
	environment->clear();
	buildCommand->clear();
	compileCommand->clear();
	goCommand->clear();
	// environment variables
	QStringList varList = project.envVars(config->currentText());
	for( QStringList::iterator i = varList.begin(); i != varList.end(); ++i ){
		QStringList strList = QStringList::split('=', *i);
		environment->insertItem(new QListViewItem(environment, strList[0], strList[1]));
	}
	// commands
	buildCommand->setText(project.buildCommand(config->currentText()));
	compileCommand->setText(project.compileCommand(config->currentText()));
	goCommand->setText(project.goCommand(config->currentText()));
}

void ProjectDlg::updateProject()
{
	// save old config values
	if(!buildCommand->text().isEmpty())
		project.setBuildCommand( config->currentText(), buildCommand->text() );
	if(!compileCommand->text().isEmpty())
		project.setCompileCommand( config->currentText(), compileCommand->text() );
	if(!goCommand->text().isEmpty())
		project.setGoCommand( config->currentText(), goCommand->text() );
}

void ProjectDlg::addFile()
{
	QStringList files = QFileDialog::getOpenFileNames(QString::null, project.currentDirPath());
	QString fileName;
	ProjectFile *pf;

	for( QStringList::iterator i = files.begin(); i != files.end(); ++i ){
		fileName = QFileInfo(*i).absFilePath().remove(project.projectDir()+QDir::separator());
		if(this->files->findItem(fileName))
			continue;
		pf = new ProjectFile(fileName, 0, 0);
		project.addFile(*pf);
		this->files->insertItem(fileName);
		emit addedFile(fileName);
	}
}

void ProjectDlg::removeFile()
{
	project.removeFile(files->currentText());
	emit removedFile(files->currentText());
	files->takeItem(files->selectedItem());
}

void ProjectDlg::addVar()
{
	QString var = QInputDialog::getText("CUTE", "Enter variable in the form var=value:");
	QStringList strList = QStringList::split("=", var);
	if(environment->findItem(strList[0], 0))
		return;
	environment->insertItem(new QListViewItem(environment, strList[0], strList[1]));
	project.addEnvVar( config->currentText(), var);
}

void ProjectDlg::changeVar()
{
	QListViewItem *item = environment->currentItem();
	project.removeEnvVar(config->currentText(), item->text(0)+"="+item->text(1));
	QString value = QInputDialog::getText("CUTE", "Enter new value:");
	if(value){
		project.addEnvVar(config->currentText(), item->text(0)+"="+value);
		item->setText(1, value);
	}
}

void ProjectDlg::deleteVar()
{
	QListViewItem *item = environment->currentItem();
	project.removeEnvVar(config->currentText(), item->text(0)+"="+item->text(1));
	environment->takeItem( environment->currentItem());
}

void ProjectDlg::addConfig()
{
	QString str = QInputDialog::getText("CUTE", "Enter name for new configuration:");
	config->insertItem(str);
	project.addConfig(str);
	project.setCurrentConfig(str);
	config->setCurrentText(str);
	updateConfig();
}

void ProjectDlg::deleteConfig()
{
	project.removeConfig(config->currentText());
	config->removeItem(config->currentItem());
	project.setCurrentConfig(config->currentText());
	updateConfig();
}

void ProjectDlg::accept()
{
	updateProject();
	project.setCurrentDirPath( workingDir->text() );
	project.setCurrentConfig( config->currentText() );
	// copy own project copy to original one
	*pro = project;
	QDialog::accept();
}

void ProjectDlg::selectWorkingDir()
{
	QString dir = QFileDialog::getExistingDirectory("CUTE", this);
	if( QDir(dir) == QDir(project.projectDir()) )
		workingDir->setText(".");
	else{
		dir = QFileInfo(dir).absFilePath().remove(project.projectDir()+QDir::separator());
		workingDir->setText(dir);
	}
}
