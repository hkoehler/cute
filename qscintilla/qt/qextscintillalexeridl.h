// This defines the interface to the QextScintillaLexerIDL class.
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


#ifndef QEXTSCINTILLALEXERIDL_H
#define QEXTSCINTILLALEXERIDL_H


#include <qobject.h>

#include <qextscintillaglobal.h>
#include <qextscintillalexercpp.h>


//! \brief The QextScintillaLexerIDL class encapsulates the Scintilla IDL
//! lexer.
class QEXTSCINTILLA_EXPORT QextScintillaLexerIDL : public QextScintillaLexerCPP
{
	Q_OBJECT

public:
	//! Construct a QextScintillaLexerIDL with parent \a parent and name
	//! \a name.  \a parent is typically the QextScintilla instance.
	QextScintillaLexerIDL(QObject *parent = 0,const char *name = 0);

	//! Destroys the QextScintillaLexerIDL instance.
	virtual ~QextScintillaLexerIDL();

	//! Returns the name of the language.
	const char *language() const;

	//! Returns the foreground colour of the text for style number
	//! \a style.
	//!
	//! \sa paper()
	QColor color(int style) const;

	//! Returns the set of keywords for the keyword set \set recognised by
	//! the lexer as a space separated string.
	const char *keywords(int set) const;

	//! Returns the descriptive name for style number \a style.  If the
	//! style is invalid for this language then QString::null is returned.
	//! This is intended to be used in user preference dialogs.
	QString description(int style) const;

private:
#if defined(Q_DISABLE_COPY)
	QextScintillaLexerIDL(const QextScintillaLexerIDL &);
	QextScintillaLexerIDL &operator=(const QextScintillaLexerIDL &);
#endif
};

#endif
