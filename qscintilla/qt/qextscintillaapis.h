// This module defines interface to the QextScintillaAPIs class.
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


#ifndef QEXTSCINTILLAAPIS_H
#define QEXTSCINTILLAAPIS_H


#include <qstringlist.h>

#include <qextscintillaglobal.h>


//! \brief The QextScintillaAPIs class represents the textual API information
//! used in call tips and for auto-completion.
//!
//! API information is read from one or more files.  Each API function is
//! described by a single line of text comprising the function's name, followed
//! by the function's optional comma separated parameters enclosed in
//! parenthesis, and finally followed by optional explanatory text.
//!
//! A function name may be followed by a `?' and a number.  The number is used
//! by auto-completion to display a registered QPixmap with the function name.
//!
//! All function names are used by auto-completion, but only those that include
//! function parameters are used in call tips.
class QEXTSCINTILLA_EXPORT QextScintillaAPIs
{
public:
	//! Constructs a QextScintillaAPIs instance.
	QextScintillaAPIs();

	//! Destroys the QextScintillaAPIs instance.
	~QextScintillaAPIs();

	//! Add the single API entry \a entry to the current set.
	void add(const QString &entry);

	//! Load the API information from the file named \a fname, adding it to
	//! the current set.  Returns TRUE if successful, otherwise FALSE.
	bool load(const QString &fname);

	//! Deletes all API information.
	void clear();

private:
	friend class QextScintilla;

	QStringList autoCompletionList(const QString &starts,bool cs);
	QString callTips(const QString &function,int maxnr,int commas);
	QString callTipsNextPrev(int dir);
	void ensureSorted();

	bool sorted;
	int ctcursor;
	QStringList apis;
	QStringList ctlist;

#if defined(Q_DISABLE_COPY)
	QextScintillaAPIs(const QextScintillaAPIs &);
	QextScintillaAPIs &operator=(const QextScintillaAPIs &);
#endif
};

#endif
