/***************************************************************************
                          cuteview.cpp  -  description
                             -------------------
    begin                : Sam Sep 28 23:53:49 CEST 2002
    copyright            : (C) 2002 by Heiko Köhler
    email                : heicom@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qpushbutton.h>
#include <qlayout.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qdom.h>
#include <qextscintillacommand.h>
#include <qextscintillacommandset.h>

#include "cute.h"
#include "lexer.h"
#include "cuteview.h"
#include "cutescintilla.h"
#include "config.h"
#include "readtags.h"

#define CHECK_FILE_CHANGED_INTERVALL 1000	// intervall in msec for checking file

ScintillaAction::ScintillaAction(unsigned int msg, unsigned long wParam, long lParam)
{
	int len_t;
	char *t;

	this->msg = msg;
	this->wParam = wParam;
	t = (char*)lParam;
	
	switch(msg)
	{
	case QextScintillaBase::SCI_ADDTEXT:
	case QextScintillaBase::SCI_REPLACESEL:
		len_t = strlen(t);
		this->lParam = (long)new char[len_t+1];
		strcpy( (char*)this->lParam, t);
		break;
	case QextScintillaBase::SCI_INSERTTEXT:
	case QextScintillaBase::SCI_APPENDTEXT:
	case QextScintillaBase::SCI_SEARCHNEXT:
	case QextScintillaBase::SCI_SEARCHPREV:
		break;
	default:
		this->lParam = 0;
	}
}

ScintillaAction::ScintillaAction(const ScintillaAction& a)
{
	int len_lParam;

	msg = a.msg;
	wParam = a.wParam;
	lParam = a.lParam;
}

ScintillaAction &ScintillaAction::operator=(const ScintillaAction& a) 
{
	int len_lParam;

	msg = a.msg;
	wParam = a.wParam;
	lParam = a.lParam;
	return *this;
}

std::list<ScintillaAction> CUTEView::macro;
bool CUTEView::recording;

CUTEView::CUTEView(QWidget *parent, QString title, tagFile *tags) : QWidget( parent)
{
	filename = 0;
	recording = false;
	searching = false;
	timer = new QTimer(this);
	
	setCaption(title);
	setFocusPolicy(QWidget::NoFocus);
	_lexer = Config::LexerManager::lexer("");
	// init scintilla view
	QHBoxLayout *box = new QHBoxLayout(this);
	view = new CUTEScintilla(this, "", tags);
	view->setWordChars( _lexer->wordChars() );
	connect(view, SIGNAL(marginClicked(int,int,Qt::ButtonState)), 
			this, SLOT(slotMarginClicked(int,int,Qt::ButtonState)));
	connect(view, SIGNAL(SCN_MACRORECORD(unsigned int,unsigned long,long)), 
			this, SLOT(recordMacro(unsigned int,unsigned long,long)));
	connect(view, SIGNAL(SCN_CHARADDED(int)), this, SLOT(slotCharAdded(int)));
	connect(view, SIGNAL(modificationChanged(bool)), this, SLOT(slotDocumentChanged()));
	connect(timer, SIGNAL(timeout()), this, SLOT(fileChangedOutside()));
	box->addWidget(view);
	// setup margins
	view->setMarginLineNumbers(CUTE_MARGIN, false);
	view->setMarginLineNumbers(CUTE_LINENUMBER_MARGIN, true);
	// define bookmark marker
	bookmark = view->markerDefine(QextScintilla::Circle);
	view->setMarkerBackgroundColor(Qt::red, bookmark);
	// define error marker
	error = view->markerDefine(QextScintilla::RightArrow);
	view->setMarkerBackgroundColor(Qt::yellow, error);
	// configure margins for markers 
	view->setMarginSensitivity(CUTE_MARGIN, true);
	view->setMarginSensitivity(CUTE_LINENUMBER_MARGIN, false);
	view->setMarginMarkerMask(CUTE_MARGIN, 1 << bookmark);
	view->setMarginMarkerMask(CUTE_FOLD_MARGIN, 0);
	view->setMarginMarkerMask(CUTE_LINENUMBER_MARGIN, 0);
	view->setMarginWidth(CUTE_MARGIN, "99");
	view->setMarginWidth(CUTE_FOLD_MARGIN, "99");
	view->setMarginWidth(CUTE_LINENUMBER_MARGIN, "9999");

	view->setCallTipsVisible(0);
	view->setUtf8(Config::edit.getInteger("useUtf8Encoding"));
	configure();
}

void CUTEView::configure()
{
	QFont defaultFont;

	view->setBackspaceUnindents(Config::edit.getInteger("backspaceUnindents"));
	view->setEolVisibility(Config::view.getInteger("eolVisibility"));
	view->setIndentationsUseTabs(Config::edit.getInteger("indentationsUseTabs"));
	view->setBraceMatching((QextScintilla::BraceMatch)Config::view.getInteger("braceMatching"));
	view->setFolding((QextScintilla::FoldStyle)Config::view.getInteger("folding"));
	view->setTabWidth(Config::edit.getInteger("tabWidth"));
	view->setTabIndents(Config::edit.getInteger("tabIndents"));
	view->setIndentationGuides(Config::view.getInteger("indentationsGuide"));
	view->setAutoCompletionCaseSensitivity(Config::edit.getInteger("autoCompletionCaseSensitivity"));
	view->setAutoCompletionSource((CUTEScintilla::AutoCompletion)Config::edit.getInteger("autoCompletionSource"));
	QextScintilla::EolMode lineFeed = (QextScintilla::EolMode)Config::edit.getInteger("lineFeed");
	view->setEolMode(lineFeed);
	view->convertEols(lineFeed);
	view->setWhitespaceVisibility((QextScintilla::WhitespaceVisibility)Config::view.getInteger("whitespaces"));
	view->setAutoCompletionThreshold(Config::edit.getInteger("autoCompletionThreshold"));
	//view->zoomTo(Config::view.getInteger("zoomLevel"));
	if(Config::edit.getInteger("autoIndent"))
		view->SendScintilla( QextScintillaBase::SCI_SETPROPERTY, "indent.automatic", "1");
	view->setAutoIndent(Config::edit.getInteger("autoIndent"));
	if(Config::view.getInteger("lineNumbers"))
		view->setMarginWidth(CUTEView::CUTE_LINENUMBER_MARGIN, "00000");
	else
		view->setMarginWidth(CUTEView::CUTE_LINENUMBER_MARGIN, 0);
  
	if(Config::view.getInteger("margin"))
		view->setMarginWidth(CUTEView::CUTE_MARGIN, "00");
	else
		view->setMarginWidth(CUTEView::CUTE_MARGIN, "");

	if(Config::view.getInteger("foldMargin"))        
		view->setMarginWidth(CUTEView::CUTE_FOLD_MARGIN, "00");
	else
		view->setMarginWidth(CUTEView::CUTE_FOLD_MARGIN, "");
	if(Config::view.getInteger("wrapLines"))
		view->SendScintilla(QextScintillaBase::SCI_SETWRAPMODE, QextScintillaBase::SC_WRAP_WORD );
	else
		view->SendScintilla(QextScintillaBase::SCI_SETWRAPMODE, (unsigned long)QextScintillaBase::SC_WRAP_NONE );
	defaultFont.fromString(Config::view.getString("defaultFont"));
	//view->setMarginsFont( defaultFont );
}

CUTEView::~CUTEView()
{
	delete view;
}

void CUTEView::sendEditor(unsigned int msg, unsigned long wParam, long lParam)
{
	view->SendScintilla(msg, wParam, lParam);
}

bool CUTEView::edited()
{
	return view->isModified();
}

bool CUTEView::canUndo()
{
	return view->isUndoAvailable();
}

bool CUTEView::canRedo()
{
	return view->isRedoAvailable();
}

void CUTEView::undo()
{
	static_cast<QextScintilla*>(view)->undo();
}

void CUTEView::redo()
{
	static_cast<QextScintilla*>(view)->redo();
}

CUTEScintilla* CUTEView::scintilla()
{
	return view;
}

void CUTEView::setActions( QMap<int, QAction*> map )
{
	view->setActions(map);
}

bool CUTEView::loadFile(const char* name)
{
	QString line;
	
	if(!name)
		return false;
	filename = new char[strlen(name)+1];
	strcpy(filename, name);
	QFile file(name);
	
	if( !file.open(IO_ReadOnly))
		return false;
	lastModified = QFileInfo(file).lastModified();
	QTextStream in(&file);
	if(Config::edit.getInteger("useUtf8Encoding"))
		in.setEncoding(QTextStream::UnicodeUTF8);
	QString text;
	while( !in.atEnd() ){
		line = in.readLine();
		text += line+"\n";
	}
	file.close();
	blockSignals(true);  // avoid flicker when tab caption is changed twice by signals
	view->setText(text);
	view->SendScintilla(QextScintillaBase::SCI_SETSAVEPOINT);
	blockSignals(false);
	setCaption(name);
	if(Config::view.getInteger("toggleAllFolds"))
		view->foldAll();
    addInfoField("File", filename);
	return true;
}

bool CUTEView::reloadFile()
{
	int line, col;
	bool ret;

	view->getCursorPosition( &line, &col );
	clear();
	ret = loadFile(filename);
	view->setCursorPosition( line, col );
	view->SendScintilla(QextScintillaBase::SCI_SETSAVEPOINT);
	return ret;
}

bool CUTEView::saveFile(const char *fn, bool force)
{
	QString filename;
	bool changeFileName=false;

	if(!view->isModified() && view->length() && !force)
		return true;
	if( fn ){
		if(!this->filename)
			changeFileName = true;
		else if(strcmp(this->filename, fn))
			changeFileName = true;
		this->filename = new char[strlen(fn)+1];
		strcpy(this->filename, fn);
	}

	filename = this->filename;
	qWarning("saveFile: "+filename);
	QFile file(filename);
	if( !file.open(IO_WriteOnly))
		return false;
	lastModified = QFileInfo(file).lastModified();
	QTextStream out(&file);
	if(Config::edit.getInteger("useUtf8Encoding"))
		out.setEncoding(QTextStream::UnicodeUTF8);
	out << view->text();
	file.close();
	view->SendScintilla(QextScintillaBase::SCI_SETSAVEPOINT);
	setCaption(filename);
    addInfoField("File", filename);
	return true;
}

void CUTEView::toggleBookmark(int lineno)
{
	int state = view->SendScintilla(QextScintillaBase::SCI_MARKERGET, lineno);
	if ( state & (1 << bookmark)) {
		for( QValueList<int>::iterator i = handles.begin(); i != handles.end(); ++i )
			if( view->markerLine(*i) == lineno ){
				handles.erase(i);
				break;
			}
		view->SendScintilla(QextScintillaBase::SCI_MARKERDELETE, lineno, bookmark);
	} else {
		int handle = view->SendScintilla(QextScintillaBase::SCI_MARKERADD, lineno, bookmark);
		handles.push_back(handle);
	}
}

void CUTEView::addError( int lineno )
{
	int handle = view->SendScintilla(QextScintillaBase::SCI_MARKERADD, lineno, error);
}

int CUTEView::addBookmark( int lineno )
{
	int handle = view->SendScintilla(QextScintillaBase::SCI_MARKERADD, lineno, bookmark);
	handles.push_back(handle);
	return handle;
}

void CUTEView::deleteAllErrors()
{
	view->SendScintilla(QextScintillaBase::SCI_MARKERDELETEALL, error);
}

void CUTEView::findNextBookmark()
{
	int lineno, index;
	view->getCursorPosition(&lineno, &index);
	int nextLine = view->SendScintilla(QextScintillaBase::SCI_MARKERNEXT, lineno + 1, 1 << bookmark);
	if (nextLine < 0)
		nextLine = view->SendScintilla(QextScintillaBase::SCI_MARKERNEXT, lineno, 1 << bookmark);
	if (nextLine < 0 || nextLine == lineno)	// No bookmark (of the given type) or only one, and already on it
		QMessageBox::warning(0,0,"No bookmark found");
	else {
		view->SendScintilla(QextScintillaBase::SCI_GOTOLINE, nextLine);
	}
}

void CUTEView::findPrevBookmark()
{
	int lineno, index;
	view->getCursorPosition(&lineno, &index);
	int prevLine = view->SendScintilla(QextScintillaBase::SCI_MARKERPREVIOUS, lineno - 1, 1 << bookmark);
	if (prevLine < 0)
		prevLine = view->SendScintilla(QextScintillaBase::SCI_MARKERPREVIOUS, lineno, 1 << bookmark);
	if (prevLine < 0 || prevLine == lineno) {	// No bookmark (of the given type) or only one, and already on it
		QMessageBox::warning(0,0,"No bookmark found");
	} else {
		view->SendScintilla(QextScintillaBase::SCI_GOTOLINE, prevLine);
	}
}

void CUTEView::closeEvent(QCloseEvent *e)
{
	emit closed(this);
	removeAllBookmarks();
	QWidget::closeEvent(e);
}

void CUTEView::slotDocumentChanged()
{
	QString tabLabel;

	if( fileName() )
		tabLabel = QFileInfo(filename).fileName();
	else
		tabLabel = caption();
	if( view->isModified() )
		tabLabel += " *";
	emit setTabCaption(this, tabLabel);
}

void CUTEView::slotMarginClicked(int margin, int line, Qt::ButtonState state)
{
	if(margin == CUTE_MARGIN)
		toggleBookmark(line);
}

void CUTEView::slotCharAdded(int)
{
	deleteAllErrors();
}

void CUTEView::removeAllBookmarks()
{
	CUTE* cute = static_cast<CUTE*>(qApp->mainWidget());
	for(int i = 0; i < view->lines(); i++){
		view->SendScintilla(QextScintillaBase::SCI_MARKERDELETE, i, bookmark);
	}
	handles.clear();
}

void CUTEView::insertCommand(QString cmd)
{
	view->beginUndoAction();
	proc = new QProcess( QStringList::split(QRegExp("\\s"), cmd ), this );
	connect( proc, SIGNAL(readyReadStdout()), this, SLOT(insertStdout()));
	proc->start();
}

void CUTEView::filter(QString cmd)
{
	QString text;
	QString fileName;
	
	// creates temporare file
	fileName = QDir::homeDirPath()+QDir::separator()+".cute/.cute.tmp";
	QFile tempFile(fileName);
	
	// writes selection to file
	tempFile.open(IO_WriteOnly);
	QTextStream str(&tempFile);
	text = view->selectedText();
	str << text;
	tempFile.close();
	view->beginUndoAction();
	view->removeSelectedText();
	
	// start shell, executing pipe command cat cute.tmp | cmd
	proc = new QProcess(QString("/bin/sh"));
	proc->addArgument("-c");
	proc->addArgument("cat " + fileName + " | " + cmd);
	connect( proc, SIGNAL(readyReadStdout()), this, SLOT(insertStdout()));
	connect( proc, SIGNAL(processExited()), this, SLOT(processExited()));
	proc->start();
}

void CUTEView::insertStdout()
{
	int lineno, index;
	QString line;
	
	while(proc->canReadLineStdout()) {
		line = proc->readLineStdout();
		line += "\n";
		view->getCursorPosition(&lineno, &index);
		view->insertAt(line, lineno, index);
		view->setCursorPosition(lineno+1, 0);
	}
}

void CUTEView::processExited()
{
	QString fileName = QDir::homeDirPath()+QDir::separator()+".cute/.cute.tmp";
	QFile file(fileName);
	
	file.remove();
	view->endUndoAction();
	delete proc;
}

void CUTEView::startMacroRecording()
{
	if(recording){
		switch(QMessageBox::warning(this, "CUTE", "Macro recording is activated", "New Start", "Cancel"))
		{
		case 0:
			stopMacroRecording();
			break;
		case 1:
			return;
		default:
			qWarning("internal error in start macro recording dialog");
		};
	}
	else
		recording = true;
	macro.clear();
	view->SendScintilla(QextScintillaBase::SCI_STARTRECORD);
}

void CUTEView::stopMacroRecording()
{
	if(!recording){
		QMessageBox::warning(this, "CUTE", "No macro recording");
		return;
	}
	recording = false;
	view->SendScintilla(QextScintillaBase::SCI_STOPRECORD);
}

void CUTEView::runMacro()
{
	if(recording){
		switch(QMessageBox::warning(this, "CUTE", "Macro recording is activated", "Stop Recording", "Cancel"))
		{
		case 0:
			stopMacroRecording();
			break;
		case 1:
			return;
		default:
			qWarning("internal error in run macro dialog");
		};
	}
	view->beginUndoAction();
	std::list<ScintillaAction>::iterator i = macro.begin();
	for(; i != macro.end(); ++i){
		view->SendScintilla(i->msg, i->wParam, i->lParam);
	}
	view->endUndoAction();
}

void CUTEView::loadMacro(QString &file)
{
	QFile f(file);
	QString macro_str;
	
	if(!f.open(IO_ReadOnly)){
		QMessageBox::warning(this, "CUTE", "Could not open file");
		return;
	}
	QTextStream str(&f);
	macro_str = str.read();
	macro.clear();
	if( !setMacro(macro_str) ){
		QMessageBox::warning(this, "CUTE", "Erro while loading macro");
		return;
	}
	f.close();
}

void CUTEView::saveMacro(QString &file)
{
	QFile f(file);
	
	if(!f.open(IO_WriteOnly)){
		QMessageBox::warning(this, "CUTE", "Could not open file");
		return;
	}
	if(!macroString()){
		QMessageBox::information(this, "CUTE", "No macro recorded");
		return;
	}
	QTextStream str(&f);
	str << macroString();
	f.close();
}

bool CUTEView::setMacro(QString macro_str)
{
	unsigned int msg;
	unsigned long wParam;
	long lParam;
	QDomDocument doc;
	
	doc.setContent(macro_str);
	QDomElement root = doc.documentElement();
	QDomNodeList nodes = root.childNodes();
	
	for(int i = 0; i < nodes.count(); i++){
		QDomElement e = nodes.item(i).toElement();
		msg = e.attribute("msg").toInt();
		wParam = e.attribute("wParam").toULong();
		lParam = reinterpret_cast<long>(e.firstChild().toCDATASection().data().latin1());
		recordMacro(msg, wParam, lParam);
	}

	return true;
}

QString CUTEView::macroString()
{
	QDomDocument doc("macro");
	QDomElement root = doc.createElement("macro");
	doc.appendChild(root);
	
	std::list<ScintillaAction>::iterator i = macro.begin();
	for(; i != macro.end(); ++i){
		QDomCDATASection text = doc.createCDATASection(QString("%1").arg((char*)i->lParam));
		QDomElement element = doc.createElement("cmd");
		element.setAttribute("msg", QString("%1").arg(i->msg));
		element.setAttribute("wParam", QString("%1").arg(i->wParam));
		element.appendChild(text);
		root.appendChild(element);
	}
	return doc.toString();
}

void CUTEView::recordMacro(unsigned int msg, unsigned long wParam, long lParam)
{
	ScintillaAction action(msg, wParam, lParam);
	macro.push_back(action);
}

bool CUTEView::findFirst(const QString &expr, bool re, bool cs, bool wo, bool wrap, bool forward, int line, int index)
{
	searchString = expr;
	searchRegExp = re;
	searchCaseSensitive = cs;
	searchWholeWord = wo;
	searchLineWrap = wrap;
	searchDirection = forward;
	searching = true;
	return view->findFirst(expr, re, cs, wo, wrap, forward, line, index);
}

bool CUTEView::findNext()
{
	int line, index;

	if(!searching)
		return false;
	view->getCursorPosition(&line, &index);
	return view->findFirst( searchString, searchRegExp, searchCaseSensitive, searchWholeWord, searchLineWrap, 
		searchDirection, line, index);
}

bool CUTEView::findPrev()
{
	int line, index;

	if(!searching)
		return false;
	view->getCursorPosition(&line, &index);
	if(searchDirection)
		index -= searchString.length();
	view->findFirst( searchString, searchRegExp, searchCaseSensitive, searchWholeWord, searchLineWrap, 
		!searchDirection, line, index);
	return view->findNext();
}

void CUTEView::setupLexer(const QString &fileName)
{
	/* if ther is a lexer already, don't choose another one */
	if(_lexer == Config::LexerManager::lexer("")){
		/* choose lexer */
		_lexer = Config::LexerManager::lexer(fileName);
		if( _lexer ){
			view->setWordChars( _lexer->wordChars() );
			_lexer->initScintilla(view);
		}
	}
}

void CUTEView::replaceAll(const QString &expr, const QString &replaceStr, bool re, bool cs, bool wo, bool wrap, bool foreward, bool confirm)
{
	bool found;
	int prevLine, prevIndex;
	int line, index;

	found = findFirst(expr,
						re,
						cs,
						wo,
						wrap,
						foreward, 0, 0);
	while(found){
		if(confirm)
			switch( QMessageBox::information(qApp->mainWidget(), "CUTE", "Replace String?", "Replace", "Omitt","Cancel") )
			{
			case 0:
				view->replace(replaceStr);
				break;
			case 1:
				break;
			case 2:
				return;
			}
		else
			view->replace(replaceStr);
		view->getCursorPosition(&prevLine, &prevIndex);
		found = findNext();
		view->getCursorPosition(&line, &index);
		if( line < prevLine)
			return;
		else if( line == prevLine && index <= prevIndex )
			return;
	}
}

void CUTEView::getCursorPosition( int &line, int &index )
{
	view->getCursorPosition( &line, &index );
}

void CUTEView::getSelection(int &lineFrom,int &indexFrom, int &lineTo,int &indexTo)
{
	view->getSelection( &lineFrom, &indexFrom, &lineTo, &indexTo);
}

void CUTEView::clear()
{
	removeAllBookmarks();
	deleteAllErrors();
	view->clear();
}

QValueList<int> CUTEView::bookmarks()
{
	QValueList<int> list;
	for( QValueList<int>::iterator i = handles.begin(); i != handles.end(); i++)
		if( view->markerLine(*i) != -1 )
			list.push_back(*i);
	return list;
}

QString CUTEView::langName()
{
	return lexer()->langName();
}

QString CUTEView::infoString()
{
    return infoFields.join(",  ");
}

void CUTEView::addInfoField(QString name, QString info)
{
    QStringList::Iterator it = infoFields.begin();
    while(it != infoFields.end() && (*it).find(name + ":") != 0) {
        ++it;
    }

    if (it == infoFields.end()) {
        // new field
        infoFields.append(name + ": " + info);
    } else {
        // replace old field
        *it = name + ": " + info;
    }

    emit infoStringChanged(this);
}

void CUTEView::removeInfoField(QString name)
{
    QStringList::Iterator it = infoFields.begin();
    while(it != infoFields.end() && (*it).find(name + ":") != 0) {
        ++it;
    }

    if (it != infoFields.end()) {
        // found field
        infoFields.remove(it);
        emit infoStringChanged(this);
    }
}

void CUTEView::showEvent(QShowEvent *)
{
    fileChangedOutside();
    timer->start(CHECK_FILE_CHANGED_INTERVALL);
}

void CUTEView::hideEvent(QHideEvent *)
{
    timer->stop();
}

void CUTEView::fileChangedOutside()
{
    static bool lock = false;

    if( filename && lock == false )
        if( lastModified < QFileInfo(filename).lastModified() ){
            lock = true;
            int ans = QMessageBox::warning(this, "CUTE", "File was modified outside editor", "Reload", "Cancel");
            if( ans == 0 )
                reloadFile();
            else
                lastModified = QFileInfo(filename).lastModified();
            lock = false;
        }
}
