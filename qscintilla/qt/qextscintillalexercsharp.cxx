// This module implements the QextScintillaLexerCSharp class.
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

#include "qextscintillalexercsharp.h"


// The ctor.
QextScintillaLexerCSharp::QextScintillaLexerCSharp(QObject *parent,const char *name)
	: QextScintillaLexerCPP(parent,name)
{
}


// The dtor.
QextScintillaLexerCSharp::~QextScintillaLexerCSharp()
{
}


// Returns the language name.
const char *QextScintillaLexerCSharp::language() const
{
	return "C#";
}


// Returns the foreground colour of the text for a style.
QColor QextScintillaLexerCSharp::color(int style) const
{
	if (style == VerbatimString)
		return QColor(0x00,0x7f,0x00);

	return QextScintillaLexerCPP::color(style);
}


// Returns the end-of-line fill for a style.
bool QextScintillaLexerCSharp::eolFill(int style) const
{
	if (style == VerbatimString)
		return TRUE;

	return QextScintillaLexerCPP::eolFill(style);
}


// Returns the font of the text for a style.
QFont QextScintillaLexerCSharp::font(int style) const
{
	if (style == VerbatimString)
#if defined(Q_OS_WIN)
		return QFont("Courier New",10);
#else
		return QFont("courier",12);
#endif

	return QextScintillaLexerCPP::font(style);
}


// Returns the set of keywords.
const char *QextScintillaLexerCSharp::keywords(int set) const
{
	if (set != 0)
		return 0;

	return "abstract as base bool break byte case catch char checked "
	       "class const continue decimal default delegate do double else "
	       "enum event explicit extern false finally fixed float for "
	       "foreach goto if implicit in int interface internal is lock "
	       "long namespace new null object operator out override params "
	       "private protected public readonly ref return sbyte sealed "
	       "short sizeof stackalloc static string struct switch this "
	       "throw true try typeof uint ulong unchecked unsafe ushort "
	       "using virtual void while";
}


// Returns the user name of a style.
QString QextScintillaLexerCSharp::description(int style) const
{
	if (style == VerbatimString)
		return tr("Verbatim string");

	return QextScintillaLexerCPP::description(style);
}


// Returns the background colour of the text for a style.
QColor QextScintillaLexerCSharp::paper(int style) const
{
	if (style == VerbatimString)
		return QColor(0xe0,0xff,0xe0);

	return QextScintillaLexer::paper(style);
}
