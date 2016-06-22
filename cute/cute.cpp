/***************************************************************************
                          cute.cpp  -  description
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

#undef slots
#include <Python.h>
#define slots

#include <qobjectlist.h>
#include <qdir.h>
#ifndef QT_NO_STYLE_MOTIF
#include <qmotifstyle.h>
#endif
#ifndef QT_NO_STYLE_CDE
#include <qcdestyle.h>
#endif
#ifndef QT_NO_STYLE_MOTIFPLUS
#include <qmotifplusstyle.h>
#endif
#ifndef QT_NO_STYLE_SGI
#include <qsgistyle.h>
#endif
#ifndef QT_NO_STYLE_WINDOWS
#include <qwindowsstyle.h>
#endif
#ifndef QT_NO_STYLE_PLATINUM
#include <qplatinumstyle.h>
#endif
#include <qinputdialog.h>
#include <qfile.h>
#include <qregexp.h>

#include <qextscintillaprinter.h>
#include <qextscintillabase.h>

#include "cute.h"
#include "project.h"
#include "bookmark.h"
#include "config.h"
#include "ui/aboutdlg.h"
#include "ui/licensedlg.h"
#include "windowdlg.h"
#include "savedlg.h"
#include "settingsdlg.h"
#include "shortcutsdlg.h"
#include "finddlg.h"
#include "replacedlg.h"
#include "findinfilesdlg.h"
#include "outputview.h"
#include "dirview.h"
#include "tagsview.h"
#include "projectview.h"
#include "tagsdlg.h"
#include "cuteshell.h"
#include "qcompletionbox.h"
#include "cmdinterpreter.h"
#include "lexer.h"
#include "helpviewer.h"
#include "projectdlg.h"

#include "cutestatusbar.h"
#include "cutehook.h"

std::list<std::string> recentFilesList;
QValueList<QString> recentProjectList;

CUTEAction::CUTEAction(QString command, QObject *parent, const char* name) 
	: QAction( parent, name )
{
	cmd = command;
	setMenuText(name);
	setText(name);
	connect(this, SIGNAL(activated()), this, SLOT(execCmd()));
}

void CUTEAction::execCmd()
{
	static_cast<CUTE*>(qApp->mainWidget())->execCmd(cmd);
}


CUTE::CUTE() : MainWindow()
{
	setCaption("CUTE ");

    // create a hook type
    CUTEHook::addHookType("fileOpen", this, SIGNAL(openFileHook(const QString)));
	
	settingsDlg = new SettingsDlg();
	replaceDlg = new ReplaceDlg();
	findDlg = new FindDlg();
	findInFilesDlg = new FindInFilesDlg();
	cmdInterpreter = new CmdInterpreter(this);
	shortcutsDlg = new ShortcutsDlg();
	tagsDlg = new TagsDlg();
	projectDlg = new ProjectDlg();
	static bool view_actions_updated = false;
	session = 0;
	proc = 0;
	tags = 0;
	project = 0;
	initActions();
	m_currentNumber = 1;
	// disable some actions 
	slotLastChildViewClosed();
	initEditActions();
	initMenuBar();
	initToolBar();
	initStatusBar();
	
	initDoc();
	initView();

	// create docklets
	sidePanel = new DockWindow(this);
	messagePanel = new DockWindow(this);
	sidePanel->setCaption("Side Panel");
	messagePanel->setCaption("Message Panel");
	// add docklets
	addDockWindow(sidePanel, DockLeft);
	addDockWindow(messagePanel, DockBottom);
	
	dirView = new DirectoryView(sidePanel, "DirView");
	connect( dirView, SIGNAL(fileSelected(const QString&)), this, SLOT(loadFileFromDir(const QString&)));
	dirView->setDir(QDir::currentDirPath());
	sidePanel->addWidget(dirView, "Dir");
	
	tagsView = new TagsView(sidePanel, "TagsView");
	connect(tagsView, SIGNAL(jump(QString, QString)), this, SLOT(findTag(QString, QString)));
	connect(tagsView, SIGNAL(buildTagsFile()), this, SLOT(slotExtraBuildTagsFile()));
	sidePanel->addWidget(tagsView, "Tags");

	projectView = new ProjectView(sidePanel, "ProjectView");
	connect(projectView, SIGNAL(loadFile(QString)), this, SLOT(loadProjectFile(QString)));
	sidePanel->addWidget(projectView, "Pro");

	output = new OutputView(messagePanel, "Messages");
	connect(output, SIGNAL(jump(QString, unsigned int)), this, SLOT(slotJump(QString, unsigned int)));
	connect(output, SIGNAL(clearAllOutputViews()), this, SLOT(slotClearAllOutputViews()));
	messagePanel->addWidget(output, "Messages");
	
	stdout = new OutputView(messagePanel, "Stdout");
	connect(stdout, SIGNAL(jump(QString, unsigned int)), this, SLOT(slotJump(QString, unsigned int)));
	connect(stdout, SIGNAL(clearAllOutputViews()), this, SLOT(slotClearAllOutputViews()));
	messagePanel->addWidget(stdout, "Stdout");
	
	stderr = new OutputView(messagePanel, "Stderr");
	connect(stderr, SIGNAL(jump(QString, unsigned int)), this, SLOT(slotJump(QString, unsigned int)));
	connect(stderr, SIGNAL(clearAllOutputViews()), this, SLOT(slotClearAllOutputViews()));
	messagePanel->addWidget(stderr, "Stderr");

	cuteShell = new CUTEShell(messagePanel, "Shell");
	messagePanel->addWidget(cuteShell, "Shell");

	grepout = new GrepOutputView(messagePanel, "Search");
	connect(grepout, SIGNAL(jump(QString, unsigned int)), this, SLOT(slotJump(QString, unsigned int)));
	connect(grepout, SIGNAL(showGrepDialog()), findInFilesDlg, SLOT(show()));
	messagePanel->addWidget(grepout, "Search");

	tagout = new TagOutputView(messagePanel, "Tags");
	connect(tagout, SIGNAL(jump(QString, unsigned int)), this, SLOT(slotJump(QString, unsigned int)));
	messagePanel->addWidget(tagout, "Tags");

	connect( findInFilesDlg, SIGNAL(readyReadStdout()), this, SLOT(readyReadGrepStdout()));
	connect( findInFilesDlg, SIGNAL(clearList()), grepout, SLOT(clear()));
	connect( findInFilesDlg, SIGNAL(viewSearch()), this, SLOT(slotViewSearch()));
	connect( findDlg, SIGNAL(foundString(QString)), grepout, SLOT(insertString(QString)));
	connect( projectDlg, SIGNAL(removedFile(QString)), projectView, SLOT(removeFile(QString)));
	connect( projectDlg, SIGNAL(addedFile(QString)), projectView, SLOT(addFile(QString)));

	connect( settingsDlg, SIGNAL(updateConfig()), this, SLOT(updateViews()));
	connect( settingsDlg, SIGNAL(updateConfig()), this, SLOT(configure()));

	connect( shortcutsDlg, SIGNAL(updateShortcuts()), this, SLOT(updateEditActions()));

	connect( cmdInterpreter, SIGNAL(readyReadStdout()), this, SLOT(readStdout()) );
	connect( cmdInterpreter, SIGNAL(readyReadStderr()), this, SLOT(readStderr()));
	connect( cmdInterpreter, SIGNAL(processExited()), this, SLOT(slotProcessExited()));

	connect( this, SIGNAL(lastWindowClosed()), this, SLOT(slotLastChildViewClosed()));
	connect( this, SIGNAL(windowChanged(QWidget*)), this, SLOT(activateWindow(QWidget*)) );
}

CUTE::~CUTE()
{
}

void CUTE::initLangMenu()
{
	// create lang menu
	std::list<std::string> str_list = Config::LexerManager::langStringList();
	std::list<std::string>::iterator i = str_list.begin();
	for(; i != str_list.end(); ++i)
		langMenu->insertItem(i->c_str());
}

void CUTE::initActions()
{
	QPixmap buildIcon = QPixmap((const char**)build_xpm);
	QPixmap closeIcon = QPixmap((const char**)close_xpm);
	QPixmap compileIcon = QPixmap((const char**)compile_xpm);
	QPixmap copyIcon = QPixmap((const char**)copy_xpm);
	QPixmap cutIcon = QPixmap((const char**)cut_xpm);
	QPixmap docinIcon = QPixmap((const char**)docin_xpm);
	QPixmap findInFilesIcon = QPixmap((const char**)findinfiles_xpm);
	QPixmap findNextIcon = QPixmap((const char**)findnext_xpm);
	QPixmap forwardIcon = QPixmap((const char**)forward_xpm);
	QPixmap helpIcon = QPixmap((const char**)help_xpm);
	QPixmap leftIcon = QPixmap((const char**)left_xpm);
	QPixmap newIcon = QPixmap((const char**)new_xpm);
	QPixmap nextIcon = QPixmap((const char**)next_xpm);
	QPixmap openIcon = QPixmap((const char**)open_xpm);
	QPixmap pasteIcon = QPixmap((const char**)paste_xpm);
	QPixmap prevIcon = QPixmap((const char**)prev_xpm);
	QPixmap printIcon = QPixmap((const char**)print_xpm);
	QPixmap qtIcon = QPixmap((const char**)qtlogo_xpm);
	QPixmap redoIcon = QPixmap((const char**)redo_xpm);
	QPixmap replaceIcon = QPixmap((const char**)replace_xpm);
	QPixmap replace2Icon = QPixmap((const char**)replace2_xpm);
	QPixmap rightIcon = QPixmap((const char**)right_xpm);
	QPixmap saveIcon = QPixmap((const char**)save_xpm);
	QPixmap saveAllIcon = QPixmap((const char**)saveall_xpm);
	QPixmap searchIcon = QPixmap((const char**)search_xpm);
	QPixmap stopIcon = QPixmap((const char**)stop_xpm);
	QPixmap undoIcon = QPixmap((const char**)undo_xpm);
	QPixmap worldIcon = QPixmap((const char**)world_xpm);
	QPixmap xbuildIcon = QPixmap((const char**)xbuild_xpm);

	QPixmap saveAsIcon = QPixmap((const char**)SaveAs_xpm);
	QPixmap executeIcon = QPixmap((const char**)ExecuteScript_xpm);
	QPixmap fullScreenIcon = QPixmap((const char**)FullScreen_xpm);

	fileNew = new QAction(tr("New File"), newIcon, tr("&New"), QKeySequence(CTRL+Key_F, Key_N), this, "new");
	fileNew->setStatusTip(tr("Creates a new document"));
	connect(fileNew, SIGNAL(activated()), this, SLOT(slotFileNew()));

	fileOpen = new QAction(tr("Open File"), openIcon, tr("&Open..."), QKeySequence(CTRL+Key_F, Key_O), this, "open");
	fileOpen->setStatusTip(tr("Opens an existing document"));
	connect(fileOpen, SIGNAL(activated()), this, SLOT(slotFileOpen()));

	fileReopen = new QAction(tr("Reload File"), QPixmap(), tr("&Reload"), QKeySequence(CTRL+Key_F, Key_R), this, "reopen");
	fileReopen->setStatusTip(tr("Reloads current document"));
	connect(fileReopen, SIGNAL(activated()), this, SLOT(slotFileReopen()));

	fileChangeWorkingDir = new QAction(tr("Change Working Dir"), QPixmap(), tr("Change &Working Dir"), 
		QKeySequence(CTRL+Key_F, Key_W), this, "changeWorkingDir");
	fileChangeWorkingDir->setStatusTip(tr("Change Working Directory of CUTE"));
	connect(fileChangeWorkingDir, SIGNAL(activated()), this, SLOT(slotFileChangeWorkingDir()));

	fileClose = new QAction(tr("Close File"), closeIcon, tr("&Close"), QKeySequence(CTRL+Key_F, Key_C), this, "close");
	fileClose->setStatusTip(tr("Closes the actual document"));
	connect(fileClose, SIGNAL(activated()), this, SLOT(slotFileClose()));

	fileCloseAll = new QAction(tr("Close All Files"), QPixmap(), tr("Close &All"), QKeySequence(CTRL+Key_F, CTRL+Key_C), this, "closeAll");
	fileCloseAll->setStatusTip(tr("Closes all loaded documents"));
	connect(fileCloseAll, SIGNAL(activated()), this, SLOT(slotFileCloseAll()));

	fileSave = new QAction(tr("Save File"), saveIcon, tr("&Save"), QKeySequence(CTRL+Key_F, Key_S), this, "save");
	fileSave->setStatusTip(tr("Saves the actual document"));
	connect(fileSave, SIGNAL(activated()), this, SLOT(slotFileSave()));
	
	fileSaveAll = new QAction(tr("Save All"), saveAllIcon, tr("Save A&ll"), QKeySequence(CTRL+Key_F, Key_A), this, "saveAll");
	fileSaveAll->setStatusTip(tr("Saves all documents"));
	connect(fileSaveAll, SIGNAL(activated()), this, SLOT(slotFileSaveAll()));
	
	fileSaveAs = new QAction(tr("Save As"), saveAsIcon, tr("Sa&ve As"), QKeySequence(CTRL+Key_F, CTRL+Key_S), this, "saveAs");
	fileSaveAs->setStatusTip(tr("Saves the actual documents"));
	connect(fileSaveAs, SIGNAL(activated()), this, SLOT(slotFileSaveAs()));

	fileLoadTagsFile = new QAction(tr("Load Tags File"), QPixmap(), tr("Load &Tags File"), QKeySequence(CTRL+Key_T, Key_L), this, 
		"loadTagsFile");
	fileLoadTagsFile->setStatusTip(tr("Loads tags file"));
	connect(fileLoadTagsFile, SIGNAL(activated()), this, SLOT(slotFileLoadTagsFile()));

	fileUnloadTagsFile = new QAction(tr("Unload Tags File"), QPixmap(), tr("&Unload Tags File"), QKeySequence(CTRL+Key_T, Key_U), this,
		"unloadTagsFile");
	fileUnloadTagsFile->setStatusTip(tr("Unload tags file"));
	connect(fileUnloadTagsFile, SIGNAL(activated()), this, SLOT(slotFileUnloadTagsFile()));
	fileUnloadTagsFile->setEnabled(false);

	filePrint = new QAction(tr("Print"), printIcon, tr("&Print"), CTRL+Key_P, this, "print");
	filePrint->setStatusTip(tr("Prints out the actual document"));
	connect(filePrint, SIGNAL(activated()), this, SLOT(slotFilePrint()));

	fileExit = new QAction(tr("Exit"), QPixmap(), tr("&Exit"), CTRL+Key_Q, this, "exit");
	fileExit->setStatusTip(tr("Quits the application"));
	connect(fileExit, SIGNAL(activated()), this, SLOT(slotFileQuit()));
	
	editGotoMatchingBrace = new QAction(tr("Match Paren"), QPixmap(), tr("&Match Paren"), CTRL+Key_ParenLeft, this,
		"gotoMatchingBrace");
	editGotoMatchingBrace->setStatusTip(tr("Jumps to matching Paren"));
	connect(editGotoMatchingBrace, SIGNAL(activated()), this, SLOT(slotEditGotoMatchingBrace()));

	editSelectToMatchingBrace = new QAction(tr("Select to Paren"), QPixmap(), tr("Select to &Paren"), ALT+CTRL+Key_ParenLeft, this,
		"selectToMatchingBrace");
	editSelectToMatchingBrace->setStatusTip(tr("Selects to matching Paren"));
	connect(editSelectToMatchingBrace, SIGNAL(activated()), this, SLOT(slotEditSelectToMatchingBrace()));

	editUndo = new QAction(tr("Undo"), undoIcon, tr("&Undo"), CTRL+Key_Z, this, "undo");
	editUndo->setStatusTip(tr("Undo last action"));
	connect(editUndo, SIGNAL(activated()), this, SLOT(slotEditUndo()));
	
	editRedo = new QAction(tr("Redo"), redoIcon, tr("&Redo"), CTRL+Key_Y, this, "redo");
	editRedo->setStatusTip(tr("Redo last action"));
	connect(editRedo, SIGNAL(activated()), this, SLOT(slotEditRedo()));
	
	editCut = new QAction(tr("Cut"), cutIcon, tr("&Cut"), SHIFT+Key_Delete, this, "cut");
	editCut->setStatusTip(tr("Cuts selected selection, puts it into the clipboard"));
	connect(editCut, SIGNAL(activated()), this, SLOT(slotEditCut()));
	
	editPaste = new QAction(tr("Paste"), pasteIcon, tr("&Paste"), SHIFT+Key_Insert, this, "paste");
	editPaste->setStatusTip(tr("Pastes the clipboard contents to actual position"));
	connect(editPaste, SIGNAL(activated()), this, SLOT(slotEditPaste()));
	
	editCopy = new QAction(tr("Copy"), copyIcon, tr("C&opy"), CTRL+Key_C, this, "copy");
	editCopy->setStatusTip(tr("Copies the selected section to the clipboard"));
	connect(editCopy, SIGNAL(activated()), this, SLOT(slotEditCopy()));
	
	editDelete = new QAction(tr("Delete"), QPixmap(), tr("&Delete"), Key_Delete, this, "delete");
	editDelete->setStatusTip(tr("Delete Selection"));
	connect(editDelete, SIGNAL(activated()), this, SLOT(slotEditDelete()));
	
	editIndent = new QAction(tr("Indent"), QPixmap(), tr("&Indent"), 0, this, "indent");
	editIndent->setStatusTip(tr("Indent"));
	connect(editIndent, SIGNAL(activated()), this, SLOT(slotEditIndent()));
	
	editUnindent = new QAction(tr("Unindent"), QPixmap(), tr("U&nindent"), SHIFT+Key_Tab, this, "unindent");
	editUnindent->setStatusTip(tr("Unindent"));
	connect(editUnindent, SIGNAL(activated()), this, SLOT(slotEditUnindent()));
	
	editSelectAll = new QAction(tr("Select All"), QPixmap(), tr("&Select All"), CTRL+Key_A, this, "selectAll");
	editSelectAll->setStatusTip(tr("Selects the whole document contents"));
	connect(editSelectAll, SIGNAL(activated()), this, SLOT(slotEditSelectAll()));
	
	editInsertCommand = new QAction(tr("Insert Command"), QPixmap(), tr("Ins&ert Command..."), 
		CTRL+Key_Greater, this, "insertCommand");
	editInsertCommand->setStatusTip(tr("Inserts output of shell command"));
	connect(editInsertCommand, SIGNAL(activated()), this, SLOT(slotEditInsertCommand()));
	
	editFilter = new QAction(tr("Filter"), QPixmap(), tr("&Filter..."), CTRL+Key_Exclam, this, "editFilter");
	editFilter->setStatusTip(tr("Filter selected text with a shell command"));
	connect(editFilter, SIGNAL(activated()), this, SLOT(slotEditFilter()));
	
	editBlockComment = new QAction(tr("Block Un/Comment"), QPixmap(), tr("&Block Un/Comment"), 
		QKeySequence("Ctrl+#"), this, "blockComment");
	editBlockComment->setStatusTip(tr("Block comment or uncomment current line or selection"));
	connect(editBlockComment, SIGNAL(activated()), this, SLOT(slotEditBlockComment()));
	
	editStreamComment = new QAction(tr("Stream Comment"), QPixmap(), tr("St&ream Comment"), 
		QKeySequence("Ctrl+Alt+#"), this, "streamComment");
	editStreamComment->setStatusTip(tr("Stream comment selection"));
	connect(editStreamComment, SIGNAL(activated()), this, SLOT(slotEditStreamComment()));
	
	editAutoComplete = new QAction(tr("Auto Complete"), QPixmap(), tr("&Auto Complete"), CTRL+Key_Return, this, 
		"autoComplete");
	editAutoComplete->setStatusTip(tr("Display an auto-completion list"));
	connect(editAutoComplete, SIGNAL(activated()), this, SLOT(autoComplete()));
	
	searchFind = new QAction(tr("Find"), searchIcon, tr("&Find"), QKeySequence(CTRL+Key_S, Key_F), this, 
		"find");
	searchFind->setStatusTip(tr("Find string in text"));
	connect(searchFind, SIGNAL(activated()), this, SLOT(slotSearchFind()));

	searchFindNext = new QAction(tr("Find Next"), findNextIcon, tr("Find &Next"), QKeySequence(CTRL+Key_S, Key_N), this,
		"findNext");
	searchFindNext->setStatusTip(tr("Find next string in text"));
	connect(searchFindNext, SIGNAL(activated()), this, SLOT(slotSearchFindNext()));

	searchFindPrevious = new QAction(tr("Find Previous"), QPixmap(), tr("Find &Previous"), QKeySequence(CTRL+Key_S, Key_P), this,
		"findPrevious");
	searchFindPrevious->setStatusTip(tr("Find string in text"));
	connect(searchFindPrevious, SIGNAL(activated()), this, SLOT(slotSearchFindPrevious()));

	searchFindInFiles = new QAction(tr("Grep"), findInFilesIcon, tr("&Grep"), QKeySequence(CTRL+Key_S, Key_G), this,
		"findInFiles");
	searchFindInFiles->setStatusTip(tr("Find string in files"));
	connect(searchFindInFiles, SIGNAL(activated()), this, SLOT(slotSearchFindInFiles()));

	searchFindSelection = new QAction(tr("Find Selection"), QPixmap(), tr("Find &Selection"), QKeySequence(CTRL+Key_S, Key_S), 
		this, "findSelection");
	searchFindSelection->setStatusTip(tr("Finds current selcted text"));
	connect(searchFindSelection, SIGNAL(activated()), this, SLOT(findSelection()));

	searchEnterSelection = new QAction(tr("Enter Selection"), QPixmap(), tr("&Enter Selection"), QKeySequence(CTRL+Key_S, Key_E),
		this, "enterSelection");
	searchEnterSelection->setStatusTip(tr("Enters current selected text as find target"));
	connect(searchEnterSelection, SIGNAL(activated()), this, SLOT(enterSelection()));

	searchReplace = new QAction(tr("Replace"), replaceIcon, tr("&Replace"), QKeySequence(CTRL+Key_S, Key_R), this, 
		"replace");
	searchReplace->setStatusTip(tr("Replace string in text"));
	connect(searchReplace, SIGNAL(activated()), this, SLOT(slotSearchReplace()));

	searchGoTo = new QAction(tr("Goto"), QPixmap(), tr("Goto &Line"), QKeySequence(CTRL+Key_S, Key_L), this,
		"goTo");
	searchGoTo->setStatusTip(tr("Goto line in text"));
	connect(searchGoTo, SIGNAL(activated()), this, SLOT(slotSearchGoto()));

	searchFindTag = new QAction(tr("Find Tag(s)"), QPixmap(), tr("Find &Tag(s)"), QKeySequence(CTRL+Key_T, Key_F), this,
		"findTag");
	searchFindTag->setStatusTip(tr("Finds selected tag"));
	connect(searchFindTag, SIGNAL(activated()), this, SLOT(slotTagContext()));

	searchJumpToTag = new QAction(tr("Jump To Tag"), QPixmap(), tr("&Jump To Tag"), QKeySequence(CTRL+Key_T, Key_J), this,
		"jumpToTag");
	searchJumpToTag->setStatusTip(tr("Jumps to selected tag"));
	connect(searchJumpToTag, SIGNAL(activated()), this, SLOT(slotSearchFindTag()));

	searchFindNextTag = new QAction(tr("Jump To Next Tag"), QPixmap(), tr("Jump To N&ext Tag"), QKeySequence(CTRL+Key_T, Key_N), 
		this, "jumpToNextTag");
	searchFindNextTag->setStatusTip(tr("Jumps to next tag"));
	connect(searchFindNextTag, SIGNAL(activated()), this, SLOT(slotSearchFindNextTag()));

	searchFindPreviousTag = new QAction(tr("Jump To Previous Tag"), QPixmap(), tr("Jump To Pre&vious Tag"), QKeySequence(CTRL+Key_T, Key_P),
		this, "jumpToPreviousTag");
	searchFindPreviousTag->setStatusTip(tr("Jumps to previous Tag"));
	connect(searchFindPreviousTag, SIGNAL(activated()), this, SLOT(slotSearchFindPreviousTag()));

	searchFindNextBookmark = new QAction(tr("Find Next Bookmark"), QPixmap(), tr("Find &Next Bookmark"), 
		QKeySequence(CTRL+Key_B, Key_N), this, "findNextBookmark");
	searchFindNextBookmark->setStatusTip(tr("Find next bookmark from current line"));
	connect(searchFindNextBookmark, SIGNAL(activated()), this, SLOT(slotSearchNextBookmark()));

	searchFindPrevBookmark = new QAction(tr("Find Previous Bookmark"), QPixmap(), tr("Find P&revious Bookmark"), 
		QKeySequence(CTRL+Key_B, Key_P), this, "findPreviousBookmark");
	searchFindPrevBookmark->setStatusTip(tr("Find previous bookmark from current line"));
	connect(searchFindPrevBookmark, SIGNAL(activated()), this, SLOT(slotSearchPrevBookmark()));

	searchToggleBookmark = new QAction(tr("Toggle Bookmark"), QPixmap(), tr("Toggle &Bookmark"), QKeySequence(CTRL+Key_B, Key_T),
		this, "toggleBookmark");
	searchToggleBookmark->setStatusTip(tr("Toggle bookmark at current line"));
	connect(searchToggleBookmark, SIGNAL(activated()), this, SLOT(slotSearchToggleBookmark()));

	searchRenameBookmark = new QAction(tr("Rename Bookmark"), QPixmap(), tr("&Rename Bookmark"), QKeySequence(CTRL+Key_B, Key_R), 
		this, "renameBookmark");
	searchRenameBookmark->setStatusTip(tr("Rename bookmark at current line"));
	connect(searchRenameBookmark, SIGNAL(activated()), this, SLOT(slotSearchRenameBookmark()));
	
	viewToggleCurrentFold = new QAction(tr("Toggle Current Fold"), QPixmap(), tr("Toggle Current Fold"), 0, this, 
		"toggleCurrentFold"); 
	connect(viewToggleCurrentFold, SIGNAL(activated()), this, SLOT(slotViewToggleCurrentFold()));

	viewToggleAllFolds = new QAction(tr("Toggle All Folds"), QPixmap(), tr("Toggle All &Folds"), 0, this, 
		"toggleAllFolds"); 
	connect(viewToggleAllFolds, SIGNAL(activated()), this, SLOT(slotViewToggleAllFolds()));

	viewWrapLines = new QAction(tr("Wrap lines"), QPixmap(), tr("W&rap lines"), 0, this, "wrapLines");
	viewWrapLines->setStatusTip(tr("Wrap/unwrap all lines"));
	viewWrapLines->setToggleAction(true);
	connect(viewWrapLines, SIGNAL(activated()), this, SLOT(slotViewWrapLines()));

	viewBraceMatch = new QAction(tr("Paren Matching"), QPixmap(), tr("&Paren Matching"), 0, this,
		"viewBraceMatch");
	viewBraceMatch->setStatusTip(tr("Enables/disables Paren matching"));
	viewBraceMatch->setToggleAction(true);
	connect(viewBraceMatch, SIGNAL(activated()), this, SLOT(slotViewBraceMatch()));
	
	viewFullscreen = new QAction(tr("Full Screen"), fullScreenIcon, tr("Full &Screen"), Key_F11, this, 
		"viewFullscreen");
	viewFullscreen->setStatusTip(tr("View textview in full screen mode"));
	connect(viewFullscreen, SIGNAL(activated()), this, SLOT(slotViewFullScreen() ) );
	viewFullscreen->setToggleAction(true);

	viewStatusbar = new QAction(tr("Statusbar"), QPixmap(), tr("&Statusbar"), 0, this, "viewStatusbar");
	viewStatusbar->setStatusTip(tr("Enables/disables the statusbar"));
	connect(viewStatusbar, SIGNAL(activated()), this, SLOT(slotViewStatusBar()));
	viewStatusbar->setToggleAction(true);

	viewToolbar = new QAction(tr("Toolbar"), QPixmap(), tr("&Toolbar"), 0, this, "viewToolbar");
	viewToolbar->setStatusTip(tr("Enables/disables the toolbar"));
	connect(viewToolbar, SIGNAL(activated()), this, SLOT(slotViewToolBar()));
	viewToolbar->setToggleAction(true);

	viewCmdToolbar = new QAction(tr("Cmd Box"), QPixmap(), tr("&Command Box"), 0, this, "viewCmdToolbar");
	viewCmdToolbar->setStatusTip(tr("Shows/Hide the cmd box"));
	connect(viewCmdToolbar, SIGNAL(activated()), this, SLOT(slotViewCmdToolBar()));
	viewCmdToolbar->setToggleAction(true);

	viewWhitespaces = new QAction(tr("Whitespaces"), QPixmap(), tr("&Whitespaces"), 0, this, "viewWhitespaces");
	viewWhitespaces->setStatusTip(tr("View whitespaces in current textview"));
	connect(viewWhitespaces, SIGNAL(activated()), this, SLOT(slotViewWhiteSpaces()));
	viewWhitespaces->setToggleAction(true);

	viewLineNumbers = new QAction(tr("Line Numbers"), QPixmap(), tr("&Line Numbers"), 0, this, "viewLineNumbers");
	viewLineNumbers->setStatusTip(tr("View line numbers in current textview"));
	connect(viewLineNumbers, SIGNAL(activated()), this, SLOT(slotViewLineNumbers()));
	viewLineNumbers->setToggleAction(true);

	viewMargin = new QAction(tr("Margin"), QPixmap(), tr("&Margin"), 0, this, "viewMargin");
	viewMargin->setStatusTip(tr("View margin in current textview"));
	connect(viewMargin, SIGNAL(activated()), this, SLOT(slotViewMargin()));
	viewMargin->setToggleAction(true);

	viewFoldMargin = new QAction(tr("Fold Margin"), QPixmap(), tr("Fold Mar&gin"), 0, this, "viewFoldMargin");
	viewFoldMargin->setStatusTip(tr("View fold margin in current textview"));
	connect(viewFoldMargin, SIGNAL(activated()), this, SLOT(slotViewFoldMargin()));
	viewFoldMargin->setToggleAction(true);

	viewShowMessagePanel = new QAction(tr("Show Message Panel"), QPixmap(), tr("Show Message Panel"), 0, this, "viewShowMessagePanel");
	viewShowMessagePanel->setStatusTip(tr("Hide/Show message/stdin/stdout/etc panel."));
	connect( viewShowMessagePanel, SIGNAL(activated()), this, SLOT(slotViewShowMessagePanel()));
	viewShowMessagePanel->setToggleAction(true);

	viewShowSidePanel = new QAction(tr("Show Side Panel"), QPixmap(), tr("Show Side Panel"), 0, this, "viewShowSidePanel");
	viewShowSidePanel->setStatusTip(tr("Hide/Show file browser and tags panel."));
	connect( viewShowSidePanel, SIGNAL(activated()), this, SLOT(slotViewShowSidePanel()));
	viewShowSidePanel->setToggleAction(true);

	viewZoomIn = new QAction(tr("Zoom In"), QPixmap(), tr("Zoom &In"), CTRL+Key_Plus, this, "zoomIn");
	viewZoomIn->setStatusTip(tr("Zoom in current textview"));
	connect(viewZoomIn, SIGNAL(activated()), this, SLOT(slotViewZoomIn()));
	
	viewZoomOut = new QAction(tr("Zoom Out"), QPixmap(), tr("Zoom &Out"), CTRL+Key_Minus, this, "zoomOut");
	viewZoomOut->setStatusTip(tr("Zoom out current textview"));
	connect(viewZoomOut, SIGNAL(activated()), this, SLOT(slotViewZoomOut()));

	viewUseMonospacedFont = new QAction(tr("Use Monospaced Font"), QPixmap(), tr("Use Monospaced Font"), CTRL+Key_F11, this, "useMonospacedFont");
	viewUseMonospacedFont->setStatusTip(tr("Use monospaced font in views"));
	connect(viewUseMonospacedFont, SIGNAL(activated()), this, SLOT(slotViewUseMonospacedFont()));
	viewUseMonospacedFont->setToggleAction(true);

	viewSwitchFocusCmdBox = new QAction(tr("Switch Focus to Command Box"), QPixmap(), tr("Command Box"), 0, this, "switchFocusCmdBox");
	viewSwitchFocusDoc = new QAction(tr("Switch Focus to Document"), QPixmap(), tr("Document"), 0, this, "switchFocusDoc");
	viewSwitchFocusDir = new QAction(tr("Switch Focus to Dir"), QPixmap(), tr("Dir"), 0, this, "switchFocusDir");
	viewSwitchFocusTagsFile = new QAction(tr("Switch Focus to Tags File"), QPixmap(), tr("Tags File"), 0, this, "switchFocusTagsFile");
	viewSwitchFocusPro = new QAction(tr("Switch Focus to Pro"), QPixmap(), tr("Pro"), 0, this, "switchFocusPro");
	viewSwitchFocusMessages = new QAction(tr("Switch Focus to Messages"), QPixmap(), tr("Messages"), 0, this, "switchFocusMessages");
	viewSwitchFocusStdout = new QAction(tr("Switch Focus to Stdout"), QPixmap(), tr("Stdout"), 0, this, "switchFocusStdout");
	viewSwitchFocusStderr = new QAction(tr("Switch Focus to Stderr"), QPixmap(), tr("Stderr"), 0, this, "switchFocusStderr");
	viewSwitchFocusShell = new QAction(tr("Switch Focus to Shell"), QPixmap(), tr("Shell"), 0, this, "switchFocusShell");
	viewSwitchFocusSearch = new QAction(tr("Switch Focus to Search"), QPixmap(), tr("Search"), 0, this, "switchFocusSearch");
	viewSwitchFocusTags = new QAction(tr("Switch Focus to Tags"), QPixmap(), tr("Tags"), 0, this, "switchFocusTags");
	connect(viewSwitchFocusCmdBox, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusCmdBox()));
	connect(viewSwitchFocusDoc, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusDoc()));
	connect(viewSwitchFocusDir, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusDir()));
	connect(viewSwitchFocusTagsFile, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusTagsFile()));
	connect(viewSwitchFocusPro, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusPro()));
	connect(viewSwitchFocusMessages, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusMessages()));
	connect(viewSwitchFocusStdout, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusStdout()));
	connect(viewSwitchFocusStderr, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusStderr()));
	connect(viewSwitchFocusShell, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusShell()));
	connect(viewSwitchFocusSearch, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusSearch()));
	connect(viewSwitchFocusTags, SIGNAL(activated()), this, SLOT(slotViewSwitchFocusTags()));
	
	toolsCompile = new QAction(tr("Compile"), compileIcon, tr("&Compile"), 0, this, "compile");
	toolsCompile->setStatusTip(tr("Compile current file"));
	connect(toolsCompile, SIGNAL(activated()), this, SLOT(slotToolsCompile()));

	toolsBuild = new QAction(tr("Build"), buildIcon, tr("&Build"), Key_F7, this, "build");
	toolsBuild->setStatusTip(tr("Run makefile"));
	connect(toolsBuild, SIGNAL(activated()), this, SLOT(slotToolsBuild()));

	toolsGo = new QAction(tr("Go"), QPixmap(), tr("&Go"), 0, this, "go");
	toolsGo->setStatusTip(tr("Run programme"));
	connect(toolsGo, SIGNAL(activated()), this, SLOT(slotToolsGo()));
	
	toolsStop = new QAction(tr("Stop"), stopIcon, tr("&Stop"), 0, this, "stop");
	toolsStop->setStatusTip(tr("Terminates current programme"));
	connect(toolsStop, SIGNAL(activated()), this, SLOT(slotToolsKill()));
	toolsStop->setEnabled(false);

	toolsNextMessage = new QAction(tr("Next Message"), rightIcon, tr("&Next Message"), 0, this, "nextMessage");
	toolsNextMessage->setStatusTip(tr("Jumps to next message"));
	connect(toolsNextMessage, SIGNAL(activated()), this, SLOT(slotToolsNextMessage()));
	
	toolsPrevMessage = new QAction(tr("Previous Message"), leftIcon, tr("&Prev Message"), 0, this, "prevMessage");
	toolsPrevMessage->setStatusTip(tr("Jumps to previous message"));
	connect(toolsPrevMessage, SIGNAL(activated()), this, SLOT(slotToolsPrevMessage()));
	
	projectOpen = new QAction(tr("Open"), openIcon, tr("&Open"), 0, this, "openProject");
	projectOpen->setStatusTip(tr("Opens a project file"));
	connect(projectOpen, SIGNAL(activated()), this, SLOT(slotProjectOpen()));
	
	projectNew = new QAction(tr("New"), newIcon, tr("&New"), 0, this, "newProject");
	projectNew->setStatusTip(tr("Creates a new project"));
	connect(projectNew, SIGNAL(activated()), this, SLOT(slotProjectNew()));
	
	projectClose = new QAction(tr("Close"), closeIcon, tr("&Close"), 0, this, "closeProject");
	projectOpen->setStatusTip(tr("Closes a project file"));
	connect(projectClose, SIGNAL(activated()), this, SLOT(slotProjectClose()));
	projectClose->setEnabled(false);
	
	projectOptions = new QAction(tr("Options"), QPixmap(), tr("O&ptions"), 0, this, "optionsProject");
	projectOptions->setStatusTip(tr("Opens project options dialog"));
	connect(projectOptions, SIGNAL(activated()), this, SLOT(slotProjectOptions()));
	projectOptions->setEnabled(false);
	
	optionsAutoIndent = new QAction(tr("Auto Indent"), QPixmap(), tr("&Auto Indent"), 0, this, "toggleAutoIndent");
	optionsAutoIndent->setStatusTip(tr("Enables/disables auto indent"));
	connect(optionsAutoIndent, SIGNAL(activated()), this, SLOT(slotOptAutoIndent()));
	optionsAutoIndent->setToggleAction(true);

	optionsPreferences = new QAction(tr("Configure CUTE"), QPixmap(), tr("&Configure CUTE"), 0, this, "");
	optionsPreferences->setStatusTip(tr("Show the preferences dialog"));
	connect(optionsPreferences, SIGNAL(activated()), this, SLOT(slotOptSettings()));

	optionsShortcuts = new QAction(tr("Configure Shortcuts"), QPixmap(), tr("Configure Shortc&uts"), 0, this, "");
	optionsShortcuts->setStatusTip(tr("Show the shortcuts dialog"));
	connect(optionsShortcuts, SIGNAL(activated()), this, SLOT(slotOptShortcuts()));

	optionsQtConfig = new QAction(tr("Qt Configuration"), QPixmap(), tr("&Qt Configuration"), 0, this, "showQtConfig");
	optionsQtConfig->setStatusTip(tr("Runs the Qt configuration tool"));
	connect(optionsQtConfig, SIGNAL(activated()), this, SLOT(slotOptQtConfig()));

	optionsEditPreConfigFile = new QAction(tr("Edit Pre Config File"), QPixmap(), tr("&Edit Pre Config File"), 
		0, this, "editPreConfigFile");
	optionsEditPreConfigFile->setStatusTip(tr("Opens the pre config file"));
	connect(optionsEditPreConfigFile, SIGNAL(activated()), this, SLOT(slotOptEditPreConfigFile()));

	optionsEditPostConfigFile = new QAction(tr("Edit Post Config File"), QPixmap(), tr("&Edit Post Config File"), 
		0, this, "editPostConfigFile");
	optionsEditPostConfigFile->setStatusTip(tr("Opens the post config file"));
	connect(optionsEditPostConfigFile, SIGNAL(activated()), this, SLOT(slotOptEditPostConfigFile()));

	optionsShowCutercFile = new QAction(tr("Edit .cuterc file"), QPixmap(), tr("&Edit .cuterc file"), 0, this, "showCutercFile");
	optionsShowCutercFile->setStatusTip(tr("Opens the automatically generated config file"));
	connect(optionsShowCutercFile, SIGNAL(activated()), this, SLOT(slotOptShowCutercFile()));

	extraRunScript = new QAction(tr("Run Script"), executeIcon, tr("&Run Script"), QKeySequence(CTRL+Key_R, Key_S), this,
		"runScript");
	extraRunScript->setStatusTip(tr("Run a python script"));
	connect(extraRunScript, SIGNAL(activated()), this, SLOT(slotExtraRunScript()));

	extraRunCurrentFile = new QAction(tr("Run Current File"), QPixmap(), tr("Run &Current File"), QKeySequence(CTRL+Key_R, Key_F), 
		this, "runCurrentFile");
	extraRunCurrentFile->setStatusTip(tr("Run a python script in the current view"));
	connect(extraRunCurrentFile, SIGNAL(activated()), this, SLOT(slotExtraRunCurrentFile()));

	extraRunPyCmd = new QAction(tr("Run Python Command"), QPixmap(), tr("Run &Python Command"), QKeySequence(CTRL+Key_R, Key_C), 
		this, "runPyCommand");
	extraRunPyCmd->setStatusTip(tr("Run a python string"));
	connect(extraRunPyCmd, SIGNAL(activated()), this, SLOT(slotExtraRunPythonString()));

	extraEvalSelection = new QAction(tr("Eval Selection"), QPixmap(), tr("&Eval Selection"), QKeySequence(CTRL+Key_R, Key_T), 
		this, "evalSelection");
	extraEvalSelection->setStatusTip(tr("Evaluate selection in the current view"));
	connect(extraEvalSelection, SIGNAL(activated()), this, SLOT(slotExtraEvalSelection()));

	extraLoadSession = new QAction(tr("Load Session"), QPixmap(), tr("&Load Session"), QKeySequence(CTRL+ALT+Key_S, Key_L), this,
		"loadSession");
	extraLoadSession->setStatusTip(tr("Loads a session"));
	connect(extraLoadSession, SIGNAL(activated()), this, SLOT(slotExtraLoadSession()));
	extraLoadSession->setEnabled(true);

	extraSaveSession = new QAction(tr("Save Session"), QPixmap(), tr("&Save Session"), QKeySequence(CTRL+ALT+Key_S, Key_S), this,
		"saveSession");
	extraSaveSession->setStatusTip(tr("Save current session"));
	connect(extraSaveSession, SIGNAL(activated()), this, SLOT(slotExtraSaveSession()));
	
	extraCloseSession = new QAction(tr("Close Session"), QPixmap(), tr("Cl&ose Session"), QKeySequence(CTRL+ALT+Key_S, Key_C), this,
		"closeSession");
	extraCloseSession->setStatusTip(tr("Closes current session"));
	connect(extraCloseSession, SIGNAL(activated()), this, SLOT(slotExtraCloseSession()));
	extraCloseSession->setEnabled(false);

	extraAddTool = new QAction(tr("Add Tool"), QPixmap(), tr("Add &Tool"), 0, this, "addTool");
	extraAddTool->setStatusTip(tr("Add a tool to the tools menu"));
	//connect(extraAddTool, SIGNAL(activated()), this, SLOT(slotExtraAddTool()));

	extraStartMacroRecording = new QAction(tr("Start Macro Recording"), QPixmap(), tr("Start Macro Recording"), 
		QKeySequence(CTRL+Key_M, Key_ParenLeft), this, "startMacroRecording");
	extraStartMacroRecording->setStatusTip(tr("Start recording of editor actions"));
	connect(extraStartMacroRecording, SIGNAL(activated()), this, SLOT(slotExtraStartMacroRecording()));

	extraStopMacroRecording = new QAction(tr("Stop Macro Recording"), QPixmap(), tr("Stop Macro Recording"), 
		QKeySequence(CTRL+Key_M, Key_ParenRight), this, "stopMacroRecording");
	extraStopMacroRecording->setStatusTip(tr("Stop recording of editor actions"));
	connect(extraStopMacroRecording, SIGNAL(activated()), this, SLOT(slotExtraStopMacroRecording()));

	extraRunMacro = new QAction(tr("Run Macro"), QPixmap(), tr("Run Macro"), QKeySequence(CTRL+Key_M, Key_R), this,
		"runMacro");
	extraRunMacro->setStatusTip(tr("Run recorded macro"));
	connect(extraRunMacro, SIGNAL(activated()), this, SLOT(slotExtraRunMacro()));

	extraLoadMacro = new QAction(tr("Load Macro"), QPixmap(), tr("Load Macro"), QKeySequence(CTRL+Key_M, Key_L), this,
		"loadMacro");
	extraLoadMacro->setStatusTip(tr("Loads a macro"));
	connect(extraLoadMacro, SIGNAL(activated()), this, SLOT(slotExtraLoadMacro()));

	extraSaveMacro = new QAction(tr("Save Macro"), QPixmap(), tr("Save Macro"), QKeySequence(CTRL+Key_M, Key_S), this,
		"saveMacro");
	extraSaveMacro->setStatusTip(tr("Save recorded macro"));
	connect(extraSaveMacro, SIGNAL(activated()), this, SLOT(slotExtraSaveMacro()));

	extraBuildTagsFile = new QAction(tr("Build Tags File"), QPixmap(xbuildIcon), tr("Build Tags File"), QKeySequence(CTRL+Key_T, Key_B), this,
		"buildTagsFile");
	extraSaveMacro->setStatusTip(tr("Build tags file of all source files in current directory"));
	connect(extraBuildTagsFile, SIGNAL(activated()), this, SLOT(slotExtraBuildTagsFile()));

	windowsNext = new QAction(tr("Next"), nextIcon, tr("&Next"), CTRL+ALT+Key_Right, this, "nextWindow");
	windowsNext->setStatusTip(tr("Switch to next the file"));
	connect(windowsNext, SIGNAL(activated()), this, SLOT(nextWindow()));

	windowsPrevious = new QAction(tr("Previous"), prevIcon, tr("&Previous"), CTRL+ALT+Key_Left, this, "previousWindow");
	windowsPrevious->setStatusTip(tr("Switch to the previous file"));
	connect(windowsPrevious, SIGNAL(activated()), this, SLOT(prevWindow()));

	windowsShowWindows = new QAction(tr("Show Windows"), QPixmap(), tr("Show Windows"), CTRL+Key_Space, this, "showWindows");
	windowsShowWindows->setStatusTip(tr("Show windows dialog"));
	connect(windowsShowWindows, SIGNAL(activated()), this, SLOT(slotWindowsDialog()));
	
	helpHandbook = new QAction(tr("Handbook"), QPixmap(), tr("&Handbook"), Key_F1, this, "helpHandbook");
	helpHandbook->setStatusTip(tr("Show Online Help"));
	connect(helpHandbook, SIGNAL(activated()), this, SLOT(slotHelpHandbook()));

	helpLicense = new QAction(tr("License"), QPixmap(), tr("&License"), 0, this, "viewLicense");
	helpLicense->setStatusTip(tr("Show GNU General Public License"));
	connect(helpLicense, SIGNAL(activated()), this, SLOT(slotHelpLicense()));

	helpAbout = new QAction(tr("About"), helpIcon, tr("&About"), 0, this, "viewAbout");
	helpAbout->setStatusTip(tr("Show about dialog"));
	connect(helpAbout, SIGNAL(activated()), this, SLOT(slotHelpAbout()));
	
	helpAboutQt = new QAction(tr("About Qt"), qtIcon, tr("About &Qt"), 0, this, "viewAboutQt");
	helpAboutQt->setStatusTip(tr("Show Qt Dialog"));
	connect(helpAboutQt, SIGNAL(activated()), this, SLOT(slotHelpAboutQt()));

	connect(toolsStop, SIGNAL(activated()), cmdInterpreter, SLOT(kill()));
	connect(toolsStop, SIGNAL(activated()), this, SLOT(slotProcessKilled()));
}

void CUTE::initEditActions()
{
	QWidget *zero = new QWidget;

	charLeftExtend = new QAction("Extend selection left one character", 
		"Extend selection left one character", QKeySequence(Key_Left+SHIFT), zero, "charLeftExtend");
	editActions.insert(QextScintillaBase::SCI_CHARLEFTEXTEND, charLeftExtend);

	charRightExtend = new QAction("Extend selection right one character", 
		"Extend selection right one character", QKeySequence(Key_Right+SHIFT), zero, "charRightExtend");
	editActions.insert(QextScintillaBase::SCI_CHARRIGHTEXTEND, charRightExtend);

	lineUpExtend = new QAction("Extend selection up one line", "Extend selection up one line", 
		QKeySequence(Key_Up+SHIFT), zero, "lineUpExtend");
	editActions.insert(QextScintillaBase::SCI_LINEUPEXTEND, lineUpExtend);

	lineDownExtend = new QAction("Extend selection down one line", "Extend selection down one line", 
		QKeySequence(Key_Down+SHIFT), zero, "lineDownExtend");
	editActions.insert(QextScintillaBase::SCI_LINEDOWNEXTEND, lineDownExtend);

	charLeft = new QAction("Move left one character", "Move left one character", QKeySequence(Key_Left), zero,
		"charLeft");
	editActions.insert(QextScintillaBase::SCI_CHARLEFT, charLeft);

	charRight = new QAction("Move right one character", "Move right one character", QKeySequence(Key_Right), zero,
		"charRight");
	editActions.insert(QextScintillaBase::SCI_CHARRIGHT, charRight);

	lineUp = new QAction("Move up one line", "Move up one line", QKeySequence(Key_Up), zero, "lineUp");
	editActions.insert(QextScintillaBase::SCI_LINEUP, lineUp);

	lineDown = new QAction("Move down one line", "Move down one line", QKeySequence(Key_Down), zero, "lineDown");
	editActions.insert(QextScintillaBase::SCI_LINEDOWN, lineDown);

	wordPartLeft = new QAction("Move left one word part", "Move left one word part", 
		QKeySequence(ALT+Key_Left), zero, "wordPartLeft");
	editActions.insert(QextScintillaBase::SCI_WORDPARTLEFT, wordPartLeft);

	wordPartRight = new QAction("Move right one word part", "Move right one word part", 
		QKeySequence(ALT+Key_Right), zero, "wordPartRight");
	editActions.insert(QextScintillaBase::SCI_WORDPARTRIGHT, wordPartRight);

	wordLeft = new QAction("Move left one word", "Move left one word", QKeySequence(Key_Left+CTRL), zero,
		"wordLeft");
	editActions.insert(QextScintillaBase::SCI_WORDLEFT, wordLeft);

	wordRight = new QAction("Move right one word", "Move right one word", QKeySequence(Key_Right+CTRL), zero,
		"wordRight");
	editActions.insert(QextScintillaBase::SCI_WORDRIGHT, wordRight);

	VCHome = new QAction("Move to first visible character in line", "Move to first visible character in line", 
		QKeySequence(Key_Home), zero, "VCHome");
	editActions.insert(QextScintillaBase::SCI_VCHOME, VCHome);

	homeDisplay = new QAction("Move to start of line", "Move to start of line", QKeySequence(Key_Home+ALT), 
		zero, "homeDisplay");
	editActions.insert(QextScintillaBase::SCI_HOMEDISPLAY, homeDisplay);
	
	lineEnd = new QAction("Move to end of line", "Move to end of line", Key_End, zero, 
		"lineEnd");
	editActions.insert(QextScintillaBase::SCI_LINEEND, lineEnd);

	lineScrollDown = new QAction("Scroll view down one line", "Scroll view down one line", 
		QKeySequence(Key_Down+CTRL), zero, "lineScrollDown");
	editActions.insert(QextScintillaBase::SCI_LINESCROLLDOWN, lineScrollDown);

	lineScrollUp = new QAction("Scroll view up one line", "Scroll view up one line", 
		QKeySequence(Key_Up+CTRL), zero, "lineScrollUp");
	editActions.insert(QextScintillaBase::SCI_LINESCROLLUP, lineScrollUp);

	paraUp = new QAction("Move up one paragraph", "Move up one paragraph", QKeySequence(Key_Up+ALT), zero,
		"paraUp");
	editActions.insert(QextScintillaBase::SCI_PARAUP, paraUp);

	paraDown = new QAction("Move down one paragraph", "Move down one paragraph", QKeySequence(Key_Down+ALT), 
		zero, "paraDown");
	editActions.insert(QextScintillaBase::SCI_PARADOWN, paraDown);

	pageUp = new QAction("Move up one page", "Move up one page", QKeySequence(Key_Prior), zero, "pageUp");
	editActions.insert(QextScintillaBase::SCI_PAGEUP, pageUp);

	pageDown = new QAction("Move down one page", "Move down one page", QKeySequence(Key_Next), zero, "pageDown");
	editActions.insert(QextScintillaBase::SCI_PAGEDOWN, pageDown);

	documentStart = new QAction("Move to start of text", "Move to start of text", 
		QKeySequence(Key_Home+CTRL), zero, "documentStart");
	editActions.insert(QextScintillaBase::SCI_DOCUMENTSTART, documentStart);

	documentEnd = new QAction("Move to end of text", "Move to end of text", QKeySequence(Key_End+CTRL), zero,
		"documentEnd");
	editActions.insert(QextScintillaBase::SCI_DOCUMENTEND, documentEnd);

	tab = new QAction("Indent one level", "Indent one level", QKeySequence(Key_Tab), zero, "tab");
	editActions.insert(QextScintillaBase::SCI_TAB, tab);

	backTab = new QAction("Move back one indentation level", "Move back one indentation level", 
		QKeySequence(Key_Tab+SHIFT), zero, "backTab");
	editActions.insert(QextScintillaBase::SCI_BACKTAB, backTab);

	selectAll = new QAction("Select all text", "Select all text", QKeySequence(CTRL+Key_A), zero, "selectAll");
	editActions.insert(QextScintillaBase::SCI_SELECTALL, selectAll);

	wordPartLeftExtend = new QAction("Extend selection left one word part", "Extend selection left one word part", 
		QKeySequence(Key_Left+SHIFT+ALT), zero, "wordPartLeftExtend");
	editActions.insert(QextScintillaBase::SCI_WORDPARTLEFTEXTEND, wordPartLeftExtend);

	wordPartRightExtend = new QAction("Extend selection right one word part", "Extend selection right one word part", 
		QKeySequence(Key_Right+SHIFT+ALT), zero, "wordPartRightExtend");
	editActions.insert(QextScintillaBase::SCI_WORDPARTRIGHTEXTEND, wordPartRightExtend);

	wordLeftExtend = new QAction("Extend selection left one word", "Extend selection left one word", 
		QKeySequence(Key_Left+SHIFT+CTRL), zero, "wordLeftExtend");
	editActions.insert(QextScintillaBase::SCI_WORDLEFTEXTEND, wordLeftExtend);

	wordRightExtend = new QAction("Extend selection right one word", "Extend selection right one word", 
		QKeySequence(Key_Right+SHIFT+CTRL), zero, "wordRightExtend");
	editActions.insert(QextScintillaBase::SCI_WORDRIGHTEXTEND, wordRightExtend);

	VCHomeExtend = new QAction("Extend selection to first visible character in line", 
		"Extend selection to first visible character in line", QKeySequence(Key_Home+SHIFT), zero, "VCHomeExtend");
	editActions.insert(QextScintillaBase::SCI_VCHOMEEXTEND, VCHomeExtend);

	homeDisplayExtend = new QAction("Extend selection to start of line", "Extend selection to start of line", 
		QKeySequence(Key_Home+SHIFT+ALT), zero, "homeDisplayExtend");
	editActions.insert(QextScintillaBase::SCI_HOMEDISPLAYEXTEND, homeDisplayExtend);

	lineEndExtend = new QAction("Extend selection to end of line", "Extend selection to end of line", 
		QKeySequence(Key_End+SHIFT), zero, "lineEndExtend");
	editActions.insert(QextScintillaBase::SCI_LINEENDEXTEND, lineEndExtend);

	paraUpExtend = new QAction("Extend selection up one paragraph", "Extend selection up one paragraph", 
		QKeySequence(Key_Up+SHIFT+ALT), zero, "paraUpExtend");
	editActions.insert(QextScintillaBase::SCI_PARAUPEXTEND, paraUpExtend);

	paraDownExtend = new QAction("Extend selection down one paragraph", "Extend selection down one paragraph", 
		QKeySequence(Key_Down+SHIFT+ALT), zero, "paraDownExtend");
	editActions.insert(QextScintillaBase::SCI_PARADOWNEXTEND, paraDownExtend);

	pageUpExtend = new QAction("Extend selection up one page", "Extend selection up one page", 
		QKeySequence(Key_Prior+SHIFT+ALT), zero, "pageUpExtend");
	editActions.insert(QextScintillaBase::SCI_PAGEUPEXTEND, pageUpExtend);

	pageDownExtend = new QAction("Extend selection down one page", "Extend selection down one page", 
		QKeySequence(Key_Next+SHIFT+ALT), zero, "pageDownExtend");
	editActions.insert(QextScintillaBase::SCI_PAGEDOWNEXTEND, pageDownExtend);

	documentStartExtend = new QAction("Extend selection to start of text", "Extend selection to start of text", 
		QKeySequence(Key_Home+SHIFT+CTRL), zero, "documentStartExtend");
	editActions.insert(QextScintillaBase::SCI_DOCUMENTSTARTEXTEND, documentStartExtend);

	documentEndExtend = new QAction("Extend selection to end of text", "Extend selection to end of text", 
		QKeySequence(Key_End+SHIFT+CTRL), zero, "documentEndExtend");
	editActions.insert(QextScintillaBase::SCI_DOCUMENTENDEXTEND, documentEndExtend);

	deleteBack = new QAction("Delete previous character", "Delete previous character", 
		QKeySequence(Key_Backspace), zero, "deleteBack");
	editActions.insert(QextScintillaBase::SCI_DELETEBACK, deleteBack);

	deleteBackNotLine = new QAction("Delete previous character if not at line start", 
		"Delete previous character if not at line start", QKeySequence(""), zero, "deleteBackNotLine");
	editActions.insert(QextScintillaBase::SCI_DELETEBACKNOTLINE, deleteBackNotLine);

	clear = new QAction("Delete current character", "Delete current character", QKeySequence(Key_Delete), zero,
		"clear");
	editActions.insert(QextScintillaBase::SCI_CLEAR, clear);

	delWordLeft = new QAction("Delete word to left", "Delete word to left", 
		QKeySequence(Key_Backspace+CTRL), zero, "delWordLeft");
	editActions.insert(QextScintillaBase::SCI_DELWORDLEFT, delWordLeft);

	delWordRight = new QAction("Delete word to right", "Delete word to right", 
		QKeySequence(Key_Delete+CTRL), zero, "delWordRight");
	editActions.insert(QextScintillaBase::SCI_DELWORDRIGHT, delWordRight);

	delLineLeft = new QAction("Delete line to left", "Delete line to left", 
		QKeySequence(Key_Backspace+SHIFT+CTRL), zero, "delLineLeft");
	editActions.insert(QextScintillaBase::SCI_DELLINELEFT, delLineLeft);

	delLineRight = new QAction("Delete line to right", "Delete line to right", 
		QKeySequence(Key_Delete+SHIFT+CTRL), zero, "delLineRight");
	editActions.insert(QextScintillaBase::SCI_DELLINERIGHT, delLineRight);

	newLine = new QAction("Insert new line", "Insert new line", QKeySequence(Key_Return), zero,
		"newLine");
	editActions.insert(QextScintillaBase::SCI_NEWLINE, newLine);

	lineDelete = new QAction("Delete current line", "Delete current line", 
		QKeySequence(Key_L+SHIFT+CTRL), zero, "lineDelete");
	editActions.insert(QextScintillaBase::SCI_LINEDELETE, lineDelete);

	lineDuplicate = new QAction("Duplicate current line", "Duplicate current line", 
		QKeySequence(CTRL+Key_D), zero, "lineDuplicate");
	editActions.insert(QextScintillaBase::SCI_LINEDUPLICATE, lineDuplicate);

	lineTranspose = new QAction("Swap current and previous lines", "Swap current and previous lines", 
		QKeySequence(Key_T+CTRL+ALT), zero, "lineTranspose");
	editActions.insert(QextScintillaBase::SCI_LINETRANSPOSE, lineTranspose);

	lineCut = new QAction("Cut current line", "Cut current line", QKeySequence(Key_L+CTRL), zero,
		"lineCut");
	editActions.insert(QextScintillaBase::SCI_LINECUT, lineCut);

	cut = new QAction("Cut selection", "Cut selection", QKeySequence(Key_Delete+SHIFT), zero, "cut");
	editActions.insert(QextScintillaBase::SCI_CUT, cut);

	lineCopy = new QAction("Copy current line", "Copy current line", QKeySequence(Key_T+SHIFT+CTRL), zero,
		"lineCopy");
	editActions.insert(QextScintillaBase::SCI_LINECOPY, lineCopy);

	copy = new QAction("Copy selection", "Copy selection", QKeySequence(Key_C+CTRL), zero, "copy");
	editActions.insert(QextScintillaBase::SCI_COPY, copy);

	paste = new QAction("Paste", "Paste", QKeySequence(Key_Insert+SHIFT), zero, "paste");
	editActions.insert(QextScintillaBase::SCI_PASTE, paste);

	redo = new QAction("Redo last command", "Redo last command", QKeySequence(Key_Y+CTRL), zero, "redo");
	editActions.insert(QextScintillaBase::SCI_REDO, redo);

	undo = new QAction("Undo the last command", "Undo the last command", QKeySequence(Key_Z+CTRL), zero, "undo");
	editActions.insert(QextScintillaBase::SCI_UNDO, undo);

	cancel = new QAction("Cancel", "Cancel", QKeySequence(Key_Escape), zero, "cancel");
	editActions.insert(QextScintillaBase::SCI_CANCEL, cancel);

	editToggleOvertype = new QAction("Toggle insert/overtype", "Toggle insert/overtype", 
		QKeySequence(Key_Insert), zero, "editToggleOvertype");
	editActions.insert(QextScintillaBase::SCI_EDITTOGGLEOVERTYPE, editToggleOvertype);

	lowerCase = new QAction("Convert selection to lower case", "Convert selection to lower case", 
		QKeySequence(Key_U+CTRL), zero, "lowerCase");
	editActions.insert(QextScintillaBase::SCI_LOWERCASE, lowerCase);

	upperCase = new QAction("Convert selection to upper case", "Convert selection to upper case", 
		QKeySequence(Key_U+CTRL+SHIFT), zero, "upperCase");
	editActions.insert(QextScintillaBase::SCI_UPPERCASE, upperCase);

	zoomIn = new QAction("Zoom in", "Zoom in", QKeySequence(CTRL+Key_Plus), zero, "zoomIn");
	editActions.insert(QextScintillaBase::SCI_ZOOMIN, zoomIn);

	zoomOut = new QAction("Zoom out", "Zoom out", QKeySequence(CTRL+Key_Minus), zero, "zoomOut");
	editActions.insert(QextScintillaBase::SCI_ZOOMOUT, zoomOut);
	
	Shortcuts::pairActions(editIndent, tab);
	Shortcuts::pairActions(editUnindent, backTab);
	Shortcuts::pairActions(editSelectAll, selectAll);
	Shortcuts::pairActions(editDelete, clear);
	Shortcuts::pairActions(editCut, cut);
	Shortcuts::pairActions(editCopy, copy);
	Shortcuts::pairActions(editPaste, paste);
	Shortcuts::pairActions(editUndo, undo);
	Shortcuts::pairActions(editRedo, redo);
	Shortcuts::pairActions(viewZoomIn, zoomIn);
	Shortcuts::pairActions(viewZoomOut, zoomOut);
}

void CUTE::initMenuBar()
{
  ///////////////////////////////////////////////////////////////////
  // MENUBAR
  
  ///////////////////////////////////////////////////////////////////
  // menuBar entry fileMenu

  fileMenu=new QPopupMenu();
  fileNew->addTo(fileMenu);
  fileOpen->addTo(fileMenu);
  recentFilesMenu = new QPopupMenu(this);
  fileMenu->insertItem("Recent &Files", recentFilesMenu);
  fileReopen->addTo(fileMenu);
  fileChangeWorkingDir->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileSave->addTo(fileMenu);
  fileSaveAs->addTo(fileMenu);
  fileSaveAll->addTo(fileMenu);
  fileClose->addTo(fileMenu);
  fileCloseAll->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileLoadTagsFile->addTo(fileMenu);
  fileUnloadTagsFile->addTo(fileMenu);
  fileMenu->insertSeparator();
  filePrint->addTo(fileMenu);
  fileMenu->insertSeparator();
  fileExit->addTo(fileMenu);     

  ///////////////////////////////////////////////////////////////////
  // menuBar entry editMenu
  editMenu=new QPopupMenu();
  editGotoMatchingBrace->addTo(editMenu);
  editSelectToMatchingBrace->addTo(editMenu);
  editMenu->insertSeparator();
  editUndo->addTo(editMenu);
  editRedo->addTo(editMenu);
  editMenu->insertSeparator();
  editCut->addTo(editMenu);
  editCopy->addTo(editMenu);
  editPaste->addTo(editMenu);
  editDelete->addTo(editMenu);
  editSelectAll->addTo(editMenu);
  editMenu->insertSeparator();
  editIndent->addTo(editMenu);
  editUnindent->addTo(editMenu);
  editMenu->insertSeparator();
  editBlockComment->addTo(editMenu);
  editStreamComment->addTo(editMenu);
  editMenu->insertSeparator();
  editAutoComplete->addTo(editMenu);
  editInsertCommand->addTo(editMenu);
  editFilter->addTo(editMenu);

  // Switch Focus Menu
  QPopupMenu *switchFocusMenu = new QPopupMenu();
  viewSwitchFocusCmdBox->addTo(switchFocusMenu);
  viewSwitchFocusDoc->addTo(switchFocusMenu);
  viewSwitchFocusDir->addTo(switchFocusMenu);
  viewSwitchFocusTagsFile->addTo(switchFocusMenu);
  viewSwitchFocusPro->addTo(switchFocusMenu);
  viewSwitchFocusMessages->addTo(switchFocusMenu);
  viewSwitchFocusStdout->addTo(switchFocusMenu);
  viewSwitchFocusStderr->addTo(switchFocusMenu);
  viewSwitchFocusShell->addTo(switchFocusMenu);
  viewSwitchFocusSearch->addTo(switchFocusMenu);
  viewSwitchFocusTags->addTo(switchFocusMenu);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry viewMenu
  viewMenu=new QPopupMenu();
  connect(viewMenu, SIGNAL(aboutToShow()), this, SLOT(updateViewMenu()));
  viewMenu->setCheckable(true);
  viewToggleCurrentFold->addTo(viewMenu);
  viewToggleAllFolds->addTo(viewMenu);
  viewBraceMatch->addTo(viewMenu);
  viewWrapLines->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewCmdToolbar->addTo(viewMenu);
  //viewStatusbar->addTo(viewMenu);
  viewMenu->insertItem("&Toolbar", this, SLOT(slotViewToolBar()), 0, ID_VIEW_TOOLBAR);
  viewShowMessagePanel->addTo(viewMenu);
  viewShowSidePanel->addTo(viewMenu);
  windowsShowWindows->addTo(viewMenu);
  viewFullscreen->addTo(viewMenu);
  viewMenu->insertItem("Switch Focus", switchFocusMenu);
  viewMenu->insertSeparator();
  viewLineNumbers->addTo(viewMenu);
  viewWhitespaces->addTo(viewMenu);
  viewMargin->addTo(viewMenu);
  viewFoldMargin->addTo(viewMenu);
  viewMenu->insertSeparator();
  viewZoomIn->addTo(viewMenu);
  viewZoomOut->addTo(viewMenu);
  // seems to be a bug in QScintilla
  viewUseMonospacedFont->addTo(viewMenu);

  viewMenu->setItemChecked(ID_VIEW_TOOLBAR, true);
  viewMenu->setItemChecked(ID_VIEW_STATUSBAR, true);
  viewMenu->setItemChecked(ID_VIEW_TASKBAR, true);

  ///////////////////////////////////////////////////////////////////
  // EDIT YOUR APPLICATION SPECIFIC MENUENTRIES HERE
  QPopupMenu *searchMenu = new QPopupMenu();
  searchFind->addTo(searchMenu);
  searchFindInFiles->addTo(searchMenu);
  searchFindNext->addTo(searchMenu);
  searchFindPrevious->addTo(searchMenu);
  searchFindSelection->addTo(searchMenu);
  searchEnterSelection->addTo(searchMenu);
  searchReplace->addTo(searchMenu);
  searchGoTo->addTo(searchMenu);
  searchMenu->insertSeparator();
  searchFindTag->addTo(searchMenu);
  searchJumpToTag->addTo(searchMenu);
  searchFindNextTag->addTo(searchMenu);
  searchFindPreviousTag->addTo(searchMenu);
  searchMenu->insertSeparator();
  bookmarkManager.createMenu();
  searchMenu->insertItem("Bookmarks", bookmarkManager.bookmarksMenu );
  searchToggleBookmark->addTo(searchMenu);
  searchFindNextBookmark->addTo(searchMenu);
  searchFindPrevBookmark->addTo(searchMenu);
  //searchRenameBookmark->addTo(searchMenu);

  toolsMenu = new QPopupMenu(this);
  toolsCompile->addTo(toolsMenu);
  toolsBuild->addTo(toolsMenu);
  toolsGo->addTo(toolsMenu);
  toolsStop->addTo(toolsMenu);
  toolsMenu->insertSeparator();
  toolsNextMessage->addTo(toolsMenu);
  toolsPrevMessage->addTo(toolsMenu);
  toolsMenu->insertSeparator();

  projectMenu = new QPopupMenu(this);
  projectNew->addTo(projectMenu);
  projectOpen->addTo(projectMenu);
  recentProjectsMenu = new QPopupMenu(this);
  projectMenu->insertItem("Recent &Projects", recentProjectsMenu);
  projectClose->addTo(projectMenu);
  projectMenu->insertSeparator();
  projectOptions->addTo(projectMenu);
  
  tabMenu = new QPopupMenu(this);
  tabMenu->setCheckable(true);
  int id;
  for( int i = 2; i < 9; i=i+2 ){
  	id = tabMenu->insertItem( QString("%1").arg(i));
  	tabMenu->setItemParameter(id, i);
  }
  id = tabMenu->insertItem(tr("other"), 10);
  tabMenu->setItemParameter(id, -1);

  lineFeed = new QPopupMenu(this);
  lineFeed->setCheckable(true);
  unix_line_feed = lineFeed->insertItem("Unix (LF)");
  win_line_feed = lineFeed->insertItem("Windows/DOS (CR+LF)");
  mac_line_feed = lineFeed->insertItem("Macintosh (CR)");
        	
  themes = new QPopupMenu(this);
  themes->setCheckable(true);
  default_style = themes->insertItem( "Default" );
#ifndef QT_NO_STYLE_MOTIF
  motif_style = themes->insertItem( "Motif" );
#endif
#ifndef QT_NO_STYLE_CDE
  cde_style = themes->insertItem( "CDE" );
#endif
#ifndef QT_NO_STYLE_MOTIFPLUS
  motifplus_style = themes->insertItem( "MotifPlus" );
#endif
#ifndef QT_NO_STYLE_SGI
  sgi_style = themes->insertItem( "SGI" );
#endif
#ifndef QT_NO_STYLE_WINDOWS
  windows_style = themes->insertItem( "Windows" );
#endif
#ifndef QT_NO_STYLE_PLATINUM
  platinum_style = themes->insertItem( "Platinum" );
#endif
  QPopupMenu *opt = new QPopupMenu(this);
  opt->insertItem("Tab Size", tabMenu);
  opt->insertItem("End Of Line", lineFeed );
  opt->insertItem("Themes", themes );
  langConfigFileMenu = createLangConfigFileMenu();
  opt->insertItem("Edit Language Configs", langConfigFileMenu);
  optionsAutoIndent->addTo(opt);
  //optionsFont->addTo(opt);
  //optionsSyntaxHighlighting->addTo(opt);
  optionsEditPreConfigFile->addTo(opt);
  optionsEditPostConfigFile->addTo(opt);
  optionsShowCutercFile->addTo(opt);
  optionsQtConfig->addTo(opt);
  opt->insertSeparator();
  optionsPreferences->addTo(opt);
  optionsShortcuts->addTo(opt);

  langMenu = new QPopupMenu(this);
 
  QPopupMenu *extra = new QPopupMenu(this);
  extraRunScript->addTo(extra);
  extraRunCurrentFile->addTo(extra);
  extraRunPyCmd->addTo(extra);
  extraEvalSelection->addTo(extra);
  extra->insertSeparator();
  extraLoadSession->addTo(extra);
  extraSaveSession->addTo(extra);
  extraCloseSession->addTo(extra);
  extra->insertSeparator();
  extraStartMacroRecording->addTo(extra);
  extraStopMacroRecording->addTo(extra);
  extraRunMacro->addTo(extra);
  extraLoadMacro->addTo(extra);
  extraSaveMacro->addTo(extra);
  extra->insertSeparator();
  extraBuildTagsFile->addTo(extra);
  //extra->insertSeparator();
  //extraAddTool->addTo(extra);

  ///////////////////////////////////////////////////////////////////
  // menuBar entry helpMenu
  helpMenu=new QPopupMenu();
  helpHandbook->addTo(helpMenu);
  helpAbout->addTo(helpMenu);
  helpLicense->addTo(helpMenu);
  helpAboutQt->addTo(helpMenu);

  ///////////////////////////////////////////////////////////////////
  // MENUBAR CONFIGURATION
  // set menuBar() the current menuBar 

  menuBar()->insertItem("&File", fileMenu);
  menuBar()->insertItem("&Edit", editMenu);
  menuBar()->insertItem("&View", viewMenu);
  menuBar()->insertItem("&Search", searchMenu);
  menuBar()->insertItem("&Project", projectMenu);
  menuBar()->insertItem("&Tools", toolsMenu);
  menuBar()->insertItem("&Options", opt);
  menuBar()->insertItem("&Language", langMenu);
  menuBar()->insertItem("E&xtra", extra);
  QPopupMenu *winMenu = windowMenu();
  menuBar()->insertItem("&Window", winMenu);
  menuBar()->insertSeparator();
  menuBar()->insertItem("&Help", helpMenu);
  
  ///////////////////////////////////////////////////////////////////
  // CONNECT THE SUBMENU SLOTS WITH SIGNALS

  QObject::connect(themes, SIGNAL( activated(int) ), this, SLOT( switchStyle(int)));
  QObject::connect(lineFeed, SIGNAL( activated(int) ), this, SLOT( switchLineFeed(int)));
  QObject::connect(tabMenu, SIGNAL( activated(int) ), this, SLOT( switchTab(int)));
  QObject::connect(langMenu, SIGNAL( activated(int) ), this, SLOT( switchLang(int)));

  QObject::connect(recentFilesMenu, SIGNAL(activated(int)), SLOT(openRecentFile(int)));
  QObject::connect(recentProjectsMenu, SIGNAL(activated(int)), SLOT(openRecentProject(int)));
  QObject::connect(bookmarkManager.bookmarksMenu, SIGNAL(aboutToShow()), SLOT(slotUpdateBookmarksMenu()));
  QObject::connect(bookmarkManager.bookmarksMenu, SIGNAL( activated(int) ), this, SLOT( slotGotoBookmark(int)));
  QObject::connect(fileMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  QObject::connect(editMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  QObject::connect(viewMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));
  QObject::connect(helpMenu, SIGNAL(highlighted(int)), SLOT(statusCallback(int)));

}

void CUTE::initToolBar()
{
  ///////////////////////////////////////////////////////////////////
  // TOOLBAR
  toolbar = new QToolBar(this);
 
	fileNew->addTo(toolbar);
	fileOpen->addTo(toolbar);
	fileSave->addTo(toolbar);
	fileSaveAll->addTo(toolbar);
	filePrint->addTo(toolbar);
	fileClose->addTo(toolbar);
	toolbar->addSeparator();
	
	editUndo->addTo(toolbar);
	editRedo->addTo(toolbar);
	editCut->addTo(toolbar);
	editCopy->addTo(toolbar);
	editPaste->addTo(toolbar);
	toolbar->addSeparator();

	searchFindInFiles->addTo(toolbar);
	searchFind->addTo(toolbar);
	searchFindNext->addTo(toolbar);
	searchReplace->addTo(toolbar);
	toolbar->addSeparator();
	
	toolsCompile->addTo(toolbar);
	toolsBuild->addTo(toolbar);
	toolsStop->addTo(toolbar);
	extraRunScript->addTo(toolbar);
	extraBuildTagsFile->addTo(toolbar);
	toolbar->addSeparator();
	
	windowsPrevious->addTo(toolbar);
	windowsNext->addTo(toolbar);

	helpAbout->addTo(toolbar);
	helpAboutQt->addTo(toolbar);
	cmdToolbar = new QToolBar(this);
	cmdLine = new QCommandBox(cmdInterpreter, cmdToolbar);
	connect(cmdLine, SIGNAL(commandExecuted()), this, SLOT(setFocus()));
	cmdToolbar->setStretchableWidget(cmdLine);
}

void CUTE::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  //STATUSBAR
  cuteStatusBar = new CuteStatusBar();
  statusBar()->addWidget(cuteStatusBar, 1, TRUE);
  QObject::connect(statusBar(), SIGNAL(messageChanged(const QString&)), cuteStatusBar, SLOT(slotStatusBarMessage(const QString&)));
  statusBar()->message(IDS_STATUS_DEFAULT);
}

void CUTE::initDoc()
{
	///////////////////////////////////////////////////////////////////
	// set an example doc here
  //doc=new CUTEDoc();
}

void CUTE::initView()
{ 
  ////////////////////////////////////////////////////////////////////
  // set an example MDI widget here
  /*view=new CUTEView(this);
  view->resize(600,400);
  addWindow( view);*/
}

void CUTE::closeAllViews()
{
	slotLastChildViewClosed();
	MainWindow::closeAllWindows();
}

void CUTE::slotLastChildViewClosed()
{
	view_actions_updated = false;

	fileReopen->setEnabled(false);
	fileClose->setEnabled(false);
	fileCloseAll->setEnabled(false);
	fileSave->setEnabled(false);
	fileSaveAs->setEnabled(false);
	fileSaveAll->setEnabled(false);
	filePrint->setEnabled(false);
	editGotoMatchingBrace->setEnabled(false);
	editSelectToMatchingBrace->setEnabled(false);
	editUndo->setEnabled(false);
	editRedo->setEnabled(false);
	editCut->setEnabled(false);
	editCopy->setEnabled(false);
	editPaste->setEnabled(false);
	editDelete->setEnabled(false);
	editSelectAll->setEnabled(false);
	editIndent->setEnabled(false);
	editUnindent->setEnabled(false);
	editInsertCommand->setEnabled(false);
	editFilter->setEnabled(false);
	editAutoComplete->setEnabled(false);
	editStreamComment->setEnabled(false);
	editBlockComment->setEnabled(false);
	searchFind->setEnabled(false);
	searchFindNext->setEnabled(false);
	searchFindPrevious->setEnabled(false);
	searchReplace->setEnabled(false);
	searchGoTo->setEnabled(false);
	searchToggleBookmark->setEnabled(false);
	//searchClearAllBookarks->setEnabled(false);
	searchFindNextBookmark->setEnabled(false);
	searchFindPrevBookmark->setEnabled(false);
	searchFindSelection->setEnabled(false);
	searchEnterSelection->setEnabled(false);
	searchFindTag->setEnabled(false);
	searchJumpToTag->setEnabled(false);
	searchFindNextTag->setEnabled(false);
	searchFindPreviousTag->setEnabled(false);
	viewZoomIn->setEnabled(false);
	viewZoomOut->setEnabled(false);
	viewToggleCurrentFold->setEnabled(false);
	viewToggleAllFolds->setEnabled(false);
	toolsCompile->setEnabled(false);
	extraRunCurrentFile->setEnabled(false);
	extraEvalSelection->setEnabled(false);
	extraStartMacroRecording->setEnabled(false);
	extraStopMacroRecording->setEnabled(false);
	extraRunMacro->setEnabled(false);
	extraLoadMacro->setEnabled(false);
	extraSaveMacro->setEnabled(false);
	windowsNext->setEnabled(false);
	windowsPrevious->setEnabled(false);
	
	setCaption("CUTE");
}

void CUTE::updateActions()
{
	CUTEView *view = static_cast<CUTEView*>(activeWindow());

	if(!view_actions_updated){
		view_actions_updated = true;
		fileReopen->setEnabled(true);
		fileClose->setEnabled(true);
		fileCloseAll->setEnabled(true);
		fileSaveAs->setEnabled(true);
		fileSaveAll->setEnabled(true);
		filePrint->setEnabled(true);
		editGotoMatchingBrace->setEnabled(true);
		editSelectToMatchingBrace->setEnabled(true);
		editPaste->setEnabled(true);
		editDelete->setEnabled(true);
		editSelectAll->setEnabled(true);
		editIndent->setEnabled(true);
		editUnindent->setEnabled(true);
		editInsertCommand->setEnabled(true);
		editFilter->setEnabled(true);
		editAutoComplete->setEnabled(true);
		editStreamComment->setEnabled(true);
		editBlockComment->setEnabled(true);
		searchFind->setEnabled(true);
		searchFindNext->setEnabled(true);
		searchFindPrevious->setEnabled(true);
		searchReplace->setEnabled(true);
		searchGoTo->setEnabled(true);
		searchToggleBookmark->setEnabled(true);
		//searchClearAllBookarks->setEnabled(false);
		searchFindNextBookmark->setEnabled(true);
		searchFindPrevBookmark->setEnabled(true);
		searchFindTag->setEnabled(true);
		searchJumpToTag->setEnabled(true);
		searchFindNextTag->setEnabled(true);
		searchFindPreviousTag->setEnabled(true);
		viewZoomIn->setEnabled(true);
		viewZoomOut->setEnabled(true);
		viewToggleCurrentFold->setEnabled(true);
		viewToggleAllFolds->setEnabled(true);
		toolsCompile->setEnabled(true);
		extraRunCurrentFile->setEnabled(true);
		extraEvalSelection->setEnabled(true);
		extraStartMacroRecording->setEnabled(true);
		extraLoadMacro->setEnabled(true);
		windowsNext->setEnabled(true);
		windowsPrevious->setEnabled(true);
	}
	
	editUndo->setEnabled(view->scintilla()->isUndoAvailable());
	editRedo->setEnabled(view->scintilla()->isRedoAvailable());
	fileSave->setEnabled(view->scintilla()->isModified());
//	fileReopen->setEnabled(true);

	bool b =  view->scintilla()->hasSelectedText();
	editCut->setEnabled(b);
	editCopy->setEnabled(b);
	searchFindSelection->setEnabled(b);
	searchEnterSelection->setEnabled(b);
	extraEvalSelection->setEnabled(b);
}

bool CUTE::queryExit()
{
	SaveDlg *dlg = new SaveDlg(this);
	int exit = dlg->exec();

	return (exit==1);
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////


void CUTE::slotFileNew()
{
	statusBar()->message("Creating new file...");

	// !This example framework doesn't control doc and view pointers!
	// but pressing the close button will already delete pView...
	QString numberString;
	numberString.setNum( m_currentNumber++);
	CUTEView* pView = new CUTEView(this, "Document" + numberString, tags);
	pView->setActions(editActions);
	pView->scintilla()->setModified(true);
	addWindow( pView );
	activateWindow(pView);
	connect( pView, SIGNAL(save()), this, SLOT(slotFileSaveAs()) );
	connect(pView, SIGNAL(closed(CUTEView*)), &bookmarkManager, SLOT(viewClosed(CUTEView*)));
	connect(pView, SIGNAL(closed(CUTEView*)),this, SLOT(updateProject(CUTEView*)));
	connect(pView, SIGNAL(setTabCaption(QWidget*, QString)),this, SLOT(setTabCaption(QWidget*, QString)));
    connect(pView, SIGNAL(infoStringChanged(QWidget*)), this, SLOT(slotViewInfoStringChanged(QWidget*)));

	connect( pView->scintilla(), SIGNAL(cursorPositionChanged(int,int)), cuteStatusBar, 
		SLOT(slotStatusBarLineCol(int,int)) );

	connect( pView->scintilla(), SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged()));
	connect( pView->scintilla(), SIGNAL(QSCN_SELCHANGED(bool)), this, SLOT(updateActions()) );
	connect( pView->scintilla(), SIGNAL(SCEN_CHANGE()), this, SLOT(updateActions()) );

	statusBar()->message(IDS_STATUS_DEFAULT);
}

void CUTE::slotFileOpen()
{
	QString fileName;
	static QString lastDir = QDir::currentDirPath();
	QStringList fileNames;

	statusBar()->message("Opening file...");

	if(project){
		if(lastDir.startsWith(project->projectDir()))   // last dir in project
			fileNames = QFileDialog::getOpenFileNames(QString::null, lastDir, this, 
				"Open file(s)", "Select one or more files to open");
		else
			fileNames = QFileDialog::getOpenFileNames(QString::null, project->projectDir(), this, 
				"Open file(s)", "Select one or more files to open");
	}
	else
		fileNames = QFileDialog::getOpenFileNames(QString::null, lastDir, this, 
			"Open file(s)", "Select one or more files to open");

	QStringList::iterator i = fileNames.begin();
	if(fileNames.count()) {
		lastDir = QFileInfo(fileNames[0]).filePath();
		while( i != fileNames.end() ){
			loadFile(*i, true);
			QString message="Loaded document: "+*i;
			statusBar()->message(message, 2000);
			++i;
		}
	}
	else
		statusBar()->message("Opening aborted", 2000);
}

void CUTE::slotFileReopen()
{
	if(!activeWindow())
		return;
	statusBar()->message("Reopen file", 2000);
	static_cast<CUTEView*>(activeWindow())->reloadFile();
}

void CUTE::slotFileChangeWorkingDir()
{
	statusBar()->message("Changing working dir...");
	QString dir = QFileDialog::getExistingDirectory( QDir::currentDirPath(), this );
	if(dir){
		QDir::setCurrent( dir );
		if(project){
			if( QDir(dir) == QDir(project->projectDir()) )
				project->setCurrentDirPath(".");
			else{
				QString rel_dir = QFileInfo(dir).absFilePath().remove(project->projectDir()+QDir::separator());
				project->setCurrentDirPath(rel_dir);
			}
		}
		statusBar()->message("Working dir changed", 2000);
	}
	else
		statusBar()->message("Changing working dir aborted", 2000);

}

void CUTE::slotLoadFile(const QString &fileName)
{
	loadFile( fileName, true );
}

void CUTE::loadFileFromDir(const QString &fileName)
{
	qDebug("loadFileFromDir: "+fileName);
	QFile f(fileName);
	if(project && project->hasFile(f))
		loadProjectFile(fileName);
	else
		loadFile(fileName);
}

bool CUTE::loadFile(const QString &fn, bool recentFile, bool showDialogs)
{
	QString fileName;
	QFileInfo file_info(fn);
	WidgetList list;

	// avoid opening file twice when it is in a symlinked dir
	fileName = file_info.dir().canonicalPath() + QDir::separator() + file_info.fileName();

	// ensure file has not been loaded yet
	list = windows();
	for(QWidget *iter = list.first(); iter; iter = list.next()) {
		if( static_cast<CUTEView*>(iter)->fileName() == 0 )
			continue;
		if( strcmp(static_cast<CUTEView*>(iter)->fileName(), fileName) == 0){
			activateWindow(static_cast<CUTEView*>(iter));
			return true;
		}
	}
	
	if( !file_info.exists() ){
		if( showDialogs )
			QMessageBox::information(this, "CUTE", QString("File %1 does not exist").arg(fileName));
		return false;
	}
	if( !file_info.isReadable() ){
		if( showDialogs )
			QMessageBox::information(this, "CUTE", QString("File %1 is not readable").arg(fileName));
		return false;
	}
	
	CUTEView* pView = new CUTEView(this, fileName, tags);
	connect( pView, SIGNAL(save()), this, SLOT(slotFileSaveAs()));
	connect( pView->scintilla(), SIGNAL(cursorPositionChanged(int,int)), cuteStatusBar, SLOT(slotStatusBarLineCol(int,int)));
	connect(pView, SIGNAL(closed(CUTEView*)), &bookmarkManager, SLOT(viewClosed(CUTEView*)));
	connect(pView, SIGNAL(closed(CUTEView*)),this, SLOT(updateProject(CUTEView*)));
	connect(pView, SIGNAL(setTabCaption(QWidget*, QString)),this, SLOT(setTabCaption(QWidget*, QString)));
    connect(pView, SIGNAL(infoStringChanged(QWidget*)), this, SLOT(slotViewInfoStringChanged(QWidget*)));

	connect( pView->scintilla(), SIGNAL(QSCN_SELCHANGED(bool)), this, SLOT(updateActions()) );
	connect( pView->scintilla(), SIGNAL(SCEN_CHANGE()), this, SLOT(updateActions()) );
	connect( pView->scintilla(), SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged()));

	addWindow( pView);
	pView->setActions(editActions);
	pView->setupLexer(fileName);// choose lexer
	pView->loadFile(fileName);
	activateWindow(pView);		//setup lang menu
	if( recentFile )
		addRecentFile(fileName);
	bookmarkManager.setBookmarks(pView);
	
	if( project )
		addProjectFile(pView);
        
    emit openFileHook(fileName);
	
	return true;
}

void CUTE::slotFileSave()
{
	if(!activeWindow())
		return;
	CUTEView *view = static_cast<CUTEView*>(activeWindow());

	if( !view->fileName() )
		slotFileSaveAs();
	else{
		view->saveFile();
		statusBar()->message("File saved", 2000);
	}
}

void CUTE::slotFileSaveAs()
{
	if(!activeWindow())
		return;
	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	statusBar()->message("Saving file under new filename...");
	QString fn = QFileDialog::getSaveFileName(QDir::currentDirPath(), 0, this);
	if (!fn.isEmpty())
	{
		if( !view->saveFile(fn, true) )		// force saving
			statusBar()->message("Saving aborted", 2000);
		else{
			view->setupLexer(fn);		// choose lexer
			addRecentFile(fn);
			activateWindow(view);
			addProjectFile(view);
			statusBar()->message("File saved", 2000);
			setTabCaption(view, QFileInfo(fn).fileName());
		}
	}
	else
	{
		statusBar()->message("Saving aborted", 2000);
	}
}

void CUTE::addProjectFile( CUTEView *view )
{
	QString fn = view->fileName();
	QFile f(fn);
	if(project && !project->hasFile( f )) {	// ensure file is not a part of project yet
		if( QMessageBox::information(this, "CUTE", "Add file to project", 
			QMessageBox::Yes|QMessageBox::Default, QMessageBox::No) == QMessageBox::Yes) {
			QString fileName = QFileInfo(fn).absFilePath().remove(project->projectDir()+QDir::separator());
			if( !QFileInfo(fileName).isRelative() ){
				QMessageBox::information(this, "CUTE", "File is not in project dir");
				return;
			}
			project->addFile(fileName, 0,0);
			projectView->addFile(fileName);
			updateProject(view, true);
		}
	}
}

void CUTE::slotFileClose()
{
	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	if( !view )
		return;

	statusBar()->message("Closing file...");
	if(view->scintilla()->isModified()){
		switch( QMessageBox::warning(this, "CUTE", "Document modified. Would you like to save it?", "Yes", "No", "Cancel") ){
		  case 0:
			slotFileSave();
			if(view->scintilla()->isModified()){
				QMessageBox::warning(this, "CUTE", "Unable to save file!");
				return;
			}
			break;
		  case 1:
			break;
		  case 2:
			return;
		}
	}
	closeActiveWindow();
	statusBar()->message("File closed", 2000);
}

void CUTE::slotFileCloseAll()
{
	WidgetList list = windows();
	bool modified = false;  // whether one or more files have been modified
	for(QWidget *iter = list.first(); iter; iter = list.next()) {
		if( static_cast<CUTEView*>(iter)->scintilla()->isModified() ){
			modified = true;
			break;
		}
	}
	if( modified ){
		SaveDlg *dlg = new SaveDlg(this, "");
		if(dlg->exec() == QDialog::Rejected) // saves modified files
			return;
	}
	
	for(QWidget *iter = list.first(); iter; iter = list.next())
		closeWindow(iter);
}

void CUTE::slotFileLoadTagsFile()
{
	QString fileName;
	tagFileInfo info;
	
	fileName = QFileDialog::getOpenFileName("tags");
	if( fileName.isEmpty() )
		return;
	tags = tagsOpen(fileName, &info);
	updateViews();
	statusBar()->message("Tags file loaded", 2000);
	if(tags)
		tagsView->readTagsFile(tags);
	if(project){
		qDebug("CUTE::slotFileLoadTagsFile tagsFile = "+tagsFile);
		qDebug("CUTE::slotFileLoadTagsFile fileName = "+fileName);
		if(tagsFile != QString::null)
			project->removeTagsFile(tagsFile);
		project->addTagsFile(fileName);
	}
	tagsFile = fileName;
	fileUnloadTagsFile->setEnabled(true);
}

void CUTE::slotFileUnloadTagsFile()
{
	if(!tags)
		return;
	tagsClose(tags);
	tags = NULL;
	statusBar()->message("Tags file unloaded", 2000);
	if( project )
		project->removeTagsFile( tagsFile );
	tagsView->clear();
	tagsFile = QString::null;
	fileUnloadTagsFile->setEnabled(false);
}

void CUTE::slotFilePrint()
{
	if(!activeWindow())
		return;

	QextScintilla *view = static_cast<CUTEView*>(activeWindow())->scintilla();
	statusBar()->message("Printing...");
	QextScintillaPrinter printer;
	if (printer.setup(this))
	{
		printer.printRange(view);
	};
	
	statusBar()->message("File printed");
}

void CUTE::slotFileQuit()
{ 
	///////////////////////////////////////////////////////////////////
	// exits the Application
	bool modified = false;

	slotProjectClose();
	WidgetList list = windows();
	for(QWidget *iter = list.first(); iter; iter = list.next())
		if(	static_cast<CUTEView*>(iter)->scintilla()->isModified()){
			modified = true;
			break;
		}
	if(!modified){
		if(session)
			saveSession();
		qApp->quit();
		return;
	}
 	SaveDlg *dlg = new SaveDlg(this);
	if(dlg->exec() == QDialog::Accepted ){
		if(session)
			saveSession();
		qApp->quit();
	}
}

void CUTE::slotEditGotoMatchingBrace()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->scintilla()->moveToMatchingBrace();   
}

void CUTE::slotEditSelectToMatchingBrace()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->scintilla()->selectToMatchingBrace();   
}

void CUTE::slotEditCut()
{
	if(!activeWindow())
		return;
	statusBar()->message("Cutting selection...");
	static_cast<CUTEView*>(activeWindow())->scintilla()->cut();   
	statusBar()->message(IDS_STATUS_DEFAULT);
}

void CUTE::slotEditCopy()
{
	if(!activeWindow())
		return;
	statusBar()->message("Copying selection to clipboard...");
	static_cast<CUTEView*>(activeWindow())->scintilla()->copy();   
	statusBar()->message(IDS_STATUS_DEFAULT);
}

void CUTE::slotEditPaste()
{
	if(!activeWindow())
		return;
	statusBar()->message("Inserting clipboard contents...");
	static_cast<CUTEView*>(activeWindow())->scintilla()->paste();
	statusBar()->message(IDS_STATUS_DEFAULT);
}

void CUTE::slotEditIndent()
{
	int startLine, startCol, endLine, endCol;
	int t = 0;

	if(!activeWindow())
		return;
	QextScintilla *view = static_cast<CUTEView*>(activeWindow())->scintilla();

	if(!view->hasSelectedText()){
		int line, col;
		view->getCursorPosition(&line, &col);
		if( view->tabIndents() ){
			view->indent(line);
			if( view->indentationsUseTabs() ){
				view->setCursorPosition(line, col+1);
			}
			else{
				int width = ( view->indentationWidth() == 0 ) ? view->tabWidth() : view->indentationWidth();
				view->setCursorPosition(line, col+width);
			}
		}
		else {
			if( view->indentationsUseTabs() ){
				view->insertAt( "\t", line, col);
				view->setCursorPosition(line, col+1);
			}
			else{
				int width = ( view->indentationWidth() == 0 ) ? view->tabWidth() : view->indentationWidth();
				view->beginUndoAction();
				for( int i = 0; i < width; i++)
					view->insertAt(" ", line, col);
				view->setCursorPosition(line, col+width);
				view->endUndoAction();
			}
		}
	}
	else{
		view->getSelection(&startLine, &startCol, &endLine, &endCol);
		if( endCol == 0 )
			t = 1;
		view->beginUndoAction();
		for(int i = startLine; i <= endLine-t; i++)
			view->indent(i);
		view->endUndoAction();
	}
}

void CUTE::slotEditUnindent()
{
	int startLine, startCol, endLine, endCol;
	int t = 0;
	int indent=0;
	int line_len;
	int diff;

	if(!activeWindow())
		return;
	QextScintilla *view = static_cast<CUTEView*>(activeWindow())->scintilla();

	view->beginUndoAction();
	if(!view->hasSelectedText()){
		int line, col;
		view->getCursorPosition(&line, &col);
		line_len = view->lineLength(line);
		view->unindent(line);
		// calculate new cursor position
		diff = line_len-view->lineLength(line);
		col = col-diff;
		if( col < 0 )
			col = 0;
		view->setCursorPosition(line, col);
	}
	else{
		view->getSelection(&startLine, &startCol, &endLine, &endCol);
		if( endCol == 0 )
			t = 1;
		for(int i = startLine; i <= endLine-t; i++)
			view->unindent(i);
	}
	view->endUndoAction();
}

void CUTE::slotEditSelectAll()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->scintilla()->selectAll();
}

void CUTE::slotEditInsertCommand()
{
	QString cmd;
	if(!activeWindow())
		return;
	QextScintilla *view = static_cast<CUTEView*>(activeWindow())->scintilla();
	cmd = QInputDialog::getText("CUTE", "Enter command");
	if( !cmd.isEmpty() ){
		view->beginUndoAction();
		insertCommand(cmd);
		view->endUndoAction();
	}
}

void CUTE::slotEditFilter()
{
	QString cmd;
	if(!activeWindow())
		return;
	QextScintilla *view = static_cast<CUTEView*>(activeWindow())->scintilla();
	cmd = QInputDialog::getText("CUTE", "Enter command to pipe selected text through");
	if( !cmd.isEmpty() ){
		view->beginUndoAction();
		filter(cmd);
		view->endUndoAction();
	}
}

void CUTE::slotEditBlockComment()
{
	QString blockCommentStart;
	int startLine, startCol, endLine, endCol;
	int t = 0;

	if(!activeWindow())
		return;
	CUTEView *cute_view = static_cast<CUTEView*>(activeWindow());
	QextScintilla *view = cute_view->scintilla();
	blockCommentStart = cute_view->lexer()->blockCommentStart();
	
	if(blockCommentStart.isEmpty())
		return;
	if(!view->hasSelectedText()){
		int line, col;
		view->getCursorPosition(&line, &col);
		if(view->text(line).startsWith(blockCommentStart)){
			view->setSelection(line, 0, line, blockCommentStart.length());
			view->removeSelectedText();
			view->setCursorPosition(line, col-blockCommentStart.length());
		}
		else{
			view->insertAt(blockCommentStart, line, 0);
			view->setCursorPosition(line, col+blockCommentStart.length());
		}
	}
	else{
		view->getSelection(&startLine, &startCol, &endLine, &endCol);
		if(view->text(startLine).startsWith(blockCommentStart)){
			view->beginUndoAction();
			if( endCol == 0 )
				t = 1;
			for(int i = startLine; i <= endLine-t; i++){
				if(!view->text(i).startsWith(blockCommentStart))
					break;
				view->setSelection(i, 0, i, blockCommentStart.length());
				view->removeSelectedText();
			}
			view->endUndoAction();
			view->setSelection(startLine, startCol-blockCommentStart.length(), 
				endLine, endCol-blockCommentStart.length());
		}
		else{
			if( endCol == 0 )
				t = 1;
			view->beginUndoAction();
			for(int i = startLine; i <= endLine-t; i++)
				view->insertAt(blockCommentStart, i, 0);
			view->endUndoAction();
			view->setSelection(startLine, startCol+blockCommentStart.length(),
				endLine, endCol+blockCommentStart.length());
		}
	}
}

void CUTE::slotEditStreamComment()
{
	QString streamCommentStart, streamCommentEnd;
	int startLine, startCol, endLine, endCol;
	
	if(!activeWindow())
		return;
	CUTEView *cute_view = static_cast<CUTEView*>(activeWindow());
	QextScintilla *view = cute_view->scintilla();
	streamCommentStart = cute_view->lexer()->streamCommentStart();
	streamCommentEnd = cute_view->lexer()->streamCommentEnd();

	if(streamCommentStart.isEmpty() && streamCommentEnd.isEmpty())
		return;
	if(view->hasSelectedText()) {
		view->getSelection(&startLine, &startCol, &endLine, &endCol);
		view->beginUndoAction();
		view->insertAt(streamCommentEnd, endLine, endCol);
		view->insertAt(streamCommentStart, startLine, startCol);
		view->endUndoAction();
		view->setCursorPosition(endLine, endCol+streamCommentEnd.length());
	}
}

void CUTE::slotViewToggleCurrentFold()
{
	int line, index;
	
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->scintilla()->getCursorPosition(&line, &index);
	static_cast<CUTEView*>(activeWindow())->scintilla()->foldLine(line);
}

void CUTE::slotViewWrapLines()
{
	if(viewWrapLines->isOn())
		Config::view.setInteger("wrapLines", true);
	else
		Config::view.setInteger("wrapLines", false);
	updateViews();
}

void CUTE::slotViewToggleAllFolds()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->scintilla()->foldAll();
}

void CUTE::slotViewBraceMatch()
{
	if( viewBraceMatch->isOn() )
		Config::view.setInteger("braceMatching", QextScintilla::StrictBraceMatch);
	else
		Config::view.setInteger("braceMatching", QextScintilla::NoBraceMatch);
	updateViews();
}

void CUTE::slotViewToolBar()
{
  statusBar()->message("Toggle toolbar...");
  ///////////////////////////////////////////////////////////////////
  // turn toolbar on or off
  
  if (toolbar->isVisible())
  {
    toolbar->hide();
    viewMenu->setItemChecked(ID_VIEW_TOOLBAR, false);
	Config::view.setInteger("toolbar", false);
  } 
  else
  {
    toolbar->show();
    viewMenu->setItemChecked(ID_VIEW_TOOLBAR, true);
	Config::view.setInteger("toolbar", true);
  };

  statusBar()->message(IDS_STATUS_DEFAULT);
}

void CUTE::slotViewCmdToolBar()
{
  statusBar()->message("Toggle toolbar...");
  ///////////////////////////////////////////////////////////////////
  // turn cmd toolbar on or off
  
  if (cmdToolbar->isVisible())
  {
    cmdToolbar->hide();
    viewCmdToolbar->setOn(false);
	Config::view.setInteger("cmdToolbar", false);
  } 
  else
  {
    cmdToolbar->show();
    viewCmdToolbar->setOn(true);
	Config::view.setInteger("cmdToolbar", true);
  };

  statusBar()->message(IDS_STATUS_DEFAULT);
}

void CUTE::slotViewSearch()
{
	messagePanel->showWidget( grepout );
}

void CUTE::slotViewStatusBar()
{
  statusBar()->message("Toggle statusbar...");
  ///////////////////////////////////////////////////////////////////
  //turn statusbar on or off
  
  if (statusBar()->isVisible())
  {
    statusBar()->hide();
    viewMenu->setItemChecked(ID_VIEW_STATUSBAR, false);
	Config::view.setInteger("statusbar", false);
  }
  else
  {
    statusBar()->show();
    viewMenu->setItemChecked(ID_VIEW_STATUSBAR, true);
	Config::view.setInteger("statusbar", true);
  }
  
  statusBar()->message(IDS_STATUS_DEFAULT);
}

void CUTE::slotViewZoomIn()
{
	if(!activeWindow())
		return;
	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	view->scintilla()->zoomIn();
	view->configure();
}

void CUTE::slotViewZoomOut()
{
	if(!activeWindow())
		return;
	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	view->scintilla()->zoomOut();
	view->configure();
}

void CUTE::slotViewUseMonospacedFont()
{
	Config::LexerManager::setUseMonospacedFont(viewUseMonospacedFont->isOn());
	Config::view.setInteger("useMonospacedFont", viewUseMonospacedFont->isOn());
	WidgetList list = windows();
	for(QWidget *iter = list.first(); iter; iter = list.next()) {
			Config::Lexer *lexer = static_cast<CUTEView*>(iter)->lexer();
			lexer->initScintilla(static_cast<CUTEView*>(iter)->scintilla());
	}
}

void CUTE::slotViewSwitchFocusCmdBox()
{
	cmdLine->clear();
	cmdLine->setFocus();
}

void CUTE::slotViewSwitchFocusDir()
{
	sidePanel->showWidget(dirView);
	dirView->setFocus();
}

void CUTE::slotViewSwitchFocusTagsFile()
{
	sidePanel->showWidget(tagsView);
	tagsView->setFocus();
}

void CUTE::slotViewSwitchFocusPro()
{
	sidePanel->showWidget(projectView);
	projectView->setFocus();
}

void CUTE::slotViewSwitchFocusMessages()
{
	messagePanel->showWidget(output);
	output->setFocus();
}

void CUTE::slotViewSwitchFocusStdout()
{
	messagePanel->showWidget(stdout);
	stdout->setFocus();
}

void CUTE::slotViewSwitchFocusStderr()
{
	messagePanel->showWidget(stderr);
	stderr->setFocus();
}

void CUTE::slotViewSwitchFocusShell()
{
	messagePanel->showWidget(cuteShell);
	cuteShell->setFocus();
}

void CUTE::slotViewSwitchFocusSearch()
{
	messagePanel->showWidget(grepout);
	grepout->setFocus();
}

void CUTE::slotViewSwitchFocusTags()
{
	messagePanel->showWidget(tagout);
	tagout->setFocus();
}

void CUTE::slotViewSwitchFocusDoc()
{
	if(!activeWindow())
		return;
	activeWindow()->setFocus();
}

void CUTE::slotViewInfoStringChanged(QWidget *view)
{
    if (view == activeWindow()) {
        cuteStatusBar->slotStatusBarInfo(static_cast<CUTEView*>(view)->infoString());
    }
}

void CUTE::slotEditUndo()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->undo();
}

void CUTE::slotEditRedo()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->redo();
}

void CUTE::slotEditDelete()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->sendEditor(QextScintillaBase::SCI_CLEAR);
}

void CUTE::slotSearchFind()
{
	if(!activeWindow())
		return;
	findDlg->show(static_cast<CUTEView*>(activeWindow()));
}

void CUTE::slotSearchFindNext()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->findNext();
}

void CUTE::slotSearchFindPrevious()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->findPrev();
}

void CUTE::slotSearchFindInFiles()
{
	findInFilesDlg->show();
	messagePanel->showWidget(grepout);
}

void CUTE::slotTagContext()
{
	tagEntry entry;
	QString name;
	int line, col;
	QString pattern;
	int options = 0;

	if(!activeWindow())
		return;
	if(!tags)
		slotFileLoadTagsFile();
	if(!tags){
		QMessageBox::warning(this, "CUTE", "No tags file loaded");
		return;
	}

	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	name = view->scintilla()->selectedText();
	if( name.isEmpty() ) {
		QMessageBox::information(this, "CUTE", "No tag selected");
	} else {
		// deletes previous tags
		tagStack.clear();
		view->scintilla()->getCursorPosition(&line, &col);
		tagStack.append(new TagPosition(view->fileName(), ++line));
		if( tagsFind(tags, &entry, name, options) == TagSuccess )
		{
			do
			{
				if(entry.address.lineNumber > 0){
					qWarning("line");
					line = entry.address.lineNumber;
					tagContext(QString(entry.file), line);
					tagStack.append(new TagPosition(entry.file, line));
				} else {
					pattern = entry.address.pattern;
					qWarning("pattern");
					tagContext(QString(entry.file), pattern);
					tagStack.append(new TagPosition(entry.file, pattern));
				}
			} while (tagsFindNext (tags, &entry) == TagSuccess);
			messagePanel->showWidget( tagout );
		} else {
			QMessageBox::information(this, "CUTE", "No tag match found");
		}
		tagStack.first();
	}
}

#define TAG_CONTEXT_LINES 1

void CUTE::tagContext(QString filename, QString pattern)
{
    QFile file(filename);
    QString pat;
    QString lines[(TAG_CONTEXT_LINES*2)+1];
    int line = 0;
    int match = 0;
    int i;

    // change ctags pattern to QRegExp pattern
	for(i = 1; i < pattern.length()-1; i++) {
		if( QString("\[]*+.(){}").contains(pattern[i]) ) {
			pat += "\\" + pattern[i];
		} else {
			pat += pattern[i];
        }
    }

    QRegExp re(pat);

    if ( file.open( IO_ReadOnly ) ) {
        QTextStream stream( &file );
        i = 0;

        while ( !stream.atEnd() && i < (TAG_CONTEXT_LINES*2)+1) {
            if (i == TAG_CONTEXT_LINES && !match) {
                for(int j = 0; j <= TAG_CONTEXT_LINES-1; j++) {
                    lines[j] = lines[j+1];
                }
            }

            lines[i] = stream.readLine(); // line of text excluding '\n'
            line++;
            if (re.search(lines[i], 0) == 0) {
                match = line;
            }

            if (i < TAG_CONTEXT_LINES || match) {
                i++;
            }
        }
        file.close();
    }
    
    if (match) {
        tagOutputAppend(filename, lines, match - TAG_CONTEXT_LINES);
    }
}

void CUTE::tagContext(QString filename, int tagline)
{
    QFile file(filename);
    QString lines[(TAG_CONTEXT_LINES*2)+1];
    int line = 1;
    int match = 0;
    int i;

    if ( file.open( IO_ReadOnly ) ) {
        QTextStream stream( &file );
        i = 0;

        while ( !stream.atEnd() && line < tagline - TAG_CONTEXT_LINES) {
            stream.readLine(); // line of text excluding '\n'
            line++;
        }
        while ( !stream.atEnd() && i < (TAG_CONTEXT_LINES*2)+1) {
            lines[i] = stream.readLine(); // line of text excluding '\n'
            i++;
            match = line;
        }
        file.close();
    }
    
    if (match) {
        tagOutputAppend(filename, lines, match);
    }
}

void CUTE::tagOutputAppend(QString filename, QString *lines, int line)
{
    QString linestr;
    QString relname;
    QextScintilla *view = static_cast<CUTEView*>(activeWindow())->scintilla();
  	int width;
    QString tab;

    // work out a tab replacement
    width = ( view->indentationWidth() == 0 ) ? view->tabWidth() : view->indentationWidth();
    tab.fill(' ', width);

    if (filename.find(QDir::currentDirPath()) == 0) {
        relname = filename.mid(QDir::currentDirPath().length());
        while (relname.find('/') == 0) {
            relname = relname.mid(1);
        }
    } else {
        relname = filename;
    }

    for(int i = 0; i < (TAG_CONTEXT_LINES*2)+1; i++) {
        linestr = linestr.setNum(line++);
        lines[i].replace('\t', tab);
        tagout->append(relname + ":" + linestr + ":    " + lines[i]);
    }
    tagout->append("=========");
}    

void CUTE::slotSearchFindTag()
{
	tagEntry entry;
	QString name;
	int options = 0;
	int line, col;
	QString pattern;
	CUTEView *view;

	if(!activeWindow())
		return;
	if(!tags)
		slotFileLoadTagsFile();
	if(!tags){
		QMessageBox::warning(this, "CUTE", "No tags file loaded");
		return;
	}

	// deletes previous tags
	tagStack.clear();
	// first tag is current position
	view = static_cast<CUTEView*>(activeWindow());
	view->scintilla()->getCursorPosition(&line, &col);
	tagStack.append(new TagPosition(view->fileName(), ++line));
	
	name = static_cast<CUTEView*>(activeWindow())->scintilla()->selectedText();
	if( name.isEmpty() )
		QMessageBox::information(this, "CUTE", "No tag selected");
	else
		if( tagsFind(tags, &entry, name, options) == TagSuccess )
		{
			do
			{
				if(entry.address.lineNumber > 0){
					qWarning("line");
					line = entry.address.lineNumber;
					tagStack.append(new TagPosition(entry.file, line));
				}
				else{
					pattern = entry.address.pattern;
					tagStack.append(new TagPosition(entry.file, pattern));
					qWarning("pattern");
				}
			} while (tagsFindNext (tags, &entry) == TagSuccess);
			tagStack.first();
			slotSearchFindNextTag();
		}
}

void CUTE::findTag(TagPosition tag)
{
	int line, col;

	if(!tags){
		QMessageBox::warning(this, "CUTE", "No tags file loaded");
		return;
	}
	if(tag.line > 0)
		slotJump(tag.file, tag.line, false);
	else
		findTag(tag.file, tag.pattern);
}

void CUTE::findTag(QString file, QString pattern)
{
	int line, col, num;
	bool ok;
	QString str, tmp;

	// if pattern is a number, jump to given line
	num = pattern.toInt(&ok);
	if(ok){
		qWarning("slotJump");
		slotJump(file, num, false);
		return;
	}
	
	// change ctags pattern to scintilla pattern
	qWarning("tags pattern: " + pattern);
	pattern = pattern.left(pattern.length()-1);
	pattern = pattern.mid(1);
	tmp = pattern[0];
	for(int i = 1; i < pattern.length()-1; i++)
		if( QString("\[]^$*+.").contains(pattern[i]) )
			tmp += "\\" + pattern[i];
		else
			tmp += pattern[i];
	tmp += pattern.right(1);
	pattern = tmp;
	qWarning("scintilla pattern: " + pattern);

	slotJump(file, 0, false);
	// slotJump has created a view
	if(!activeWindow()){
		QMessageBox::warning(this, "CUTE", "internal error");
		return;
	}
	static_cast<CUTEView*>(activeWindow())->findFirst(pattern, true, true, false, false, true, 0, 0);
	static_cast<CUTEView*>(activeWindow())->scintilla()->getCursorPosition(&line, &col);
	line++;
	slotJump(file, line, false);
}

void CUTE::slotSearchFindNextTag()
{
	if(!tags){
		QMessageBox::warning(this, "CUTE", "No tags file loaded");
		return;
	}
	if( tagStack.current() != tagStack.getLast() ){
		findTag(*tagStack.next());
		statusBar()->message("Next tag");
	}
	else
		statusBar()->message("Last tag found", 2000);
}

void CUTE::slotSearchFindPreviousTag()
{
	if(!tags){
		QMessageBox::warning(this, "CUTE", "No tags file loaded");
		return;
	}
	if( tagStack.current() != tagStack.getFirst() ){
		findTag(*tagStack.prev());
		statusBar()->message("Previous tag");
	}
	else
		statusBar()->message("Returned to first tag", 2000);
}

void CUTE::slotSearchReplace()
{
	if(!activeWindow())
		return;
	replaceDlg->show(static_cast<CUTEView*>(activeWindow()));
}

void CUTE::slotSearchGoto()
{
	if(!activeWindow())
		return;
	int line = QInputDialog::getInteger("CUTE", "Enter line number:");
	static_cast<CUTEView*>(activeWindow())->sendEditor(QextScintillaBase::SCI_GOTOLINE, line-1);
}

void CUTE::slotSearchToggleBookmark()
{
	int line, index;
	if(!activeWindow())
		return;
	QextScintilla *view = static_cast<CUTEView*>(activeWindow())->scintilla();
	view->getCursorPosition(&line, &index);
	static_cast<CUTEView*>(activeWindow())->toggleBookmark(line);
}

void CUTE::slotSearchNextBookmark()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->findNextBookmark();
}

void CUTE::slotSearchPrevBookmark()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->findPrevBookmark();
}

void CUTE::slotSearchRenameBookmark()
{
	if(!activeWindow())
		return;
}

void CUTE::slotViewFullScreen()
{
	static bool menuBarHidden=false, toolBarHidden=false, commandBoxHidden=false;
	
	if(viewFullscreen->isOn()){
		if(!Config::view.getInteger("fullScreenShowMenuBar") && menuBar()->isVisible() ) {
			menuBar()->hide();
			menuBarHidden = true;
		}
		if(!Config::view.getInteger("fullScreenShowToolBar") && toolbar->isVisible() ) {
			toolbar->hide();
			toolBarHidden = true;
		}
		if(!Config::view.getInteger("fullScreenShowCommandBox") && cmdLine->isVisible() ) {
			cmdLine->hide();
			commandBoxHidden = true;
		}
		showFullScreen();
		viewFullscreen->setOn(true);
	}
	else{
		showNormal();
		if(toolBarHidden){
			toolbar->show();
			toolBarHidden = false;
		}
		if(menuBarHidden){
			menuBar()->show();
			menuBarHidden = false;
		}
		if(commandBoxHidden) {
			cmdLine->show();
			commandBoxHidden = false;
		}
		viewFullscreen->setOn(false);
	}
}

void CUTE::slotViewLineNumbers()
{
	Config::view.setInteger("lineNumbers", viewLineNumbers->isOn());
	updateViews();
}

void CUTE::slotViewWhiteSpaces()
{
	if(viewWhitespaces->isOn())
		Config::view.setInteger("whitespaces", QextScintilla::WsVisible);
	else
		Config::view.setInteger("whitespaces", QextScintilla::WsInvisible);
	updateViews();
}

void CUTE::slotViewMargin()
{
	Config::view.setInteger("margin", viewMargin->isOn());
	updateViews();
}

void CUTE::slotViewFoldMargin()
{
	Config::view.setInteger("foldMargin", viewFoldMargin->isOn());
	updateViews();
}

void CUTE::slotViewShowMessagePanel()
{
	setMessagePanelVisibility(viewShowMessagePanel->isOn());
}

void CUTE::slotViewShowSidePanel()
{
	setSidePanelVisibility(viewShowSidePanel->isOn());
}

void CUTE::setMessagePanelVisibility(bool show)
{
	if( messagePanel->isVisible() == show )
		return;
	if( show )
		messagePanel->show();
	else
		messagePanel->hide();
}

void CUTE::setSidePanelVisibility(bool show)
{
	if( sidePanel->isVisible() == show )
		return;
	if( show )
		sidePanel->show();
	else
		sidePanel->hide();
}

bool CUTE::isMessagePanelVisible()
{
	return messagePanel->isVisible();
}

bool CUTE::isSidePanelVisible()
{
	return sidePanel->isVisible();
}

void CUTE::slotToolsCompile()
{
	if(!activeWindow())
		return;

	setMessagePanelVisibility(true);
	messagePanel->showWidget(output);
	if( project )
		execCmd( project->compileCommand(project->currentConfig()) );
	else {
		QString fileName = static_cast<CUTEView*>(activeWindow())->fileName();
		QFileInfo fi(fileName);
		QString objName = fi.baseName() + ".o";
		execCmd("make " + objName);
	}
	toolsBuild->setEnabled(false);
	toolsCompile->setEnabled(false);
	toolsGo->setEnabled(false);
}

void CUTE::slotToolsBuild()
{
	bool modified = false;
	
	setMessagePanelVisibility(true);
	messagePanel->showWidget(output);
	WidgetList list = windows();
	for(QWidget *iter = list.first(); iter; iter = list.next()){
		static_cast<CUTEView*>(iter)->deleteAllErrors();
		if(	static_cast<CUTEView*>(iter)->scintilla()->isModified())
			modified = true;
	}
	if(modified){
		SaveDlg *dlg = new SaveDlg(this);
		if(dlg->exec() != 1)
			return;
	}
	toolsBuild->setEnabled(false);
	toolsCompile->setEnabled(false);
	toolsGo->setEnabled(false);
	if( project )
		execCmd(project->buildCommand(project->currentConfig()));
	else
		execCmd("make");
}

void CUTE::slotToolsGo()
{
	if(!activeWindow())
		return;

	setMessagePanelVisibility(true);
	messagePanel->showWidget(output);
	toolsBuild->setEnabled(false);
	toolsCompile->setEnabled(false);
	toolsGo->setEnabled(false);
	if(project)
		execCmd(project->goCommand(project->currentConfig()));
	else {
		QString fileName = static_cast<CUTEView*>(activeWindow())->fileName();
		QFileInfo fi(fileName);
		execCmd(":!"+QDir::currentDirPath() + QDir::separator() + fi.baseName());
	}
}

void CUTE::slotToolsKill()
{
	toolsBuild->setEnabled(true);
	emit killProcess();
}

void CUTE::slotToolsNextMessage()
{
	stderr->slotNextMessage();
}

void CUTE::slotToolsPrevMessage()
{
	stderr->slotPrevMessage();
}

void CUTE::loadProjectFile( ProjectFile *file )
{
	QString fileName;

	if( QFileInfo(file->fileName).isRelative() )
		fileName = project->projectDir()+QDir::separator()+file->fileName;
	else
		fileName = file->fileName;

	int count = windowsCount();
	loadFile(fileName);
	if( count == windowsCount() ) // file was already loaded
		return;
	if(activeWindow()){
		static_cast<CUTEView*>(activeWindow())->scintilla()->setCursorPosition(file->line, file->index);
		static_cast<CUTEView*>(activeWindow())->scintilla()->SendScintilla(QextScintilla::SCI_SETZOOM, file->zoom);
		if( !file->lang.isEmpty() )
			switchLang( file->lang );
	}
}

void CUTE::loadProjectFile( QString fileName )
{
	QString fn;
	if( QFileInfo(fileName).isRelative() )
		fn = project->projectDir()+QDir::separator()+fileName;
	else
		fn = fileName;
	QFile qFile(fn);
	ProjectFile *proFile;
	
	proFile = project->file(qFile);
	
	if( proFile )
		loadProjectFile( proFile );
}

void CUTE::slotProjectNew()
{
	int line, index;

	if(project){
		QMessageBox::information(this, "CUTE", "There is a project already loaded");
		return;
	}
	QString file = QFileDialog::getSaveFileName(QDir::homeDirPath(), "*.cute", this);
	if(file.isEmpty())
		return;
	if( QFileInfo(file).extension(false) != "cute" )
		file += ".cute";
	addRecentProject(file, false);
	updateRecentProjectsMenu();
	project = new Project(file);
	projectView->setProjectDir(QFileInfo(file).dirPath(true));
	connect(projectView, SIGNAL(removedFile(QString)), project, SLOT(removeFile(QString)));
	projectClose->setEnabled(true);
	projectOptions->setEnabled(true);
	projectOpen->setEnabled(false);
	projectNew->setEnabled(false);
}

void CUTE::slotProjectOpen(QString file, bool recentProject)
{
	if(project){
		QMessageBox::information(this, "CUTE", "There is a project already loaded");
		return;
	}
	if(file == QString::null)
		file = QFileDialog::getOpenFileName(QDir::homeDirPath(), "*.cute", this);
	if(file.isEmpty())
		return;
	project = new Project();
	project->open(file);
	projectDir = QFileInfo(file).dir();
	projectView->setProjectDir(projectDir.canonicalPath());
	// setup environment variables and current dir
	setupEnvironment();
	//QDir::setCurrent(projectDir.absPath());
	// load files
	QPtrList<ProjectFile> files = project->files();
	for(ProjectFile *f = files.first(); f; f = files.next() ){
		projectView->addFile(f->fileName);
	}
	// load bookmarks
	QPtrList<ProjectBookmark> bms = project->bookmarks();
	for(ProjectBookmark *bm = bms.first(); bm; bm = bms.next()){
		bookmarkManager.addBookmark(bm);
	}
	// load files into editor
	QPtrList<ProjectFile> ldFiles = project->loadedFiles(true);
	for(ProjectFile *f = ldFiles.first(); f; f = ldFiles.next() ){
		loadProjectFile(f);
	}
	// load tags file
	QStringList tagsFiles = project->tagsFiles();
	if( tagsFiles.count() )
		loadTags( projectDir.absPath()+QDir::separator()+tagsFiles[0] );  // only one tags file is supported at the moment
	// set current file
	if( project->currentFile() )
		loadFile( project->projectDir()+QDir::separator()+project->currentFile() );

	if( recentProject ){
		addRecentProject(file);
		updateRecentFilesMenu();
	}
	connect(projectView, SIGNAL(removedFile(QString)), project, SLOT(removeFile(QString)));
	projectClose->setEnabled(true);
	projectOptions->setEnabled(true);
	projectOpen->setEnabled(false);
	projectNew->setEnabled(false);
}

void CUTE::slotProjectClose(bool closeFiles)
{
	int line, index;

	if(project){
		project->removeBookmarks();
		// save bookmarks
		QPtrList<Bookmark> bms = bookmarkManager.bookmarks();
		for( Bookmark *bm = bms.first(); bm; bm = bms.next() ){
			QFile f(bm->file());
			if(project->hasFile( f )){
				project->addBookmark(bm);
				bookmarkManager.removeBookmark(bm);
			}
		}
		// save zoom levels and cursor positions of files
		WidgetList list = windows();
		for(QWidget *iter = list.first(); iter; iter = list.next()) {
			CUTEView *view = static_cast<CUTEView*>(iter);
			updateProject(view, true);
		}
		// clear tags docklet when project has a tags file
		if( project->tagsFiles().count() && tags){
			tagsClose(tags);
			tags = NULL;
			statusBar()->message("Tags file unloaded", 2000);
			tagsView->clear();
			tagsFile = QString::null;
			fileUnloadTagsFile->setEnabled(false);
		}
	
		// set current file in project
		if( activeWindow() ){
			QString currentFile = static_cast<CUTEView*>(activeWindow())->fileName();
			QFile file(currentFile);
			if(project->hasFile(file))
				project->setCurrentFile( currentFile );
			else
				project->setCurrentFile( QString::null );
		}
		else
			project->setCurrentFile( QString::null );

		if(!project->save())
			QMessageBox::warning(this, "CUTE", "Can not save project");
		else{
			WidgetList list = windows();
			if( closeFiles ) {
				bool modified = false;  // whether one or more files have been modified
				for(QWidget *iter = list.first(); iter; iter = list.next()) {
					QFile f(static_cast<CUTEView*>(iter)->fileName());
					if( project->hasFile(f) )
						if( static_cast<CUTEView*>(iter)->scintilla()->isModified() ){
							modified = true;
							break;
						}
				}
				if( modified ){
					SaveDlg *dlg = new SaveDlg(this, "", true);
					if(dlg->exec() == QDialog::Rejected) 
						return;
				}
			}
			
			projectView->clear();
			// removes all scintilla bookmarks, which were in the project before
			for(QWidget *iter = list.first(); iter; iter = list.next()) {
				QFile f(static_cast<CUTEView*>(iter)->fileName());
				if( project->hasFile(f) ){
					static_cast<CUTEView*>(iter)->removeAllBookmarks();
					if( closeFiles )
						closeWindow(iter);  // no bookmarks are inserted into menu
				}
			}
			
			delete project;
			project = 0;
		}
	}
	projectClose->setEnabled(false);
	projectOptions->setEnabled(false);
	projectOpen->setEnabled(true);
	projectNew->setEnabled(true);
}

void CUTE::slotProjectOptions()
{
	if(!project)
		return;
	
	QString oldConfig = project->currentConfig();
	projectDlg->show(project);
	setupEnvironment(oldConfig);
}

void CUTE::slotOptSettings()
{
	settingsDlg->show();
}

void CUTE::slotOptShortcuts()
{
	shortcutsDlg->show();
}

void CUTE::slotOptAutoIndent()
{
	Config::edit.setInteger("autoIndent", optionsAutoIndent->isOn());
	updateViews();
}
void CUTE::slotOptEditPreConfigFile()
{
	if(loadFile(QDir::homeDirPath()+"/cute.pre_config", false, false)) {
		Config::Lexer *lexer = Config::LexerManager::lexer("file.py");
		lexer->initScintilla(static_cast<CUTEView*>(activeWindow())->scintilla());
	}
	else{
		switch( QMessageBox::information(this, 
			"CUTE", QDir::homeDirPath()+"/cute.pre_config does not exist. create it?",
			QMessageBox::Yes, QMessageBox::No) )
		{
			case QMessageBox::Yes:
			{
				QFile file(QDir::homeDirPath()+"/cute.pre_config");
				file.open(IO_WriteOnly);
				file.close();
				slotOptEditPreConfigFile();
				break;
			}
			case QMessageBox::No:
				break;
			default:
				qDebug("internal error at line %d", __LINE__);
		}
	}
}

void CUTE::slotOptEditPostConfigFile()
{
	if(loadFile(QDir::homeDirPath()+"/cute.post_config", false, false)) {
		Config::Lexer *lexer = Config::LexerManager::lexer("file.py");
		lexer->initScintilla(static_cast<CUTEView*>(activeWindow())->scintilla());
	}
	else{
		switch( QMessageBox::information(this, 
			"CUTE", QDir::homeDirPath()+"/cute.post_config does not exist. create it?",
			QMessageBox::Yes, QMessageBox::No) )
		{
			case QMessageBox::Yes:
			{
				QFile file(QDir::homeDirPath()+"/cute.post_config");
				file.open(IO_WriteOnly);
				file.close();
				slotOptEditPostConfigFile();
				break;
			}
			case QMessageBox::No:
				break;
			default:
				qDebug("internal error at line %d", __LINE__);
		}
	}
}

void CUTE::slotOptQtConfig()
{
	execCmd("qtconfig");
}

void CUTE::slotOptShowCutercFile()
{
	if(loadFile(QDir::homeDirPath()+"/.cuterc")){
		Config::Lexer *lexer = Config::LexerManager::lexer("file.py");
		lexer->initScintilla(static_cast<CUTEView*>(activeWindow())->scintilla());
	}
}

void CUTE::slotExtraRunScript()
{
	QString src;
	
	QString current = QDir::currentDirPath();
	QString file = QFileDialog::getOpenFileName(QDir::homeDirPath()+"/.cute/scripts/", "*.py", this);
	QDir::setCurrent(current);
	
	if( file.isEmpty() )
		return;
	src = "execfile(\"" + file + "\")\n";
	char *c_str = new char[src.length()+1];
	strcpy( c_str, src.latin1() );

	PyRun_SimpleString( c_str );
	delete[] c_str;
}

void CUTE::slotExtraRunCurrentFile()
{
	if(!activeWindow())
		return;

	slotFileSave();
	QString file = static_cast<CUTEView*>(activeWindow())->fileName();
	if( file.isEmpty() ){
		QMessageBox::information( this, "CUTE", "Can not open file" );
		return;
	}
	QString src = "execfile(\"" + file + "\")\n";
	char *c_str = new char[src.length()+1];
	strcpy( c_str, src.latin1() );
	PyRun_SimpleString( c_str );

	delete[] c_str;
}

void CUTE::slotExtraRunPythonString()
{
	QString src = QInputDialog::getText( "CUTE", "Enter python string");
	if(src.isEmpty())
		return;
	src += "\n";
	char *c_str = new char[src.length()+1];
	strcpy( c_str, src.latin1() );
	PyRun_SimpleString( c_str );
	delete[] c_str;
}

void CUTE::slotExtraEvalSelection()
{
	if(!activeWindow())
		return;
	QString src = static_cast<CUTEView*>(activeWindow())->scintilla()->selectedText();
	if(src == QString::null)
		return;
	src += "\n";
	char *c_str = new char[src.length()+1];
	strcpy( c_str, src.latin1() );
	
	if(PyRun_SimpleString( c_str ) == -1)
		QMessageBox::warning(this, "CUTE", "An exception was raised");
	delete[] c_str;
}

void CUTE::slotExtraBuildTagsFile()
{
	QString args;

	if(proc)
		return;
	if(tagsDlg->exec() == QDialog::Rejected)
		return;

	extraBuildTagsFile->setEnabled(false);
	proc = new QProcess(QString("/bin/sh"), this);

	proc->addArgument("-c");
	args += "ctags --fields=+Kaiz ";
	if(tagsDlg->recursive())
		args += " --recurse ";
	args += tagsDlg->dir()+QDir::separator()+"*";
	proc->addArgument(args);
	connect(proc, SIGNAL(processExited()), this, SLOT(loadTags()));
	proc->start();
}

void CUTE::loadTags(QString file)
{
	tagFileInfo info;
	
	delete proc;
	proc = 0;
	if(tags){
		tagsClose(tags);
		tags = 0;
	}
	if( file == QString::null )
		file = QDir::currentDirPath()+QDir::separator()+"tags";
	if( !QFileInfo(file).exists() ){
		QMessageBox::information(this, "CUTE", "No tags file found");
		if(project)
			project->removeTagsFile(file);
		fileUnloadTagsFile->setEnabled(false);
		return;
	}
	tags = tagsOpen(file, &info);
	updateViews();
	if(tags)
		tagsView->readTagsFile(tags);
	extraBuildTagsFile->setEnabled(true);
	if( project ){
		if( tagsFile != QString::null )
			project->removeTagsFile(tagsFile);
		project->addTagsFile(file);
	}
	tagsFile = file;
	fileUnloadTagsFile->setEnabled(true);
}

void CUTE::saveSession()
{
	int line, index;
	qDebug("save Session: %s", session);
	QFile file(session);
	file.open(IO_WriteOnly);
	QTextStream str(&file);
	
	str << "#This is an automatically generated session script for CUTE.\n"
			"#Do not modify UNLESS YOU know what you are doing.\n\n";
	// save current dir, can be overwriten by project
	str << "setCurrentDir(\"" << QDir::currentDirPath() << "\")\n\n";
	// save bookmarks, not in project 
	str << "\n#bookmarks\n";
	QPtrList<Bookmark> bms = bookmarkManager.bookmarks();
	for(Bookmark *bm = bms.first(); bm; bm = bms.next()){
		QFile f( bm->file() );
		if( project && project->hasFile(f) )
			continue;
		else
			str << "addBookmark(\"" << bm->file() << "\"," << bm->line() << ")\n";
	}
	// save loaded files, not in project
	WidgetList list = windows();
	for(QWidget *iter = list.first(); iter; iter = list.next()){
		QFile f(static_cast<CUTEView*>(iter)->fileName());
		QFileInfo fi( f );
		if( project && project->hasFile(f) )
			continue;
		static_cast<CUTEView*>(iter)->scintilla()->getCursorPosition(&line, &index);
		str << "load(\"" << fi.absFilePath() << "\")\n";
		str << "setCursorPosition(" << line << ", " << index << ")\n";
	}
	slotUpdateBookmarksMenu();
	// save loaded project
	if(project){
		str << "\n#loaded project\n";
		str << "loadProject(\"" << project->name() << "\")\n";
	}
	file.close();
}

void CUTE::slotExtraSaveSession()
{
	QString sessionName;
	
	if(!session)
		sessionName = QFileDialog::getSaveFileName(QDir::homeDirPath()+"/.cute/sessions/", "*", this).latin1();
	else
		sessionName = session;
	if(!sessionName.isEmpty()){
		setSession(sessionName);
		saveSession();
	}
}

void CUTE::loadSession(QString file)
{
	QString src;
	
	if( file.isEmpty() )
		return;
	src = "execfile(\"" + file + "\")\n";
	char *c_str = new char[src.length()+1];
	strcpy( c_str, src.latin1() );

	PyRun_SimpleString( c_str );
	delete[] c_str;
	setSession(file.latin1());
}

void CUTE::slotExtraLoadSession()
{
	if(session){
		QMessageBox::warning(this, "CUTE", "There is a session already loaded");
		return;
	}
	QString dir = QDir::currentDirPath();
	QString file = QFileDialog::getOpenFileName(QDir::homeDirPath()+"/.cute/sessions/", "*", this);
	QDir::setCurrent(dir);
	loadSession(file);
	extraLoadSession->setEnabled(false);
	extraSaveSession->setEnabled(true);
	extraCloseSession->setEnabled(true);
}

void CUTE::slotExtraCloseSession()
{
	if(!session){
		QMessageBox::information(this, "CUTE", "There is no session loaded");
		return;
	}
	saveSession();
	slotProjectClose(false);
	// remove all bookmarks
	QPtrList<QWidget> list = windows();
	for( QWidget *w = list.first(); w; w = list.next() )
		static_cast<CUTEView*>(w)->removeAllBookmarks();

	slotFileCloseAll();
	session = 0;
	extraLoadSession->setEnabled(true);
	extraCloseSession->setEnabled(false);
}

void CUTE::slotExtraStartMacroRecording()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->startMacroRecording();
	extraStartMacroRecording->setEnabled(false);
	extraStopMacroRecording->setEnabled(true);
	extraRunMacro->setEnabled(false);
	extraLoadMacro->setEnabled(false);
	extraSaveMacro->setEnabled(false);
}

void CUTE::slotExtraStopMacroRecording()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->stopMacroRecording();
	extraStartMacroRecording->setEnabled(true);
	extraStopMacroRecording->setEnabled(false);
	extraRunMacro->setEnabled(true);
	extraLoadMacro->setEnabled(true);
	extraSaveMacro->setEnabled(true);
}

void CUTE::slotExtraRunMacro()
{
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->runMacro();
}

void CUTE::slotExtraLoadMacro()
{
	if(!activeWindow())
		return;
	QString file = QFileDialog::getOpenFileName(QDir::homeDirPath()+"/.cute/macros/", "*.macro", this);
	if(file){
		static_cast<CUTEView*>(activeWindow())->loadMacro(file);
		extraStartMacroRecording->setEnabled(true);
		extraStopMacroRecording->setEnabled(false);
		extraRunMacro->setEnabled(true);
		extraLoadMacro->setEnabled(true);
		extraSaveMacro->setEnabled(true);
	}
}

void CUTE::slotExtraSaveMacro()
{
	if(!activeWindow())
		return;
	
	QString file = QFileDialog::getSaveFileName(QDir::homeDirPath()+"/.cute/macros/", "*.macro", this);
	if(file)
		static_cast<CUTEView*>(activeWindow())->saveMacro(file);
}

void CUTE::slotWindowsDialog()
{
	WindowDlg *dlg = new WindowDlg(this);
	dlg->show();
}

void CUTE::slotHelpHandbook()
{
	helpViewer = new HelpViewer(Config::cute.getString("docPath"), ".", 0, "");
	helpViewer->show();
}

void CUTE::slotHelpAbout()
{
	AboutDlg *dlg = new AboutDlg(this);
	dlg->show();
}

void CUTE::slotHelpLicense()
{
	LicenseDlg *dlg = new LicenseDlg(this);
	dlg->show();
}

void CUTE::slotHelpAboutQt()
{
  QMessageBox::aboutQt(this);
}

void CUTE::slotStatusHelpMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message of whole statusbar temporary (text, msec)
  statusBar()->message(text, 2000);
}

void CUTE::statusCallback(int id_)
{
  switch (id_)
  {
    case ID_VIEW_TOOLBAR:
         slotStatusHelpMsg("Enables/disables the toolbar");
         break;

    case ID_VIEW_STATUSBAR:
         slotStatusHelpMsg("Enables/disables the statusbar");
         break;

    case ID_VIEW_TASKBAR:
         slotStatusHelpMsg( "Enables / disables the taskbar");
         break;
  }
}

void CUTE::switchStyle(int style)
{
	if( style == default_style ){
		QMessageBox::information(this, "CUTE", "Style will change when you restart CUTE");
		Config::cute.setInteger("theme", Config::Default);
	}
#ifndef QT_NO_STYLE_MOTIF
	else if( style == motif_style ){
		qApp->setStyle( new QMotifStyle() );
		Config::cute.setInteger("theme", Config::Motif);
	}
#endif
#ifndef QT_NO_STYLE_CDE
	else if( style == cde_style ){
		qApp->setStyle( new QCDEStyle() );
		Config::cute.setInteger("theme", Config::CDE);
	}
#endif
#ifndef QT_NO_STYLE_MOTIFPLUS
	else if( style == motifplus_style ){
		qApp->setStyle( new QMotifPlusStyle() );
		Config::cute.setInteger("theme", Config::MotifPlus);
	}
#endif
#ifndef QT_NO_STYLE_SGI
	else if( style == sgi_style ){
		qApp->setStyle( new QSGIStyle() );
		Config::cute.setInteger("theme", Config::SGI);
	}
#endif
#ifndef QT_NO_STYLE_WINDOWS
	else if( style == windows_style ){
		qApp->setStyle( new QWindowsStyle() );
		Config::cute.setInteger("theme", Config::Windows);
	}
#endif
#ifndef QT_NO_STYLE_PLATINUM
	else if( style == platinum_style ){
		qApp->setStyle( new QPlatinumStyle() );
		Config::cute.setInteger("theme", Config::Platinum);
	}
#endif
	
	for( int i = 0; i < themes->count(); i++ ){
		themes->setItemChecked( themes->idAt(i), false );
	}
	themes->setItemChecked( style, true );
}

void CUTE::showEvent( QShowEvent *e )
{
	static bool configured=false;
	
	if(activeWindow())
		setCaption( "CUTE: " + static_cast<CUTEView*>(activeWindow())->caption() );
	if(!configured){
		updateViews();
		updateRecentFilesMenu();
		configure();
		configured=true;
	}
	
	MainWindow::showEvent(e);
}

void CUTE::closeEvent( QCloseEvent *e )
{
	slotFileQuit();
}

void CUTE::switchTab( int item )
{
	bool ok;
	int tabw;

	if(!activeWindow())
		return;
	tabw = tabMenu->text(item).toInt(&ok);
	if(ok)
		tabMenu->setItemChecked(item, true);
	else
		tabw = QInputDialog::getInteger("CUTE", "Enter Tab Width:" );
	Config::edit.setInteger("tabWidth", tabw);
	updateViews();
	for( int i = 0; i < tabMenu->count(); i++ ){
		if( tabMenu->text( tabMenu->idAt(i) ).toInt() == tabw )
			tabMenu->setItemChecked( tabMenu->idAt(i), true );
		else
			tabMenu->setItemChecked( tabMenu->idAt(i), false );
	}
}

void CUTE::switchLineFeed( int item )
{
    if( item == unix_line_feed)
		Config::edit.setInteger("lineFeed", QextScintilla::EolUnix);
	else if( item == win_line_feed)
		Config::edit.setInteger("lineFeed", QextScintilla::EolWindows);
	else if( item == mac_line_feed)
		Config::edit.setInteger("lineFeed", QextScintilla::EolMac);
	
	for( int i = 0; i < lineFeed->count(); i++ ){
		lineFeed->setItemChecked( lineFeed->idAt(i), false );
	}
	lineFeed->setItemChecked(item, true);
	updateViews();
}

void  CUTE::switchLang( int item )
{
	if(!activeWindow())
		return;
	
	const char *name = langMenu->text(item).latin1();
	switchLang( name );
	for( int i = 0; i < langMenu->count(); i++ ){
		langMenu->setItemChecked( langMenu->idAt(i), false );
	}
	langMenu->setItemChecked( item, true );
}

void  CUTE::switchLang( QString name )
{
	if(!activeWindow())
		return;
	
	Config::Lexer *lexer = Config::LexerManager::lexerByLang(name);
	if(lexer){
		lexer->initScintilla(static_cast<CUTEView*>(activeWindow())->scintilla());
		static_cast<CUTEView*>(activeWindow())->setLexer(lexer);
	}
	else
		QMessageBox::information(this, "CUTE", "Lexer not configured");
}

/** Update all windows in order to apply config changes */
void CUTE::updateViews()
{
	WidgetList list = windows();
	for(QWidget *iter = list.first(); iter; iter = list.next()) {
			static_cast<CUTEView*>(iter)->configure();
			static_cast<CUTEView*>(iter)->scintilla()->setTagsFile(tags);
	}
}

/** Saves all loaded documents */
void CUTE::slotFileSaveAll()
{
	WidgetList list = windows();
	for(QWidget *iter = list.first(); iter; iter = list.next())
		static_cast<CUTEView*>(iter)->saveFile();
	fileSave->setEnabled(false);
}

void CUTE::execCmd(QString cmd)
{
	output->clear();
	stdout->clear();
	stderr->clear();

	setMessagePanelVisibility(true);
	messagePanel->showWidget(output);
	toolsStop->setEnabled(true);

	cmdInterpreter->exec(cmd);
}

void CUTE::evalCmdParameter()
{
	for(int i = 1; i < qApp->argc(); i++ ){
		QFileInfo file_info(qApp->argv()[i]);
		if( !file_info.exists() ){
			QFile file(qApp->argv()[i]);
			file.open(IO_WriteOnly);
			file.close();
		}
		QDir dir = QDir::current();
		QString fileName = dir.filePath(qApp->argv()[i]);
		loadFile(fileName, true);
	}
}

void CUTE::activateWindow(QWidget *pWnd)
{
    CUTEView *pView = static_cast<CUTEView*>(pWnd);
    
    // get information string from view and update status bar
	QString info = pView->infoString();
    cuteStatusBar->slotStatusBarInfo(info);

	if( !(windowsCount() == 0) ){
		updateCaption();
	}
	MainWindow::activateWindow(pWnd);
	Config::Lexer *lexer = pView->lexer();
	int id;

	for(int i = 0; i < langMenu->count(); i++)
		if( !strcmp(langMenu->text(langMenu->idAt(i)), lexer->langName()) ){
			id = langMenu->idAt(i);
			break;
		}

	for(int i = 0; i < langMenu->count(); i++)
		langMenu->setItemChecked(langMenu->idAt(i), false);
	langMenu->setItemChecked(id, true);
	updateActions();
}

void CUTE::slotUpdateBookmarksMenu()
{
	bookmarkManager.updateMenu();
}

void CUTE::slotGotoBookmark(int id)
{
	Bookmark *bm = bookmarkManager.menuBookmark(id);

	loadFile(bm->file());
	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	if(view)
		view->sendEditor(QextScintillaBase::SCI_GOTOLINE, bm->line());
}

void CUTE::readyReadGrepStdout()
{
	QString line;
	bool withPath=false;

	if(QDir::current() != QDir(findInFilesDlg->workingDir()))
		withPath = true;
	while(findInFilesDlg->canReadLineStdout()){
		line = findInFilesDlg->readLineStdout();
		line.stripWhiteSpace();
		line.replace(QRegExp("\\s"), " ");
		if(withPath)
			line = findInFilesDlg->workingDir()+QDir::separator()+line;
		grepout->append(line);
	}
}

void CUTE::readStdout()
{
	QString line;

	while(cmdInterpreter->canReadLineStdout()){
		line = cmdInterpreter->readLineStdout();
		line.stripWhiteSpace();
		line.replace(QRegExp("\\s"), " ");
		output->append(line);
		stdout->append(line);
	}
}

void CUTE::readStderr()
{
	QString line;

	while(cmdInterpreter->canReadLineStderr()){
		line = cmdInterpreter->readLineStderr();
		line.stripWhiteSpace();
		line.replace(QRegExp("\\s"), " ");
		output->append(line);
		stderr->append(line);
	}
}

void CUTE::slotProcessExited()
{
	toolsBuild->setEnabled(true);
	if(activeWindow()){
		toolsCompile->setEnabled(true);
		toolsGo->setEnabled(true);
	}
	stderr->setCurrentItem(0);

	toolsStop->setEnabled(false);
}

void CUTE::slotProcessKilled()
{
	toolsBuild->setEnabled(true);
	toolsCompile->setEnabled(true);
	toolsGo->setEnabled(true);
	stderr->setCurrentItem(0);

	toolsStop->setEnabled(false);
}

void CUTE::slotJump(QString fileName, unsigned int lineno, bool mark)
{
	QFileInfo fi(fileName);
	fileName = fi.absFilePath();
	qDebug("file: " + fileName + " num: %u",lineno);
	loadFile(fileName);
	if(!activeWindow())
		return;
	static_cast<CUTEView*>(activeWindow())->sendEditor(QextScintillaBase::SCI_GOTOLINE, lineno-1);
	if(mark)
		static_cast<CUTEView*>(activeWindow())->addError(lineno-1);
	activeWindow()->setFocus();
}

void CUTE::printOutput( std::string str )
{
	output->append(str.c_str());
}

void CUTE::clearOutput()
{
	output->clear();
}

void CUTE::slotClearAllOutputViews()
{
	output->clear();
	stderr->clear();
	stdout->clear();
}

void CUTE::addRecentFile(QString file)
{
	if( !QFileInfo(file).exists() )
		return;
	std::list<std::string>::iterator iter = recentFilesList.begin();
	for(; iter != recentFilesList.end(); ++iter)
		if( *iter == std::string(file.latin1()) ){
			recentFilesList.erase(iter);
			break;
		}
	recentFilesList.push_front(std::string(file.latin1()));
	updateRecentFilesMenu();
}

void CUTE::updateRecentFilesMenu()
{
	// remove list  entries in order to make size of file list equal or less maxRecentFiles
	while( recentFilesList.size() > Config::cute.getInteger("maxRecentFiles") )
		recentFilesList.pop_back();

	recentFilesMenu->clear();
	// insert recent file items in recentFileMenu
	std::list<std::string>::iterator iter = recentFilesList.begin();
	for(; iter != recentFilesList.end(); ++iter)
		recentFilesMenu->insertItem( iter->c_str() );
}

void CUTE::openRecentFile( int item )
{
	QString file = recentFilesMenu->text(item);
	if(!loadFile(file, false, false))
		if( QMessageBox::information(this, "CUTE", "File does not exists. Remove it from menu?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes ){
			recentFilesMenu->removeItem(item);
			recentFilesList.remove(file.latin1());
		}
}

void CUTE::addRecentProject(QString file, bool mustExists)
{
	if( !QFileInfo(file).exists() && mustExists)
		return;
	
	QValueList<QString>::iterator i = recentProjectList.begin();
	for(; i != recentProjectList.end(); ++i)
		if( *i == file ){
			recentProjectList.erase(i);
			break;
		}
	recentProjectList.push_front(file);
	updateRecentProjectsMenu();
}

void CUTE::updateRecentProjectsMenu()
{
	recentProjectsMenu->clear();
	// insert recent project items in recentProjectMenu
	QValueList<QString>::iterator i = recentProjectList.begin();
	for(; i != recentProjectList.end(); ++i)
		recentProjectsMenu->insertItem( *i );
}

void CUTE::openRecentProject( int item )
{
	QString file = recentProjectsMenu->text(item);
	if( !QFileInfo(file).exists() ){
		if( QMessageBox::information(this, "CUTE", "Project does not exists. Remove it from menu?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes ){
			recentProjectsMenu->removeItem(item);
			recentProjectList.remove(file);
		}
	}
	else
		slotProjectOpen(file, false);
}

void CUTE::filter(QString cmd)
{
	if(!activeWindow())
		return;
	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	view->filter(cmd);
}

void CUTE::insertCommand(QString cmd)
{
	if(!activeWindow())
		return;
	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	view->insertCommand(cmd);
}

void CUTE::updateEditActions()
{
	WidgetList list = windows();

	Shortcuts::synchronizeActions();

	if(!activeWindow())
		return;

	for(QWidget *iter = list.first(); iter; iter = list.next()){
		static_cast<CUTEView*>(iter)->setActions(editActions);
	}
}

void CUTE::configure()
{
	viewWrapLines->setOn(Config::view.getInteger("wrapLines"));
	viewStatusbar->setOn(Config::view.getInteger("statusbar"));
	viewCmdToolbar->setOn(Config::view.getInteger("cmdToolbar"));
	viewLineNumbers->setOn(Config::view.getInteger("lineNumbers"));
	viewMargin->setOn(Config::view.getInteger("margin"));
	viewFoldMargin->setOn(Config::view.getInteger("foldMargin"));
	viewBraceMatch->setOn(Config::view.getInteger("braceMatching")!=QextScintilla::NoBraceMatch);
	viewUseMonospacedFont->setOn(Config::view.getInteger("useMonospacedFont"));
	slotViewUseMonospacedFont();
	optionsAutoIndent->setOn(Config::edit.getInteger("autoIndent"));

	if(Config::view.getInteger("whitespaces") != QextScintilla::WsInvisible)
		viewWhitespaces->setOn(true);
	else
		viewWhitespaces->setOn(false);
	if(!Config::view.getInteger("toolbar")){
		toolbar->hide();
		viewMenu->setItemChecked(ID_VIEW_TOOLBAR, false);
	}
	else{
		toolbar->show();
		viewMenu->setItemChecked(ID_VIEW_TOOLBAR, true);
	}
	if(!Config::view.getInteger("cmdToolbar"))
		cmdToolbar->hide();
	if(Config::view.getInteger("statusbar")){
		statusBar()->show();
		viewMenu->setItemChecked(ID_VIEW_STATUSBAR, true);
	}
	else{
		statusBar()->hide();
		viewMenu->setItemChecked(ID_VIEW_STATUSBAR, false);
	}
	
	// configure tab menu
	for(int i = 0; i < tabMenu->count(); i++)
		if(tabMenu->text(tabMenu->idAt(i)).toInt() == Config::edit.getInteger("tabWidth"))
			tabMenu->setItemChecked(tabMenu->idAt(i), true);
		
	// configure line feed menu
	QextScintilla::EolMode eolMode;
	eolMode = (QextScintilla::EolMode)Config::edit.getInteger("lineFeed");
	if(eolMode == QextScintilla::EolUnix )
		lineFeed->setItemChecked(unix_line_feed, true);
	else if(eolMode == QextScintilla::EolWindows )
		lineFeed->setItemChecked(win_line_feed, true);
	else if(eolMode == QextScintilla::EolMac )
		lineFeed->setItemChecked(mac_line_feed, true);
	
	// configure themes menu
	int t = Config::cute.getInteger("theme");
#ifndef QT_NO_STYLE_MOTIF
	if( t == Config::Motif ){
		themes->setItemChecked(motif_style, true);
		qApp->setStyle( new QMotifStyle() );
	}
	else
#endif
#ifndef QT_NO_STYLE_CDE
	if( t == Config::CDE ){
		themes->setItemChecked(cde_style, true);
		qApp->setStyle( new QCDEStyle() );
	}
	else
#endif
#ifndef QT_NO_STYLE_MOTIFPLUS
	if( t == Config::MotifPlus ){
		themes->setItemChecked(motifplus_style, true);
		qApp->setStyle( new QMotifPlusStyle() );
	}
	else
#endif
#ifndef QT_NO_STYLE_SGI
	if( t == Config::SGI ){
		themes->setItemChecked(sgi_style, true);
		qApp->setStyle( new QSGIStyle() );
	}
	else
#endif
#ifndef QT_NO_STYLE_WINDOWS
	if( t == Config::Windows ){
		themes->setItemChecked(windows_style, true);
		qApp->setStyle( new QWindowsStyle() );
	}
	else
#endif
#ifndef QT_NO_STYLE_PLATINUM
	if( t == Config::Platinum ){
		themes->setItemChecked(platinum_style, true);
		qApp->setStyle( new QPlatinumStyle() );
	}
	else
#endif
	if( t == Config::Default ){
		themes->setItemChecked(default_style, true);
	}
	
	viewShowMessagePanel->setOn(Config::cute.getInteger("showMessagePanel"));
	viewShowSidePanel->setOn(Config::cute.getInteger("showSidePanel"));
	
	setSidePanelVisibility(Config::cute.getInteger("showSidePanel"));
	setMessagePanelVisibility(Config::cute.getInteger("showMessagePanel"));
}

void CUTE::addTool(char *name, char *cute_command)
{
	CUTEAction *cute_action;

	cute_action = new CUTEAction(cute_command, this, name);
	cute_action->addTo(toolsMenu);
}

void CUTE::addPythonTool(QAction *action)
{
	action->addTo(toolsMenu);
}

void CUTE::autoComplete()
{
	CUTEView *view = static_cast<CUTEView*>(activeWindow());

	if(!view)
		return;
	
	view->scintilla()->autoComplete();
}

void CUTE::enterSelection()
{
	// Coerce qscintilla to use the currently-selected text as the target of searches.  This is similar to
	// findSelection( ), but does not move the cursor.

	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	if(!view)
		return;

	// Since qscintilla does not support directly setting the search target without advancing the cursor,
	// what we do is search for the selected text starting at the current location.  Theoretically, this
	// should not move the cursor....

	int lineFrom, indexFrom, lineTo, indexTo;

	view->getSelection( lineFrom, indexFrom, lineTo, indexTo);

	if (lineFrom >= 0 && indexFrom >= 0) {
		QString sel = view->scintilla()->selectedText().stripWhiteSpace();
		if(sel && !sel.isEmpty()) {
			// regex = false, caseSensitive = false, wholeWord = false, wrap = false, forward = true
			view->findFirst( sel, false, true, false, false, true, lineFrom, indexFrom );
		}
	}
}

void CUTE::findSelection()
{
	CUTEView *view = static_cast<CUTEView*>(activeWindow());
	if(!view)
		return;

	QString sel = view->scintilla()->selectedText().stripWhiteSpace();
	if(sel)
		view->findFirst( sel );
}

QPopupMenu *CUTE::createLangConfigFileMenu()
{
	QDir dir(QDir::homeDirPath()+QDir::separator()+".cute"+QDir::separator()+"langs");
	QStringList dirList = dir.entryList();
	QPopupMenu *menu = new QPopupMenu;
	
	for( int i = 2; i < dirList.count(); i++ )
		if(QFileInfo(dir.absPath() + QDir::separator() + dirList[i]).isFile())
			menu->insertItem(dirList[i]);

	QObject::connect(menu, SIGNAL( activated(int) ), this, SLOT( slotLangConfigFileMenu(int)));
	return menu;
}

void CUTE::slotLangConfigFileMenu(int id)
{
	QString file = QDir::homeDirPath()+QDir::separator()+".cute"+QDir::separator()+"langs"+
		QDir::separator()+langConfigFileMenu->text(id);
	loadFile(file);
}

void CUTE::updateViewMenu()
{
	viewShowMessagePanel->setOn( isMessagePanelVisible() );
	viewShowSidePanel->setOn( isSidePanelVisible() );
}

void CUTE::setupEnvironment(QString oldConfig)
{
	QStringList env;
	QStringList::iterator i;

	if(!project)
		return;

	// delete old config
	if( !oldConfig.isEmpty() ){
		env = project->envVars(oldConfig);
		i = env.begin();
		for(; i != env.end(); ++i) {
			QStringList var = QStringList::split("=", *i);
			unsetenv( var[0] );
		}
	}
	// setup new config
	env = project->envVars(project->currentConfig());
	i = env.begin();
	for(; i != env.end(); ++i) {
		QStringList var = QStringList::split("=", *i);
		setenv( var[0], var[1], true);
	}
	// restart sub process
	cmdInterpreter->createNewProcess();
	// set working dir
	if( project->currentDirPath() == "." )
		QDir::setCurrent(project->projectDir());
	else
		QDir::setCurrent(project->projectDir()+QDir::separator()+project->currentDirPath());
}

void CUTE::updateProject(CUTEView *view, bool loadFile)
{
	int line, index, zoom;
	QFile f(view->fileName());

	if(project){
		view->scintilla()->getCursorPosition(&line, &index);
		zoom = view->scintilla()->SendScintilla( QextScintillaBase::SCI_GETZOOM );
		QString lang = view->langName();
		ProjectFile pf( QFileInfo(f).absFilePath(), line, index, zoom, loadFile, lang);
		project->modifyFile( f, pf );
	}
}

void CUTE::slotModificationChanged()
{
	updateCaption();
}

void CUTE::updateCaption()
{
	QString fileName;
	CUTEView *view = static_cast<CUTEView*>(activeWindow());

	if(!view)
		return;

	if( view->fileName() )
		fileName = view->fileName();
	else
		fileName = view->caption();
	if( view->scintilla()->isModified() )
		fileName += " *";
	setCaption("CUTE: " + fileName);
}
