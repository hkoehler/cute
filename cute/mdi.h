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

#ifndef __QMDI_H__
#define __QMDI_H__

#include <qmainwindow.h>
#include <qdockwindow.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qpopupmenu.h>
#include <qptrlist.h>

typedef QPtrList<QWidget> WidgetList;

class MDITabWidget : public QTabWidget
{
	Q_OBJECT
public:
	MDITabWidget(QWidget *parent=0, const char *name=0);
	void addWindow( QWidget *view );
	void closeWindow( QWidget *view );
};

class DockWindow : public QDockWindow
{
	Q_OBJECT
public:
	DockWindow( QWidget *parent=0, const char *name=0 );
	void addWidget( QWidget *widget, QString name );
	void showWidget( QWidget *widget );
private:
	QTabWidget *tabWidget;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent=0, const char *name=0);
	QWidget *activeWindow();
	WidgetList windows();
	QPopupMenu *windowMenu();
	int windowsCount();
public slots:
	virtual void newWindow() = 0;
	void closeWindow( QWidget *view );
	void addWindow( QWidget *view );
	void closeAllWindows(); 
	void closeActiveWindow();
	void nextWindow();
	void prevWindow();
	void activateWindow(QWidget *);
	void setTabCaption(QWidget *view, QString name);
protected slots:
	void updateWindowMenu();
signals:
	void lastWindowClosed();
	void windowChanged(QWidget *);
private:
	DockWindow *dockLeft, *dockBottom;
	MDITabWidget *mdiTabWidget;
	WidgetList widgetList;
	QPopupMenu *__windowMenu;
};

#endif
