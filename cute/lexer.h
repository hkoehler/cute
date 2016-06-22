/***************************************************************************
                          lexer.h  -  represents a scintilla lexer
                             -------------------
    begin                : Sam, 22 Feb 2003
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
 
#ifndef __LEXER_H__
#define __LEXER_H__

#include <map>
#include <string>
#include <vector>
#include <list>
#include <qstringlist.h>
#include <qextscintilla.h>
#include "py_config.h"

namespace Config{

class LexerManager;
/** wraps a scintilla lexer which can be configured via python */
class Lexer
{
	friend class LexerManager;
public:
	/** creates lexer wrapper which scintilla lexer has the given scintilla lexer ID and name */
	Lexer(int scintillaID, char*name);
	/** initializes given scintilla's lexer */
	void initScintilla(QextScintilla *sc);
	/** add new property to python interface of lexer and sets default values*/
	void addProperty(std::string name, int scintillaID, int fore, char *font=0, int size=0,
		bool bold=false, bool italic=false, bool underline=false, int back=0, bool filleol=false);
	/** add keywords */
	void addKeywords(const char *keyWordList);
	/** set file extension for which the lexer should be used, separated by blanks */
	void setFileExtensions( QString fileExt );
	/** sets name of lexed language */
	void setLangName(const char* name) { lang_name=name; }
	/** returns name of language */
	const char* langName() { return lang_name; }
	/** set stream comment start end end token */
	void setStreamComment( QString start, QString end );
	/** set block comment start token */
	void setBlockComment( QString start );
	/** set word chars */
	void setWordChars( QString wc );
	/** retrieves stream comment start token */
	QString streamCommentStart();
	/** retrieves stream comment end token */
	QString streamCommentEnd();
	/** retrieves block comment start token */
	QString blockCommentStart();
	/** retrieves word chars */
	QString wordChars();
	/** sets wether to use monospaced fonts */
	void setUseMonospacedFont(bool f);
private:
	/** maps names to scintilla IDs */
	std::map<std::string, int> property_names;
	/** keywordsets */
	std::vector<const char*> keywords;
	/** name of language */
	const char *lang_name;
	/** id of lexer */
	int lexerID;
	/** the python config module */
	ConfigModule configModule;
	/** file extensions belonging to language */
	QStringList fileExtensions;
	/** default font */
	QFont defaultFont;
	/** wether to use monospaced fonts */
	bool useMonospacedFont;
};

/** class which manages all available lexers */
class LexerManager
{
public:
	/** retrieves lexer via integer value */
	static Lexer *lexer(int);
	/** retrieves lexer via filename */
	static Lexer *lexer(const char *filename);
	/** retrieves lexer via language name */
	static Lexer *lexerByLang(const char* name);
	/** retrieves names of all languages */
	static std::list<std::string> langStringList();
	/** add new lexer to LexerManager */
	static void addLexer(Lexer*);
	/** initializes all managed lexers */
	static void initLexers();
	/** wether to use monospaced fonts */
	static void setUseMonospacedFont(bool s);
private:
	/** map scintilla ID to lexer */
	static std::map<int, Lexer*> lexers;
	/** list of all managed lexers */
	static std::list<Lexer*> lexer_list;
};

}	// namespace Config

#endif
