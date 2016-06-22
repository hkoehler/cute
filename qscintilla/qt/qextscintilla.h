// This module defines the "official" high-level API of the Qt port of
// Scintilla.
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


#ifndef QEXTSCINTILLA_H
#define QEXTSCINTILLA_H


#include <qobject.h>
#include <qguardedptr.h>

#include <qextscintillaglobal.h>
#include <qextscintillabase.h>
#include <qextscintilladocument.h>


class QColor;
class QextScintillaLexer;
class QextScintillaAPIs;
class QextScintillaCommandSet;


//! \brief The QextScintilla class implements a higher level, more Qt-like,
//! API to the Scintilla editor widget.
//!
//! QextScintilla implements methods, signals and slots similar to those found
//! in other Qt editor classes. It also provides a higher level interface to
//! features specific to Scintilla such as syntax styling, call tips,
//! auto-indenting and auto-completion than that provided by QextScintillaBase.
//!
//! The API is modelled on QTextEdit - a method of the same name should behave
//! in the same way.
class QEXTSCINTILLA_EXPORT QextScintilla : public QextScintillaBase
{
	Q_OBJECT

public:
	//! This enum defines the different auto-indentation styles.
	enum {
		//! A line is automatically indented to match the previous
		//! line.
		AiMaintain = 0x01,

		//! If the language supported by the current lexer has a
		//! specific start of block character (e.g. { in C++), then a
		//! line that begins with that character is indented as well as
		//! the lines that make up the block.  It may be logically ored
		//! with AiClosing.
		AiOpening = 0x02,

		//! If the language supported by the current lexer has a
		//! specific end of block character (e.g. } in C++), then a
		//! line that begins with that character is indented as well as
		//! the lines that make up the block.  It may be logically ored
		//! with AiOpening.
		AiClosing = 0x04
	};

	//! This enum defines the different sources for auto-completion lists.
	enum AutoCompletionSource {
		//! The source is the current document.
		AcsDocument,

		//! The source is any installed APIs.
		AcsAPIs
	};

	//! This enum defines the different brace matching modes.  The
	//! character pairs {}, [] and () are treated as braces.  The Python
	//! lexer will also match a : with the end of the corresponding
	//! indented block.
	enum BraceMatch {
		//! Brace matching is disabled.
		NoBraceMatch,

		//! Brace matching is enabled for a brace immediately before
		//! the current position.
		StrictBraceMatch,

		//! Brace matching is enabled for a brace immediately before or
		//! after the current position.
		SloppyBraceMatch
	};

	//! This enum defines the different end-of-line modes.
	enum EolMode {
		//! A carriage return/line feed as used on Windows systems.
		EolWindows = SC_EOL_CRLF,

		//! A line feed as used on Unix systems.
		EolUnix = SC_EOL_LF,

		//! A carriage return as used on Mac systems.
		EolMac = SC_EOL_CR
	};

	//! This enum defines the different styles for the folding margin.
	enum FoldStyle {
		//! Folding is disabled.
		NoFoldStyle,

		//! Plain folding style using plus and minus symbols.
		PlainFoldStyle,

		//! Circled folding style using circled plus and minus symbols.
		CircledFoldStyle,

		//! Boxed folding style using boxed plus and minus symbols.
		BoxedFoldStyle,

		//! Circled tree style using a flattened tree with circled plus
		//! and minus symbols and rounded corners.
		CircledTreeFoldStyle,

		//! Boxed tree style using a flattened tree with boxed plus and
		//! minus symbols and right-angled corners.
		BoxedTreeFoldStyle
	};

	//! This enum defines the different pre-defined marker symbols.
	enum MarkerSymbol {
		//! A circle.
		Circle = SC_MARK_CIRCLE,

		//! A rectangle.
		Rectangle = SC_MARK_ROUNDRECT,

		//! A triangle pointing to the right.
		RightTriangle = SC_MARK_ARROW,

		//! A smaller rectangle.
		SmallRectangle = SC_MARK_SMALLRECT,

		//! An arrow pointing to the right.
		RightArrow = SC_MARK_SHORTARROW,

		//! An invisible marker that allows code to track the movement
		//! of lines.
		Invisible = SC_MARK_EMPTY,

		//! A triangle pointing down.
		DownTriangle = SC_MARK_ARROWDOWN,

		//! A drawn minus sign.
		Minus = SC_MARK_MINUS,

		//! A drawn plus sign.
		Plus = SC_MARK_PLUS,

		//! A vertical line drawn in the background colour.
		VerticalLine = SC_MARK_VLINE,

		//! A bottom left corner drawn in the background colour.
		BottomLeftCorner = SC_MARK_LCORNER,

		//! A vertical line with a centre right horizontal line drawn
		//! in the background colour.
		LeftSideSplitter = SC_MARK_TCORNER,

		//! A drawn plus sign in a box.
		BoxedPlus = SC_MARK_BOXPLUS,

		//! A drawn plus sign in a connected box.
		BoxedPlusConnected = SC_MARK_BOXPLUSCONNECTED,

		//! A drawn minus sign in a box.
		BoxedMinus = SC_MARK_BOXMINUS,

		//! A drawn minus sign in a connected box.
		BoxedMinusConnected = SC_MARK_BOXMINUSCONNECTED,

		//! A rounded bottom left corner drawn in the background
		//! colour.
		RoundedBottomLeftCorner = SC_MARK_LCORNERCURVE,

		//! A vertical line with a centre right curved line drawn in
		//! the background colour.
		LeftSideRoundedSplitter = SC_MARK_TCORNERCURVE,

		//! A drawn plus sign in a circle.
		CircledPlus = SC_MARK_CIRCLEPLUS,

		//! A drawn plus sign in a connected box.
		CircledPlusConnected = SC_MARK_CIRCLEPLUSCONNECTED,

		//! A drawn minus sign in a circle.
		CircledMinus = SC_MARK_CIRCLEMINUS,

		//! A drawn minus sign in a connected circle.
		CircledMinusConnected = SC_MARK_CIRCLEMINUSCONNECTED,

		//! No symbol is drawn but the line of text is drawn with the
		//! same background colour.
		Background = SC_MARK_BACKGROUND,

		//! Three drawn dots.
		ThreeDots = SC_MARK_DOTDOTDOT,

		//! Three drawn arrows pointing right.
		ThreeRightArrows = SC_MARK_ARROWS
	};

	//! This enum defines the different whitespace visibility modes.  When
	//! whitespace is visible spaces are displayed as small centred dots
	//! and tabs are displayed as light arrows pointing to the right.
	enum WhitespaceVisibility {
		//! Whitespace is invisible.
		WsInvisible = SCWS_INVISIBLE,

		//! Whitespace is always visible.
		WsVisible = SCWS_VISIBLEALWAYS,

		//! Whitespace is visible after the whitespace used for
		//! indentation.
		WsVisibleAfterIndent = SCWS_VISIBLEAFTERINDENT
	};

	//! This enum defines the different line wrap modes.
	enum WrapMode {
		//! Lines are not wrapped.
		WrapNone = SC_WRAP_NONE,

		//! Lines are wrapped at word boundaries.
		WrapWord = SC_WRAP_WORD
        };

	//! Construct an empty QextScintilla with parent \a parent, name \a
	//! name, and widget flags \a f.
	QextScintilla(QWidget *parent = 0,const char *name = 0,WFlags f = 0);

	//! Destroys the QextScintilla instance.
	virtual ~QextScintilla();

	//! Returns TRUE if auto-completion lists are case sensitive.
	//!
	//! \sa setAutoCompletionCaseSensitivity()
	bool autoCompletionCaseSensitivity();

	//! Returns TRUE if the rest of the word to the right of the current
	//! cursor is removed when an item from an auto-completion list is
	//! selected.
	//!
	//! \sa setAutoCompletionReplaceWord()
	bool autoCompletionReplaceWord();

	//! Returns TRUE if the only item in an auto-completion list with a
	//! single entry is automatically used and the list not displayed.
	//!
	//! \sa setAutoCompletionShowSingle()
	bool autoCompletionShowSingle();

	//! Returns the current source for the auto-completion list when it is
	//! being displayed automatically as the user types.
	//!
	//! \sa setAutoCompletionSource()
	AutoCompletionSource autoCompletionSource() const {return acSource;}

	//! Returns the current threshold for the automatic display of the
	//! auto-completion list as the user types.
	//!
	//! \sa setAutoCompletionThreshold()
	int autoCompletionThreshold() const {return acThresh;}

	//! Returns TRUE if auto-indentation is enabled.
	//!
	//! \sa setAutoIndent()
	bool autoIndent() const {return autoInd;}

	//! Returns TRUE if the backspace key unindents a line instead of
	//! deleting a character.  The default is FALSE.
	//!
	//! \sa setBackspaceUnindents(), tabIndents(), setTabIndents()
	bool backspaceUnindents();

	//! Mark the beginning of a sequence of actions that can be undone by
	//! a single call to undo().
	//!
	//! \sa endUndoAction(), undo()
	void beginUndoAction();

	//! Returns the brace matching mode.
	//!
	//! \sa setBraceMatching()
	BraceMatch braceMatching() const {return braceMode;}

	//! Returns the maximum number of call tips that are displayed.
	//!
	//! \sa setCallTipsVisible()
	int callTipsVisible() const {return maxCallTips;}

	//! Clear all registered images.
	//!
	//! \sa registerImage()
	void clearRegisteredImages();

	//! All the lines of the text have their end-of-lines converted to mode
	//! \a mode.
	//!
	//! \sa eolMode(), setEolMode()
	void convertEols(EolMode mode);

	//! Returns the attached document.
	//!
	//! \sa setDocument()
	QextScintillaDocument document() {return doc;}

	//! Mark the end of a sequence of actions that can be undone by a
	//! single call to undo().
	//!
	//! \sa beginUndoAction(), undo()
	void endUndoAction();

	//! Returns the end-of-line mode.
	//!
	//! \sa setEolMode()
	EolMode eolMode();

	//! Returns the visibility of end-of-lines.
	//!
	//! \sa setEolVisibility()
	bool eolVisibility();

	//! Find the next occurrence of the string \a expr and return TRUE if
	//! \a expr was found, otherwise returns FALSE.  If \a expr is found it
	//! becomes the current selection.
	//!
	//! If \a re is TRUE then \a expr is interpreted as a regular
	//! expression rather than a simple string.
	//!
	//! If \a cs is TRUE then the search is case sensitive.
	//!
	//! If \a wo is TRUE then the search looks for whole word matches only,
	//! otherwise it searches for any matching text.
	//!
	//! If \a wrap is TRUE then the search wraps around the end of the
	//! text.
	//!
	//! If \a forward is TRUE (the default) then the search is forward from
	//! the starting position to the end of the text, otherwise it is
	//! backwards to the beginning of the text. 
	//!
	//! If either \a line or \a index are negative (the default) then the
	//! search begins from the current cursor position.  Otherwise the
	//! search begins at position \a index of line \a line.
	//!
	//! \sa findNext(), replace()
	virtual bool findFirst(const QString &expr,bool re,bool cs,bool wo,
			       bool wrap,bool forward = TRUE,int line = -1,
			       int index = -1);

	//! Find the next occurence of the string found using findFirst().
	//!
	//! \sa findFirst(), replace()
	virtual bool findNext();

	//! Returns the current folding style.
	//!
	//! \sa setFolding()
	FoldStyle folding() const {return fold;}

	//! Sets \a *line and \a *index to the line and index of the cursor.
	//!
	//! \sa setCursorPosition()
	void getCursorPosition(int *line,int *index);

	//! If there is a selection, \a *lineFrom is set to the line number in
	//! which the selection begins and \a *lineTo is set to the line number
	//! in which the selection ends.  (They could be the same.)
	//! \a *indexFrom is set to the index at which the selection begins
	//! within \a *lineFrom, and \a *indexTo is set to the index at which
	//! the selection ends within \a *lineTo.  If there is no selection,
	//! \a *lineFrom, \a *indexFrom, \a *lineTo and \a *indexTo are all set
	//! to -1. 
	//!
	//! \sa setSelection()
	void getSelection(int *lineFrom,int *indexFrom,
			  int *lineTo,int *indexTo);

	//! Returns TRUE if some text is selected.
	//!
	//! \sa selectedText()
	bool hasSelectedText() const {return selText;}

	//! Returns the number of characters that line \a line is indented by.
	//!
	//! \sa setIndentation()
	int indentation(int line);

	//! Returns TRUE if the display of indentation guides is enabled.
	//!
	//! \sa setIndentationGuides()
	bool indentationGuides();

	//! Returns TRUE if indentations are created using tabs and spaces,
	//! rather than just spaces.  The default is TRUE.
	//!
	//! \sa setIndentationsUseTabs()
	bool indentationsUseTabs();

	//! Returns the indentation width in characters.  The default is 0
	//! which means that the value returned by tabWidth() is actually used.
	//!
	//! \sa setIndentationWidth(), tabWidth()
	int indentationWidth();

	//! Returns TRUE if the text has been modified.
	//!
	//! \sa setModified(), modificationChanged()
	bool isModified();

	//! Returns TRUE if the text edit is read-only.
	//!
	//! \sa setReadOnly()
	bool isReadOnly();

	//! Returns TRUE if there is something that can be redone.
	//!
	//! \sa redo()
	bool isRedoAvailable();

	//! Returns TRUE if there is something that can be undone.
	//!
	//! \sa undo()
	bool isUndoAvailable();

	//! Returns TRUE if text is interpreted as being UTF8 encoded.  The
	//! default is to interpret the text as Latin1 encoded.
	//!
	//! \sa setUtf8()
	bool isUtf8();

	//! Returns the line which is at position \a pos or -1 if there is no
	//! line at that position.
	int lineAt(const QPoint &pos);

	//! Returns the length of line \a line or -1 if there is no such line.
	int lineLength(int line);

	//! Returns the number of lines of text.
	int lines();

	//! Returns the length of the text edit's text.
	int length();

	//! Returns TRUE if line numbers are enabled for margin \a margin.
	//!
	//! \sa setMarginLineNumbers(), QextScintillaBase::SCI_GETMARGINTYPEN
	bool marginLineNumbers(int margin);

	//! Returns the marker mask of margin \a margin.
	//!
	//! \sa setMarginMask(), QextScintillaMarker,
	//! QextScintillaBase::SCI_GETMARGINMASKN
	int marginMarkerMask(int margin);

	//! Returns TRUE if margin \a margin is sensitive to mouse clicks.
	//!
	//! \sa setMarginSensitivity(), marginClicked(),
	//! QextScintillaBase::SCI_GETMARGINTYPEN
	bool marginSensitivity(int margin);

	//! Returns the width in pixels of margin \a margin.
	//!
	//! \sa setMarginWidth(), QextScintillaBase::SCI_GETMARGINWIDTHN
	int marginWidth(int margin);

	//! Define a marker using the symbol \a sym with the marker number
	//! \a mnr.  If \a mnr is -1 then the marker number is automatically
	//! allocated.  The marker number is returned or -1 if the marker
	//! number was already allocated or too many markers have been defined.
	//!
	//! Markers are small geometric symbols and character used, for
	//! example, to indicate the current line or, in debuggers, to indicate
	//! breakpoints.  If a margin has a width of 0 then its markers are not
	//! drawn, but their background colours affect the background colour of
	//! the corresponding line of text.
	//!
	//! There may be up to 32 markers defined and each line of text has a
	//! set of these markers associated with it.  Markers are drawn
	//! according to their numerical identifier.  Markers try to move with
	//! their text by tracking where the start of their line moves to.  For
	//! example, when a line is deleted its markers are added to previous
	//! line's markers.
	//!
	//! Each marker is identified by a marker number.  Each instance of a
	//! marker is identified by a marker handle.
	int markerDefine(MarkerSymbol sym,int mnr = -1);

	//! Define a marker using the character \a ch with the marker number
	//! \a mnr.  If \a mnr is -1 then the marker number is automatically
	//! allocated.  The marker number is returned or -1 if the marker
	//! number was already allocated or too many markers have been defined.
	int markerDefine(char ch,int mnr = -1);

	//! Define a marker using a copy of the pixmap \a pm with the marker
	//! number \a mnr.  If \a mnr is -1 then the marker number is
	//! automatically allocated.  The marker number is returned or -1 if
	//! the marker number was already allocated or too many markers have
	//! been defined.
	int markerDefine(const QPixmap *pm,int mnr = -1);

	//! Add a marker number \a mnr to line number \a linenr.  A handle for
	//! the marker is returned which can be used to track the marker's
	//! position, or -1 if the \a mnr was invalid.
	//!
	//! \sa markerDelete(), markerDeleteAll(), markerDeleteHandle()
	int markerAdd(int linenr,int mnr);

	//! Returns the 32 bit mask of marker numbers at line number \a linenr.
	//!
	//! \sa markerAdd()
	unsigned markersAtLine(int linenr);

	//! Delete all markers with the marker number \a mnr in the line
	//! \a linenr.  If \a mnr is -1 then delete all markers from line
	//! \a linenr.
	//!
	//! \sa markerAdd(), markerDeleteAll(), markerDeleteHandle()
	void markerDelete(int linenr,int mnr = -1);

	//! Delete the all markers with the marker number \a mnr.  If \a mnr is
	//! -1 then delete all markers.
	//!
	//! \sa markerAdd(), markerDelete(), markerDeleteHandle()
	void markerDeleteAll(int mnr = -1);

	//! Delete the the marker instance with the marker handle \a mhandle.
	//!
	//! \sa markerAdd(), markerDelete(), markerDeleteAll()
	void markerDeleteHandle(int mhandle);

	//! Return the line number that contains the marker instance with the
	//! marker handle \a mhandle.
	int markerLine(int mhandle);

	//! Return the number of the next line to contain at least one marker
	//! from a 32 bit mask of markers.  \a linenr is the line number to
	//! start the search from.  \a mask is the mask of markers to search
	//! for.
	//!
	//! \sa markerFindPrevious()
	int markerFindNext(int linenr,unsigned mask);

	//! Return the number of the previous line to contain at least one
	//! marker from a 32 bit mask of markers.  \a linenr is the line number
	//! to start the search from.  \a mask is the mask of markers to search
	//! for.
	//!
	//! \sa markerFindNext()
	int markerFindPrevious(int linenr,unsigned mask);

	//! Recolours the document between the \a start and \a end positions.
	//! \a start defaults to the start of the document and \a end defaults
	//! to the end of the document.
	virtual void recolor(int start = 0,int end = -1);

	//! Register an image \a pm with ID \a id.  Registered images can be
	//! displayed in auto-completion lists.
	//!
	//! \sa clearRegisteredImages(), QextScintillaAPIs
	void registerImage(int id,const QPixmap *pm);

	//! Replace the current selection, set by a previous call to
	//! findFirst() or findNext(), with \a replaceStr.
	//!
	//! \sa findFirst(), findNext()
	virtual void replace(const QString &replaceStr);

	//! Reset the fold margin colours to their defaults.
	//!
	//! \sa setFoldMarginColors()
	void resetFoldMarginColors();

	//! The fold margin may be drawn as a one pixel sized checkerboard
	//! pattern of two colours, \a fore and \a back.
	//!
	//! \sa resetFoldMarginColors()
	void setFoldMarginColors(const QColor &fore,const QColor &back);

	//! Sets the APIs used for auto-completion to \a apis.  If \a apis is 0
	//! then any existing APIs are removed.
	//!
	//! \sa autoCompleteFromAPIs(), setCallTipsAPIs()
	void setAutoCompletionAPIs(QextScintillaAPIs *apis = 0);

	//! A fill-up character is one that, when entered while an
	//! auto-completion list is being displayed, causes the currently
	//! selected item from the list to be added to the text followed by the
	//! fill-up character.  \a fillups is the set of fill-up characters.
	//! The default is that no fill-up characters are set.
	void setAutoCompletionFillups(const char *fillups);

	//! Sets the APIs used for call tips to \a apis.  If \a apis is 0 then
	//! then call tips are disabled.
	//!
	//! \sa setAutoCompletionAPIs()
	void setCallTipsAPIs(QextScintillaAPIs *apis = 0);

	//! Set the background colour of call tips to \a col.  The default is
	//! white.
	void setCallTipsBackgroundColor(const QColor &col);

	//! Set the foreground colour of call tips to \a col.  The default is
	//! mid-gray.
	void setCallTipsForegroundColor(const QColor &col);

	//! Set the highlighted colour of call tip text to \a col.  The default
	//! is dark blue.
	void setCallTipsHighlightColor(const QColor &col);

	//! Set the maximum number of call tips that are displayed to \a nr.
	//! If the maximum number is 0 then all applicable call tips are
	//! displayed.  If the maximum number is -1 then one call tip will be
	//! displayed with up and down arrows that allow the use to scroll
	//! through the full list.  The default is -1.
	//!
	//! \sa callTipsVisible()
	void setCallTipsVisible(int nr);

	//! Attach the document \a document, replacing the currently attached
	//! document.
	//!
	//! \sa document()
	void setDocument(const QextScintillaDocument &document);

	//! Set the background colour of marker \a mnr to \a col.  If \a mnr is
	//! -1 then the colour of all markers is set.  The default is white.
	//!
	//! \sa setMarkerForegroundColor()
	void setMarkerBackgroundColor(const QColor &col,int mnr = -1);

	//! Set the foreground colour of marker \a mnr to \a col.  If \a mnr is
	//! -1 then the colour of all markers is set.  The default is black.
	//!
	//! \sa setMarkerBackgroundColor()
	void setMarkerForegroundColor(const QColor &col,int mnr = -1);

	//! Set the background colour used to display matched braces to \a col.
	//! The default is white.
	//!
	//! \sa setMatchedBraceForegroundColor()
	void setMatchedBraceBackgroundColor(const QColor &col);

	//! Set the foreground colour used to display matched braces to \a col.
	//! The default is red.
	//!
	//! \sa setMatchedBraceBackgroundColor()
	void setMatchedBraceForegroundColor(const QColor &col);

	//! Set the background colour used to display unmatched braces to
	//! \a col.  The default is white.
	//!
	//! \sa setUnmatchedBraceForegroundColor()
	void setUnmatchedBraceBackgroundColor(const QColor &col);

	//! Set the foreground colour used to display unmatched braces to
	//! \a col.  The default is blue.
	//!
	//! \sa setUnmatchedBraceBackgroundColor()
	void setUnmatchedBraceForegroundColor(const QColor &col);

	//! Returns the selected text or an empty string if there is no
	//! currently selected text.
	//!
	//! \sa hasSelectedText()
	QString selectedText();

	//! The standard command set is returned.
	QextScintillaCommandSet *standardCommands() const {return stdCmds;}

	//! Returns TRUE if the tab key indents a line instead of inserting a
	//! tab character.  The default is TRUE.
	//!
	//! \sa setTabIndents(), backspaceUnindents(), setBackspaceUnindents()
	bool tabIndents();

	//! Returns the tab width in characters.  The default is 8.
	//!
	//! \sa setTabWidth()
	int tabWidth();

	//! Returns the text edit's text.
	//!
	//! \sa setText()
	QString text();

	//! \overload
	//!
	//! Returns the text of line \a line.
	//!
	//! \sa setText()
	QString text(int line);

	//! Returns the visibility of whitespace.
	//!
	//! \sa setWhitespaceVisibility()
	WhitespaceVisibility whitespaceVisibility();

public slots:
	//! Appends the text \a text to the end of the text edit.  Note that
	//! the undo/redo history is cleared by this function.
	virtual void append(const QString &text);

	//! Display an auto-completion list based on any installed APIs and the
	//! characters immediately to the left of the cursor.  If there are no
	//! significant characters to the left of the cursor, then the list is
	//! still displayed.
	//!
	//! \sa autoCompleteFromDocument(), setAutoCompletionAPIs()
	virtual void autoCompleteFromAPIs();

	//! Display an auto-completion list based on the current contents of
	//! the document and the characters immediately to the left of the
	//! cursor.  If there are no significant characters to the left of the
	//! cursor, then the list is not displayed.
	//!
	//! \sa autoCompleteFromAPIs()
	virtual void autoCompleteFromDocument();

	//! Display a call tip based on the the characters immediately to the
	//! left of the cursor.
	virtual void callTip();

	//! Deletes all the text in the text edit.
	virtual void clear();

	//! Copies any selected text to the clipboard.
	//!
	//! \sa copyAvailable(), cut(), paste()
	virtual void copy();

	//! Copies any selected text to the clipboard and then deletes the
	//! text.
	//!
	//! \sa copy(), paste()
	virtual void cut();

	//! Ensures that the cursor is visible.
	virtual void ensureCursorVisible();

	//! Ensures that the line number \a line is visible.
	virtual void ensureLineVisible(int line);

	//! If any lines are currently folded then they are all unfolded.
	//! Otherwise all lines are folded.  This has the same effect as
	//! clicking in the fold margin with the shift and control keys
	//! pressed.
	virtual void foldAll();

	//! If the line \a line is folded then it is unfolded.  Otherwise it is
	//! folded.  This has the same effect as clicking in the fold margin.
	virtual void foldLine(int line);

	//! Increases the indentation of line \a line by an indentation width.
	//!
	//! \sa unindent()
	virtual void indent(int line);

	//! Insert the text \a text in the line \a line at the position
	//! \a index.
	virtual void insertAt(const QString &text,int line,int index);

	//! If the cursor is either side of a brace character then move it to
	//! the position of the corresponding brace.
	virtual void moveToMatchingBrace();

	//! Pastes any text from the clipboard into the text edit at the
	//! current cursor position.
	//!
	//! \sa copy(), cut()
	virtual void paste();

	//! Redo the last change or sequence of changes.
	//!
	//! \sa isRedoAvailable()
	virtual void redo();

	//! Removes any selected text.
	virtual void removeSelectedText();

	//! If \a select is TRUE (the default) then all the text is selected.
	//! If \a select is FALSE then any currently selected text is
	//! deselected.
	virtual void selectAll(bool select = TRUE);

	//! If the cursor is either side of a brace character then move it to
	//! the position of the corresponding brace and select the text between
	//! the braces.
	virtual void selectToMatchingBrace();

	//! If \a cs is TRUE then auto-completion lists are case sensitive.
	//! The default is TRUE.
	//!
	//! \sa autoCompletionCaseSensitivity()
	virtual void setAutoCompletionCaseSensitivity(bool cs);

	//! If \a replace is TRUE then when an item from an auto-completion
	//! list is selected, the rest of the word to the right of the current
	//! cursor is removed.  The default is FALSE.
	//!
	//! \sa autoCompletionReplaceWord()
	virtual void setAutoCompletionReplaceWord(bool replace);

	//! If \a single is TRUE then when there is only a single entry in an
	//! auto-completion list it is automatically used and the list is not
	//! displayed.  This only has an effect when auto-completion is
	//! explicitly requested (using autoCompleteFromAPIs() and
	//! autoCompleteFromDocument()) and has no effect when auto-completion
	//! is triggered as the user types.  The default is FALSE.
	//!
	//! \sa autoCompletionShowSingle()
	virtual void setAutoCompletionShowSingle(bool single);

	//! Sets the source for the auto-completion list when it is being
	//! displayed automatically as the user types to \a source.  The
	//! default is AcsDocument.
	//!
	//! \sa autoCompletionSource()
	virtual void setAutoCompletionSource(AutoCompletionSource source);

	//! Sets the threshold for the automatic display of the auto-completion
	//! list as the user types to \a thresh.  The threshold is the number
	//! of characters that the user must type before the list is displayed.
	//! If the threshold is less than or equal to 0 then the list is
	//! disabled.  The default is -1.
	//!
	//! \sa autoCompletionThreshold()
	virtual void setAutoCompletionThreshold(int thresh);

	//! If \a autoindent is TRUE then auto-indentation is enabled.  The
	//! default is FALSE.
	//!
	//! \sa autoIndent()
	virtual void setAutoIndent(bool autoindent);

	//! Sets the brace matching mode to \a bm.  The default is
	//! NoBraceMatching.
	//!
	//! \sa braceMatching()
	virtual void setBraceMatching(BraceMatch bm);

	//! If \a deindent is TRUE then the backspace key will unindent a line
	//! rather then delete a character.
	//!
	//! \sa backspaceUnindents(), tabIndents(), setTabIndents()
	virtual void setBackspaceUnindents(bool unindent);

	//! Sets the foreground colour of the caret to \a col.
	virtual void setCaretForegroundColor(const QColor &col);

	//! Sets the background colour of the line containing the caret to \a
	//! col.
	//!
	//! \sa setCaretLineVisible()
	virtual void setCaretLineBackgroundColor(const QColor &col);

	//! Enables or disables, according to \a enable, the background color
	//! of the line containing the caret.
	//!
	//! \sa setCaretLineBackgroundColor()
	virtual void setCaretLineVisible(bool enable);

	//! Sets the width of the caret to \a width pixels.  A \a width of 0
	//! makes the caret invisible.
	virtual void setCaretWidth(int width);

	//! Sets the cursor to the line \a line at the position \a index.
	//!
	//! \sa getCursorPosition()
	virtual void setCursorPosition(int line,int index);

	//! Sets the end-of-line mode to \a mode.  The default is the
	//! platform's natural mode.
	//!
	//! \sa eolMode()
	virtual void setEolMode(EolMode mode);

	//! If \a visible is TRUE then end-of-lines are made visible.  The
	//! default is that they are invisible.
	//!
	//! \sa eolVisibility()
	virtual void setEolVisibility(bool visible);

	//! Sets the folding style for margin 2 to \a fold.  The default is
	//! NoFoldStyle (ie. folding is disabled).
	//!
	//! \sa folding()
	virtual void setFolding(FoldStyle fold);

	//! Sets the indentation of line \a to \a indentation characters.
	//!
	//! \sa indentation()
	virtual void setIndentation(int line,int indentation);

	//! Enables or disables, according to \a enable, this display of
	//! indentation guides.
	//!
	//! \sa indentationGuides()
	virtual void setIndentationGuides(bool enable);

	//! Set the background colour of indentation guides to \a col.
	//!
	//! \sa setIndentationGuidesForegroundColor()
	virtual void setIndentationGuidesBackgroundColor(const QColor &col);

	//! Set the foreground colour of indentation guides to \a col.
	//!
	//! \sa setIndentationGuidesBackgroundColor()
	virtual void setIndentationGuidesForegroundColor(const QColor &col);

	//! If \a tabs is TRUE then indentations are created using tabs and
	//! spaces, rather than just spaces.
	//!
	//! \sa indentationsUseTabs()
	virtual void setIndentationsUseTabs(bool tabs);

	//! Sets the indentation width to \a width characters.  If \a width is
	//! 0 then the value returned by tabWidth() is used.
	//!
	//! \sa indentationWidth(), tabWidth()
	virtual void setIndentationWidth(int width);

	//! Sets the specific language lexer used to style text to \a lex.  If
	//! \a lex is 0 then syntax styling is disabled.
	virtual void setLexer(QextScintillaLexer *lex = 0);

	//! Set the background colour of all margins to \a col.  The default is
	//! a gray.
	//!
	//! \sa setMarginsForegroundColor()
	virtual void setMarginsBackgroundColor(const QColor &col);

	//! Set the font used in all margins to \a f.
	virtual void setMarginsFont(const QFont &f);

	//! Set the foreground colour of all margins to \a col.  The default is
	//! black.
	//!
	//! \sa setMarginsBackgroundColor()
	virtual void setMarginsForegroundColor(const QColor &col);

	//! Enables or disables, according to \a lnrs, the display of line
	//! numbers in margin \a margin.
	//!
	//! \sa marginLineNumbers(), QextScintillaBase::SCI_SETMARGINTYPEN
	virtual void setMarginLineNumbers(int margin,bool lnrs);

	//! Sets the marker mask of margin \a margin to \a mask.  Only those
	//! markers whose bit is set in the mask are displayed in the margin.
	//!
	//! \sa marginMarkerMask(), QextScintillaMarker,
	//! QextScintillaBase::SCI_SETMARGINMASKN
	virtual void setMarginMarkerMask(int margin,int mask);

	//! Enables or disables, according to \a sens, the sensitivity of
	//! margin \a margin to mouse clicks.  If the user clicks in a
	//! sensitive margin the marginClicked() signal is emitted.
	//!
	//! \sa marginSensitivity(), marginClicked(),
	//! QextScintillaBase::SCI_SETMARGINSENSITIVEN
	virtual void setMarginSensitivity(int margin,bool sens);

	//! Sets the width of margin \a margin to \a width pixels.  If the
	//! width of a margin is 0 then it is not displayed.
	//!
	//! \sa marginWidth(), QextScintillaBase::SCI_SETMARGINWIDTHN
	virtual void setMarginWidth(int margin,int width);

	//! Sets the width of margin \a margin so that it is wide enough to
	//! display \a s in the current margin font.
	//!
	//! \sa marginWidth(), QextScintillaBase::SCI_SETMARGINWIDTHN
	virtual void setMarginWidth(int margin,const QString &s);

	//! Sets the modified state of the text edit to \a m.  Note that it is
	//! only possible to clear the modified state (where \a m is FALSE).
	//! Attempts to set the modified state (where \a m is TRUE) are
	//! ignored.
	//!
	//! \sa isModified(), modificationChanged()
	virtual void setModified(bool m);

	//! Sets the read-only state of the text edit to \a ro.
	//!
	//! \sa isReadOnly()
	virtual void setReadOnly(bool ro);

	//! Sets the selection which starts at position \a indexFrom in line
	//! \a lineFrom and ends at position \a indexTo in line \a lineTo.  The
	//! cursor is moved to the end of the selection.
	//!
	//! \sa getSelection()
	virtual void setSelection(int lineFrom,int indexFrom,
				  int lineTo,int indexTo);

	//! Sets the background colour of selected text to \a col.
	//!
	//! \sa setSelectionForegroundColor()
	virtual void setSelectionBackgroundColor(const QColor &col);

	//! Sets the foreground colour of selected text to \a col.
	//!
	//! \sa setSelectionBackgroundColor()
	virtual void setSelectionForegroundColor(const QColor &col);

	//! If \a indent is TRUE then the tab key will indent a line rather
	//! then insert a tab character.
	//!
	//! \sa tabIndents(), backspaceUnindents(), setBackspaceUnindents()
	virtual void setTabIndents(bool indent);

	//! Sets the tab width to \a width characters.
	//!
	//! \sa tabWidth()
	virtual void setTabWidth(int width);

	//! Replaces all of the current text with \a text.  Note that the
	//! undo/redo history is cleared by this function.
	//!
	//! \sa text()
	virtual void setText(const QString &text);

	//! Sets the current text encoding.  If \a cp is TRUE then UTF8 is
	//! used, otherwise Latin1 is used.
	//!
	//! \sa isUtf8()
	virtual void setUtf8(bool cp);

	//! Sets the visibility of whitespace to mode \a mode.  The default is
	//! that whitespace is invisible.
	//!
	//! \sa whitespaceVisibility()
	virtual void setWhitespaceVisibility(WhitespaceVisibility mode);

	//! Undo the last change or sequence of changes.
	//!
	//! Scintilla has multiple level undo and redo.  It will continue to
	//! record undoable actions until memory runs out.  Sequences of
	//! typing or deleting are compressed into single actions to make it
	//! easier to undo and redo at a sensible level of detail.  Sequences
	//! of actions can be combined into actions that are undone as a unit.
	//! These sequences occur between calls to beginUndoAction() and
	//! endUndoAction().  These sequences can be nested and only the top
	//! level sequences are undone as units. 
	//!
	//! \sa beginUndoAction(), endUndoAction(), isUndoAvailable()
	virtual void undo();

	//! Decreases the indentation of line \a line by an indentation width.
	//!
	//! \sa indent()
	virtual void unindent(int line);

	//! Zooms in on the text by by making the base font size \a range
	//! points larger and recalculating all font sizes.
	//!
	//! \sa zoomOut(), zoomTo()
	virtual void zoomIn(int range);

	//! \overload
	//!
	//! Zooms in on the text by by making the base font size one point
	//! larger and recalculating all font sizes.
	virtual void zoomIn();

	//! Zooms out on the text by by making the base font size \a range
	//! points smaller and recalculating all font sizes.
	//!
	//! \sa zoomIn(), zoomTo()
	virtual void zoomOut(int range);

	//! \overload
	//!
	//! Zooms out on the text by by making the base font size one point
	//! larger and recalculating all font sizes.
	virtual void zoomOut();

	//! Zooms the text by making the base font size \a size points and
	//! recalculating all font sizes.
	//!
	//! \sa zoomIn(), zoomOut()
	virtual void zoomTo(int size);

signals:
	//! This signal is emitted whenever the cursor position changes.
	//! \a line contains the line number and \a pos contains the
	//! character position within the line.
	void cursorPositionChanged(int line,int pos);

	//! This signal is emitted whenever text is selected or de-selected.
	//! \a yes is TRUE if text has been selected and FALSE if text has been
	//! deselected.  If \a yes is TRUE then copy() can be used to copy the
	//! selection to the clipboard.  If \a yes is FALSE then copy() does
	//! nothing. 
	//!
	//! \sa copy(), selectionChanged()
	void copyAvailable(bool yes);

	//! This signal is emitted whenever the user clicks on a sensitive
	//! margin.  \a margin is the margin.  \a line is the number of the
	//! line where the user clicked.  \a state is the state of the modifier
	//! keys (ShiftButton, ControlButton and AltButton) when the user
	//! clicked.
	//!
	//! \sa marginSensitivity(), setMarginSensitivity()
	void marginClicked(int margin,int line,Qt::ButtonState state);

	//! This signal is emitted whenever the user attempts to modify
	//! read-only text.
	//!
	//! \sa isReadOnly(), setReadOnly()
	void modificationAttempted();

	//! This signal is emitted whenever the modification state of the text
	//! changes.  \a m is TRUE if the text has been modified.
	//!
	//! \sa isModified(), setModified()
	void modificationChanged(bool m);

	//! This signal is emitted whenever the selection changes.
	//!
	//! \sa copyAvailable()
	void selectionChanged();

	//! This signal is emitted whenever the text in the text edit changes.
	void textChanged();

private slots:
	void handleCallTipClick(int dir);
	void handleCharAdded(int charadded);
	void handleMarginClick(int pos,int margin,int modifiers);
	void handleModified(int pos,int mtype,const char *text,int len,
			    int added,int line,int foldNow,int foldPrev);
	void handlePropertyChange(const char *prop,const char *val);
	void handleSavePointReached();
	void handleSavePointLeft();
	void handleSelectionChanged(bool yes);

	void handleStyleColorChange(const QColor &c,int style);
	void handleStyleEolFillChange(bool eolfill,int style);
	void handleStyleFontChange(const QFont &f,int style);
	void handleStylePaperChange(const QColor &c,int style);

	void handleUpdateUI();

private:
	enum IndentState {
		isNone,
		isKeywordStart,
		isBlockStart,
		isBlockEnd
	};

	void maintainIndentation(char ch,long pos);
	void autoIndentation(char ch,long pos);
	void autoIndentLine(long pos,int line,int indent);
	int blockIndent(int line);
	IndentState getIndentState(int line);
	bool rangeIsWhitespace(long spos,long epos);
	int findStyledWord(const char *text,int style,const char *words);

	void checkMarker(int &mnr);
	long posFromLineIndex(int line,int index);
	void lineIndexFromPos(long pos,int *line,int *index);
	int currentIndent();
	int indentWidth();
	bool doFind();
	long simpleFind();
	void foldClick(int lineClick,int bstate);
	void foldChanged(int line,int levelNow,int levelPrev);
	void foldExpand(int &line,bool doExpand,bool force = FALSE,
			int visLevels = 0,int level = -1);
	void setFoldMarker(int marknr,int mark = SC_MARK_EMPTY);
	QString convertText(const char *s);
	void setStylesFont(const QFont &f,int style);

	void braceMatch();
	bool findMatchingBrace(long &brace,long &other,BraceMatch mode);
	long checkBrace(long pos,int brace_style,bool &colonMode);
	void gotoMatchingBrace(bool select);

	void startAutoCompletion(AutoCompletionSource acs,bool checkThresh,
				 bool emptyRoot,bool single);

	bool currentCharInWord();
	bool isWordChar(char ch) const;

	bool ensureRW();

	struct FindState
	{
		FindState() : inProgress(0) {}

		bool inProgress;
		QString expr;
		bool wrap;
		bool forward;
		int flags;
		long startpos;
		long endpos;
	};

	FindState findState;

	unsigned allocatedMarkers;
	long oldPos;
	bool selText;
	FoldStyle fold;
	bool autoInd;
	BraceMatch braceMode;
	AutoCompletionSource acSource;
	int acThresh;
	QextScintillaAPIs *acAPIs;
	QextScintillaAPIs *ctAPIs;
	int maxCallTips;
	bool showSingle;
	long ctpos;
	QGuardedPtr<QextScintillaLexer> lexer;
	const char *wordChars;
	QString acWordList;
	QString ctWordList;
	QextScintillaCommandSet *stdCmds;
	QextScintillaDocument doc;

#if defined(Q_DISABLE_COPY)
	QextScintilla(const QextScintilla &);
	QextScintilla &operator=(const QextScintilla &);
#endif
};

#endif
