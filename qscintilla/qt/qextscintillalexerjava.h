// This defines the interface to the QextScintillaLexerJava class.
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


#ifndef QEXTSCINTILLALEXERJAVA_H
#define QEXTSCINTILLALEXERJAVA_H


#include <qobject.h>

#include <qextscintillaglobal.h>
#include <qextscintillalexercpp.h>


//! \brief The QextScintillaLexerJava class encapsulates the Scintilla Java
//! lexer.
class QEXTSCINTILLA_EXPORT QextScintillaLexerJava : public QextScintillaLexerCPP
{
	Q_OBJECT

public:
	//! Construct a QextScintillaLexerJava with parent \a parent and name
	//! \a name.  \a parent is typically the QextScintilla instance.
	QextScintillaLexerJava(QObject *parent = 0,const char *name = 0);

	//! Destroys the QextScintillaLexerJava instance.
	virtual ~QextScintillaLexerJava();

	//! Returns the name of the language.
	const char *language() const;

	//! Returns the set of keywords for the keyword set \set recognised by
	//! the lexer as a space separated string.
	const char *keywords(int set) const;

private:
#if defined(Q_DISABLE_COPY)
	QextScintillaLexerJava(const QextScintillaLexerJava &);
	QextScintillaLexerJava &operator=(const QextScintillaLexerJava &);
#endif
};

#endif
