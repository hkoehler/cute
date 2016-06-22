// This defines the interface to the QextScintillaLexerSQL class.
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


#ifndef QEXTSCINTILLALEXERSQL_H
#define QEXTSCINTILLALEXERSQL_H


#include <qobject.h>

#include <qextscintillaglobal.h>
#include <qextscintillalexer.h>


//! \brief The QextScintillaLexerSQL class encapsulates the Scintilla SQL
//! lexer.
class QEXTSCINTILLA_EXPORT QextScintillaLexerSQL : public QextScintillaLexer
{
	Q_OBJECT

public:
	//! This enum defines the meanings of the different styles used by the
	//! SQL lexer.
	enum {
		//! The default.
		Default = 0,

		//! A comment.
		Comment = 1,

		//! A line comment.
		LineComment = 2,

		//! A number.
		Number = 4,

		//! A keyword.
		Keyword = 5,

		//! A single-quoted string.
		SingleQuotedString = 6,

		//! An operator.
		Operator = 10,

		//! An identifier
		Identifier = 11
	};

	//! Construct a QextScintillaLexerSQL with parent \a parent and name
	//! \a name.  \a parent is typically the QextScintilla instance.
	QextScintillaLexerSQL(QObject *parent = 0,const char *name = 0);

	//! Destroys the QextScintillaLexerSQL instance.
	virtual ~QextScintillaLexerSQL();

	//! Returns the name of the language.
	const char *language() const;

	//! Returns the name of the lexer.  Some lexers support a number of
	//! languages.
	const char *lexer() const;

	//! \internal Returns the style used for braces for brace matching.
	int braceStyle() const;

	//! Returns the foreground colour of the text for style number
	//! \a style.
	//!
	//! \sa paper()
	QColor color(int style) const;

	//! Returns the font for style number \a style.
	QFont font(int style) const;

	//! Returns the set of keywords for the keyword set \set recognised by
	//! the lexer as a space separated string.
	const char *keywords(int set) const;

	//! Returns the descriptive name for style number \a style.  If the
	//! style is invalid for this language then QString::null is returned.
	//! This is intended to be used in user preference dialogs.
	QString description(int style) const;

private:
#if defined(Q_DISABLE_COPY)
	QextScintillaLexerSQL(const QextScintillaLexerSQL &);
	QextScintillaLexerSQL &operator=(const QextScintillaLexerSQL &);
#endif
};

#endif
