// This module defines various things common to all of the Qt port to
// Scintilla.
//
// Copyright (c) 2004
// 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
// 
// This file is part of QScintilla.
// 
// This copy of QScintilla is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option) any
// later version.
// 
// QScintilla is supplied in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// QScintilla; see the file LICENSE.  If not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#ifndef QEXTSCINTILLAGLOBAL_H
#define QEXTSCINTILLAGLOBAL_H


#include <qglobal.h>


#define	QSCINTILLA_VERSION	0x010300
#define	QSCINTILLA_VERSION_STR	"1.3"
#define	QSCINTILLA_BUILD	"88"


// Under Windows, define QEXTSCINTILLA_MAKE_DLL to create a Scintilla DLL, or
// define QEXTSCINTILLA_DLL to link against a Scintilla DLL, or define neither
// to either build or link against a static Scintilla library.
#if defined(Q_WS_WIN)

#if defined(QEXTSCINTILLA_DLL)
#define	QEXTSCINTILLA_EXPORT	__declspec(dllimport)
#elif defined(QEXTSCINTILLA_MAKE_DLL)
#define	QEXTSCINTILLA_EXPORT	__declspec(dllexport)
#endif

#endif

#if !defined(QEXTSCINTILLA_EXPORT)
#define	QEXTSCINTILLA_EXPORT
#endif


#endif
