/***************************************************************************
                          helpviewer.h  -  HTML view for help
                             -------------------
    begin                : Fri Sep 05 23:53:49 CEST 2003
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
 
#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <qmainwindow.h>
#include <qtextbrowser.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdir.h>

class QComboBox;
class QPopupMenu;

class HelpViewer : public QMainWindow
{
	Q_OBJECT
public:
	HelpViewer( const QString& home_,  const QString& path, QWidget* parent = 0, const char *name=0 );
	~HelpViewer();

private slots:
	void setBackwardAvailable( bool );
	void setForwardAvailable( bool );
	
	void sourceChanged( const QString& );
	void openFile();
	void newWindow();
	void print();
	
	void pathSelected( const QString & );
	void histChosen( int );
	void bookmChosen( int );
	void addBookmark();
private:
	void readHistory();
	void readBookmarks();

	QTextBrowser* browser;
	QComboBox *pathCombo;
	int backwardId, forwardId;
	QStringList history, bookmarks;
	QMap<int, QString> mHistory, mBookmarks;
	QPopupMenu *hist, *bookm;
};

#endif
