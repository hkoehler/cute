/***************************************************************************
                          dirview.h -  description
                             -------------------
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

#ifndef DIRVIEW_H
#define DIRVIEW_H

#include <qlistview.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>

class QWidget;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;

/** list item for files */
class FileItem : public QListViewItem
{
public:
	FileItem( QListViewItem *parent, const QString &s1, const QString &s2 )
	: QListViewItem( parent, s1, s2 ), pix( 0 ) {}

	const QPixmap *pixmap( int i ) const;
	void setPixmap( QPixmap *p );
	QString className() { return "FileItem"; }
private:
	QPixmap *pix;
};

/** list item for dirs */
class Directory : public QListViewItem
{
public:
	Directory( QListView * parent, const QString& filename );
	Directory( Directory * parent, const QString& filename, const QString &col2 )
		: QListViewItem( parent, filename, col2 ), pix( 0 ) {}
	Directory( Directory * parent, const QString& filename );
	
	QString text( int column ) const;
	
	QString fullName();
	/** opens dir item */
	void setOpen( bool );
	void setup();
	
	const QPixmap *pixmap( int i ) const;
	void setPixmap( QPixmap *p );
	QString className() { return "Directory"; }
private:
	QFile f;
	Directory * p;
	bool readable;
	bool showDirsOnly;
	QPixmap *pix;
};

/** views a dir tree */
class DirectoryView : public QListView
{
    Q_OBJECT
public:
    DirectoryView( QWidget *parent = 0, const char* name=0, QString dir = "/", bool sdo = FALSE );
    bool showDirsOnly() { return dirsOnly; }
public slots:
    void setDir( const QString & );

signals:
    void folderSelected( const QString & );
	void fileSelected( const QString & );

protected slots:
    void slotFolderSelected( QListViewItem * );
    void openFolder();

protected:
    void contentsMousePressEvent( QMouseEvent *e );
    void contentsMouseReleaseEvent( QMouseEvent *e );
    void contentsMouseDoubleClickEvent( QMouseEvent *e );

private:
    QString fullPath(QListViewItem* item);
    bool dirsOnly;
    QListViewItem *oldCurrent;
    QListViewItem *dropItem;
    QTimer* autoopen_timer;
    QPoint presspos;
    bool mousePressed;
};

#endif
