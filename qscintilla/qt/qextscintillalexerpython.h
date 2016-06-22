// This defines the interface to the QextScintillaLexerPython class.
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


#ifndef QEXTSCINTILLALEXERPYTHON_H
#define QEXTSCINTILLALEXERPYTHON_H


#include <qobject.h>

#include <qextscintillaglobal.h>
#include <qextscintillalexer.h>


//! \brief The QextScintillaLexerPython class encapsulates the Scintilla Python
//! lexer.
class QEXTSCINTILLA_EXPORT QextScintillaLexerPython : public QextScintillaLexer
{
	Q_OBJECT

public:
	//! This enum defines the meanings of the different styles used by the
	//! Python lexer.
	enum {
		//! The default.
		Default = 0,

		//! \obsolete White space.
		WhiteSpace = Default,

		//! A comment.
		Comment = 1,

		//! A number.
		Number = 2,

		//! A double-quoted string.
		DoubleQuotedString = 3,

		//! A single-quoted string.
		SingleQuotedString = 4,

		//! A keyword.
		Keyword = 5,

		//! A triple single-quoted string.
		TripleSingleQuotedString = 6,

		//! A triple double-quoted string.
		TripleDoubleQuotedString = 7,

		//! The name of a class.
		ClassName = 8,

		//! The name of a function or method.
		FunctionMethodName = 9,

		//! An operator.
		Operator = 10,

		//! An identifier
		Identifier = 11,

		//! A comment block.
		CommentBlock = 12,

		//! The end of a line where a string is not closed.
		UnclosedString = 13
	};

	//! This enum defines the different conditions that can cause
	//! indentations to be displayed as being bad.
	enum IndentationWarning {
		//! Bad indentation is not displayed differently.
		NoWarning = 0,

		//! The indentation is inconsistent when compared to the
		//! previous line, ie. it is made up of a different combination
		//! of tabs and/or spaces.
		Inconsistent = 1,

		//! The indentation is made up of spaces followed by tabs.
		TabsAfterSpaces = 2,

		//! The indentation contains spaces.
		Spaces = 3,

		//! The indentation contains tabs.
		Tabs = 4
	};

	//! Construct a QextScintillaLexerPython with parent \a parent and name
	//! \a name.  \a parent is typically the QextScintilla instance.
	QextScintillaLexerPython(QObject *parent = 0,const char *name = 0);

	//! Destroys the QextScintillaLexerPython instance.
	virtual ~QextScintillaLexerPython();

	//! Returns the name of the language.
	const char *language() const;

	//! Returns the name of the lexer.  Some lexers support a number of
	//! languages.
	const char *lexer() const;

	//! \internal Returns the number of lines prior to the current one when
	//! determining the scope of a block when auto-indenting.
	int blockLookback() const;

	//! \internal Returns a space separated list of words or characters in
	//! a particular style that define the start of a block for
	//! auto-indentation.  The styles is returned via \a style.
	const char *blockStart(int *style = 0) const;

	//! \internal Returns the style used for braces for brace matching.
	int braceStyle() const;

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

	//! Returns TRUE if indented comment blocks can be folded.
	//!
	//! \sa setFoldComments()
	bool foldComments() const;

	//! Returns TRUE if triple quoted strings can be folded.
	//!
	//! \sa setFoldQuotes()
	bool foldQuotes() const;

	//! Returns the condition that will cause bad indentations to be
	//! displayed.
	//!
	//! \sa setIndentationWarning()
	IndentationWarning indentationWarning() const;

public slots:
	//! If \a fold is TRUE then indented comment blocks can be folded.  The
	//! default is FALSE.
	//!
	//! \sa foldComments()
	virtual void setFoldComments(bool fold);

	//! If \a fold is TRUE then triple quoted strings can be folded.  The
	//! default is FALSE.
	//!
	//! \sa foldQuotes()
	virtual void setFoldQuotes(bool fold);

	//! Sets the condition that will cause bad indentations to be
	//! displayed.
	//!
	//! \sa indentationWarning()
	virtual void setIndentationWarning(IndentationWarning warn);

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
	void setCommentProp();
	void setQuotesProp();
	void setTabWhingeProp();

	bool fold_comments;
	bool fold_quotes;
	IndentationWarning indent_warn;

	friend class QextScintillaLexerHTML;

	static const char *keywordClass;

#if defined(Q_DISABLE_COPY)
	QextScintillaLexerPython(const QextScintillaLexerPython &);
	QextScintillaLexerPython &operator=(const QextScintillaLexerPython &);
#endif
};

#endif
