// This defines the interface to the QextScintillaLexerCPP class.
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


#ifndef QEXTSCINTILLALEXERCPP_H
#define QEXTSCINTILLALEXERCPP_H


#include <qobject.h>

#include <qextscintillaglobal.h>
#include <qextscintillalexer.h>


//! \brief The QextScintillaLexerCPP class encapsulates the Scintilla C++
//! lexer.
class QEXTSCINTILLA_EXPORT QextScintillaLexerCPP : public QextScintillaLexer
{
	Q_OBJECT

public:
	//! This enum defines the meanings of the different styles used by the
	//! C++ lexer.
	enum {
		//! The default.
		Default = 0,

		//! \obsolete White space.
		WhiteSpace = Default,

		//! A C comment.
		Comment = 1,

		//! A C++ comment line.
		CommentLine = 2,

		//! A JavaDoc style C comment.
		CommentDoc = 3,

		//! A number.
		Number = 4,

		//! A keyword.
		Keyword = 5,

		//! A double-quoted string.
		DoubleQuotedString = 6,

		//! A single-quoted string.
		SingleQuotedString = 7,

		//! An IDL UUID.
		UUID = 8,

		//! A pre-processor block.
		PreProcessor = 9,

		//! An operator.
		Operator = 10,

		//! An identifier
		Identifier = 11,

		//! The end of a line where a string is not closed.
		UnclosedString = 12,

		//! A C# verbatim string.
		VerbatimString = 13,

		//! A JavaScript regular expression.
		Regex = 14,

		//! A JavaDoc style C++ comment line.
		CommentLineDoc = 15,

		//! A keyword defined in keyword set number 2.  The class must
		//! be sub-classed and re-implement keywords() to make use of
		//! this style.
		KeywordSet2 = 16,

		//! A JavaDoc keyword.
		CommentDocKeyword = 17,

		//! A JavaDoc keyword error.
		CommentDocKeywordError = 18,

		//! A global class or typedef defined in keyword set number 4.
		//! The class must be sub-classed and re-implement keywords()
		//! to make use of this style.
		GlobalClass = 19
	};

	//! Construct a QextScintillaLexerCPP with parent \a parent and name
	//! \a name.  \a parent is typically the QextScintilla instance.
	//! \a caseInsensitiveKeywords is TRUE if the lexer ignores the case of
	//! keywords.
	QextScintillaLexerCPP(QObject *parent = 0,const char *name = 0,
			      bool caseInsensitiveKeywords = FALSE);

	//! Destroys the QextScintillaLexerCPP instance.
	virtual ~QextScintillaLexerCPP();

	//! Returns the name of the language.
	const char *language() const;

	//! Returns the name of the lexer.  Some lexers support a number of
	//! languages.
	const char *lexer() const;

	//! \internal Returns a space separated list of words or characters in
	//! a particular style that define the end of a block for
	//! auto-indentation.  The styles is returned via \a style.
	const char *blockEnd(int *style = 0) const;

	//! \internal Returns a space separated list of words or characters in
	//! a particular style that define the start of a block for
	//! auto-indentation.  The styles is returned via \a style.
	const char *blockStart(int *style = 0) const;

	//! \internal Returns a space separated list of keywords in a
	//! particular style that define the start of a block for
	//! auto-indentation.  The styles is returned via \a style.
	const char *blockStartKeyword(int *style = 0) const;

	//! \internal Returns the style used for braces for brace matching.
	int braceStyle() const;

	//! \internal Returns the string of characters that comprise a word.
	const char *wordCharacters() const;

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

	//! Causes all properties to be refreshed by emitting the
	//! propertyChanged() signal as required.
	void refreshProperties();

	//! Returns TRUE if "} else {" lines can be folded.
	//!
	//! \sa setFoldAtElse()
	bool foldAtElse() const;

	//! Returns TRUE if multi-line comment blocks can be folded.
	//!
	//! \sa setFoldComments()
	bool foldComments() const;

	//! Returns TRUE if trailing blank lines are included in a fold block.
	//!
	//! \sa setFoldCompact()
	bool foldCompact() const;

	//! Returns TRUE if preprocessor blocks can be folded.
	//!
	//! \sa setFoldPreprocessor()
	bool foldPreprocessor() const;

	//! Returns TRUE if preprocessor lines (after the preprocessor
	//! directive) are styled.
	//!
	//! \sa setStylePreprocessor()
	bool stylePreprocessor() const;

public slots:
	//! If \a fold is TRUE then "} else {" lines can be folded.  The
	//! default is FALSE.
	//!
	//! \sa foldAtElse()
	virtual void setFoldAtElse(bool fold);

	//! If \a fold is TRUE then multi-line comment blocks can be folded.
	//! The default is FALSE.
	//!
	//! \sa foldComments()
	virtual void setFoldComments(bool fold);

	//! If \a fold is TRUE then trailing blank lines are included in a fold
	//! block. The default is TRUE.
	//!
	//! \sa foldCompact()
	virtual void setFoldCompact(bool fold);

	//! If \a fold is TRUE then preprocessor blocks can be folded.  The
	//! default is TRUE.
	//!
	//! \sa foldPreprocessor()
	virtual void setFoldPreprocessor(bool fold);

	//! If \a style is TRUE then preprocessor lines (after the preprocessor
	//! directive) are styled.  The default is FALSE.
	//!
	//! \sa stylePreprocessor()
	virtual void setStylePreprocessor(bool style);

protected:
	//! The lexer's properties are read from the settings \a qs.  \a prefix
	//! (which has a trailing '/') should be used as a prefix to the key of
	//! each setting.  TRUE is returned if there is no error.
	//!
	bool readProperties(QSettings &qs,const QString &prefix);

	//! The lexer's properties are written to the settings \a qs.
	//! \a prefix (which has a trailing '/') should be used as a prefix to
	//! the key of each setting.  TRUE is returned if there is no error.
	//!
	bool writeProperties(QSettings &qs,const QString &prefix) const;

private:
	void setAtElseProp();
	void setCommentProp();
	void setCompactProp();
	void setPreprocProp();
	void setStylePreprocProp();

	bool fold_atelse;
	bool fold_comments;
	bool fold_compact;
	bool fold_preproc;
	bool style_preproc;

	bool nocase;

#if defined(Q_DISABLE_COPY)
	QextScintillaLexerCPP(const QextScintillaLexerCPP &);
	QextScintillaLexerCPP &operator=(const QextScintillaLexerCPP &);
#endif
};

#endif
