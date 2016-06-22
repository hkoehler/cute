// This module implements the QextScintillaLexerJavaScript class.
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

#include "qextscintillalexerjavascript.h"


// The list of JavaScript keywords that can be used by other friendly lexers.
const char *QextScintillaLexerJavaScript::keywordClass =
	"abstract boolean break byte case catch char class const continue "
	"debugger default delete do double else enum export extends final "
	"finally float for function goto if implements import in instanceof "
	"int interface long native new package private protected public "
	"return short static super switch synchronized this throw throws "
	"transient try typeof var void volatile while with";


// The ctor.
QextScintillaLexerJavaScript::QextScintillaLexerJavaScript(QObject *parent,const char *name)
	: QextScintillaLexerCPP(parent,name)
{
}


// The dtor.
QextScintillaLexerJavaScript::~QextScintillaLexerJavaScript()
{
}


// Returns the language name.
const char *QextScintillaLexerJavaScript::language() const
{
	return "JavaScript";
}


// Returns the foreground colour of the text for a style.
QColor QextScintillaLexerJavaScript::color(int style) const
{
	if (style == Regex)
		return QColor(0x3f,0x7f,0x3f);

	return QextScintillaLexerCPP::color(style);
}


// Returns the end-of-line fill for a style.
bool QextScintillaLexerJavaScript::eolFill(int style) const
{
	if (style == Regex)
		return TRUE;

	return QextScintillaLexerCPP::eolFill(style);
}


// Returns the font of the text for a style.
QFont QextScintillaLexerJavaScript::font(int style) const
{
	if (style == Regex)
#if defined(Q_OS_WIN)
		return QFont("Courier New",10);
#else
		return QFont("courier",12);
#endif

	return QextScintillaLexerCPP::font(style);
}


// Returns the set of keywords.
const char *QextScintillaLexerJavaScript::keywords(int set) const
{
	if (set != 0)
		return 0;

	return keywordClass;
}


// Returns the user name of a style.
QString QextScintillaLexerJavaScript::description(int style) const
{
	if (style == Regex)
		return tr("Regular expression");

	return QextScintillaLexerCPP::description(style);
}


// Returns the background colour of the text for a style.
QColor QextScintillaLexerJavaScript::paper(int style) const
{
	if (style == Regex)
		return QColor(0xe0,0xf0,0xff);

	return QextScintillaLexer::paper(style);
}
