/***************************************************************************
                          project.h  -  Project feature
                             -------------------
    begin                : Wed Feb 11 23:53:49 CEST 2004
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

#ifndef __PROJECT_H__ 
#define __PROJECT_H__ 

#include <qobject.h>
#include <qdom.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qfileinfo.h>

#include "bookmark.h"

struct ProjectFile
{
	ProjectFile(QString f, int l, int i, int z=0, bool ld=false, QString _lang="") : 
		fileName(f), line(l), index(i), zoom(z), load(ld), lang(_lang) {}
	int line, index;
	QString fileName;
	int zoom;
	bool load;
	QString lang;
};

class Project : public QObject
{
	Q_OBJECT
public:
	Project();
	Project( QString fileName );
	bool open( const char *filename );
	bool save( const char *filename = 0 );
	void setName( const char *filename ) { __file = QString(filename); }
	QString name() { return __file; }
	Project &operator=(Project &);
	void print() { qDebug(doc.toString()); }
	QString projectDir() const { return QFileInfo(__file).dirPath(true); }
	
	const QString currentDirPath() const;
	const QString currentConfig() const;
	const QString currentFile() const;
	const QString buildCommand(QString config) const;
	const QString compileCommand(QString config) const;
	const QString goCommand(QString config) const;
	const QStringList tagsFiles() const;
	const QStringList configs() const;
	const QPtrList<ProjectBookmark> bookmarks() const; 
	const QPtrList<ProjectFile> files() const;
	const QPtrList<ProjectFile> loadedFiles(bool clearLoadAttr=false) const;
	const QStringList envVars(QString config) const;
	ProjectFile *file( QFile &f) const;
	bool hasFile(QFile &f) const;
	
	bool setBuildCommand(QString config, QString command);
	bool setCompileCommand(QString config, QString command);
	bool setGoCommand(QString config, QString command);
	bool addEnvVar( QString config, QString var );
	bool modifyFile( QFile &file, ProjectFile proFile );
public slots:
	void setCurrentDirPath(QString dirPath);
	void setCurrentConfig(QString current);
	/** set current file, if current is "" then current-file tag is deleted */
	void setCurrentFile(QString current);
	void addTagsFile(QString fileName);
	void removeTagsFile(QString tagsFile);
	void addConfig(QString config);
	void removeConfig(QString config);
	void addBookmark( Bookmark *b );
	void addBookmark( QString name, QString file, int line ) {addBookmark( new ProjectBookmark(name, file, line));}
	void removeBookmarks();
	void addFile( QString file, int line, int index, int zoom=0) {addFile(ProjectFile(file, line, index, zoom)); }
	void addFile( ProjectFile file );
	void removeFile( QString file );
	void removeEnvVar( QString config, QString var );
protected:
	void createRoot();
	const QString command(QString config, QString cmd) const;
	bool setCommand(QString config, QString command_type, QString command);
private:
	QDomDocument doc;
	QString __file;
};

#endif
