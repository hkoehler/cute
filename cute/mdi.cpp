/***************************************************************************
                          mdi.cpp  -  multi document interface
                             -------------------
    begin                : Wed Mar 10 23:53:49 CEST 2004
    copyright            : (C) 2004 by Heiko Köhler
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

#include <qfileinfo.h>

#include "mdi.h"

/** class MDITabWidget *******************************************************/
MDITabWidget::MDITabWidget(QWidget *parent, const char *name) 
	: QTabWidget(parent, name)
{
}

void MDITabWidget::addWindow( QWidget *view )
{
	if( view->caption() == QString::null )
		addTab(view, "unnamed");
	else
		addTab(view, QFileInfo(view->caption()).fileName());
}

void MDITabWidget::closeWindow( QWidget *view )
{
	removePage(view);
}

/** class DockWindow *******************************************************/
DockWindow::DockWindow( QWidget *parent, const char *name ) 
	: QDockWindow(parent, name)
{
	tabWidget = new QTabWidget(this);
	setWidget(tabWidget);
	setCloseMode(QDockWindow::Always);
	setResizeEnabled(true);
	setVerticallyStretchable(true);
	setHorizontallyStretchable(true);
}

void DockWindow::addWidget( QWidget *widget, QString name )
{
	tabWidget->addTab(widget, name);
}

void DockWindow::showWidget( QWidget *w)
{
	tabWidget->showPage( w );
}

/** class MainWindow *******************************************************/
MainWindow::MainWindow(QWidget *parent, const char *name) 
	: QMainWindow(parent, name)
{
	// add central widget
	mdiTabWidget = new MDITabWidget(this);
	setCentralWidget(mdiTabWidget);
	// add window menu
	__windowMenu = new QPopupMenu(this);
	__windowMenu->setCheckable(true);
	connect( __windowMenu, SIGNAL(aboutToShow()), this,SLOT(updateWindowMenu()));
	connect( __windowMenu, SIGNAL(activated(int)), mdiTabWidget,SLOT(setCurrentPage(int)));
	connect( mdiTabWidget, SIGNAL(currentChanged(QWidget*)), this, SIGNAL(windowChanged(QWidget*)) );
}

void MainWindow::addWindow( QWidget *view )
{
	widgetList.append(view);
	mdiTabWidget->addWindow(view);
	view->show();
	view->setFocus();
}

void MainWindow::closeWindow( QWidget *view )
{
	view->close();
	widgetList.remove(view);
	mdiTabWidget->closeWindow(view);
	if( mdiTabWidget->count() == 0 )
		emit lastWindowClosed();
}

QWidget *MainWindow::activeWindow()
{
	return mdiTabWidget->currentPage();
}

WidgetList MainWindow::windows()
{
	return widgetList;
}

QPopupMenu *MainWindow::windowMenu()
{
	return __windowMenu;
}

int MainWindow::windowsCount()
{
	return mdiTabWidget->count();
}

void MainWindow::updateWindowMenu()
{
	__windowMenu->clear();
	for( int i = 0; i < mdiTabWidget->count(); i++ ){
		 __windowMenu->setItemChecked(i, false);
		__windowMenu->insertItem(mdiTabWidget->label(i), i, i);
	}
	__windowMenu->setItemChecked( mdiTabWidget->currentPageIndex(), true );
}

void MainWindow::nextWindow()
{
	int index = mdiTabWidget->currentPageIndex();
	mdiTabWidget->setCurrentPage(++index);
}

void MainWindow::prevWindow()
{
	int index = mdiTabWidget->currentPageIndex();
	mdiTabWidget->setCurrentPage(--index);
}

void MainWindow::closeAllWindows() 
{
	WidgetList list = windows();

	for(QWidget *iter = list.first(); iter; iter = list.next())
		closeWindow(iter);

	emit lastWindowClosed();
}

void MainWindow::closeActiveWindow() 
{
	closeWindow(mdiTabWidget->currentPage());
	if( mdiTabWidget->count() == 0 )
		emit lastWindowClosed();
}

void MainWindow::activateWindow(QWidget *view) 
{
	mdiTabWidget->showPage( view );
	view->setFocus();
}

void MainWindow::setTabCaption(QWidget *view, QString name)
{
	mdiTabWidget->changeTab(view, name);
}
