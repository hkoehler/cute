# The project file for the qscintilla library.
#
# Copyright (c) 2004
# 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
# 
# This file is part of QScintilla.
# 
# This copy of QScintilla is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option) any
# later version.
# 
# QScintilla is supplied in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
# 
# You should have received a copy of the GNU General Public License along with
# QScintilla; see the file LICENSE.  If not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


unix:VERSION = 4.0.0


TEMPLATE = lib
TARGET = qscintilla
DESTDIR = ../lib
CONFIG += qt warn_off release staticlib thread
INCLUDEPATH = . ../include ../src
DEFINES = QEXTSCINTILLA_MAKE_DLL QT SCI_LEXER

HEADERS = \
	qextscintillaglobal.h \
	qextscintilla.h \
	qextscintillaapis.h \
	qextscintillabase.h \
	qextscintillacommand.h \
	qextscintillacommandset.h \
	qextscintilladocument.h \
	qextscintillalexer.h \
	qextscintillalexercpp.h \
	qextscintillalexercsharp.h \
	qextscintillalexerhtml.h \
	qextscintillalexeridl.h \
	qextscintillalexerjava.h \
	qextscintillalexerjavascript.h \
	qextscintillalexerperl.h \
	qextscintillalexerpython.h \
	qextscintillalexersql.h \
	qextscintillamacro.h \
	qextscintillaprinter.h \
	SciListBox.h \
	ScintillaQt.h \
	../include/Accessor.h \
	../include/KeyWords.h \
	../include/Platform.h \
	../include/PropSet.h \
	../include/SString.h \
	../include/SciLexer.h \
	../include/Scintilla.h \
	../include/ScintillaWidget.h \
	../include/WindowAccessor.h \
	../src/AutoComplete.h \
	../src/CallTip.h \
	../src/CellBuffer.h \
	../src/ContractionState.h \
	../src/Document.h \
	../src/DocumentAccessor.h \
	../src/Editor.h \
	../src/ExternalLexer.h \
	../src/Indicator.h \
	../src/KeyMap.h \
	../src/LineMarker.h \
	../src/RESearch.h \
	../src/SVector.h \
	../src/ScintillaBase.h \
	../src/Style.h \
	../src/StyleContext.h \
	../src/ViewStyle.h \
	../src/XPM.h

SOURCES = \
	qextscintilla.cxx \
	qextscintillaapis.cxx \
	qextscintillabase.cxx \
	qextscintillacommand.cxx \
	qextscintillacommandset.cxx \
	qextscintilladocument.cxx \
	qextscintillalexer.cxx \
	qextscintillalexercpp.cxx \
	qextscintillalexercsharp.cxx \
	qextscintillalexerhtml.cxx \
	qextscintillalexeridl.cxx \
	qextscintillalexerjava.cxx \
	qextscintillalexerjavascript.cxx \
	qextscintillalexerperl.cxx \
	qextscintillalexerpython.cxx \
	qextscintillalexersql.cxx \
	qextscintillamacro.cxx \
	qextscintillaprinter.cxx \
	SciListBox.cxx \
	PlatQt.cxx \
	ScintillaQt.cxx \
	../src/AutoComplete.cxx \
	../src/CallTip.cxx \
	../src/CellBuffer.cxx \
	../src/ContractionState.cxx \
	../src/Document.cxx \
	../src/DocumentAccessor.cxx \
	../src/Editor.cxx \
	../src/ExternalLexer.cxx \
	../src/Indicator.cxx \
        ../src/KeyMap.cxx \
	../src/KeyWords.cxx \
	../src/LexAda.cxx \
	../src/LexAPDL.cxx \
	../src/LexAsm.cxx \
	../src/LexAU3.cxx \
	../src/LexAVE.cxx \
	../src/LexBaan.cxx \
	../src/LexBash.cxx \
	../src/LexBullant.cxx \
	../src/LexCLW.cxx \
	../src/LexConf.cxx \
	../src/LexCPP.cxx \
	../src/LexCrontab.cxx \
	../src/LexCSS.cxx \
	../src/LexEiffel.cxx \
	../src/LexErlang.cxx \
	../src/LexEScript.cxx \
	../src/LexForth.cxx \
	../src/LexFortran.cxx \
	../src/LexGui4Cli.cxx \
	../src/LexHTML.cxx \
	../src/LexKix.cxx \
	../src/LexLisp.cxx \
	../src/LexLout.cxx \
	../src/LexLua.cxx \
	../src/LexMatlab.cxx \
	../src/LexMetapost.cxx \
	../src/LexMMIXAL.cxx \
	../src/LexMPT.cxx \
	../src/LexMSSQL.cxx \
	../src/LexNsis.cxx \
	../src/LexOthers.cxx \
	../src/LexPascal.cxx \
	../src/LexPB.cxx \
	../src/LexPerl.cxx \
	../src/LexPOV.cxx \
	../src/LexPS.cxx \
	../src/LexPython.cxx \
	../src/LexRuby.cxx \
	../src/LexScriptol.cxx \
	../src/LexSpecman.cxx \
	../src/LexSQL.cxx \
	../src/LexTeX.cxx \
	../src/LexVB.cxx \
	../src/LexVerilog.cxx \
	../src/LexYAML.cxx \
	../src/LineMarker.cxx \
	../src/PropSet.cxx \
        ../src/RESearch.cxx \
        ../src/ScintillaBase.cxx \
	../src/Style.cxx \
	../src/StyleContext.cxx \
	../src/ViewStyle.cxx \
	../src/WindowAccessor.cxx \
	../src/XPM.cxx

TRANSLATIONS = \
	qscintilla_de.ts
