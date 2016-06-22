// This module implements the QextScintillaLexerSQL class.
//
// Copyright (c) 2004
// 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
// 
// This file is part of QScintilla.
// 
// This copy of QScintilla is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option) any
// later version.
// 
// QScintilla is supplied in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// QScintilla; see the file LICENSE.  If not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>

#include "qextscintillalexersql.h"


// The ctor.
QextScintillaLexerSQL::QextScintillaLexerSQL(QObject *parent,const char *name)
	: QextScintillaLexer(parent,name)
{
}


// The dtor.
QextScintillaLexerSQL::~QextScintillaLexerSQL()
{
}


// Returns the language name.
const char *QextScintillaLexerSQL::language() const
{
	return "SQL";
}


// Returns the lexer name.
const char *QextScintillaLexerSQL::lexer() const
{
	return "sql";
}


// Return the style used for braces.
int QextScintillaLexerSQL::braceStyle() const
{
	return Operator;
}


// Returns the foreground colour of the text for a style.
QColor QextScintillaLexerSQL::color(int style) const
{
	switch (style)
	{
	case Default:
		return QColor(0x80,0x80,0x80);

	case Comment:
	case LineComment:
		return QColor(0x00,0x7f,0x00);

	case Number:
		return QColor(0x00,0x7f,0x7f);

	case Keyword:
		return QColor(0x00,0x00,0x7f);

	case SingleQuotedString:
		return QColor(0x7f,0x00,0x7f);

	case Operator:
	case Identifier:
		break;
	}

	return QextScintillaLexer::color(style);
}


// Returns the font of the text for a style.
QFont QextScintillaLexerSQL::font(int style) const
{
	QFont f;

	switch (style)
	{
	case Comment:
	case LineComment:
#if defined(Q_OS_WIN)
		f = QFont("Comic Sans MS",9);
#else
		f = QFont("new century schoolbook",12);
#endif
		break;

	case SingleQuotedString:
#if defined(Q_OS_WIN)
		f = QFont("Courier New",10);
#else
		f = QFont("courier",12);
#endif
		break;

	case Keyword:
	case Operator:
		f = QextScintillaLexer::font(style);
		f.setBold(TRUE);
		break;

	default:
		f = QextScintillaLexer::font(style);
	}

	return f;
}


// Returns the set of keywords.
const char *QextScintillaLexerSQL::keywords(int set) const
{
	if (set != 0)
		return 0;

	return
		"ABSOLUTE ACTION ADD ADMIN AFTER AGGREGATE ALIAS ALL ALLOCATE "
		"ALTER AND ANY ARE ARRAY AS ASC ASSERTION AT AUTHORIZATION "
		"BEFORE BEGIN BINARY BIT BLOB BOOLEAN BOTH BREADTH BY "
		"CALL CASCADE CASCADED CASE CAST CATALOG CHAR CHARACTER CHECK "
		"CLASS CLOB CLOSE COLLATE COLLATION COLUMN COMMIT COMPLETION "
		"CONNECT CONNECTION CONSTRAINT CONSTRAINTS CONSTRUCTOR "
		"CONTINUE CORRESPONDING CREATE CROSS CUBE CURRENT "
		"CURRENT_DATE CURRENT_PATH CURRENT_ROLE CURRENT_TIME "
		"CURRENT_TIMESTAMP CURRENT_USER CURSOR CYCLE "
		"DATA DATE DAY DEALLOCATE DEC DECIMAL DECLARE DEFAULT "
		"DEFERRABLE DEFERRED DELETE DEPTH DEREF DESC DESCRIBE "
		"DESCRIPTOR DESTROY DESTRUCTOR DETERMINISTIC DICTIONARY "
		"DIAGNOSTICS DISCONNECT DISTINCT DOMAIN DOUBLE DROP DYNAMIC "
		"EACH ELSE END END-EXEC EQUALS ESCAPE EVERY EXCEPT EXCEPTION "
		"EXEC EXECUTE EXTERNAL "
		"FALSE FETCH FIRST FLOAT FOR FOREIGN FOUND FROM FREE FULL "
		"FUNCTION "
		"GENERAL GET GLOBAL GO GOTO GRANT GROUP GROUPING "
		"HAVING HOST HOUR "
		"IDENTITY IGNORE IMMEDIATE IN INDICATOR INITIALIZE INITIALLY "
		"INNER INOUT INPUT INSERT INT INTEGER INTERSECT INTERVAL INTO "
		"IS ISOLATION ITERATE "
		"JOIN "
		"KEY "
		"LANGUAGE LARGE LAST LATERAL LEADING LEFT LESS LEVEL LIKE "
		"LIMIT LOCAL LOCALTIME LOCALTIMESTAMP LOCATOR "
		"MAP MATCH MINUTE MODIFIES MODIFY MODULE MONTH "
		"NAMES NATIONAL NATURAL NCHAR NCLOB NEW NEXT NO NONE NOT NULL "
		"NUMERIC "
		"OBJECT OF OFF OLD ON ONLY OPEN OPERATION OPTION OR ORDER "
		"ORDINALITY OUT OUTER OUTPUT "
		"PAD PARAMETER PARAMETERS PARTIAL PATH POSTFIX PRECISION "
		"PREFIX PREORDER PREPARE PRESERVE PRIMARY PRIOR PRIVILEGES "
		"PROCEDURE PUBLIC "
		"READ READS REAL RECURSIVE REF REFERENCES REFERENCING "
		"RELATIVE RESTRICT RESULT RETURN RETURNS REVOKE RIGHT ROLE "
		"ROLLBACK ROLLUP ROUTINE ROW ROWS "
		"SAVEPOINT SCHEMA SCROLL SCOPE SEARCH SECOND SECTION SELECT "
		"SEQUENCE SESSION SESSION_USER SET SETS SIZE SMALLINT SOME "
		"SPACE SPECIFIC SPECIFICTYPE SQL SQLEXCEPTION SQLSTATE "
		"SQLWARNING START STATE STATEMENT STATIC STRUCTURE "
		"SYSTEM_USER "
		"TABLE TEMPORARY TERMINATE THAN THEN TIME TIMESTAMP "
		"TIMEZONE_HOUR TIMEZONE_MINUTE TO TRAILING TRANSACTION "
		"TRANSLATION TREAT TRIGGER TRUE "
		"UNDER UNION UNIQUE UNKNOWN UNNEST UPDATE USAGE USER USING "
		"VALUE VALUES VARCHAR VARIABLE VARYING VIEW "
		"WHEN WHENEVER WHERE WITH WITHOUT WORK WRITE "
		"YEAR "
		"ZONE";
}


// Returns the user name of a style.
QString QextScintillaLexerSQL::description(int style) const
{
	switch (style)
	{
	case Default:
		return tr("Default");

	case Comment:
		return tr("Comment");

	case LineComment:
		return tr("Line comment");

	case Number:
		return tr("Number");

	case Keyword:
		return tr("Keyword");

	case SingleQuotedString:
		return tr("Single-quoted string");

	case Operator:
		return tr("Operator");

	case Identifier:
		return tr("Identifier");
	}

	return QString::null;
}
