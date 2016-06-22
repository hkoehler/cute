// This module implements the QextScintillaLexerPython class.
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

#include "qextscintillalexerpython.h"


// The list of Python keywords that can be used by other friendly lexers.
const char *QextScintillaLexerPython::keywordClass =
	"and assert break class continue def del elif else except exec "
	"finally for from global if import in is lambda None not or pass "
	"print raise return try while yield";


// The ctor.
QextScintillaLexerPython::QextScintillaLexerPython(QObject *parent,
						   const char *name)
	: QextScintillaLexer(parent,name), fold_comments(FALSE),
	  fold_quotes(FALSE), indent_warn(NoWarning)
{
}


// The dtor.
QextScintillaLexerPython::~QextScintillaLexerPython()
{
}


// Returns the language name.
const char *QextScintillaLexerPython::language() const
{
	return "Python";
}


// Returns the lexer name.
const char *QextScintillaLexerPython::lexer() const
{
	return "python";
}


// Return the list of characters that can start a block.
const char *QextScintillaLexerPython::blockStart(int *style) const
{
	if (style)
		*style = Operator;

	return ":";
}


// Return the number of lines to look back when auto-indenting.
int QextScintillaLexerPython::blockLookback() const
{
	// This must be 0 otherwise de-indenting a Python block gets very
	// difficult.
	return 0;
}


// Return the style used for braces.
int QextScintillaLexerPython::braceStyle() const
{
	return Operator;
}


// Returns the foreground colour of the text for a style.
QColor QextScintillaLexerPython::color(int style) const
{
	switch (style)
	{
	case Default:
		return QColor(0x80,0x80,0x80);

	case Comment:
		return QColor(0x00,0x7f,0x00);

	case Number:
		return QColor(0x00,0x7f,0x7f);

	case DoubleQuotedString:
	case SingleQuotedString:
		return QColor(0x7f,0x00,0x7f);

	case Keyword:
		return QColor(0x00,0x00,0x7f);

	case TripleSingleQuotedString:
	case TripleDoubleQuotedString:
		return QColor(0x7f,0x00,0x00);

	case ClassName:
		return QColor(0x00,0x00,0xff);

	case FunctionMethodName:
		return QColor(0x00,0x7f,0x7f);

	case Operator:
	case Identifier:
		break;

	case CommentBlock:
		return QColor(0x7f,0x7f,0x7f);

	case UnclosedString:
		return QColor(0x00,0x00,0x00);
	}

	return QextScintillaLexer::color(style);
}


// Returns the end-of-line fill for a style.
bool QextScintillaLexerPython::eolFill(int style) const
{
	return (style == UnclosedString);
}


// Returns the font of the text for a style.
QFont QextScintillaLexerPython::font(int style) const
{
	QFont f;

	switch (style)
	{
	case Comment:
#if defined(Q_OS_WIN)
		f = QFont("Comic Sans MS",9);
#else
		f = QFont("new century schoolbook",12);
#endif
		break;

	case DoubleQuotedString:
	case SingleQuotedString:
	case UnclosedString:
#if defined(Q_OS_WIN)
		f = QFont("Courier New",10);
#else
		f = QFont("courier",12);
#endif
		break;

	case Keyword:
	case ClassName:
	case FunctionMethodName:
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
const char *QextScintillaLexerPython::keywords(int set) const
{
	if (set != 0)
		return 0;

	return keywordClass;
}


// Returns the user name of a style.
QString QextScintillaLexerPython::description(int style) const
{
	switch (style)
	{
	case Default:
		return tr("Default");

	case Comment:
		return tr("Comment");

	case Number:
		return tr("Number");

	case DoubleQuotedString:
		return tr("Double-quoted string");

	case SingleQuotedString:
		return tr("Single-quoted string");

	case Keyword:
		return tr("Keyword");

	case TripleSingleQuotedString:
		return tr("Triple single-quoted string");

	case TripleDoubleQuotedString:
		return tr("Triple double-quoted string");

	case ClassName:
		return tr("Class name");

	case FunctionMethodName:
		return tr("Function or method name");

	case Operator:
		return tr("Operator");

	case Identifier:
		return tr("Identifier");

	case CommentBlock:
		return tr("Comment block");

	case UnclosedString:
		return tr("Unclosed string");
	}

	return QString::null;
}


// Returns the background colour of the text for a style.
QColor QextScintillaLexerPython::paper(int style) const
{
	if (style == UnclosedString)
		return QColor(0xe0,0xc0,0xe0);

	return QextScintillaLexer::paper(style);
}


// Refresh all properties.
void QextScintillaLexerPython::refreshProperties()
{
	setCommentProp();
	setQuotesProp();
	setTabWhingeProp();
}


// Read properties from the settings.
bool QextScintillaLexerPython::readProperties(QSettings &qs,const QString &prefix)
{
	int rc = TRUE, num;
	bool ok, flag;

	// Read the fold comments flag.
	flag = qs.readBoolEntry(prefix + "foldcomments",FALSE,&ok);

	if (ok)
		fold_comments = flag;
	else
		rc = FALSE;

	// Read the fold quotes flag.
	flag = qs.readBoolEntry(prefix + "foldquotes",FALSE,&ok);

	if (ok)
		fold_quotes = flag;
	else
		rc = FALSE;

	// Read the indentation warning.
	num = qs.readNumEntry(prefix + "indentwarning",(int)NoWarning,&ok);

	if (ok)
		indent_warn = (IndentationWarning)num;
	else
		rc = FALSE;

	return rc;
}


// Write properties to the settings.
bool QextScintillaLexerPython::writeProperties(QSettings &qs,const QString &prefix) const
{
	int rc = TRUE;

	// Write the fold comments flag.
	if (!qs.writeEntry(prefix + "foldcomments",fold_comments))
		rc = FALSE;

	// Write the fold quotes flag.
	if (!qs.writeEntry(prefix + "foldquotes",fold_quotes))
		rc = FALSE;

	// Write the indentation warning.
	if (!qs.writeEntry(prefix + "indentwarning",(int)indent_warn))
		rc = FALSE;

	return rc;
}


// Return TRUE if comments can be folded.
bool QextScintillaLexerPython::foldComments() const
{
	return fold_comments;
}


// Set if comments can be folded.
void QextScintillaLexerPython::setFoldComments(bool fold)
{
	fold_comments = fold;

	setCommentProp();
}


// Set the "fold.comment.python" property.
void QextScintillaLexerPython::setCommentProp()
{
	emit propertyChanged("fold.comment.python",(fold_comments ? "1" : "0"));
}


// Return TRUE if quotes can be folded.
bool QextScintillaLexerPython::foldQuotes() const
{
	return fold_quotes;
}


// Set if quotes can be folded.
void QextScintillaLexerPython::setFoldQuotes(bool fold)
{
	fold_quotes = fold;

	setQuotesProp();
}


// Set the "fold.quotes.python" property.
void QextScintillaLexerPython::setQuotesProp()
{
	emit propertyChanged("fold.quotes.python",(fold_quotes ? "1" : "0"));
}


// Return the indentation warning.
QextScintillaLexerPython::IndentationWarning QextScintillaLexerPython::indentationWarning() const
{
	return indent_warn;
}


// Set the indentation warning.
void QextScintillaLexerPython::setIndentationWarning(IndentationWarning warn)
{
	indent_warn = warn;

	setTabWhingeProp();
}


// Set the "tab.timmy.whinge.level" property.
void QextScintillaLexerPython::setTabWhingeProp()
{
	emit propertyChanged("tab.timmy.whinge.level",QString::number(indent_warn));
}
