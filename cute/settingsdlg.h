/***************************************************************************
                          settingsdlg.h  -  settings dialog
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

#ifndef __SETTINGSDLG_H__
#define __SETTINGSDLG_H__

#include "ui/settings.h"

/** views some options and change them*/
class SettingsDlg : public SettingsDialog
{
	Q_OBJECT
public:
	SettingsDlg(QWidget *parent=0, const char* name=0);
	/** initializes dialog values and shows dialog */
	void show();
protected slots:
	/** called when ok button is pressed */
	void ok();
	/** change CUTE configuration emits updateConfig()*/
	void applySettings();
signals:
	/** emitted when updating config */
	void updateConfig();
};

#endif
