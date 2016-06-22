/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sam Sep 28 23:53:49 CEST 2002
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

#undef slots
#include <Python.h>
#define slots

#include <qapplication.h>
#include <qfont.h>
#include <qwindowsstyle.h>
#include <qplatinumstyle.h>
#include <qtextcodec.h>
#include <qtranslator.h>

#include "config.h"
#include "cute.h"
#include "lexer.h"
#include "icons/cute.xpm"

extern "C" void initcute();

int main(int argc, char **argv)
{
	QApplication a(argc, argv);
	QTranslator tor( 0 );
	// set the location where your .qm files are in load() below as the last parameter instead of "."
	// for development, use "/" to use the english original as
	// .qm files are stored in the base project directory.
	tor.load( QString("cute.") + QTextCodec::locale(), "." );
	a.installTranslator( &tor );
	//a.setFont(QFont("Helvetica", 10));
	CUTE *cute=new CUTE();
	a.setMainWidget(cute);
	
	Py_SetProgramName(argv[0]);
	// initalize python interpreter  
	Py_Initialize();
	initcute();
	Config::initconfig();
	PySys_SetArgv(argc, argv);
	PyRun_SimpleString("from cute import *\n");
	PyRun_SimpleString( "from config import *\n");
	PyRun_SimpleString( "from lang import *\n");
	Config::readConfig();
	
	cute->configure();
	cute->updateEditActions();
	cute->initLangMenu();
	cute->evalCmdParameter();
	cute->setCaption("CUTE");
	cute->setIcon(QPixmap(icon_xpm));
	
	cute->show();
	
	int ret =  a.exec();
	Config::writeConfig();
	Py_Finalize();
	return ret;
}
