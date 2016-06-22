// This defines the interface to the QextScintillaLexerJavaScript class.
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


#ifndef QEXTSCINTILLALEXERJSCRIPT_H
#define QEXTSCINTILLALEXERJSCRIPT_H


#include <qobject.h>

#include <qextscintillaglobal.h>
#include <qextscintillalexercpp.h>


//! \brief The QextScintillaLexerJavaScript class encapsulates the Scintilla
//! JavaScript lexer.
class QEXTSCINTILLA_EXPORT QextScintillaLexerJavaScript : public QextScintillaLexerCPP
{
	Q_OBJECT

public:
	//! Construct a QextScintillaLexerJavaScript with parent \a parent and
	//! name \a name.  \a parent is typically the QextScintilla instance.
	QextScintillaLexerJavaScript(QObject *parent = 0,const char *name = 0);

	//! Destroys the QextScintillaLexerJavaScript instance.
	virtual ~QextScintillaLexerJavaScript();

	//! Returns the name of the language.
	const char *language() const;

	//! Returns the foreground colour of the text for style number
	//! \a style.
	//!
	//! \sa paper()
	QColor color(int style) const;

	//! Returns the end-of-line fill for style number \a style.
	bool eolFill(int style) const;

	//! Returns the font for style number \a style.
	QFont font(int style) const;

	//! Returns the set of keywords for the keyword set \set recognised by
	//! the lexer as a space separated string.
	const char *keywords(int set) const;

	//! Returns the descriptive name for style number \a style.  If the
	//! style is invalid for this language then QString::null is returned.
	//! This is intended to be used in user preference dialogs.
	QString description(int style) const;

	//! Returns the background colour of the text for style number
	//! \a style.
	//!
	//! \sa color()
	QColor paper(int style) const;

private:
	friend class QextScintillaLexerHTML;

	static const char *keywordClass;

#if defined(Q_DISABLE_COPY)
	QextScintillaLexerJavaScript(const QextScintillaLexerJavaScript &);
	QextScintillaLexerJavaScript &operator=(const QextScintillaLexerJavaScript &);
#endif
};

#endif
