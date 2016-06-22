// This defines the interface to the QextScintillaCommandSet class.
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


#ifndef QEXTSCINTILLACOMMANDSET_H
#define QEXTSCINTILLACOMMANDSET_H


#include <qptrlist.h>

#include <qextscintillaglobal.h>


class QSettings;
class QextScintilla;
class QextScintillaCommand;


//! \brief The QextScintillaCommandSet class represents the set of all internal
//! editor commands that may have keys bound.
//!
//! Methods are provided to access the individual commands and to read and
//! write the current bindings from and to settings files.
class QEXTSCINTILLA_EXPORT QextScintillaCommandSet
{
public:
	//! The key bindings for each command in the set are read from the
	//! settings \a qs.  \a prefix is prepended to the key of each entry.
	//! TRUE is returned if there was no error.
	//!
	//! \sa writeSettings()
	bool readSettings(QSettings &qs,const char *prefix = "/Scintilla");

	//! The key bindings for each command in the set are written to the
	//! settings \a qs.  \a prefix is prepended to the key of each entry.
	//! TRUE is returned if there was no error.
	//!
	//! \sa readSettings()
	bool writeSettings(QSettings &qs,const char *prefix = "/Scintilla");

	//! The commands in the set are returned as a list.
	const QPtrList<QextScintillaCommand> &commands() const {return cmds;}

private:
	friend class QextScintilla;

	QextScintillaCommandSet(QextScintilla *qs);

	QextScintilla *qsci;
	QPtrList<QextScintillaCommand> cmds;

#if defined(Q_DISABLE_COPY)
	QextScintillaCommandSet(const QextScintillaCommandSet &);
	QextScintillaCommandSet &operator=(const QextScintillaCommandSet &);
#endif
};

#endif
