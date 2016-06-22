/***************************************************************************
                          replacedlg.h  -  description
                             -------------------
    begin                : Fre Nov 8 2002
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
#ifndef __REPLACEDLG_H__
#define __REPLACEDLG_H__

#include <qextscintilla.h>
#include "ui/replacedialog.h"

class CUTEView; 
/** replaces a string of regular expression in current view */
class ReplaceDlg : public ReplaceDialog
{
	Q_OBJECT
public:
	ReplaceDlg(CUTEView *parent);
	ReplaceDlg() {}
  /** No descriptions */
  void newSearchString();
  void show(CUTEView*);
public slots:
	/** replace next occurence of search string */
	void replace();
	/** replace all occurences of search string in whole document */
	void replaceAll();
	/** replace all occurences of search string in selection */
	void replaceInSelection();
protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent *);
private: // Public attributes
  /** weather search string was found before */
  bool firstFound;
  /** scintilla text area for replacing */
  CUTEView* view;
};
#endif
