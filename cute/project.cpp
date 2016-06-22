/***************************************************************************
                          project.cpp  -  Projekt feature
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
 
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
 
#include "project.h"

Project::Project()
{
	createRoot();
}

Project::Project( QString fileName )
{
	createRoot();
	open( fileName );
}

bool Project::open( const char *fileName )
{
	QFile file(fileName);
	
	this->__file = fileName;
	if( !file.open(IO_ReadOnly) )
		return false;
	if( !doc.setContent(&file) ){
		file.close();
		return false;
	}
	file.close();
	this->__file = fileName;
	return true;
}

bool Project::save( const char *f )
{
	if( f != 0 )
		this->__file = f;
	else if( this->__file == QString::null )
		return false;
	QFile file(this->__file);
	if( !file.open(IO_WriteOnly) )
		return false;
	QTextStream str(&file);
	str << doc.toString(4);
	file.close();
	return true;
}

Project &Project::operator=(Project &p)
{
	doc.removeChild(doc.documentElement());
	doc.appendChild( p.doc.importNode( p.doc.documentElement(), true) );
	__file = p.__file;
	return *this;
}

const QString Project::currentDirPath() const
{
	QDomNode cwd = doc.documentElement().namedItem("cwd");
	if( cwd.isElement() )
		return cwd.toElement().firstChild().toCDATASection().data();
	return QString::null;
}

const QString Project::currentConfig() const
{
	QDomNode cc = doc.documentElement().namedItem("current-config");
	if( cc.isElement() )
		return cc.toElement().attribute("name");
	return QString::null;
}

const QString Project::currentFile() const
{
	QDomNode cc = doc.documentElement().namedItem("current-file");
	if( cc.isElement() )
		return cc.toElement().firstChild().toCDATASection().data();
	return QString::null;
}

const QString Project::command( QString config, QString cmd ) const
{
	QDomNodeList nodes = doc.documentElement().childNodes();
	
	for( int i = 0; i < nodes.count(); i++ )
		if( nodes.item(i).isElement() ){
			QDomElement e = nodes.item(i).toElement();
			if( e.tagName() == "config" && e.attribute("name") == config )
				return e.namedItem(cmd).firstChild().toCDATASection().data();
		}
	return QString::null;
}

const QString Project::buildCommand(QString config) const
{
	return command(config, "build");
}

const QString Project::compileCommand(QString config) const
{
	return command(config, "compile");
}

const QString Project::goCommand(QString config) const
{
	return command(config, "go");
}

const QStringList Project::tagsFiles() const
{
	QStringList list;
	QDomNodeList nodes = doc.documentElement().childNodes();

	for(int i = 0; i < nodes.count(); i++)
		if( nodes.item(i).isElement() ){
			QDomElement e = nodes.item(i).toElement();
			if( e.tagName() == "tagsfile" )
				list.append( e.firstChild().toCDATASection().data() );
		}
	return list;
}

const QStringList Project::configs() const
{
	QStringList list;
	QDomNodeList nodes = doc.documentElement().childNodes();

	for(int i = 0; i < nodes.count(); i++)
		if( nodes.item(i).isElement() ){
			QDomElement e = nodes.item(i).toElement();
			if( e.tagName() == "config" )
				list.append( e.attribute("name") );
		}
	return list;
}

const QPtrList<ProjectBookmark> Project::bookmarks() const
{
	QPtrList<ProjectBookmark> list;
	QDomNodeList nodes = doc.documentElement().childNodes();

	for(int i = 0; i < nodes.count(); i++)
		if( nodes.item(i).isElement() ){
			QDomElement e = nodes.item(i).toElement();
			if( e.tagName() == "bookmark" ) {
				list.append( new ProjectBookmark( e.attribute("name"), e.attribute("file"), e.attribute("line").toInt() ) );
			}
		}
	return list;
}

const QPtrList<ProjectFile> Project::files() const
{
	QPtrList<ProjectFile> list;
	QDomNodeList nodes = doc.documentElement().childNodes();

	for(int i = 0; i < nodes.count(); i++)
		if( nodes.item(i).isElement() ){
			QDomElement e = nodes.item(i).toElement();
			if( e.tagName() == "file" ){
				int line, index, zoom;
				bool load = false;
				QString cursor = e.attribute("cursor");
				QStringList token = QStringList::split(":", cursor);
				QString file = e.firstChild().toCDATASection().data();
				QString lang = e.attribute("lang");
				line = token[0].toInt();
				index = token[1].toInt();
				zoom = e.attribute("zoom").toInt();
				if( e.attribute("load", "no") == "yes" )
					load = true;
				ProjectFile *pf = new ProjectFile( file, line, index, zoom, load, lang );
				list.append( pf );
			}
		}
	return list;
}

const QPtrList<ProjectFile> Project::loadedFiles(bool clearLoadAttr) const
{
	QPtrList<ProjectFile> list;
	QDomNodeList nodes = doc.documentElement().childNodes();

	for(int i = 0; i < nodes.count(); i++)
		if( nodes.item(i).isElement() ){
			QDomElement e = nodes.item(i).toElement();
			if( e.tagName() == "file" ){
				if( e.attribute("load", "no") == "yes" ){
					int line, index, zoom;
					bool load = false;
					QString cursor = e.attribute("cursor");
					QStringList token = QStringList::split(":", cursor);
					QString file = e.firstChild().toCDATASection().data();
					QString lang = e.attribute("lang");
					line = token[0].toInt();
					index = token[1].toInt();
					zoom = e.attribute("zoom").toInt();
					load = true;
					ProjectFile *pf = new ProjectFile( file, line, index, zoom, load, lang );
					list.append( pf );
					if( clearLoadAttr )
						e.setAttribute("load", "no");
				}
			}
		}
	return list;
}

ProjectFile *Project::file( QFile &file ) const
{
	QString fileName = QFileInfo(file).absFilePath().remove(projectDir()+QDir::separator());
	QPtrList<ProjectFile> list = files();
	for( ProjectFile *f = list.first(); f; f = list.next() )
		if( f->fileName == fileName )
			return f;
	return 0;
}

bool Project::hasFile( QFile &f ) const
{
	return file(f);
}

const QStringList Project::envVars(QString config) const
{
	QStringList list;
	QDomNodeList nodes = doc.documentElement().childNodes();

	for(int i = 0; i < nodes.count(); i++)
		if( nodes.item(i).isElement() ){
			QDomElement e = nodes.item(i).toElement();
			if( e.tagName() == "config" && e.attribute("name") == config ){
				QDomNodeList vars = e.childNodes();
				for(int j = 0; j < vars.count(); j++)
					if( vars.item(j).isElement() ){
						QDomElement v = vars.item(j).toElement();
						if( v.tagName() == "var" )
							list.append(v.firstChild().toCDATASection().data());
					}
			}
		}
	return list;
}

void Project::setCurrentDirPath(QString dirPath)
{
	QDomNode cwdNode;
	QDomElement new_cwd = doc.createElement("cwd");
	QDomCDATASection cdata = doc.createCDATASection(dirPath);

	cwdNode = doc.documentElement().namedItem("cwd");
	if(!cwdNode.isNull())
		doc.documentElement().removeChild(cwdNode);
	doc.documentElement().appendChild( new_cwd );
	new_cwd.appendChild(cdata);
}

void Project::setCurrentConfig(QString cc)
{
	QDomNode ccNode;
	QDomElement new_cc = doc.createElement("current-config");
	
	new_cc.setAttribute("name", cc);
	ccNode = doc.documentElement().namedItem("current-config");
	if(!ccNode.isNull())
		doc.documentElement().removeChild(ccNode);
	doc.documentElement().appendChild( new_cc );
}

void Project::setCurrentFile(QString file)
{
	QDomNode node;
	QDomElement new_node = doc.createElement("current-file");
	QString fileName = QFileInfo(file).absFilePath().remove(projectDir()+QDir::separator());
	QDomCDATASection cdata = doc.createCDATASection(fileName);

	node = doc.documentElement().namedItem("current-file");
	if(!node.isNull())
		doc.documentElement().removeChild(node);
	if(file){
		doc.documentElement().appendChild( new_node );
		new_node.appendChild(cdata);
	}
}

bool Project::setCommand(QString cfg, QString command_type, QString cmd)
{
	QDomNode buildCommand;
	QDomElement new_buildCommand = doc.createElement(command_type);
	QDomCDATASection cdata = doc.createCDATASection(cmd);
	QDomNodeList configs;

	configs = doc.documentElement().childNodes();
	for( int i = 0; i < configs.count(); i++ )
		if( configs.item(i).isElement() ) {
			QDomElement e = configs.item(i).toElement();
			if( e.tagName() == "config" && e.attribute("name") == cfg ) {
				buildCommand = e.namedItem(command_type);
				if( !buildCommand.isNull() )
					e.removeChild(buildCommand);
				e.appendChild( new_buildCommand );
				new_buildCommand.appendChild(cdata);
				return true;
			}
		}
	return false;
}

bool Project::setBuildCommand(QString cfg, QString cmd)
{
	return setCommand(cfg, "build", cmd);
}

bool Project::setCompileCommand(QString cfg, QString cmd)
{
	return setCommand(cfg, "compile", cmd);
}

bool Project::setGoCommand(QString cfg, QString cmd)
{
	return setCommand(cfg, "go", cmd);
}

void Project::addTagsFile(QString fileName)
{
	fileName = QFileInfo(fileName).absFilePath().remove(projectDir()+QDir::separator());
	QDomElement new_tagsFile = doc.createElement("tagsfile");
	QDomCDATASection cdata = doc.createCDATASection(fileName);
	
	new_tagsFile.appendChild(cdata);
	doc.documentElement().appendChild(new_tagsFile);
}

void Project::removeTagsFile(QString tagsFile)
{
	tagsFile = QFileInfo(tagsFile).absFilePath().remove(projectDir()+QDir::separator());
	QDomNodeList tagsFiles = doc.documentElement().childNodes();
	QDomElement e;
	
	for(int i = 0; i < tagsFiles.count(); i++)
		if( tagsFiles.item(i).isElement() ){
			e = tagsFiles.item(i).toElement();
			if( e.tagName() == "tagsfile" ){
				if( e.firstChild().toCDATASection().data() == tagsFile ){
					doc.documentElement().removeChild(e);
					i--;
				}
			}
		}
}

void Project::addConfig(QString config)
{
	QDomElement new_config = doc.createElement("config");
	
	new_config.setAttribute("name", config);
	doc.documentElement().appendChild(new_config);
}

void Project::removeConfig(QString config)
{
	QDomNodeList configs = doc.documentElement().childNodes();
	QDomElement e;
	
	for(int i = 0; i < configs.count(); i++)
		if( configs.item(i).isElement() ){
			e = configs.item(i).toElement();
			if( e.tagName() == "config" && e.attribute("name") == config )
				doc.documentElement().removeChild(configs.item(i));
				return;
		}
}

void Project::addBookmark( Bookmark *b )
{
	QDomElement bookmark = doc.createElement("bookmark");
	QString fileName = QFileInfo(b->file()).absFilePath().remove(projectDir()+QDir::separator());

	bookmark.setAttribute("line", QString("%1").arg(b->line()));
	bookmark.setAttribute("file", fileName);
	bookmark.setAttribute("name", b->name());
	doc.documentElement().appendChild(bookmark);
}

void Project::removeBookmarks()
{
	QDomNodeList bookmarks = doc.documentElement().childNodes();

	for( int i = 0; i < bookmarks.count(); i++ ){
		if( bookmarks.item(i).isElement() ){
			QDomElement e = bookmarks.item(i).toElement();
			if( e.tagName() == "bookmark" ){
				doc.documentElement().removeChild(e);
				i--;
			}
		}
	}
}

void Project::addFile( ProjectFile file )
{
	QString fileName = file.fileName;
	QDomElement f = doc.createElement("file");
	QDomCDATASection cdata = doc.createCDATASection(fileName);

	f.setAttribute("cursor", QString("%1:%2").arg(file.line).arg(file.index));
	f.setAttribute("zoom", file.zoom);
	if( file.load )
		f.setAttribute("load", QString("%1").arg(file.load));
	if( file.lang )
		f.setAttribute("lang", file.lang);
	f.appendChild(cdata);
	doc.documentElement().appendChild(f);
}

void Project::removeFile( QString file )
{
	QDomNodeList files = doc.documentElement().childNodes();
	
	for( int i = 0; i < files.count(); i++ ){
		if( files.item(i).isElement() ){
			QDomElement e = files.item(i).toElement();
			if( e.tagName() == "file" ){
				QDomCDATASection cdata = e.firstChild().toCDATASection();
				if(cdata.data() == file){
					doc.documentElement().removeChild(e);
					return;
				}
			}
		}
	}
}

bool Project::addEnvVar( QString cfg, QString var )
{
	QDomNode envVar;
	QDomElement new_envVar = doc.createElement("var");
	QDomCDATASection cdata = doc.createCDATASection(var);
	QDomNodeList configs;

	configs = doc.documentElement().childNodes();
	for( int i = 0; i < configs.count(); i++ )
		if( configs.item(i).isElement() ) {
			QDomElement e = configs.item(i).toElement();
			if( e.tagName() == "config" && e.attribute("name") == cfg ) {
				e.appendChild( new_envVar );
				new_envVar.appendChild(cdata);
				return true;
			}
		}
	return false;
}

bool Project::modifyFile( QFile &file, ProjectFile newFile )
{
	QString fileName = QFileInfo(file).absFilePath().remove(projectDir()+QDir::separator());
	QPtrList<ProjectFile> list;
	QDomNodeList nodes = doc.documentElement().childNodes();

	for(int i = 0; i < nodes.count(); i++)
		if( nodes.item(i).isElement() ){
			QDomElement e = nodes.item(i).toElement();
			if( e.tagName() == "file" )
				if( e.firstChild().toCDATASection().data() == fileName ){
					e.setAttribute("cursor", QString("%1:%2").arg(newFile.line).arg(newFile.index));
					e.setAttribute("zoom", QString("%1").arg(newFile.zoom));
					if( newFile.load )
						e.setAttribute("load", "yes");
					e.setAttribute("lang", newFile.lang);
					return true;
				}
		}
	return false;
}

void Project::removeEnvVar( QString cfg, QString var )
{
	QDomNodeList configs;
	QDomNodeList vars;

	configs = doc.documentElement().childNodes();
	for( int i = 0; i < configs.count(); i++ )
		if( configs.item(i).isElement() ) {
			QDomElement e = configs.item(i).toElement();
			if( e.tagName() == "config" && e.attribute("name") == cfg ) {
				vars = e.childNodes();
				for(int j = 0; j < vars.count(); j++)
					if( vars.item(j).isElement() ) {
						QDomElement v = vars.item(j).toElement();
						if( v.tagName() == "var" && v.firstChild().isCDATASection() ){
							if( v.firstChild().toCDATASection().data() == var ){
								e.removeChild(v);
								return;
							}
						}
					}
			}
		}
}

void Project::createRoot()
{
	if( !doc.hasChildNodes() ){
		QDomNode root = doc.createElement( "project" );
		doc.appendChild(root);
	}
}
