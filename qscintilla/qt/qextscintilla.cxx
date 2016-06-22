// This module implements the "official" high-level API of the Qt port of
// Scintilla.  It is modelled on QTextEdit - a method of the same name should
// behave in the same way.
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


#include <string.h>
#include <qcolor.h>

#include "qextscintilla.h"
#include "qextscintillalexer.h"
#include "qextscintillaapis.h"
#include "qextscintillacommandset.h"


// Make sure these match the values in Scintilla.h.  We don't #include that
// file because it just causes more clashes.
#define	KEYWORDSET_MAX	8
#define	MARKER_MAX	31


// The default fold margin width.
static const int defaultFoldMarginWidth = 14;

// The default set of characters that make up a word.
static const char *defaultWordChars = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


// The ctor.
QextScintilla::QextScintilla(QWidget *parent,const char *name,WFlags f) :
				QextScintillaBase(parent,name,f),
				allocatedMarkers(0), oldPos(-1),
				selText(FALSE), fold(NoFoldStyle),
				autoInd(FALSE), braceMode(NoBraceMatch),
				acSource(AcsDocument), acThresh(-1), acAPIs(0),
				ctAPIs(0), maxCallTips(-1), showSingle(FALSE)
{
	connect(this,SIGNAL(SCN_MODIFYATTEMPTRO()),
		     SIGNAL(modificationAttempted()));

	connect(this,SIGNAL(SCN_MODIFIED(int,int,const char *,int,int,int,int,int)),
		     SLOT(handleModified(int,int,const char *,int,int,int,int,int)));
	connect(this,SIGNAL(SCN_CALLTIPCLICK(int)),
		     SLOT(handleCallTipClick(int)));
	connect(this,SIGNAL(SCN_CHARADDED(int)),
		     SLOT(handleCharAdded(int)));
	connect(this,SIGNAL(SCN_MARGINCLICK(int,int,int)),
		     SLOT(handleMarginClick(int,int,int)));
	connect(this,SIGNAL(SCN_SAVEPOINTREACHED()),
		     SLOT(handleSavePointReached()));
	connect(this,SIGNAL(SCN_SAVEPOINTLEFT()),
		     SLOT(handleSavePointLeft()));
	connect(this,SIGNAL(SCN_UPDATEUI()),
		     SLOT(handleUpdateUI()));
	connect(this,SIGNAL(QSCN_SELCHANGED(bool)),
		     SLOT(handleSelectionChanged(bool)));

#if defined(Q_OS_WIN)
	setEolMode(EolWindows);
#elif defined(Q_OS_MAC)
	setEolMode(EolMac);
#else
	setEolMode(EolUnix);
#endif

	SendScintilla(SCI_SETPROPERTY,"fold","1");

	setMatchedBraceForegroundColor(blue);
	setUnmatchedBraceForegroundColor(red);

	setLexer();

	// Set the visible policy.  These are the same as SciTE's defaults
	// which, presumably, are sensible.
	SendScintilla(SCI_SETVISIBLEPOLICY,VISIBLE_STRICT|VISIBLE_SLOP,4);

	// Create the standard command set.
	stdCmds = new QextScintillaCommandSet(this);

	doc.display(this,0);
}


// The dtor.
QextScintilla::~QextScintilla()
{
	doc.undisplay(this);
}


// Enable/disable auto-indent.
void QextScintilla::setAutoIndent(bool autoindent)
{
	autoInd = autoindent;
}


// Set the brace matching mode.
void QextScintilla::setBraceMatching(BraceMatch bm)
{
	braceMode = bm;
}


// Handle the addition of a character.
void QextScintilla::handleCharAdded(int charadded)
{
	// Ignore if there is a selection.
	long pos = SendScintilla(SCI_GETSELECTIONSTART);

	if (pos != SendScintilla(SCI_GETSELECTIONEND))
		return;

	// Handle call tips.
	if (strchr("(),",charadded) != NULL)
		callTip();

	// Handle auto-indentation.
	if (autoInd)
		if (!lexer || (lexer -> autoIndentStyle() & AiMaintain))
			maintainIndentation(charadded,pos);
		else
			autoIndentation(charadded,pos);

	// See if we might want to start auto-completion.
	if (!SendScintilla(SCI_CALLTIPACTIVE) &&
	    !SendScintilla(SCI_AUTOCACTIVE) &&
	    acThresh >= 1 && isWordChar(charadded))
		startAutoCompletion(acSource,TRUE,FALSE,FALSE);
}


// Handle a possible change to any current call tip.
void QextScintilla::callTip()
{
	if (!ctAPIs)
		return;

	long pos = SendScintilla(SCI_GETCURRENTPOS);
	long linenr = SendScintilla(SCI_LINEFROMPOSITION,pos);
	long linelen = SendScintilla(SCI_LINELENGTH,linenr);

	char *lbuf = new char[linelen + 1];

	int loff = SendScintilla(SCI_GETCURLINE,linelen + 1,lbuf);

	int commas = 0, start = -1;

	// Move backwards through the line looking for the start of the current
	// call tip and working out which argument it is.
	while (loff > 0)
	{
		char ch = lbuf[--loff];

		if (ch == ',')
			++commas;
		else if (ch == ')')
		{
			int depth = 1;

			// Ignore everything back to the start of the
			// corresponding parenthesis.
			while (loff > 0)
			{
				ch = lbuf[--loff];

				if (ch == ')')
					++depth;
				else if (ch == '(' && --depth == 0)
					break;
			}
		}
		else if (ch == '(' && loff > 0)
		{
			if (isWordChar(lbuf[loff - 1]))
			{
				// The parenthesis is preceded by a word so
				// find the start of that word.
				lbuf[loff--] = '\0';

				while (loff >= 0 && isWordChar(lbuf[loff]))
					--loff;

				start = loff + 1;
				break;
			}

			// We are between parentheses that do not correspond to
			// a call tip, so reset the argument count.
			commas = 0;
		}
	}

	// Cancel any existing call tip.
	SendScintilla(SCI_CALLTIPCANCEL);

	// Done if there is no new call tip to set.
	if (start < 0)
	{
		delete []lbuf;
		return;
	}

	QString ct = ctAPIs -> callTips(&lbuf[start],maxCallTips,commas);

	delete []lbuf;

	if (ct.isEmpty())
		return;

	ctpos = SendScintilla(SCI_POSITIONFROMLINE,linenr) + start;

	SendScintilla(SCI_CALLTIPSHOW,ctpos,ct.latin1());

	// Done if there is more than one line in the call tip or there isn't a
	// down arrow at the start.
	if (ct[0] == '\002' || ct.find('\n') >= 0)
		return;

	// Highlight the current argument.
	int astart;

	if (commas == 0)
		astart = ct.find('(');
	else
	{
		astart = -1;

		do
			astart = ct.find(',',astart + 1);
		while (astart >= 0 && --commas > 0);
	}

	int len = ct.length();

	if (astart < 0 || ++astart == len)
		return;

	// The end is at the next comma or unmatched closing parenthesis.
	int aend, depth = 0;

	for (aend = astart; aend < len; ++aend)
	{
		QChar ch = ct.at(aend);

		if (ch == ',' && depth == 0)
			break;
		else if (ch == '(')
			++depth;
		else if (ch == ')')
		{
			if (depth == 0)
				break;

			--depth;
		}
	}

	if (astart != aend)
		SendScintilla(SCI_CALLTIPSETHLT,astart,aend);
}


// Handle a call tip click.
void QextScintilla::handleCallTipClick(int dir)
{
	if (!ctAPIs)
		return;

	QString ct = ctAPIs -> callTipsNextPrev(dir);

	if (ct.isNull())
		return;

	SendScintilla(SCI_CALLTIPSHOW,ctpos,ct.latin1());
}


// Possibly start auto-completion.
void QextScintilla::startAutoCompletion(AutoCompletionSource acs,
					bool checkThresh,bool emptyRoot,
					bool single)
{
	long wend, wstart;

	if (emptyRoot)
		wend = wstart = 0;
	else
	{
		// See how long a word has been entered so far.
		wend = SendScintilla(SCI_GETCURRENTPOS);
		wstart = SendScintilla(SCI_WORDSTARTPOSITION,wend,TRUE);
	}

	int wlen = wend - wstart;

	if (checkThresh && wlen < acThresh)
		return;

	// Get the word entered so far.
	char *word = new char[wlen + 1];
	char *cp = word;

	for (long i = wstart; i < wend; ++i)
		*cp++ = SendScintilla(SCI_GETCHARAT,i);

	*cp = '\0';

	// Generate the string representing the valid words to select from.
	QStringList wlist;
	bool cs = !SendScintilla(SCI_AUTOCGETIGNORECASE);

	if (acs == AcsDocument)
	{
		SendScintilla(SCI_SETSEARCHFLAGS,SCFIND_WORDSTART | (cs ? SCFIND_MATCHCASE : 0));

		long pos = 0;
		long dlen = SendScintilla(SCI_GETLENGTH);
		QString root(word);

		for (;;)
		{
			long fstart;

			SendScintilla(SCI_SETTARGETSTART,pos);
			SendScintilla(SCI_SETTARGETEND,dlen);

			if ((fstart = SendScintilla(SCI_SEARCHINTARGET,wlen,word)) < 0)
				break;

			// Move past the root part.
			pos = fstart + wlen;

			// Skip if this is the word we are auto-completing.
			if (fstart == wstart)
				continue;

			// Get the rest of this word.
			QString w(root);

			while (pos < dlen)
			{
				char ch = SendScintilla(SCI_GETCHARAT,pos);

				if (!isWordChar(ch))
					break;

				w += ch;

				++pos;
			}

			// Add the word if it isn't already there.
			if (wlist.findIndex(w) < 0)
				wlist.append(w);
		}

		wlist.sort();
	}
	else if (acAPIs)
		wlist = acAPIs -> autoCompletionList(word,cs);

	delete []word;

	if (wlist.isEmpty())
		return;

	char sep = SendScintilla(SCI_AUTOCGETSEPARATOR);
	acWordList = wlist.join(QChar(sep));

	SendScintilla(SCI_AUTOCSETCHOOSESINGLE,single);
	SendScintilla(SCI_AUTOCSHOW,wlen,acWordList.latin1());
}


// Maintain the indentation of the previous line.
void QextScintilla::maintainIndentation(char ch,long pos)
{
	if (ch != '\r' && ch != '\n')
		return;

	int curr_line = SendScintilla(SCI_LINEFROMPOSITION,pos);

	// Get the indentation of the preceding non-zero length line.
	int ind = 0;

	for (int line = curr_line - 1; line >= 0; --line)
	{
		if (SendScintilla(SCI_GETLINEENDPOSITION,line) >
		    SendScintilla(SCI_POSITIONFROMLINE,line))
		{
			ind = indentation(line);
			break;
		}
	}

	if (ind > 0)
		autoIndentLine(pos,curr_line,ind);
}


// Implement auto-indentation.
void QextScintilla::autoIndentation(char ch,long pos)
{
	int curr_line = SendScintilla(SCI_LINEFROMPOSITION,pos);
	int ind_width = indentationWidth();
	long curr_line_start = SendScintilla(SCI_POSITIONFROMLINE,curr_line);

	const char *block_start = lexer -> blockStart();
	bool start_single = (block_start && strlen(block_start) == 1);

	const char *block_end = lexer -> blockEnd();
	bool end_single = (block_end && strlen(block_end) == 1);

	if (end_single && block_end[0] == ch)
	{
		if (!(lexer -> autoIndentStyle() & AiClosing) && rangeIsWhitespace(curr_line_start,pos - 1))
			autoIndentLine(pos,curr_line,blockIndent(curr_line - 1) - indentationWidth());
	}
	else if (start_single && block_start[0] == ch)
	{
		// De-indent if we have already indented because the previous
		// line was a start of block keyword.
		if (!(lexer -> autoIndentStyle() & AiOpening) && getIndentState(curr_line - 1) == isKeywordStart && rangeIsWhitespace(curr_line_start,pos - 1))
			autoIndentLine(pos,curr_line,blockIndent(curr_line - 1) - indentationWidth());
	}
	else if (ch == '\r' || ch == '\n')
		autoIndentLine(pos,curr_line,blockIndent(curr_line - 1));
}


// Set the indentation for a line.
void QextScintilla::autoIndentLine(long pos,int line,int indent)
{
	if (indent < 0)
		return;

	long pos_before = SendScintilla(SCI_GETLINEINDENTPOSITION,line);
	SendScintilla(SCI_SETLINEINDENTATION,line,indent);
	long pos_after = SendScintilla(SCI_GETLINEINDENTPOSITION,line);
	long new_pos = -1;

	if (pos_after > pos_before)
		new_pos = pos + (pos_after - pos_before);
	else if (pos_after < pos_before && pos >= pos_after)
		if (pos >= pos_before)
			new_pos = pos + (pos_after - pos_before);
		else
			new_pos = pos_after;

	if (new_pos >= 0)
		SendScintilla(SCI_SETSEL,new_pos,new_pos);
}


// Return the indentation of the block defined by the given line (or something
// significant before).
int QextScintilla::blockIndent(int line)
{
	if (line < 0)
		return 0;

	// Handle the trvial case.
	if (!lexer -> blockStartKeyword() && !lexer -> blockStart() && !lexer -> blockEnd())
		return indentation(line);

	int line_limit = line - lexer -> blockLookback();

	if (line_limit < 0)
		line_limit = 0;

	for (int l = line; l >= line_limit; --l)
	{
		IndentState istate = getIndentState(l);

		if (istate != isNone)
		{
			int ind_width = indentationWidth();
			int ind = indentation(l);

			if (istate == isBlockStart)
			{
				if (!(lexer -> autoIndentStyle() & AiOpening))
					ind += ind_width;
			}
			else if (istate == isBlockEnd)
			{
				if (lexer -> autoIndentStyle() & AiOpening)
					ind -= ind_width;

				if (ind < 0)
					ind = 0;
			}
			else if (line == l)
				ind += ind_width;

			return ind;
		}
	}

	return indentation(line);
}


// Return TRUE if all characters starting at spos up to, but not including
// epos, are spaces or tabs.
bool QextScintilla::rangeIsWhitespace(long spos,long epos)
{
	while (spos < epos)
	{
		char ch = SendScintilla(SCI_GETCHARAT,spos);

		if (ch != ' ' && ch != '\t')
			return FALSE;

		++spos;
	}

	return TRUE;
}


// Returns the indentation state of a line.
QextScintilla::IndentState QextScintilla::getIndentState(int line)
{
	IndentState istate;

	// Get the styled text.
	long spos = SendScintilla(SCI_POSITIONFROMLINE,line);
	long epos = SendScintilla(SCI_POSITIONFROMLINE,line + 1);

	char *text = new char[(epos - spos + 1) * 2];

	SendScintilla(SCI_GETSTYLEDTEXT,spos,epos,text);

	const char *words;
	int style, bstart_off, bend_off;

	// Block start/end takes precedence over keywords.
	words = lexer -> blockStart(&style);
	bstart_off = findStyledWord(text,style,words);

	words = lexer -> blockEnd(&style);
	bend_off = findStyledWord(text,style,words);

	if (bstart_off > bend_off)
		istate = isBlockStart;
	else if (bend_off > bstart_off)
		istate = isBlockEnd;
	else
	{
		words = lexer -> blockStartKeyword(&style);

		istate = (findStyledWord(text,style,words) >= 0) ? isKeywordStart : isNone;
	}

	delete[] text;

	return istate;
}


// text is a pointer to some styled text (ie. a character byte followed by a
// style byte).  style is a style number.  words is a space separated list of
// words.  Returns the position in the text of the last one of the words with
// the style.  The reason we are after the last, and not the first, occurance
// is that we are looking for words that start and end a block where the latest
// one is the most significant.
int QextScintilla::findStyledWord(const char *text,int style,const char *words)
{
	if (!words)
		return -1;

	// Find the range of text with the style we are looking for.
	const char *stext;

	for (stext = text; stext[1] != style; stext += 2)
		if (stext[0] == '\0')
			return -1;

	// Move to the last character.
	const char *etext = text;

	while (etext[2] != '\0')
		etext += 2;

	// Backtrack until we find the style.  There will be one.
	while (etext[1] != style)
		etext -= 2;

	// Look for each word in turn.
	while (words[0] != '\0')
	{
		// Find the end of the word.
		const char *eword = words;

		while (eword[1] != ' ' && eword[1] != '\0')
			++eword;

		// Now search the text backwards.
		const char *wp = eword;

		for (const char *tp = etext; tp >= stext; tp -= 2)
		{
			if (tp[0] != wp[0] || tp[1] != style)
			{
				// Reset the search.
				wp = eword;
				continue;
			}

			// See if all the word has matched.
			if (wp-- == words)
				return (tp - text) / 2;
		}

		// Move to the start of the next word if there is one.
		words = eword + 1;

		if (words[0] == ' ')
			++words;
	}

	return -1;
}


// Return TRUE if the code page is UTF8.
bool QextScintilla::isUtf8()
{
	return (SendScintilla(SCI_GETCODEPAGE) == SC_CP_UTF8);
}


// Set the code page.
void QextScintilla::setUtf8(bool cp)
{
	SendScintilla(SCI_SETCODEPAGE,(cp ? SC_CP_UTF8 : 0));
}


// Return the end-of-line mode.
QextScintilla::EolMode QextScintilla::eolMode()
{
	return (EolMode)SendScintilla(SCI_GETEOLMODE);
}


// Set the end-of-line mode.
void QextScintilla::setEolMode(EolMode mode)
{
	SendScintilla(SCI_SETEOLMODE,mode);
}


// Convert the end-of-lines to a particular mode.
void QextScintilla::convertEols(EolMode mode)
{
	SendScintilla(SCI_CONVERTEOLS,mode);
}


// Return the end-of-line visibility.
bool QextScintilla::eolVisibility()
{
	return SendScintilla(SCI_GETVIEWEOL);
}


// Set the end-of-line visibility.
void QextScintilla::setEolVisibility(bool visible)
{
	SendScintilla(SCI_SETVIEWEOL,visible);
}


// Return the whitespace visibility.
QextScintilla::WhitespaceVisibility QextScintilla::whitespaceVisibility()
{
	return (WhitespaceVisibility)SendScintilla(SCI_GETVIEWWS);
}


// Set the whitespace visibility.
void QextScintilla::setWhitespaceVisibility(WhitespaceVisibility mode)
{
	SendScintilla(SCI_SETVIEWWS,mode);
}


// Set the folding style.
void QextScintilla::setFolding(FoldStyle folding)
{
	fold = folding;

	if (folding == NoFoldStyle)
	{
		SendScintilla(SCI_SETMARGINWIDTHN,2,0L);
		return;
	}

	int mask = SendScintilla(SCI_GETMODEVENTMASK);
	SendScintilla(SCI_SETMODEVENTMASK,mask | SC_MOD_CHANGEFOLD);

	SendScintilla(SCI_SETFOLDFLAGS,SC_FOLDFLAG_LINEAFTER_CONTRACTED);

	SendScintilla(SCI_SETMARGINTYPEN,2,SC_MARGIN_SYMBOL);
	SendScintilla(SCI_SETMARGINMASKN,2,SC_MASK_FOLDERS);
	SendScintilla(SCI_SETMARGINSENSITIVEN,2,1);

	// Set the marker symbols to use.
	switch (folding)
	{
	case PlainFoldStyle:
		setFoldMarker(SC_MARKNUM_FOLDEROPEN,SC_MARK_MINUS);
		setFoldMarker(SC_MARKNUM_FOLDER,SC_MARK_PLUS);
		setFoldMarker(SC_MARKNUM_FOLDERSUB);
		setFoldMarker(SC_MARKNUM_FOLDERTAIL);
		setFoldMarker(SC_MARKNUM_FOLDEREND);
		setFoldMarker(SC_MARKNUM_FOLDEROPENMID);
		setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL);

		break;

	case CircledFoldStyle:
		setFoldMarker(SC_MARKNUM_FOLDEROPEN,SC_MARK_CIRCLEMINUS);
		setFoldMarker(SC_MARKNUM_FOLDER,SC_MARK_CIRCLEPLUS);
		setFoldMarker(SC_MARKNUM_FOLDERSUB);
		setFoldMarker(SC_MARKNUM_FOLDERTAIL);
		setFoldMarker(SC_MARKNUM_FOLDEREND);
		setFoldMarker(SC_MARKNUM_FOLDEROPENMID);
		setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL);

		break;

	case BoxedFoldStyle:
		setFoldMarker(SC_MARKNUM_FOLDEROPEN,SC_MARK_BOXMINUS);
		setFoldMarker(SC_MARKNUM_FOLDER,SC_MARK_BOXPLUS);
		setFoldMarker(SC_MARKNUM_FOLDERSUB);
		setFoldMarker(SC_MARKNUM_FOLDERTAIL);
		setFoldMarker(SC_MARKNUM_FOLDEREND);
		setFoldMarker(SC_MARKNUM_FOLDEROPENMID);
		setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL);

		break;

	case CircledTreeFoldStyle:
		setFoldMarker(SC_MARKNUM_FOLDEROPEN,SC_MARK_CIRCLEMINUS);
		setFoldMarker(SC_MARKNUM_FOLDER,SC_MARK_CIRCLEPLUS);
		setFoldMarker(SC_MARKNUM_FOLDERSUB,SC_MARK_VLINE);
		setFoldMarker(SC_MARKNUM_FOLDERTAIL,SC_MARK_LCORNERCURVE);
		setFoldMarker(SC_MARKNUM_FOLDEREND,SC_MARK_CIRCLEPLUSCONNECTED);
		setFoldMarker(SC_MARKNUM_FOLDEROPENMID,SC_MARK_CIRCLEMINUSCONNECTED);
		setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL,SC_MARK_TCORNERCURVE);

		break;

	case BoxedTreeFoldStyle:
		setFoldMarker(SC_MARKNUM_FOLDEROPEN,SC_MARK_BOXMINUS);
		setFoldMarker(SC_MARKNUM_FOLDER,SC_MARK_BOXPLUS);
		setFoldMarker(SC_MARKNUM_FOLDERSUB,SC_MARK_VLINE);
		setFoldMarker(SC_MARKNUM_FOLDERTAIL,SC_MARK_LCORNER);
		setFoldMarker(SC_MARKNUM_FOLDEREND,SC_MARK_BOXPLUSCONNECTED);
		setFoldMarker(SC_MARKNUM_FOLDEROPENMID,SC_MARK_BOXMINUSCONNECTED);
		setFoldMarker(SC_MARKNUM_FOLDERMIDTAIL,SC_MARK_TCORNER);

		break;
	}

	SendScintilla(SCI_SETMARGINWIDTHN,2,defaultFoldMarginWidth);
}


// Set up a folder marker.
void QextScintilla::setFoldMarker(int marknr,int mark)
{
	SendScintilla(SCI_MARKERDEFINE,marknr,mark);

	if (mark != SC_MARK_EMPTY)
	{
		SendScintilla(SCI_MARKERSETFORE,marknr,white);
		SendScintilla(SCI_MARKERSETBACK,marknr,black);
	}
}


// Handle a click in the fold margin.  This is mostly taken from SciTE.
void QextScintilla::foldClick(int lineClick,int bstate)
{
	if ((bstate & ShiftButton) && (bstate & ControlButton))
	{
		foldAll();
		return;
	}

	int levelClick = SendScintilla(SCI_GETFOLDLEVEL,lineClick);

	if (levelClick & SC_FOLDLEVELHEADERFLAG)
	{
		if (bstate & ShiftButton)
		{
			// Ensure all children are visible.
			SendScintilla(SCI_SETFOLDEXPANDED,lineClick,1);
			foldExpand(lineClick,TRUE,TRUE,100,levelClick);
		}
		else if (bstate & ControlButton)
		{
			if (SendScintilla(SCI_GETFOLDEXPANDED,lineClick))
			{
				// Contract this line and all its children.
				SendScintilla(SCI_SETFOLDEXPANDED,lineClick,0L);
				foldExpand(lineClick,FALSE,TRUE,0,levelClick);
			}
			else
			{
				// Expand this line and all its children.
				SendScintilla(SCI_SETFOLDEXPANDED,lineClick,1);
				foldExpand(lineClick,TRUE,TRUE,100,levelClick);
			}
		}
		else
		{
			// Toggle this line.
			SendScintilla(SCI_TOGGLEFOLD,lineClick);
		}
	}
}


// Do the hard work of hiding and showing  lines.  This is mostly taken from
// SciTE.
void QextScintilla::foldExpand(int &line,bool doExpand,bool force,
			       int visLevels,int level)
{
	int lineMaxSubord = SendScintilla(SCI_GETLASTCHILD,line,level & SC_FOLDLEVELNUMBERMASK);

	line++;

	while (line <= lineMaxSubord)
	{
		if (force)
		{
			if (visLevels > 0)
				SendScintilla(SCI_SHOWLINES,line,line);
			else
				SendScintilla(SCI_HIDELINES,line,line);
		}
		else if (doExpand)
			SendScintilla(SCI_SHOWLINES,line,line);

		int levelLine = level;

		if (levelLine == -1)
			levelLine = SendScintilla(SCI_GETFOLDLEVEL,line);

		if (levelLine & SC_FOLDLEVELHEADERFLAG)
		{
			if (force)
			{
				if (visLevels > 1)
					SendScintilla(SCI_SETFOLDEXPANDED,line,1);
				else
					SendScintilla(SCI_SETFOLDEXPANDED,line,0L);

				foldExpand(line,doExpand,force,visLevels - 1);
			}
			else if (doExpand)
			{
				if (!SendScintilla(SCI_GETFOLDEXPANDED,line))
					SendScintilla(SCI_SETFOLDEXPANDED,line,1);

				foldExpand(line,TRUE,force,visLevels - 1);
			}
			else
				foldExpand(line,FALSE,force,visLevels - 1);
		}
		else
			line++;
	}
}


// Fully expand (if there is any line currently folded) all text.  Otherwise,
// fold all text.  This is mostly taken from SciTE.
void QextScintilla::foldAll()
{
	recolor();

	int maxLine = SendScintilla(SCI_GETLINECOUNT);
	bool expanding = TRUE;

	for (int lineSeek = 0; lineSeek < maxLine; lineSeek++)
	{
		if (SendScintilla(SCI_GETFOLDLEVEL,lineSeek) & SC_FOLDLEVELHEADERFLAG)
		{
			expanding = !SendScintilla(SCI_GETFOLDEXPANDED,lineSeek);
			break;
		}
	}

	for (int line = 0; line < maxLine; line++)
	{
		int level = SendScintilla(SCI_GETFOLDLEVEL,line);

		if ((level & SC_FOLDLEVELHEADERFLAG) &&
		    (SC_FOLDLEVELBASE == (level & SC_FOLDLEVELNUMBERMASK)))
		{
			if (expanding)
			{
				SendScintilla(SCI_SETFOLDEXPANDED,line,1);
				foldExpand(line,TRUE,FALSE,0,level);
				line--;
			}
			else
			{
				int lineMaxSubord = SendScintilla(SCI_GETLASTCHILD,line,-1);

				SendScintilla(SCI_SETFOLDEXPANDED,line,0L);

				if (lineMaxSubord > line)
					SendScintilla(SCI_HIDELINES,line + 1,lineMaxSubord);
			}
		}
	}
}


// Handle a fold change.  This is mostly taken from SciTE.
void QextScintilla::foldChanged(int line,int levelNow,int levelPrev)
{
        if (levelNow & SC_FOLDLEVELHEADERFLAG)
	{
                if (!(levelPrev & SC_FOLDLEVELHEADERFLAG))
                        SendScintilla(SCI_SETFOLDEXPANDED,line,1);
        }
	else if (levelPrev & SC_FOLDLEVELHEADERFLAG)
	{
                if (!SendScintilla(SCI_GETFOLDEXPANDED,line))
		{
                        // Removing the fold from one that has been contracted
			// so should expand.  Otherwise lines are left
			// invisible with no way to make them visible.
                        foldExpand(line,TRUE,FALSE,0,levelPrev);
                }
        }
}


// Toggle the fold for a line if it contains a fold marker.
void QextScintilla::foldLine(int line)
{
	SendScintilla(SCI_TOGGLEFOLD,line);
}


// Handle the SCN_MODIFIED notification.
void QextScintilla::handleModified(int pos,int mtype,const char *text,int len,
				   int added,int line,int foldNow,int foldPrev)
{
	if (mtype & SC_MOD_CHANGEFOLD)
	{
		if (fold)
			foldChanged(line,foldNow,foldPrev);
	}
	else if (mtype & (SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT))
		emit textChanged();
}


// Zoom in a number of points.
void QextScintilla::zoomIn(int range)
{
	zoomTo(SendScintilla(SCI_GETZOOM) + range);
}


// Zoom in a single point.
void QextScintilla::zoomIn()
{
	SendScintilla(SCI_ZOOMIN);
}


// Zoom out a number of points.
void QextScintilla::zoomOut(int range)
{
	zoomTo(SendScintilla(SCI_GETZOOM) - range);
}


// Zoom out a single point.
void QextScintilla::zoomOut()
{
	SendScintilla(SCI_ZOOMOUT);
}


// Set the zoom to a number of points.
void QextScintilla::zoomTo(int size)
{
	if (size < -10)
		size = -10;
	else if (size > 20)
		size = 20;

	SendScintilla(SCI_SETZOOM,size);
}


// Find the first occurrence of a string.
bool QextScintilla::findFirst(const QString &expr,bool re,bool cs,bool wo,
                              bool wrap,bool forward,int line,int index)
{
	findState.inProgress = FALSE;

	if (expr.isEmpty())
		return FALSE;

	findState.expr = expr;
	findState.wrap = wrap;
	findState.forward = forward;

	findState.flags = (cs ? SCFIND_MATCHCASE : 0) |
			  (wo ? SCFIND_WHOLEWORD : 0) |
			  (re ? SCFIND_REGEXP : 0);

	if (line < 0 || index < 0)
		findState.startpos = SendScintilla(SCI_GETCURRENTPOS);
	else
		findState.startpos = posFromLineIndex(line,index);

	if (forward)
		findState.endpos = SendScintilla(SCI_GETLENGTH);
	else
		findState.endpos = 0;

	return doFind();
}


// Find the next occurrence of a string.
bool QextScintilla::findNext()
{
	if (!findState.inProgress)
		return FALSE;

	return doFind();
}


// Do the hard work of findFirst() and findNext().
bool QextScintilla::doFind()
{
	SendScintilla(SCI_SETSEARCHFLAGS,findState.flags);

	long pos = simpleFind();

	// See if it was found.  If not and wraparound is wanted, try again.
	if (pos == -1 && findState.wrap)
	{
		if (findState.forward)
		{
			findState.startpos = 0;
			findState.endpos = SendScintilla(SCI_GETLENGTH);
		}
		else
		{
			findState.startpos = SendScintilla(SCI_GETLENGTH);
			findState.endpos = 0;
		}

		pos = simpleFind();
	}

	if (pos == -1)
	{
		findState.inProgress = FALSE;
		return FALSE;
	}

	// It was found.
	long targstart = SendScintilla(SCI_GETTARGETSTART);
	long targend = SendScintilla(SCI_GETTARGETEND);

	// Ensure the text found is visible.
	int startLine = SendScintilla(SCI_LINEFROMPOSITION,targstart);
	int endLine = SendScintilla(SCI_LINEFROMPOSITION,targend);

	for (int i = startLine; i <= endLine; ++i)
		SendScintilla(SCI_ENSUREVISIBLEENFORCEPOLICY,i);

	// Now set the selection.
	SendScintilla(SCI_SETSEL,targstart,targend);

	// Finally adjust the start position so that we don't find the same one
	// again.
	if (findState.forward)
		findState.startpos = targend;
	else if ((findState.startpos = targstart - 1) < 0)
		findState.startpos = 0;

	findState.inProgress = TRUE;
	return TRUE;
}


// Do a simple find between the start and end positions.
long QextScintilla::simpleFind()
{
	if (findState.startpos == findState.endpos)
		return -1;

	SendScintilla(SCI_SETTARGETSTART,findState.startpos);
	SendScintilla(SCI_SETTARGETEND,findState.endpos);

	long pos;

	if (isUtf8())
	{
		QCString s = findState.expr.utf8();

		pos = SendScintilla(SCI_SEARCHINTARGET,s.length(),s.data());
	}
	else
	{
		const char *s = findState.expr.latin1();

		pos = SendScintilla(SCI_SEARCHINTARGET,strlen(s),s);
	}

	return pos;
}


// Replace the text found with the previous findFirst() or findNext().
void QextScintilla::replace(const QString &replaceStr)
{
	if (!findState.inProgress)
		return;

	long start = SendScintilla(SCI_GETSELECTIONSTART);

	SendScintilla(SCI_TARGETFROMSELECTION);

	long len;
	int cmd = (findState.flags & SCFIND_REGEXP) ? SCI_REPLACETARGETRE : SCI_REPLACETARGET;

	if (isUtf8())
		len = SendScintilla(cmd,-1,replaceStr.utf8().data());
	else
		len = SendScintilla(cmd,-1,replaceStr.latin1());

	// Reset the selection.
	SendScintilla(SCI_SETSELECTIONSTART,start);
	SendScintilla(SCI_SETSELECTIONEND,start + len);

	if (findState.forward)
		findState.startpos = start + len;
}


// Query the modified state.
bool QextScintilla::isModified()
{
	return SendScintilla(SCI_GETMODIFY);
}


// Set the modified state.
void QextScintilla::setModified(bool m)
{
	if (!m)
		SendScintilla(SCI_SETSAVEPOINT);
}


// Handle the SCN_MARGINCLICK notification.
void QextScintilla::handleMarginClick(int pos,int modifiers,int margin)
{
	int state = 0;

	if (modifiers & SCMOD_SHIFT)
		state |= ShiftButton;

	if (modifiers & SCMOD_CTRL)
		state |= ControlButton;

	if (modifiers & SCMOD_ALT)
		state |= AltButton;

	int line = SendScintilla(SCI_LINEFROMPOSITION,pos);

	if (fold && margin == 2)
		foldClick(line,state);
	else
		emit marginClicked(margin,line,(ButtonState)state);
}


// Handle the SCN_SAVEPOINTREACHED notification.
void QextScintilla::handleSavePointReached()
{
	emit modificationChanged(FALSE);
}


// Handle the SCN_SAVEPOINTLEFT notification.
void QextScintilla::handleSavePointLeft()
{
	emit modificationChanged(TRUE);
}


// Handle the QSCN_SELCHANGED signal.
void QextScintilla::handleSelectionChanged(bool yes)
{
	selText = yes;

	emit copyAvailable(yes);
	emit selectionChanged();
}


// Get the current selection.
void QextScintilla::getSelection(int *lineFrom,int *indexFrom,
				 int *lineTo,int *indexTo)
{
	if (selText)
	{
		lineIndexFromPos(SendScintilla(SCI_GETSELECTIONSTART),
				 lineFrom,indexFrom);
		lineIndexFromPos(SendScintilla(SCI_GETSELECTIONEND),
				 lineTo,indexTo);
	}
	else
		*lineFrom = *indexFrom = *lineTo = *indexTo = -1;
}


// Sets the current selection.
void QextScintilla::setSelection(int lineFrom,int indexFrom,
				 int lineTo,int indexTo)
{
	SendScintilla(SCI_SETSELECTIONSTART,posFromLineIndex(lineFrom,indexFrom));
	SendScintilla(SCI_SETSELECTIONEND,posFromLineIndex(lineTo,indexTo));
}


// Set the background colour of selected text.
void QextScintilla::setSelectionBackgroundColor(const QColor &col)
{
	SendScintilla(SCI_SETSELBACK,1,col);
}


// Set the foreground colour of selected text.
void QextScintilla::setSelectionForegroundColor(const QColor &col)
{
	SendScintilla(SCI_SETSELFORE,1,col);
}


// Set the width of the caret.
void QextScintilla::setCaretWidth(int width)
{
	SendScintilla(SCI_SETCARETWIDTH,width);
}


// Set the foreground colour of the caret.
void QextScintilla::setCaretForegroundColor(const QColor &col)
{
	SendScintilla(SCI_SETCARETFORE,col);
}


// Set the background colour of the line containing the caret.
void QextScintilla::setCaretLineBackgroundColor(const QColor &col)
{
	SendScintilla(SCI_SETCARETLINEBACK,col);
}


// Set the state of the background colour of the line containing the caret.
void QextScintilla::setCaretLineVisible(bool enable)
{
	SendScintilla(SCI_SETCARETLINEVISIBLE,enable);
}


// Query the read-only state.
bool QextScintilla::isReadOnly()
{
	return SendScintilla(SCI_GETREADONLY);
}


// Set the read-only state.
void QextScintilla::setReadOnly(bool ro)
{
	SendScintilla(SCI_SETREADONLY,ro);
}


// Append the given text.
void QextScintilla::append(const QString &text)
{
	bool ro = ensureRW();

	if (isUtf8())
	{
		QCString s = text.utf8();

		SendScintilla(SCI_APPENDTEXT,s.length(),s.data());
	}
	else
	{
		const char *s = text.latin1();

		SendScintilla(SCI_APPENDTEXT,strlen(s),s);
	}

	SendScintilla(SCI_EMPTYUNDOBUFFER);

	setReadOnly(ro);
}


// Insert the given text at the given position.
void QextScintilla::insertAt(const QString &text,int line,int index)
{
	bool ro = ensureRW();
	long position = posFromLineIndex(line,index);

	SendScintilla(SCI_BEGINUNDOACTION);

	if (isUtf8())
		SendScintilla(SCI_INSERTTEXT,position,text.utf8().data());
	else
		SendScintilla(SCI_INSERTTEXT,position,text.latin1());

	SendScintilla(SCI_ENDUNDOACTION);

	setReadOnly(ro);
}


// Begin a sequence of undoable actions.
void QextScintilla::beginUndoAction()
{
	SendScintilla(SCI_BEGINUNDOACTION);
}


// End a sequence of undoable actions.
void QextScintilla::endUndoAction()
{
	SendScintilla(SCI_ENDUNDOACTION);
}


// Redo a sequence of actions.
void QextScintilla::redo()
{
	SendScintilla(SCI_REDO);
}


// Undo a sequence of actions.
void QextScintilla::undo()
{
	SendScintilla(SCI_UNDO);
}


// See if there is something to redo.
bool QextScintilla::isRedoAvailable()
{
	return SendScintilla(SCI_CANREDO);
}


// See if there is something to undo.
bool QextScintilla::isUndoAvailable()
{
	return SendScintilla(SCI_CANUNDO);
}


// Return the number of lines.
int QextScintilla::lines()
{
	return SendScintilla(SCI_GETLINECOUNT);
}


// Return the line at a position.
int QextScintilla::lineAt(const QPoint &pos)
{
	long chpos = SendScintilla(SCI_POSITIONFROMPOINTCLOSE,pos.x(),pos.y());

	if (chpos < 0)
		return -1;

	return SendScintilla(SCI_LINEFROMPOSITION,chpos);
}


// Return the length of a line.
int QextScintilla::lineLength(int line)
{
	if (line < 0 || line >= SendScintilla(SCI_GETLINECOUNT))
		return -1;

	return SendScintilla(SCI_LINELENGTH,line);
}


// Return the length of the current text.
int QextScintilla::length()
{
	return SendScintilla(SCI_GETTEXTLENGTH);
}


// Remove any selected text.
void QextScintilla::removeSelectedText()
{
	SendScintilla(SCI_REPLACESEL,"");
}


// Return the current selected text.
QString QextScintilla::selectedText()
{
	if (!selText)
		return QString();

	// Scintilla doesn't tell us the length of the selected text so we use
	// the length of the whole document.
	char *buf = new char[length() + 1];

	SendScintilla(SCI_GETSELTEXT,buf);

	QString qs = convertText(buf);
	delete[] buf;

	return qs;
}


// Return the current text.
QString QextScintilla::text()
{
	int buflen = length() + 1;
	char *buf = new char[buflen];

	SendScintilla(SCI_GETTEXT,buflen,buf);

	QString qs = convertText(buf);
	delete[] buf;

	return qs;
}


// Return the text of a line.
QString QextScintilla::text(int line)
{
	int line_len = lineLength(line);

	if (line_len < 1)
		return QString();

	char *buf = new char[line_len + 1];

	SendScintilla(SCI_GETLINE,line,buf);
	buf[line_len] = '\0';

	QString qs = convertText(buf);
	delete[] buf;

	return qs;
}


// Set the given text.
void QextScintilla::setText(const QString &text)
{
	bool ro = ensureRW();

	if (isUtf8())
		SendScintilla(SCI_SETTEXT,text.utf8().data());
	else
		SendScintilla(SCI_SETTEXT,text.latin1());

	SendScintilla(SCI_EMPTYUNDOBUFFER);

	setReadOnly(ro);
}


// Get the cursor position
void QextScintilla::getCursorPosition(int *line,int *index)
{
	long pos = SendScintilla(SCI_GETCURRENTPOS);
	long lin = SendScintilla(SCI_LINEFROMPOSITION,pos);
	long linpos = SendScintilla(SCI_POSITIONFROMLINE,lin);

	*line = lin;
	*index = pos - linpos;
}


// Set the cursor position
void QextScintilla::setCursorPosition(int line,int index)
{
	SendScintilla(SCI_GOTOPOS,posFromLineIndex(line,index));
}


// Ensure the cursor is visible.
void QextScintilla::ensureCursorVisible()
{
	SendScintilla(SCI_SCROLLCARET);
}


// Ensure a line is visible.
void QextScintilla::ensureLineVisible(int line)
{
	SendScintilla(SCI_ENSUREVISIBLEENFORCEPOLICY,line);
}


// Copy text to the clipboard.
void QextScintilla::copy()
{
	SendScintilla(SCI_COPY);
}


// Cut text to the clipboard.
void QextScintilla::cut()
{
	SendScintilla(SCI_CUT);
}


// Paste text from the clipboard.
void QextScintilla::paste()
{
	SendScintilla(SCI_PASTE);
}


// Select all text, or deselect any selected text.
void QextScintilla::selectAll(bool select)
{
	if (selText)
		SendScintilla(SCI_SETANCHOR,SendScintilla(SCI_GETCURRENTPOS));
	else
		SendScintilla(SCI_SELECTALL);
}


// Delete all text.
void QextScintilla::clear()
{
	bool ro = ensureRW();

	SendScintilla(SCI_BEGINUNDOACTION);
	SendScintilla(SCI_CLEARALL);
	SendScintilla(SCI_ENDUNDOACTION);

	setReadOnly(ro);
}


// Return the indentation of a line.
int QextScintilla::indentation(int line)
{
	return SendScintilla(SCI_GETLINEINDENTATION,line);
}


// Set the indentation of a line.
void QextScintilla::setIndentation(int line,int indentation)
{
	SendScintilla(SCI_BEGINUNDOACTION);
	SendScintilla(SCI_SETLINEINDENTATION,line,indentation);
	SendScintilla(SCI_ENDUNDOACTION);
}


// Indent a line.
void QextScintilla::indent(int line)
{
	setIndentation(line,indentation(line) + indentWidth());
}


// Unindent a line.
void QextScintilla::unindent(int line)
{
	int newIndent = indentation(line) - indentWidth();

	if (newIndent < 0)
		newIndent = 0;

	setIndentation(line,newIndent);
}


// Return the indentation of the current line.
int QextScintilla::currentIndent()
{
	return indentation(SendScintilla(SCI_LINEFROMPOSITION,SendScintilla(SCI_GETCURRENTPOS)));
}


// Return the current indentation width.
int QextScintilla::indentWidth()
{
	int w = indentationWidth();

	if (w == 0)
		w = tabWidth();

	return w;
}


// Return the state of indentation guides.
bool QextScintilla::indentationGuides()
{
	return SendScintilla(SCI_GETINDENTATIONGUIDES);
}


// Enable and disable indentation guides.
void QextScintilla::setIndentationGuides(bool enable)
{
	SendScintilla(SCI_SETINDENTATIONGUIDES,enable);
}


// Set the background colour of indentation guides.
void QextScintilla::setIndentationGuidesBackgroundColor(const QColor &col)
{
	SendScintilla(SCI_STYLESETBACK,STYLE_INDENTGUIDE,col);
}


// Set the foreground colour of indentation guides.
void QextScintilla::setIndentationGuidesForegroundColor(const QColor &col)
{
	SendScintilla(SCI_STYLESETFORE,STYLE_INDENTGUIDE,col);
}


// Return the indentation width.
int QextScintilla::indentationWidth()
{
	return SendScintilla(SCI_GETINDENT);
}


// Set the indentation width.
void QextScintilla::setIndentationWidth(int width)
{
	SendScintilla(SCI_SETINDENT,width);
}


// Return the tab width.
int QextScintilla::tabWidth()
{
	return SendScintilla(SCI_GETTABWIDTH);
}


// Set the tab width.
void QextScintilla::setTabWidth(int width)
{
	SendScintilla(SCI_SETTABWIDTH,width);
}


// Return the effect of the backspace key.
bool QextScintilla::backspaceUnindents()
{
	return SendScintilla(SCI_GETBACKSPACEUNINDENTS);
}


// Set the effect of the backspace key.
void QextScintilla::setBackspaceUnindents(bool unindents)
{
	SendScintilla(SCI_SETBACKSPACEUNINDENTS,unindents);
}


// Return the effect of the tab key.
bool QextScintilla::tabIndents()
{
	return SendScintilla(SCI_GETTABINDENTS);
}


// Set the effect of the tab key.
void QextScintilla::setTabIndents(bool indents)
{
	SendScintilla(SCI_SETTABINDENTS,indents);
}


// Return the indentation use of tabs.
bool QextScintilla::indentationsUseTabs()
{
	return SendScintilla(SCI_GETUSETABS);
}


// Set the indentation use of tabs.
void QextScintilla::setIndentationsUseTabs(bool tabs)
{
	SendScintilla(SCI_SETUSETABS,tabs);
}


// Return the state of line numbers in a margin.
bool QextScintilla::marginLineNumbers(int margin)
{
	return SendScintilla(SCI_GETMARGINTYPEN,margin);
}


// Enable and disable line numbers in a margin.
void QextScintilla::setMarginLineNumbers(int margin,bool lnrs)
{
	SendScintilla(SCI_SETMARGINTYPEN,margin,lnrs ? SC_MARGIN_NUMBER : 0);
}


// Return the marker mask of a margin.
int QextScintilla::marginMarkerMask(int margin)
{
	return SendScintilla(SCI_GETMARGINMASKN,margin);
}


// Set the marker mask of a margin.
void QextScintilla::setMarginMarkerMask(int margin,int mask)
{
	SendScintilla(SCI_SETMARGINMASKN,margin,mask);
}


// Return the state of a margin's sensitivity.
bool QextScintilla::marginSensitivity(int margin)
{
	return SendScintilla(SCI_GETMARGINSENSITIVEN,margin);
}


// Enable and disable a margin's sensitivity.
void QextScintilla::setMarginSensitivity(int margin,bool sens)
{
	SendScintilla(SCI_SETMARGINSENSITIVEN,margin,sens);
}


// Return the width of a margin.
int QextScintilla::marginWidth(int margin)
{
	return SendScintilla(SCI_GETMARGINWIDTHN,margin);
}


// Set the width of a margin.
void QextScintilla::setMarginWidth(int margin,int width)
{
	SendScintilla(SCI_SETMARGINWIDTHN,margin,width);
}


// Set the width of a margin to the width of some text.
void QextScintilla::setMarginWidth(int margin,const QString &s)
{
	int width;

	if (isUtf8())
		width = SendScintilla(SCI_TEXTWIDTH,STYLE_LINENUMBER,s.utf8().data());
	else
		width = SendScintilla(SCI_TEXTWIDTH,STYLE_LINENUMBER,s.latin1());

	setMarginWidth(margin,width);
}


// Set the background colour of all margins.
void QextScintilla::setMarginsBackgroundColor(const QColor &col)
{
	handleStylePaperChange(col,STYLE_LINENUMBER);
}


// Set the foreground colour of all margins.
void QextScintilla::setMarginsForegroundColor(const QColor &col)
{
	handleStyleColorChange(col,STYLE_LINENUMBER);
}


// Set the font of all margins.
void QextScintilla::setMarginsFont(const QFont &f)
{
	setStylesFont(f,STYLE_LINENUMBER);
}


// Define a marker based on a symbol.
int QextScintilla::markerDefine(MarkerSymbol sym,int mnr)
{
	checkMarker(mnr);

	if (mnr >= 0)
		SendScintilla(SCI_MARKERDEFINE,mnr,static_cast<long>(sym));

	return mnr;
}


// Define a marker based on a character.
int QextScintilla::markerDefine(char ch,int mnr)
{
	checkMarker(mnr);

	if (mnr >= 0)
		SendScintilla(SCI_MARKERDEFINE,mnr,static_cast<long>(SC_MARK_CHARACTER) + ch);

	return mnr;
}


// Define a marker based on a QPixmap.
int QextScintilla::markerDefine(const QPixmap *pm,int mnr)
{
	checkMarker(mnr);

	if (mnr >= 0)
		SendScintilla(SCI_MARKERDEFINEPIXMAP,mnr,pm);

	return mnr;
}


// Add a marker to a line.
int QextScintilla::markerAdd(int linenr,int mnr)
{
	if (mnr < 0 || mnr > MARKER_MAX || (allocatedMarkers & (1 << mnr)) == 0)
		return -1;

	return SendScintilla(SCI_MARKERADD,linenr,mnr);
}


// Get the marker mask for a line.
unsigned QextScintilla::markersAtLine(int linenr)
{
	return SendScintilla(SCI_MARKERGET,linenr);
}


// Delete a marker from a line.
void QextScintilla::markerDelete(int linenr,int mnr)
{
	if (mnr <= MARKER_MAX)
	{
		if (mnr < 0)
		{
			unsigned am = allocatedMarkers;

			for (int m = 0; m <= MARKER_MAX; ++m)
			{
				if (am & 1)
					SendScintilla(SCI_MARKERDELETE,linenr,m);

				am >>= 1;
			}
		}
		else if (allocatedMarkers & (1 << mnr))
			SendScintilla(SCI_MARKERDELETE,linenr,mnr);
	}
}


// Delete a marker from the text.
void QextScintilla::markerDeleteAll(int mnr)
{
	if (mnr <= MARKER_MAX)
	{
		if (mnr < 0)
			SendScintilla(SCI_MARKERDELETEALL,-1);
		else if (allocatedMarkers & (1 << mnr))
			SendScintilla(SCI_MARKERDELETEALL,mnr);
	}
}


// Delete a marker handle from the text.
void QextScintilla::markerDeleteHandle(int mhandle)
{
	SendScintilla(SCI_MARKERDELETEHANDLE,mhandle);
}


// Return the line containing a marker instance.
int QextScintilla::markerLine(int mhandle)
{
	return SendScintilla(SCI_MARKERLINEFROMHANDLE,mhandle);
}


// Search forwards for a marker.
int QextScintilla::markerFindNext(int linenr,unsigned mask)
{
	return SendScintilla(SCI_MARKERNEXT,linenr,mask);
}


// Search backwards for a marker.
int QextScintilla::markerFindPrevious(int linenr,unsigned mask)
{
	return SendScintilla(SCI_MARKERPREVIOUS,linenr,mask);
}


// Set the marker background colour.
void QextScintilla::setMarkerBackgroundColor(const QColor &col,int mnr)
{
	if (mnr <= MARKER_MAX)
	{
		if (mnr < 0)
		{
			unsigned am = allocatedMarkers;

			for (int m = 0; m <= MARKER_MAX; ++m)
			{
				if (am & 1)
					SendScintilla(SCI_MARKERSETBACK,m,col);

				am >>= 1;
			}
		}
		else if (allocatedMarkers & (1 << mnr))
			SendScintilla(SCI_MARKERSETBACK,mnr,col);
	}
}


// Set the marker foreground colour.
void QextScintilla::setMarkerForegroundColor(const QColor &col,int mnr)
{
	if (mnr <= MARKER_MAX)
	{
		if (mnr < 0)
		{
			unsigned am = allocatedMarkers;

			for (int m = 0; m <= MARKER_MAX; ++m)
			{
				if (am & 1)
					SendScintilla(SCI_MARKERSETFORE,m,col);

				am >>= 1;
			}
		}
		else if (allocatedMarkers & (1 << mnr))
			SendScintilla(SCI_MARKERSETFORE,mnr,col);
	}
}


// Check a marker, allocating a marker number if necessary.
void QextScintilla::checkMarker(int &mnr)
{
	if (mnr >= 0)
	{
		// Check the explicit marker number isn't already allocated.
		if (mnr > MARKER_MAX || allocatedMarkers & (1 << mnr))
			mnr = -1;
	}
	else
	{
		unsigned am = allocatedMarkers;

		// Find the smallest unallocated marker number.
		for (mnr = 0; mnr <= MARKER_MAX; ++mnr)
		{
			if ((am & 1) == 0)
				break;

			am >>= 1;
		}
	}

	// Define the marker if it is valid.
	if (mnr >= 0)
		allocatedMarkers |= (1 << mnr);
}


// Reset the fold margin colours.
void QextScintilla::resetFoldMarginColors()
{
	SendScintilla(SCI_SETFOLDMARGINHICOLOUR,0,0L);
	SendScintilla(SCI_SETFOLDMARGINCOLOUR,0,0L);
}


// Set the fold margin colours.
void QextScintilla::setFoldMarginColors(const QColor &fore,const QColor &back)
{
	SendScintilla(SCI_SETFOLDMARGINHICOLOUR,1,fore);
	SendScintilla(SCI_SETFOLDMARGINCOLOUR,1,back);
}


// Set the call tips background colour.
void QextScintilla::setCallTipsBackgroundColor(const QColor &col)
{
	SendScintilla(SCI_CALLTIPSETBACK,col);
}


// Set the call tips foreground colour.
void QextScintilla::setCallTipsForegroundColor(const QColor &col)
{
	SendScintilla(SCI_CALLTIPSETFORE,col);
}


// Set the call tips highlight colour.
void QextScintilla::setCallTipsHighlightColor(const QColor &col)
{
	SendScintilla(SCI_CALLTIPSETFOREHLT,col);
}


// Set the matched brace background colour.
void QextScintilla::setMatchedBraceBackgroundColor(const QColor &col)
{
	SendScintilla(SCI_STYLESETBACK,STYLE_BRACELIGHT,col);
}


// Set the matched brace foreground colour.
void QextScintilla::setMatchedBraceForegroundColor(const QColor &col)
{
	SendScintilla(SCI_STYLESETFORE,STYLE_BRACELIGHT,col);
}


// Set the unmatched brace background colour.
void QextScintilla::setUnmatchedBraceBackgroundColor(const QColor &col)
{
	SendScintilla(SCI_STYLESETBACK,STYLE_BRACEBAD,col);
}


// Set the unmatched brace foreground colour.
void QextScintilla::setUnmatchedBraceForegroundColor(const QColor &col)
{
	SendScintilla(SCI_STYLESETFORE,STYLE_BRACEBAD,col);
}


// Set the lexer.
void QextScintilla::setLexer(QextScintillaLexer *lex)
{
	// Disconnect any previous lexer.
	if (lexer)
	{
		lexer -> disconnect(this);

		SendScintilla(SCI_STYLERESETDEFAULT);
	}

	// Connect up the new lexer.
	lexer = lex;

	if (lex)
	{
		int i, nrStyles, bits;

		wordChars = lex -> wordCharacters();

		if (!wordChars)
			wordChars = defaultWordChars;

		bits = lex -> styleBits();
		nrStyles = 1 << bits;

		SendScintilla(SCI_SETSTYLEBITS,bits);

		// Scintilla v1.48 has a bug that causes indentation guides to
		// be displayed wrongly if only spaces are used and the default
		// font and the current font have different space widths.  By
		// setting the default font to the lexer's default font we
		// reduce the chance of this happening.  It also means that
		// indentations won't suddenly get smaller when the bug is
		// fixed.
		setStylesFont(lex -> defaultFont(),STYLE_DEFAULT);

		connect(lex,SIGNAL(colorChanged(const QColor &,int)),
			SLOT(handleStyleColorChange(const QColor &,int)));
		connect(lex,SIGNAL(eolFillChanged(bool,int)),
			SLOT(handleStyleEolFillChange(bool,int)));
		connect(lex,SIGNAL(fontChanged(const QFont &,int)),
			SLOT(handleStyleFontChange(const QFont &,int)));
		connect(lex,SIGNAL(paperChanged(const QColor &,int)),
			SLOT(handleStylePaperChange(const QColor &,int)));
		connect(lex,SIGNAL(propertyChanged(const char *,const char *)),
			SLOT(handlePropertyChange(const char *,const char *)));

		SendScintilla(SCI_SETLEXERLANGUAGE,lex -> lexer());

		// Set the keywords.
		for (i = 0; i <= KEYWORDSET_MAX; ++i)
		{
			const char *kw = lex -> keywords(i);

			if (kw)
				SendScintilla(SCI_SETKEYWORDS,i,kw);
		}

		// Initialise each style.
		for (i = 0; i < nrStyles; ++i)
		{
			if (lex -> description(i).isNull())
				continue;

			handleStyleColorChange(lex -> color(i),i);
			handleStyleEolFillChange(lex -> eolFill(i),i);
			handleStyleFontChange(lex -> font(i),i);
			handleStylePaperChange(lex -> paper(i),i);
		}

		// Initialise the properties.
		lex -> refreshProperties();
	}
	else
	{
		wordChars = defaultWordChars;
		SendScintilla(SCI_SETLEXER,SCLEX_NULL);
	}
}


// Handle a change in lexer style foreground colour.
void QextScintilla::handleStyleColorChange(const QColor &c,int style)
{
	SendScintilla(SCI_STYLESETFORE,style,c);
}


// Handle a change in lexer style end-of-line fill.
void QextScintilla::handleStyleEolFillChange(bool eolfill,int style)
{
	SendScintilla(SCI_STYLESETEOLFILLED,style,eolfill);
}


// Handle a change in lexer style font.
void QextScintilla::handleStyleFontChange(const QFont &f,int style)
{
	setStylesFont(f,style);

	if (style == lexer -> braceStyle())
	{
		setStylesFont(f,STYLE_BRACELIGHT);
		setStylesFont(f,STYLE_BRACEBAD);
	}
}


// Set the font for a style.
void QextScintilla::setStylesFont(const QFont &f,int style)
{
	SendScintilla(SCI_STYLESETFONT,style,f.family().latin1());
	SendScintilla(SCI_STYLESETSIZE,style,f.pointSize());
	SendScintilla(SCI_STYLESETBOLD,style,f.bold());
	SendScintilla(SCI_STYLESETITALIC,style,f.italic());
	SendScintilla(SCI_STYLESETUNDERLINE,style,f.underline());
}


// Handle a change in lexer style background colour.
void QextScintilla::handleStylePaperChange(const QColor &c,int style)
{
	SendScintilla(SCI_STYLESETBACK,style,c);
}


// Handle a change in lexer property.
void QextScintilla::handlePropertyChange(const char *prop,const char *val)
{
	SendScintilla(SCI_SETPROPERTY,prop,val);
}


// Handle a change to the user visible user interface.
void QextScintilla::handleUpdateUI()
{
	long newPos = SendScintilla(SCI_GETCURRENTPOS);

	if (newPos != oldPos)
	{
		oldPos = newPos;

		int line = SendScintilla(SCI_LINEFROMPOSITION,newPos);
		int col = SendScintilla(SCI_GETCOLUMN,newPos);

		emit cursorPositionChanged(line,col);
	}

	if (braceMode != NoBraceMatch)
		braceMatch();
}


// Handle brace matching.
void QextScintilla::braceMatch()
{
	long braceAtCaret, braceOpposite;

	findMatchingBrace(braceAtCaret,braceOpposite,braceMode);

	if (braceAtCaret >= 0 && braceOpposite < 0)
	{
		SendScintilla(SCI_BRACEBADLIGHT,braceAtCaret);
                SendScintilla(SCI_SETHIGHLIGHTGUIDE,0UL);
	}
	else
	{
		char chBrace = SendScintilla(SCI_GETCHARAT,braceAtCaret);

		SendScintilla(SCI_BRACEHIGHLIGHT,braceAtCaret,braceOpposite);

		long columnAtCaret = SendScintilla(SCI_GETCOLUMN,braceAtCaret);
                long columnOpposite = SendScintilla(SCI_GETCOLUMN,braceOpposite);

		if (chBrace == ':')
		{
			long lineStart = SendScintilla(SCI_LINEFROMPOSITION,braceAtCaret);
			long indentPos = SendScintilla(SCI_GETLINEINDENTPOSITION,lineStart);
                        long indentPosNext = SendScintilla(SCI_GETLINEINDENTPOSITION,lineStart + 1);

                        columnAtCaret = SendScintilla(SCI_GETCOLUMN,indentPos);

                        long columnAtCaretNext = SendScintilla(SCI_GETCOLUMN,indentPosNext);
                        long indentSize = SendScintilla(SCI_GETINDENT);

			if (columnAtCaretNext - indentSize > 1)
				columnAtCaret = columnAtCaretNext - indentSize;

			if (columnOpposite == 0)
				columnOpposite = columnAtCaret;
		}

		long column = columnAtCaret;

		if (column > columnOpposite)
			column = columnOpposite;

		SendScintilla(SCI_SETHIGHLIGHTGUIDE,column);
	}
}


// Check if the character at a position is a brace.
long QextScintilla::checkBrace(long pos,int brace_style,bool &colonMode)
{
	long brace_pos = -1;
	char ch = SendScintilla(SCI_GETCHARAT,pos);

	if (ch == ':')
	{
		// A bit of a hack.
		if (lexer && strcmp(lexer -> lexer(),"python") == 0)
		{
			brace_pos = pos;
			colonMode = TRUE;
		}
	}
	else if (ch && strchr("[](){}<>",ch))
	{
		if (brace_style < 0)
			brace_pos = pos;
		else
		{
			int style = SendScintilla(SCI_GETSTYLEAT,pos) & 0x1f;

			if (style == brace_style)
				brace_pos = pos;
		}
	}

	return brace_pos;
}


// Find a brace and it's match.  Return TRUE if the current position is inside
// a pair of braces.
bool QextScintilla::findMatchingBrace(long &brace,long &other,BraceMatch mode)
{
	bool colonMode = FALSE;
	int brace_style = (lexer ? lexer -> braceStyle() : -1);

	brace = -1;
	other = -1;

	long caretPos = SendScintilla(SCI_GETCURRENTPOS);

	if (caretPos > 0)
		brace = checkBrace(caretPos - 1,brace_style,colonMode);

	bool isInside = FALSE;

	if (brace < 0 && mode == SloppyBraceMatch)
	{
		brace = checkBrace(caretPos,brace_style,colonMode);

		if (brace >= 0 && !colonMode)
			isInside = TRUE;
	}

	if (brace >= 0)
	{
		if (colonMode)
		{
			// Find the end of the Python indented block.
			long lineStart = SendScintilla(SCI_LINEFROMPOSITION,brace);
			long lineMaxSubord = SendScintilla(SCI_GETLASTCHILD,lineStart,-1);

			other = SendScintilla(SCI_GETLINEENDPOSITION,lineMaxSubord);
		}
		else
			other = SendScintilla(SCI_BRACEMATCH,brace);

		if (other > brace)
			isInside = !isInside;
	}

	return isInside;
}


// Move to the matching brace.
void QextScintilla::moveToMatchingBrace()
{
	gotoMatchingBrace(FALSE);
}


// Select to the matching brace.
void QextScintilla::selectToMatchingBrace()
{
	gotoMatchingBrace(TRUE);
}


// Move to the matching brace and optionally select the text.
void QextScintilla::gotoMatchingBrace(bool select)
{
	long braceAtCaret;
	long braceOpposite;

	bool isInside = findMatchingBrace(braceAtCaret,braceOpposite,SloppyBraceMatch);

	if (braceOpposite >= 0)
	{
		// Convert the character positions into caret positions based
		// on whether the caret position was inside or outside the
		// braces.
		if (isInside)
		{
			if (braceOpposite > braceAtCaret)
				braceAtCaret++;
			else
				braceOpposite++;
		}
		else
		{
			if (braceOpposite > braceAtCaret)
				braceOpposite++;
			else
				braceAtCaret++;
		}

		ensureLineVisible(SendScintilla(SCI_LINEFROMPOSITION,braceOpposite));

		if (select)
			SendScintilla(SCI_SETSEL,braceAtCaret,braceOpposite);
		else
			SendScintilla(SCI_SETSEL,braceOpposite,braceOpposite);
	}
}


// Return a position from a line number and an index within the line.
long QextScintilla::posFromLineIndex(int line,int index)
{
	long pos = SendScintilla(SCI_POSITIONFROMLINE,line) + index;
	long eol = SendScintilla(SCI_GETLINEENDPOSITION,line);

	if (pos > eol)
		pos = eol;

	return pos;
}


// Return a line number and an index within the line from a position.
void QextScintilla::lineIndexFromPos(long pos,int *line,int *index)
{
	long lin = SendScintilla(SCI_LINEFROMPOSITION,pos);
	long linpos = SendScintilla(SCI_POSITIONFROMLINE,lin);

	*line = lin;
	*index = pos - linpos;
}


// Convert a Scintilla string to a Qt Unicode string.
QString QextScintilla::convertText(const char *s)
{
	if (isUtf8())
		return QString::fromUtf8(s);

	QString qs;

	qs.setLatin1(s);

	return qs;
}


// Set the source of the auto-completion list.
void QextScintilla::setAutoCompletionSource(AutoCompletionSource source)
{
	acSource = source;
}


// Set the threshold for automatic auto-completion.
void QextScintilla::setAutoCompletionThreshold(int thresh)
{
	acThresh = thresh;
}


// Set the APIs for auto-completion.
void QextScintilla::setAutoCompletionAPIs(QextScintillaAPIs *apis)
{
	acAPIs = apis;
}


// Explicitly auto-complete from the APIs.
void QextScintilla::autoCompleteFromAPIs()
{
	// If we are not in a word then display all APIs.
	startAutoCompletion(AcsAPIs,FALSE,!currentCharInWord(),showSingle);
}


// Explicitly auto-complete from the document.
void QextScintilla::autoCompleteFromDocument()
{
	// If we are not in a word then ignore.
	if (currentCharInWord())
		startAutoCompletion(AcsDocument,FALSE,FALSE,showSingle);
}


// Return TRUE if the current character (ie. the one before the carat) is part
// of a word.
bool QextScintilla::currentCharInWord()
{
	long pos = SendScintilla(SCI_GETCURRENTPOS);

	if (pos <= 0)
		return FALSE;

	return isWordChar(SendScintilla(SCI_GETCHARAT,pos - 1));
}


// Check if a character can be in a word.
bool QextScintilla::isWordChar(char ch) const
{
	return (ch != '\0' && strchr(wordChars,ch) != NULL);
}


// Recolour the document.
void QextScintilla::recolor(int start,int end)
{
	SendScintilla(SCI_COLOURISE,start,end);
}


// Registered an image.
void QextScintilla::registerImage(int id,const QPixmap *pm)
{
	SendScintilla(SCI_REGISTERIMAGE,id,pm);
}


// Clear all registered images.
void QextScintilla::clearRegisteredImages()
{
	SendScintilla(SCI_CLEARREGISTEREDIMAGES);
}


// Set the fill-up characters for auto-completion.
void QextScintilla::setAutoCompletionFillups(const char *fillups)
{
	SendScintilla(SCI_AUTOCSETFILLUPS,fillups);
}


// Set the case sensitivity for auto-completion.
void QextScintilla::setAutoCompletionCaseSensitivity(bool cs)
{
	SendScintilla(SCI_AUTOCSETIGNORECASE,!cs);
}


// Return the case sensitivity for auto-completion.
bool QextScintilla::autoCompletionCaseSensitivity()
{
	return !SendScintilla(SCI_AUTOCGETIGNORECASE);
}


// Set the replace word mode for auto-completion.
void QextScintilla::setAutoCompletionReplaceWord(bool replace)
{
	SendScintilla(SCI_AUTOCSETDROPRESTOFWORD,replace);
}


// Return the replace word mode for auto-completion.
bool QextScintilla::autoCompletionReplaceWord()
{
	return SendScintilla(SCI_AUTOCGETDROPRESTOFWORD);
}


// Set the single item mode for auto-completion.
void QextScintilla::setAutoCompletionShowSingle(bool single)
{
	showSingle = single;
}


// Return the single item mode for auto-completion.
bool QextScintilla::autoCompletionShowSingle()
{
	return showSingle;
}


// Set the APIs for call tips.
void QextScintilla::setCallTipsAPIs(QextScintillaAPIs *apis)
{
	ctAPIs = apis;
}


// Set maximum number of call tips displayed.
void QextScintilla::setCallTipsVisible(int nr)
{
	maxCallTips = nr;
}


// Set the document to display.
void QextScintilla::setDocument(const QextScintillaDocument &document)
{
	if (doc.pdoc != document.pdoc)
	{
		doc.undisplay(this);
		doc.attach(document);
		doc.display(this,&document);
	}
}


// Ensure the document is read-write and return True if was was read-only.
bool QextScintilla::ensureRW()
{
	bool ro = isReadOnly();

	if (ro)
		setReadOnly(FALSE);

	return ro;
}
