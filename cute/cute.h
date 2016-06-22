/***************************************************************************
                          cute.h  -  description
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

#ifndef __CUTE_H__
#define __CUTE_H__

// include files for QT
#include <qfeatures.h>
#include <qstring.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qaccel.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qpixmap.h>
#include <qtoolbutton.h>
#include <qapplication.h>
#include <qstatusbar.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qaction.h>
#include <qregexp.h>
#include <qcombobox.h>
#include <qmap.h>
#include <qtextbrowser.h>
#include <qptrlist.h>
#include <qprocess.h>
#include <qvaluelist.h>

#include <map>
#include <list>
#include <string>

// application specific includes
#include "mdi.h"
#include "resource.h"
#include "readtags.h"
#include "icons/pixmaps.h"
#include "cuteview.h"
#include "python_api.h"
#include "shortcuts.h"

class Project;
class ProjectFile;
class ProjectDlg;
class OutputView;
class GrepOutputView;
class TagOutputView;
class DirectoryView;
class TagsView;
class ProjectView;
class TagsDlg;
class QCommandBox;
class CmdInterpreter;
class SettingsDlg;
class FindDlg;
class ReplaceDlg;
class FindInFilesDlg;
class ShortcutsDlg;
class CUTEScintilla;
class CUTEShell;
class HelpViewer;
class CuteStatusBar;

/** represents a menu item, executing a CUTE command */
class CUTEAction : public QAction
{
	Q_OBJECT
public:
	/** constructs a CUTE action with given name, executing given command */
	CUTEAction(QString command, QObject *parent, const char *name=0);
protected slots:
	/** called when action activated */
	void execCmd();
private:
	/** command to execute */
	QString cmd;
};

extern std::list<std::string> recentFilesList;
extern QValueList<QString> recentProjectList;

/**
  * This Class is the base class for your application. It sets up the main
  * window and providing a menubar, toolbar
  * and statusbar. For the main view, an instance of class CUTE is
  * created which creates your view.
  */
class CUTE : public MainWindow
{
	Q_OBJECT
	struct TagPosition;
	friend class SaveDlg;
	friend class CUTEScintilla;
	friend class CmdInterpreter;
	friend class Shortcuts;
	friend void API::initcute();
	friend PyObject *API::activateView(PyObject*, PyObject*);
	friend PyObject *API::viewList(PyObject*, PyObject*);
public:
	/** construtor */
	CUTE();
	/** destructor */
	~CUTE();
	/** initalizes all lexers */
	void initLangMenu();
	/** initialize all QActions for the tool and menubars */
	void initActions();
	/** initalize all scintilla edit actions */
	void initEditActions();
	/** initMenuBar creates the menu_bar and inserts the menuitems */
	void initMenuBar();
	/** this creates the toolbars. Change the toobar look and add new toolbars in this
	 * function */
	void initToolBar();
	/** setup the statusbar */
	void initStatusBar();
	/** setup the document*/
	void initDoc();
	/** setup the mainview*/
	void initView();
	/** loads a file */
	bool loadFile(const QString&, bool recentFile=false, bool showDialogs=true);
	/** overloaded for Message box on last window exit */
	bool queryExit();
	/** writes string to output view */
	void printOutput(std::string str);
	/** clears output view */
	void clearOutput();
	/** sets the session`s file name */
	void setSession(const char *str) { 
		session = new char[strlen(str)+1]; strcpy(session, str); }
	/** saves all names of loaded files bookmarks etc... */
	void saveSession();
	/** loads all files in session and bookmarks etc... */
	void loadSession(QString file);
	/** filters selection with a shell command */
	void filter(QString cmd);
	/** insert output of a shell command at current cursor position */
	void insertCommand(QString cmd);
	/** append to tag output */
	void tagContext(QString filename, QString pattern);
    void tagContext(QString filename, int tagline);
    void tagOutputAppend(QString filename, QString *lines, int line);
public slots:
	/** creates a new file */
	void newWindow() { slotFileNew(); }
	/** switch argument for Statusbar help entries on slot selection */
	void statusCallback(int id_);
	/** generate a new document in the actual view */
	void slotFileNew();
	/** open a document */
	void slotFileOpen();
	/** reopens a file */
	void slotFileReopen();
	/** change working dir */
	void slotFileChangeWorkingDir();
	/** save a document */
	void slotFileSave();
	/** save a document under a different filename*/
	void slotFileSaveAs();
	/** Saves all loaded documents */
	void slotFileSaveAll();
	/** close all files */
	void slotFileCloseAll();
	/** close the actual file */
	void slotFileClose();
	/** loads tag file */
	void slotFileLoadTagsFile();
	/** unloads tag file */
	void slotFileUnloadTagsFile();
	/** print the actual file */
	void slotFilePrint();
	/** exits the application */
	void slotFileQuit();
	/** jums to the matching brace */
	void slotEditGotoMatchingBrace();
	/** selects to the matching brace */
	void slotEditSelectToMatchingBrace();
	/** undo the last text operation */
	void slotEditUndo();
	/** redo the last undone text operation */
	void slotEditRedo();
	/** put the marked text/object into the clipboard and remove
	* it from the document */
	void slotEditCut();
	/** put the marked text/object into the clipboard*/
	void slotEditCopy();
	/** paste the clipboard into the document*/
	void slotEditPaste();
	/** remove the marked text/object from the document */
	void slotEditDelete();
	/** indents selected text */
	void slotEditIndent();
	/** unindents selected text */
	void slotEditUnindent();
	/** select all */
	void slotEditSelectAll();
	/** insert output of a shell command */
	void slotEditInsertCommand();
	/** filter selected text with a shell command */
	void slotEditFilter();
	/** block un/comment */
	void slotEditBlockComment();
	/** stream un/comment */
	void slotEditStreamComment();
	/** shows the find dialog */
	void slotSearchFind();
	/** redo the last find operation */
	void slotSearchFindNext();
	/** finds the previous matching */
	void slotSearchFindPrevious();
	/** searches a string in each file of a directory */
	void slotSearchFindInFiles();
	/** replaces all matching string */
	void slotSearchReplace();
	/** go to the given line */
	void slotSearchGoto();
	/** finds selected tag(s) */
	void slotSearchFindTag();
	/** view selected tag(s) */
	void slotTagContext();
	/** jumps to next tag position */
	void slotSearchFindNextTag();
	/** jumps back to previous tag position */
	void slotSearchFindPreviousTag();
	/** toggle a bookmark at the current line */
	void slotSearchToggleBookmark();
	/** find next bookmark */
	void slotSearchNextBookmark();
	/** find previous bookmark */
	void slotSearchPrevBookmark();
	/** renames the bookmark at the current line */
	void slotSearchRenameBookmark();
	/** toggle current fold*/
	void slotViewToggleCurrentFold();
	/** toggle all folds*/
	void slotViewToggleAllFolds();
	/**wraps all lines */
	void slotViewWrapLines();
	/** brace match */
	void slotViewBraceMatch();
	/** toggle the toolbar*/
	void slotViewToolBar();
	/** toggle the toolbar*/
	void slotViewCmdToolBar();
	/** shows the search view */
	void slotViewSearch();
	/** toggle the statusbar*/
	void slotViewStatusBar();
	/** toggle full screen mode*/
	void slotViewFullScreen();
	/** toggle line numbers*/
	void slotViewLineNumbers();
	/** toggle white spaces*/
	void slotViewWhiteSpaces();
	/** toggle margin*/
	void slotViewMargin();
	/** toggle fold margin*/
	void slotViewFoldMargin();
	/** zoom in */
	void slotViewZoomIn();
	/** zoom out */
	void slotViewZoomOut();
	/** wether to use monospaced font*/
	void slotViewUseMonospacedFont();
	/** hide/show all bottom docklets */
	void slotViewShowMessagePanel();
	/** hide/show all side docklets */
	void slotViewShowSidePanel();
	void slotViewSwitchFocusCmdBox();
	void slotViewSwitchFocusDir();
	void slotViewSwitchFocusTagsFile();
	void slotViewSwitchFocusPro();
	void slotViewSwitchFocusMessages();
	void slotViewSwitchFocusStdout();
	void slotViewSwitchFocusStderr();
	void slotViewSwitchFocusShell();
	void slotViewSwitchFocusSearch();
	void slotViewSwitchFocusTags();
	void slotViewSwitchFocusDoc();
    /** update status bar information string */
    void slotViewInfoStringChanged(QWidget *view);
	/** compile current file*/
	void slotToolsCompile();
	/** run makefile*/
	void slotToolsBuild();
	/** run the application*/
	void slotToolsGo();
	/** kills the current process */
	void slotToolsKill();
	/** jumps to next message */
	void slotToolsNextMessage();
	/** jumps to previous message */
	void slotToolsPrevMessage();
	/** creates a new project */
	void slotProjectNew();
	/** opens a project file */
	void slotProjectOpen(QString file=QString::null, bool recentProject = true);
	/** closes a project */
	void slotProjectClose(bool closeFiles = true);
	/** shows the project options dialog */
	void slotProjectOptions();
	/** shows settings dialog */
	void slotOptSettings();
	/** show shortcut dialog */
	void slotOptShortcuts();
	/** auto indent */
	void slotOptAutoIndent();
	/** opens the pre config file */
	void slotOptEditPreConfigFile();
	/** opens the post config file */
	void slotOptEditPostConfigFile();
	/** shows read-only ~/.cuterc file */
	void slotOptShowCutercFile();
	/** executes qtconfig */
	void slotOptQtConfig();
	/** runs a CUTE/Python script */
	void slotExtraRunScript();
	/** runs the current file */
	void slotExtraRunCurrentFile();
	/** runs a python command */
	void slotExtraRunPythonString();
	/** runs the selection with built-in python interpreter */
	void slotExtraEvalSelection();
	/** saves the current session in a CUTE/python script */
	void slotExtraSaveSession();
	/**loads a session */
	void slotExtraLoadSession();
	/** unloads all files of  current session */
	void slotExtraCloseSession();
	/**starts macro recording */
	void slotExtraStartMacroRecording();
	/**stops macro recording */
	void slotExtraStopMacroRecording();
	/**runs recorded macro */
	void slotExtraRunMacro();
	/** shows an open dialog  and loads current macro */
	void slotExtraLoadMacro();
	/** shows an save dialog  and saves current macro */
	void slotExtraSaveMacro();
	/** builds ctags file of all sourcefiles in current dir */
	void slotExtraBuildTagsFile();
	/** shows the window Dialog */
	void slotWindowsDialog();
	/** show online documentation */
	void slotHelpHandbook();
	/** shows an about dlg*/
	void slotHelpAbout();
	/** shows the license dlg*/
	void slotHelpLicense();
	/** shows the about Qt dlg*/
	void slotHelpAboutQt();
	/** change the status message of the whole statusbar temporary */
	void slotStatusHelpMsg(const QString &text);
	/** called when item in style menu is activated */
	void switchStyle(int item);
	/** called when item in line feed menu is activated */
	void switchLineFeed(int item);
	/** called when item in tab menu is activated */
	void switchTab(int item);
	/** called when item in language menu is activated */
	void switchLang(int item);
	/** Update all windows in order to apply config changes */
	void updateViews();
	/** executes a command from the command line */
	void execCmd(QString cmd = QString::null);
	/** evaluates the command parameter and maybe loads some f */
	void evalCmdParameter();
	/** changes the status bar */
	void slotUpdateBookmarksMenu();
	/** jumps to selected bookmark */
	void slotGotoBookmark(int id);
	/** add a entry in the recent document menu */
	void addRecentFile(QString file);
	/** add a entry in the recent project menu */
	void addRecentProject(QString file, bool mustExists=true);
	/** sets all configurations */
	void configure();
	/** clears message, stdout and stderr view */
	void slotClearAllOutputViews();
	/** adds item to tools menu */
	void addTool(char *name, char *cute_command);
	/** adds item to tools menu */
	void addPythonTool(QAction *action);
	/** updates all shortcuts in views */
	void updateEditActions();
	/** jumps to given position/pattern in given file */
	void findTag(QString file, QString pattern);
	/** called when all child views are closed */
	void closeAllViews();
protected slots:
	/** called when view is activted */
	void activateWindow(QWidget *pWnd);
	/** there are characters ready to read from stdout */
	void readyReadGrepStdout();
	/** read from stdout of sub process */
	void readStdout();
	/** read from stderr of sub process */
	void readStderr();
	/** called when sub process exited */
	void slotProcessExited();
	/** called when sub process is killed */
	void slotProcessKilled();
	/** jump to given line in given file and marks line when third arg is true*/
	void slotJump(QString fileName, unsigned int lineno, bool mark=true);
	void updateRecentFilesMenu();
	void openRecentFile( int item );
	void updateRecentProjectsMenu();
	void openRecentProject( int item );
	void slotLoadFile(const QString&);
	void slotLastChildViewClosed();
	void autoComplete();
	void findSelection();
	void enterSelection();
	void findTag(TagPosition tag);
	void slotLangConfigFileMenu(int item);
	/** update viewShowMessagePanel and viewShowSidePanel when view menu is about to be shown */
	void updateViewMenu();
	/** en/disable some actions */
	void updateActions();
	/** loads a file from dir docklet*/
	void loadFileFromDir(const QString &);
	/** load tags file and generate tag tree */
	void loadTags( QString file = QString::null );
	/** opens a file of a project */
	void loadProjectFile(ProjectFile*);
	/** same as above, opens a file of a project */
	void loadProjectFile(QString);
	/** sets environments of CUTE and sub processes */
	void setupEnvironment(QString oldConfig=QString::null);
	/** adds a file to project */
	void addProjectFile( CUTEView *view );
	/** switch language of current view to that one given */
	void switchLang(QString name);
	/** update file in project */
	void updateProject(CUTEView *, bool loadFile=false);
	/** called when modification state of current view has changed */
	void slotModificationChanged();
signals:
	void killProcess();
    void openFileHook(const QString filename);
protected:
	/** handle show event */
	void showEvent( QShowEvent *e );
	/** handle close event */
	void closeEvent( QCloseEvent *e );
	/** creates menu with language config file entries */
	QPopupMenu *createLangConfigFileMenu();
	/** show/hide message panel */
	void setMessagePanelVisibility(bool);
	/** show/hide side panel */
	void setSidePanelVisibility(bool);
	/** checks message panel visibility */
	bool isMessagePanelVisible();
	/** checks side panel visibility */
	bool isSidePanelVisible();
	/** sets CUTE's caption */
	void updateCaption();
private:
	/** stores position of tag, needed by tag stack */
	struct TagPosition
	{
		TagPosition(QString f, int l) : file(f), line(l) {}
		TagPosition(QString f, QString p) : file(f), pattern(p), line(-1) {}
		QString file;
		QString pattern;
		int line;
	};
	/** the current project file */
	Project *project;
	/** the project dir */
	QDir projectDir;
	/** file_menu contains all items of the menubar entry "File" */
	QPopupMenu *fileMenu;
	/** edit_menu contains all items of the menubar entry "Edit" */
	QPopupMenu *editMenu;
	/** view_menu contains all items of the menubar entry "View" */
	QPopupMenu *viewMenu;
	/** lang_menu contains all items of the menubar entry "Language" */
	QPopupMenu *langMenu;
	/** view_menu contains all items of the menubar entry "Help" */
	QPopupMenu *helpMenu;
	/** themes_menu contains all themes */
	QPopupMenu *themes;
	/** tabulator menu */
	QPopupMenu *tabMenu;
	/** menu for unix, win or mac line feed */
	QPopupMenu *lineFeed;
	/** menu for project */
	QPopupMenu *projectMenu;
	/** menu for tools and scripts */
	QPopupMenu *toolsMenu;
	/** menu for recent files */
	QPopupMenu *recentFilesMenu;
	/** menu for recent projects */
	QPopupMenu *recentProjectsMenu;
	/** menu for language config files */
	QPopupMenu *langConfigFileMenu;
	
	QToolBar *toolbar;
	QToolBar *cmdToolbar;
	QCommandBox *cmdLine;
	int m_currentNumber;	// view number
	
	QAction *fileNew, *fileOpen, *fileReopen, *fileChangeWorkingDir, *fileClose, *fileCloseAll, *fileSave, *fileSaveAll, 
		*fileSaveAs, *filePrint, *fileLoadTagsFile, *fileUnloadTagsFile, *fileExit;
	QAction *editGotoMatchingBrace, *editSelectToMatchingBrace, *editUndo, *editRedo, *editCut, *editCopy, 
		*editPaste, *editDelete, *editSelectAll, *editIndent, *editUnindent, *editInsertCommand, *editFilter, 
		*editAutoComplete, *editStreamComment, *editBlockComment;
	QAction *searchFind, *searchFindNext, *searchFindPrevious, *searchFindInFiles, *searchReplace,
		*searchGoTo, *searchToggleBookmark, *searchClearAllBookarks, *searchRenameBookmark,
		*searchFindNextBookmark, *searchFindPrevBookmark, *searchFindSelection, *searchEnterSelection,
		*searchFindTag, *searchFindNextTag, *searchFindPreviousTag, *searchJumpToTag;
	QAction *viewFullscreen, *viewToolbar, *viewStatusbar, *viewWhitespaces, *viewEndOfLines,
		*viewLineNumbers, *viewMargin, *viewFoldMargin, *viewZoomIn, *viewZoomOut, *viewToggleCurrentFold,
		*viewToggleAllFolds, *viewWrapLines, *viewBraceMatch, *viewCmdToolbar, *viewShowDirView, 
		*viewShowMessages, *viewShowStdout, *viewShowStderr, *viewShowShell, *viewShowSearch,
		*viewShowMessagePanel, *viewShowSidePanel, *viewShowTags, *viewShowProject;
	QAction *viewSwitchFocusDir, *viewSwitchFocusTagsFile, *viewSwitchFocusPro, *viewSwitchFocusMessages, 
		*viewSwitchFocusStdout, *viewSwitchFocusStderr, *viewSwitchFocusShell, *viewSwitchFocusSearch, 
		*viewSwitchFocusTags, *viewSwitchFocusDoc, *viewSwitchFocusCmdBox, *viewUseMonospacedFont;
	QAction *toolsCompile, *toolsBuild, *toolsGo, *toolsStop, *toolsNextMessage, *toolsPrevMessage;
	QAction *projectNew, *projectOpen, *projectClose, *projectOptions;
	QAction *optionsPreferences, *optionsShortcuts, *optionsEditPreConfigFile, *optionsEditPostConfigFile, 
		*optionsQtConfig, *optionsAutoIndent, *optionsShowCutercFile;
	QAction *extraRunScript, *extraRunCurrentFile, *extraSaveSession, *extraLoadSession, *extraAddTool, 
		*extraRunCmd, *extraRunPyCmd, *extraEvalSelection, *extraCloseSession, *extraStartMacroRecording, 
		*extraStopMacroRecording, *extraRunMacro, *extraLoadMacro, *extraSaveMacro, *extraBuildTagsFile;
	QAction *windowsNext, *windowsPrevious, *windowsTile, *windowsCascade, *windowsShowWindows;
	QAction *helpHandbook, *helpAbout, *helpLicense, *helpAboutQt;

	/** all edit commands of scintilla wrapped as QActions */
	QAction *charLeft, *charRight, *lineUp, *lineDown, *wordPartLeft, *wordPartRight, *wordLeft, 
			*wordRight, *VCHome, *homeDisplay, *lineEnd, *lineScrollDown, *lineScrollUp, *paraUp, 
			*paraDown, *pageUp, *pageDown, *documentStart, *documentEnd, *tab, *backTab, *selectAll,
			*charLeftExtend, *charRightExtend, *lineUpExtend, *lineDownExtend, *wordPartLeftExtend,
			*wordPartRightExtend, *wordLeftExtend, *wordRightExtend, *VCHomeExtend, *homeDisplayExtend,
			*lineEndExtend, *paraUpExtend, *paraDownExtend, *pageUpExtend, *pageDownExtend,
			*documentStartExtend, *documentEndExtend, *deleteBack, *deleteBackNotLine, *clear,
			*delWordLeft, *delWordRight, *delLineLeft, *delLineRight, *newLine, *lineDelete,
			*lineDuplicate, *lineTranspose, *lineCut, *cut, *lineCopy, *copy, *paste, *redo, *undo,
			*cancel, *editToggleOvertype, *lowerCase, *upperCase, *zoomIn, *zoomOut;

#ifndef QT_NO_STYLE_MOTIF
	int motif_style;
#endif
#ifndef QT_NO_STYLE_CDE
	int cde_style;
#endif
#ifndef QT_NO_STYLE_INTERLACE
	int interlace_style;
#endif
#ifndef QT_NO_STYLE_MOTIFPLUS
	int motifplus_style;
#endif
#ifndef QT_NO_STYLE_SGI
	int sgi_style;
#endif
#ifndef QT_NO_STYLE_WINDOWS
	int windows_style;
#endif
#ifndef QT_NO_STYLE_PLATINUM
	int platinum_style;
#endif
	int default_style;
	int metal_style, step_style;
	int unix_line_feed, win_line_feed, mac_line_feed;
	/** project dialog */
	ProjectDlg *projectDlg;
	/** Replace dialog */
	ReplaceDlg *replaceDlg;
	/** Find dialog */
	FindDlg *findDlg;
	/** Grep  dialog */
	FindInFilesDlg *findInFilesDlg;
	/** Settings dialog */
	SettingsDlg *settingsDlg;
	/** Shortcuts dialog */
	ShortcutsDlg *shortcutsDlg;
	/** dock widgets containing dir view, messages ... */
	DockWindow *sidePanel, *messagePanel;
	/** output view for external processes */
	OutputView *stdout, *stderr, *output;
	/** dialog for building tags file */
	TagsDlg *tagsDlg;
	GrepOutputView *grepout;
	TagOutputView *tagout;
	/** cute shell docklet */
	CUTEShell *cuteShell;
	/** tree view in side panel */
	DirectoryView *dirView;
	/** tags view in side panel */
	TagsView *tagsView;
	/** project view in side panel */
	ProjectView *projectView;
	/** online help viewer */
	HelpViewer *helpViewer;
	/** is a session loaded */
	char *session;
	/** the command interpreter which is used for running all command typed in the command box */
	CmdInterpreter *cmdInterpreter;
	/** the tags file */
	tagFile *tags;
	/** name of tags file */
	QString tagsFile;
	/** the tag stack (wich is actually not a stack)*/
	QPtrList<TagPosition> tagStack;
	/** last dir of open dialog */
	QString lastDir;
	/** all edit actions */
	QMap<int, QAction*> editActions;
	/** process for executing ctags */
	QProcess *proc;
	/** wether edit and view actions need to be updated */
	bool view_actions_updated;
    /** status bar widget */
    CuteStatusBar *cuteStatusBar;
};

#endif

