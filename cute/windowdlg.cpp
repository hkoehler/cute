/***************************************************************************
                          windowdlg.cpp  -  description
                             -------------------
    begin                : Son Nov 24 2002
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

#include <qapplication.h>
#include <qstringlist.h>

#include "windowdlg.h"
#include "mdi.h"
#include "cuteview.h"

WindowDlg::WindowDlg( QWidget* parent, const char* name, bool modal, WFlags fl )
{
	updateListView();
	connect(WinListBox, SIGNAL(selected(const QString&)), this, SLOT(switchWindow(const QString&)));
}

void WindowDlg::updateListView()
{
	QString name;
	WinListBox->clear();
	QStringList strList;
	WidgetList list = static_cast<MainWindow*>(qApp->mainWidget())->windows();
	for(QWidget *iter = list.first(); iter; iter = list.next()){
		name = static_cast<CUTEView*>(iter)->caption();
		strList.append(name);
		viewMap.insert(name, iter);
	}
	strList.sort();
	WinListBox->insertStringList(strList);
}

WindowDlg::~WindowDlg()
{
}

void WindowDlg::close_slot()
{
	MainWindow* cute = static_cast<MainWindow*>(qApp->mainWidget());
	QWidget *view = viewMap[WinListBox->currentText()];
	if(view){
		cute->closeWindow(view);
		updateListView();
	}
}

void WindowDlg::new_slot()
{
	MainWindow *cute = static_cast<MainWindow*>(qApp->mainWidget());
	cute->newWindow();
	hide();
}

void WindowDlg::ok_slot()
{
	switchWindow(WinListBox->currentText());
}

void WindowDlg::switchWindow(const QString &item)
{
	MainWindow *mainWin = static_cast<MainWindow*>(qApp->mainWidget());
	QWidget *view = viewMap[item];
	if(view){
		mainWin->activateWindow(view);
		hide();
	}
}
