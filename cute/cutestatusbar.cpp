/***************************************************************************
                          cutestatusbar.cpp  -  description
                             -------------------
    begin                : Wed Apr 14 2004
    copyright            : (C) 2004 by Nick Thompson
    email                : nickthompson@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <qlabel.h>

#include "cutestatusbar.h"

CuteStatusBar::CuteStatusBar(QWidget* parent, const char* name, WFlags fl)
    : StatusBar( parent, name, fl )
{ /* NOOP */ }

CuteStatusBar::~CuteStatusBar() { /* NOOP */ }

void CuteStatusBar::slotStatusBarMessage(const QString &text)
{
    message->setText(text);
}

void CuteStatusBar::slotStatusBarInfo(const QString &text)
{
    information->setText(text);
}

void CuteStatusBar::slotStatusBarLineCol(int line, int pos)
{
	QString str;
	str.sprintf("line: %d     col: %d", line+1, pos+1);
    lineCol->setText(str);
}
