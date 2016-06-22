/***************************************************************************
                          settingsdlg.cpp  -  settings dialog
                             -------------------
    begin                : Mon, 10 Mär 2003
    copyright            : (C) 2003 by Heiko Köhler
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

#include <qcombobox.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qextscintilla.h>

#include "cutescintilla.h"
#include "settingsdlg.h"
#include "py_config.h"

SettingsDlg::SettingsDlg(QWidget *parent, const char* name)
{
	setWFlags(WShowModal);
}

void SettingsDlg::show()
{
	theme->setCurrentItem( Config::cute.getInteger("theme"));
	maxNumbersOfRecentFiles->setValue( Config::cute.getInteger("maxRecentFiles"));
	maxLengthOfSeletedString->setValue( Config::cute.getInteger("maxSelectedStringLengthInDialog"));
	docPath->setText( Config::cute.getString("docPath") );
	
	tabWidth->setValue( Config::edit.getInteger("tabWidth"));
	autoCompletionThreshold->setValue( Config::edit.getInteger("autoCompletionThreshold"));

	if(Config::edit.getInteger("autoCompletionSource") == CUTEScintilla::Document)
		autoCompletionSource->setCurrentItem(0);
	else if(Config::edit.getInteger("autoCompletionSource") == CUTEScintilla::TagsFile)
		autoCompletionSource->setCurrentItem(1);
	else if(Config::edit.getInteger("autoCompletionSource") == CUTEScintilla::None)
		autoCompletionSource->setCurrentItem(2);

	if(Config::edit.getInteger("lineFeed") == QextScintilla::EolMac)
		lineFeeds->setCurrentItem(0);
	else if(Config::edit.getInteger("lineFeed") == QextScintilla::EolUnix)
		lineFeeds->setCurrentItem(1);
	else if(Config::edit.getInteger("lineFeed") == QextScintilla::EolWindows)
		lineFeeds->setCurrentItem(2);

	backspaceUnindents->setChecked( Config::edit.getInteger("backspaceUnindents"));
	indentationsUseTabs->setChecked( Config::edit.getInteger("indentationsUseTabs"));
	autoIndent->setChecked( Config::edit.getInteger("autoIndent"));
	tabIndents->setChecked( Config::edit.getInteger("tabIndents"));
	
	if(Config::view.getInteger("folding") == QextScintilla::NoFoldStyle)
		foldStyle->setCurrentItem(0);
	else if(Config::view.getInteger("folding") == QextScintilla::PlainFoldStyle)
		foldStyle->setCurrentItem(1);
	else if(Config::view.getInteger("folding") == QextScintilla::CircledFoldStyle)
		foldStyle->setCurrentItem(2);
	else if(Config::view.getInteger("folding") == QextScintilla::BoxedFoldStyle)
		foldStyle->setCurrentItem(3);
	else if(Config::view.getInteger("folding") == QextScintilla::CircledTreeFoldStyle)
		foldStyle->setCurrentItem(4);
	else if(Config::view.getInteger("folding") == QextScintilla::BoxedTreeFoldStyle)
		foldStyle->setCurrentItem(5);

	if(Config::view.getInteger("braceMatching") == QextScintilla::NoBraceMatch)
		braceMatching->setCurrentItem(0);
	else if(Config::view.getInteger("braceMatching") == QextScintilla::StrictBraceMatch)
		braceMatching->setCurrentItem(1);
	else if(Config::view.getInteger("braceMatching") == QextScintilla::SloppyBraceMatch)
		braceMatching->setCurrentItem(2);
	
	if(Config::view.getInteger("whitespaces") == QextScintilla::WsInvisible)
		whitespaces->setCurrentItem(0);
	else if(Config::view.getInteger("whitespaces") == QextScintilla::WsVisible)
		whitespaces->setCurrentItem(1);
	else if(Config::view.getInteger("whitespaces") == QextScintilla::WsVisibleAfterIndent)
		whitespaces->setCurrentItem(2);
	
	statusbar->setChecked( Config::view.getInteger("statusbar"));
	toolbar->setChecked( Config::view.getInteger("toolbar"));
	commandToolbar->setChecked( Config::view.getInteger("cmdToolbar"));
	margin->setChecked( Config::view.getInteger("margin"));
	foldMargin->setChecked( Config::view.getInteger("foldMargin"));
	lineNumbers->setChecked( Config::view.getInteger("lineNumbers"));
	endOfLinesVisibility->setChecked( Config::view.getInteger("eolVisibility"));
	indentationGuides->setChecked( Config::view.getInteger("indentationsGuide"));
	toggleAllFolds->setChecked( Config::view.getInteger("toggleAllFolds"));
	wrapLines->setChecked( Config::view.getInteger("wrapLines"));
	zoomLevel->setValue( Config::view.getInteger("zoomLevel"));
	autoCompletionCaseSensitivity->setChecked(Config::edit.getInteger("autoCompletionCaseSensitivity"));
	useUtf8Encoding->setChecked(Config::edit.getInteger("useUtf8Encoding"));
	
	showMessagePanel->setChecked( Config::cute.getInteger("showMessagePanel"));
	showSidePanel->setChecked( Config::cute.getInteger("showSidePanel"));

	fullScreenShowCommandBox->setChecked( Config::view.getInteger("fullScreenShowCommandBox") );
	fullScreenShowMenuBar->setChecked( Config::view.getInteger("fullScreenShowMenuBar") );
	fullScreenShowToolBar->setChecked( Config::view.getInteger("fullScreenShowToolBar") );
	
	SettingsDialog::show();
}

void SettingsDlg::ok()
{
	applySettings();
	hide();
}

void SettingsDlg::applySettings()
{
	Config::cute.setInteger("theme", theme->currentItem());
	Config::cute.setInteger("maxRecentFiles", maxNumbersOfRecentFiles->value());
	Config::cute.setInteger("maxSelectedStringLengthInDialog", maxLengthOfSeletedString->value());
	Config::cute.setString("docPath", docPath->text() );
	
	Config::edit.setInteger("tabWidth", tabWidth->value());
	Config::edit.setInteger("autoCompletionThreshold", autoCompletionThreshold->value());
	Config::edit.setInteger("autoCompletionCaseSensitivity", autoCompletionCaseSensitivity->isChecked());
	Config::edit.setInteger("autoCompletionReplaceWord", autoCompletionReplaceWord->isChecked());

	if(autoCompletionSource->currentItem() == 0)
		Config::edit.setInteger("autoCompletionSource", CUTEScintilla::Document);
	else if(autoCompletionSource->currentItem() == 1)
		Config::edit.setInteger("autoCompletionSource", CUTEScintilla::TagsFile);
	else if(autoCompletionSource->currentItem() == 2)
		Config::edit.setInteger("autoCompletionSource", CUTEScintilla::None);

	if(lineFeeds->currentItem() == 0)
		Config::edit.setInteger("lineFeed", QextScintilla::EolMac);
	else if(lineFeeds->currentItem() == 1)
		Config::edit.setInteger("lineFeed", QextScintilla::EolUnix);
	else if(lineFeeds->currentItem() == 2)
		Config::edit.setInteger("lineFeed", QextScintilla::EolWindows);

	Config::edit.setInteger("backspaceUnindents", backspaceUnindents->isChecked());
	Config::edit.setInteger("indentationsUseTabs", indentationsUseTabs->isChecked());
	Config::edit.setInteger("autoIndent", autoIndent->isChecked());
	Config::edit.setInteger("tabIndents", tabIndents->isChecked());
	Config::edit.setInteger("useUtf8Encoding", useUtf8Encoding->isChecked());

	if(foldStyle->currentItem() == 0)
		Config::view.setInteger("folding", QextScintilla::NoFoldStyle);
	else if(foldStyle->currentItem() == 1)
		Config::view.setInteger("folding", QextScintilla::PlainFoldStyle);
	else if(foldStyle->currentItem() == 2)
		Config::view.setInteger("folding", QextScintilla::CircledFoldStyle);
	else if(foldStyle->currentItem() == 3)
		Config::view.setInteger("folding", QextScintilla::BoxedFoldStyle);
	else if(foldStyle->currentItem() == 4)
		Config::view.setInteger("folding", QextScintilla::CircledTreeFoldStyle);
	else if(foldStyle->currentItem() == 5)
		Config::view.setInteger("folding", QextScintilla::BoxedTreeFoldStyle);

	if( braceMatching->currentItem() == 0 )
		Config::view.setInteger("braceMatching", QextScintilla::NoBraceMatch);
	else if( braceMatching->currentItem() == 1 )
		Config::view.setInteger("braceMatching", QextScintilla::StrictBraceMatch);
	else if( braceMatching->currentItem() == 2 )
		Config::view.setInteger("braceMatching", QextScintilla::SloppyBraceMatch);
	
	if( whitespaces->currentItem() == 0 )
		Config::view.setInteger("whitespaces", QextScintilla::WsInvisible);
	if( whitespaces->currentItem() == 1 )
		Config::view.setInteger("whitespaces", QextScintilla::WsVisible);
	if( whitespaces->currentItem() == 2 )
		Config::view.setInteger("whitespaces", QextScintilla::WsVisibleAfterIndent);

	Config::view.setInteger("statusbar", statusbar->isChecked());
	Config::view.setInteger("toolbar", toolbar->isChecked());
	Config::view.setInteger("cmdToolbar", commandToolbar->isChecked());
	Config::view.setInteger("margin", margin->isChecked());
	Config::view.setInteger("foldMargin", foldMargin->isChecked());
	Config::view.setInteger("lineNumbers", lineNumbers->isChecked());
	Config::view.setInteger("eolVisibility", endOfLinesVisibility->isChecked());
	Config::view.setInteger("indentationsGuide", indentationGuides->isChecked());
	Config::view.setInteger("toggleAllFolds", toggleAllFolds->isChecked());
	Config::view.setInteger("wrapLines", wrapLines->isChecked());
	Config::view.setInteger("zoomLevel", zoomLevel->value());

	Config::cute.setInteger("showMessagePanel", showMessagePanel->isChecked());
	Config::cute.setInteger("showSidePanel", showSidePanel->isChecked());
	
	Config::view.setInteger("fullScreenShowMenuBar", fullScreenShowMenuBar->isChecked());
	Config::view.setInteger("fullScreenShowToolBar", fullScreenShowToolBar->isChecked());
	Config::view.setInteger("fullScreenShowCommandBox", fullScreenShowCommandBox->isChecked());
	
	emit updateConfig();
}
