/***************************************************************************
                          cutestatusbar.h  -  description
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
#ifndef __CUTESTATUSBAR_H__
#define __CUTESTATUSBAR_H__

#include "ui/statusbar.h"

class CuteStatusBar : public StatusBar
{
    Q_OBJECT
public:
    CuteStatusBar( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CuteStatusBar();
public slots:
    void slotStatusBarMessage(const QString &text);
    void slotStatusBarInfo(const QString &text);
    void slotStatusBarLineCol(int line, int pos);
};

#endif
