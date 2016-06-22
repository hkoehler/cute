/***************************************************************************
                          config.h  -  description
                             -------------------
    begin                : Mon Okt 28 2002
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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string> 
#include <qextscintilla.h>
#include <qextscintillacommand.h>
#include <qextscintillacommandset.h>
#include "py_config.h"

/** where the language configuration files are, default /usr/share/cute/langs,
	normally are copied in user .cute/langs dir at the first start */
#define LANG_DIR "/usr/share/cute/langs/"
#define DOC_PATH "/usr/share/doc/cute/index.html"

/** namespace for whole configuration */
namespace Config
{
	/** enum for GUI style */
	enum { Default=0, CDE, Motif, MotifPlus, Platinum, SGI, Windows };
	/** enum for MDI mode */
	enum { ChildframeMode=0, TabPageMode, ToplevelMode };
	/** enum for auto indentation */
	enum { AiMaintain = QextScintilla::AiMaintain, AiOpening = QextScintilla::AiOpening, 
	AiClosing = QextScintilla::AiClosing};
	/** reads the configuration file .cuterc*/
	void readConfig();
	/**writes the configuration file .cuterc*/
	void writeConfig();
}

#endif
