// This module implements the QextScintillaLexerCPP class.
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

#include "qextscintillalexercpp.h"


// The ctor.
QextScintillaLexerCPP::QextScintillaLexerCPP(QObject *parent,const char *name,
					     bool caseInsensitiveKeywords)
	: QextScintillaLexer(parent,name), fold_atelse(FALSE),
	  fold_comments(FALSE), fold_compact(TRUE), fold_preproc(TRUE), 
	  style_preproc(FALSE), nocase(caseInsensitiveKeywords)
{
}


// The dtor.
QextScintillaLexerCPP::~QextScintillaLexerCPP()
{
}


// Returns the language name.
const char *QextScintillaLexerCPP::language() const
{
	return "C++";
}


// Returns the lexer name.
const char *QextScintillaLexerCPP::lexer() const
{
	return (nocase ? "cppnocase" : "cpp");
}


// Return the list of keywords that can start a block.
const char *QextScintillaLexerCPP::blockStartKeyword(int *style) const
{
	if (style)
		*style = Keyword;

	return "case catch class default do else for if private protected "
	       "public struct try union while";
}


// Return the list of characters that can start a block.
const char *QextScintillaLexerCPP::blockStart(int *style) const
{
	if (style)
		*style = Operator;

	return "{";
}


// Return the list of characters that can end a block.
const char *QextScintillaLexerCPP::blockEnd(int *style) const
{
	if (style)
		*style = Operator;

	return "}";
}


// Return the style used for braces.
int QextScintillaLexerCPP::braceStyle() const
{
	return Operator;
}


// Return the string of characters that comprise a word.
const char *QextScintillaLexerCPP::wordCharacters() const
{
	return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#";
}


// Returns the foreground colour of the text for a style.
QColor QextScintillaLexerCPP::color(int style) const
{
	switch (style)
	{
	case Default:
		return QColor(0x80,0x80,0x80);

	case Comment:
	case CommentLine:
		return QColor(0x00,0x7f,0x00);

	case CommentDoc:
	case CommentLineDoc:
		return QColor(0x3f,0x70,0x3f);

	case Number:
		return QColor(0x00,0x7f,0x7f);

	case Keyword:
		return QColor(0x00,0x00,0x7f);

	case DoubleQuotedString:
	case SingleQuotedString:
		return QColor(0x7f,0x00,0x7f);

	case PreProcessor:
		return QColor(0x7f,0x7f,0x00);

	case Operator:
	case UnclosedString:
		return QColor(0x00,0x00,0x00);

	case Identifier:
		break;

	case Regex:
		return QColor(0x3f,0x7f,0x3f);

	case CommentDocKeyword:
		return QColor(0x30,0x60,0xa0);

	case CommentDocKeywordError:
		return QColor(0x80,0x40,0x20);
	}

	return QextScintillaLexer::color(style);
}


// Returns the end-of-line fill for a style.
bool QextScintillaLexerCPP::eolFill(int style) const
{
	return (style == UnclosedString);
}


// Returns the font of the text for a style.
QFont QextScintillaLexerCPP::font(int style) const
{
	QFont f;

	switch (style)
	{
	case Comment:
	case CommentLine:
	case CommentDoc:
	case CommentLineDoc:
	case CommentDocKeyword:
	case CommentDocKeywordError:
#if defined(Q_OS_WIN)
		f = QFont("Comic Sans MS",9);
#else
		f = QFont("new century schoolbook",12);
#endif
		break;

	case Keyword:
	case Operator:
		f = QextScintillaLexer::font(style);
		f.setBold(TRUE);
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

	default:
		f = QextScintillaLexer::font(style);
	}

	return f;
}


// Returns the set of keywords.
const char *QextScintillaLexerCPP::keywords(int set) const
{
	if (set == 0)
		return
			"and and_eq asm auto bitand bitor bool break case "
			"catch char class compl const const_cast continue "
			"default delete do double dynamic_cast else enum "
			"explicit export extern false float for friend goto if "
			"inline int long mutable namespace new not not_eq "
			"operator or or_eq private protected public register "
			"reinterpret_cast return short signed sizeof static "
			"static_cast struct switch template this throw true "
			"try typedef typeid typename union unsigned using "
			"virtual void volatile wchar_t while xor xor_eq";

	if (set == 2)
		return
			"a addindex addtogroup anchor arg attention author b "
			"brief bug c class code date def defgroup deprecated "
			"dontinclude e em endcode endhtmlonly endif "
			"endlatexonly endlink endverbatim enum example "
			"exception f$ f[ f] file fn hideinitializer "
			"htmlinclude htmlonly if image include ingroup "
			"internal invariant interface latexonly li line link "
			"mainpage name namespace nosubgrouping note overload "
			"p page par param post pre ref relates remarks return "
			"retval sa section see showinitializer since skip "
			"skipline struct subsection test throw todo typedef "
			"union until var verbatim verbinclude version warning "
			"weakgroup $ @ \\ & < > # { }";

	return 0;
}


// Returns the user name of a style.
QString QextScintillaLexerCPP::description(int style) const
{
	switch (style)
	{
	case Default:
		return tr("Default");

	case Comment:
		return tr("C comment");

	case CommentLine:
		return tr("C++ comment");

	case CommentDoc:
		return tr("JavaDoc style C comment");

	case Number:
		return tr("Number");

	case Keyword:
		return tr("Keyword");

	case DoubleQuotedString:
		return tr("Double-quoted string");

	case SingleQuotedString:
		return tr("Single-quoted string");

	case PreProcessor:
		return tr("Pre-processor block");

	case Operator:
		return tr("Operator");

	case Identifier:
		return tr("Identifier");

	case UnclosedString:
		return tr("Unclosed string");

	case CommentLineDoc:
		return tr("JavaDoc style C++ comment");

	case KeywordSet2:
		return tr("Secondary keywords and identifiers");

	case CommentDocKeyword:
		return tr("JavaDoc keyword");

	case CommentDocKeywordError:
		return tr("JavaDoc keyword error");

	case GlobalClass:
		return tr("Global classes and typedefs");
	}

	return QString::null;
}


// Returns the background colour of the text for a style.
QColor QextScintillaLexerCPP::paper(int style) const
{
	if (style == UnclosedString)
		return QColor(0xe0,0xc0,0xe0);

	return QextScintillaLexer::paper(style);
}


// Refresh all properties.
void QextScintillaLexerCPP::refreshProperties()
{
	setAtElseProp();
	setCommentProp();
	setCompactProp();
	setPreprocProp();
	setStylePreprocProp();
}


// Read properties from the settings.
bool QextScintillaLexerCPP::readProperties(QSettings &qs,const QString &prefix)
{
	int rc = TRUE;
	bool ok, flag;

	// Read the fold at else flag.
	flag = qs.readBoolEntry(prefix + "foldatelse",FALSE,&ok);

	if (ok)
		fold_atelse = flag;
	else
		rc = FALSE;

	// Read the fold comments flag.
	flag = qs.readBoolEntry(prefix + "foldcomments",FALSE,&ok);

	if (ok)
		fold_comments = flag;
	else
		rc = FALSE;

	// Read the fold compact flag.
	flag = qs.readBoolEntry(prefix + "foldcompact",TRUE,&ok);

	if (ok)
		fold_compact = flag;
	else
		rc = FALSE;

	// Read the fold preprocessor flag.
	flag = qs.readBoolEntry(prefix + "foldpreprocessor",TRUE,&ok);

	if (ok)
		fold_preproc = flag;
	else
		rc = FALSE;

	// Read the style preprocessor flag.
	flag = qs.readBoolEntry(prefix + "stylepreprocessor",FALSE,&ok);

	if (ok)
		style_preproc = flag;
	else
		rc = FALSE;

	return rc;
}


// Write properties to the settings.
bool QextScintillaLexerCPP::writeProperties(QSettings &qs,const QString &prefix) const
{
	int rc = TRUE;

	// Write the fold at else flag.
	if (!qs.writeEntry(prefix + "foldatelse",fold_atelse))
		rc = FALSE;

	// Write the fold comments flag.
	if (!qs.writeEntry(prefix + "foldcomments",fold_comments))
		rc = FALSE;

	// Write the fold compact flag.
	if (!qs.writeEntry(prefix + "foldcompact",fold_compact))
		rc = FALSE;

	// Write the fold preprocessor flag.
	if (!qs.writeEntry(prefix + "foldpreprocessor",fold_preproc))
		rc = FALSE;

	// Write the style preprocessor flag.
	if (!qs.writeEntry(prefix + "stylepreprocessor",style_preproc))
		rc = FALSE;

	return rc;
}


// Return TRUE if else can be folded.
bool QextScintillaLexerCPP::foldAtElse() const
{
	return fold_atelse;
}


// Set if else can be folded.
void QextScintillaLexerCPP::setFoldAtElse(bool fold)
{
	fold_atelse = fold;

	setAtElseProp();
}


// Set the "fold.at.else" property.
void QextScintillaLexerCPP::setAtElseProp()
{
	emit propertyChanged("fold.at.else",(fold_atelse ? "1" : "0"));
}


// Return TRUE if comments can be folded.
bool QextScintillaLexerCPP::foldComments() const
{
	return fold_comments;
}


// Set if comments can be folded.
void QextScintillaLexerCPP::setFoldComments(bool fold)
{
	fold_comments = fold;

	setCommentProp();
}


// Set the "fold.comment" property.
void QextScintillaLexerCPP::setCommentProp()
{
	emit propertyChanged("fold.comment",(fold_comments ? "1" : "0"));
}


// Return TRUE if folds are compact.
bool QextScintillaLexerCPP::foldCompact() const
{
	return fold_compact;
}


// Set if folds are compact
void QextScintillaLexerCPP::setFoldCompact(bool fold)
{
	fold_compact = fold;

	setCompactProp();
}


// Set the "fold.compact" property.
void QextScintillaLexerCPP::setCompactProp()
{
	emit propertyChanged("fold.compact",(fold_compact ? "1" : "0"));
}


// Return TRUE if preprocessor blocks can be folded.
bool QextScintillaLexerCPP::foldPreprocessor() const
{
	return fold_preproc;
}


// Set if preprocessor blocks can be folded.
void QextScintillaLexerCPP::setFoldPreprocessor(bool fold)
{
	fold_preproc = fold;

	setPreprocProp();
}


// Set the "fold.preprocessor" property.
void QextScintillaLexerCPP::setPreprocProp()
{
	emit propertyChanged("fold.preprocessor",(fold_preproc ? "1" : "0"));
}


// Return TRUE if preprocessor lines are styled.
bool QextScintillaLexerCPP::stylePreprocessor() const
{
	return style_preproc;
}


// Set if preprocessor lines are styled.
void QextScintillaLexerCPP::setStylePreprocessor(bool style)
{
	style_preproc = style;

	setStylePreprocProp();
}


// Set the "style.within.preprocessor" property.
void QextScintillaLexerCPP::setStylePreprocProp()
{
	emit propertyChanged("style.within.preprocessor",(style_preproc ? "1" : "0"));
}
