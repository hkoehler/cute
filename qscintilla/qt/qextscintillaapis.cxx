// This module implements the QextScintillaAPIs class.
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


#include <qfile.h>
#include <qregexp.h>

#include "qextscintillaapis.h"


// The ctor.
QextScintillaAPIs::QextScintillaAPIs() : sorted(TRUE), ctcursor(0)
{
}


// The dtor.
QextScintillaAPIs::~QextScintillaAPIs()
{
}


// Clear out all API information.
bool QextScintillaAPIs::load(const QString &fname)
{
	QFile f(fname);

	if (!f.open(IO_ReadOnly))
		return FALSE;

	QTextStream ts(&f);

	for (;;)
	{
		QString line = ts.readLine();

		if (line.isNull())
			break;

		apis.append(line);
	}

	sorted = FALSE;

	return TRUE;
}


// Add a single API entry.
void QextScintillaAPIs::add(const QString &entry)
{
	apis.append(entry);
	sorted = FALSE;
}


// Clear out all API information.
void QextScintillaAPIs::clear()
{
	apis.clear();
	sorted = TRUE;
}


// Return a list of sorted, unique auto-completion words.
QStringList QextScintillaAPIs::autoCompletionList(const QString &starts,bool cs)
{
	ensureSorted();

	QStringList::ConstIterator it = apis.begin();

	// Find the first match in the sorted list.
	if (!starts.isEmpty())
		while (it != apis.end())
		{
			if ((*it).find(starts,0,cs) == 0)
				break;

			++it;
		}

	QStringList aclist;
	QString prev;

	while (it != apis.end())
	{
		if (!starts.isEmpty() && (*it).find(starts,0,cs) != 0)
			break;

		QString w = (*it).section('(',0,0);

		if (w != prev)
		{
			aclist.append(w);
			prev = w;
		}

		++it;
	}

	return aclist;
}


// Return the call tip for a function.
QString QextScintillaAPIs::callTips(const QString &function,int maxnr,
				    int commas)
{
	ensureSorted();

	QStringList::ConstIterator it;

	// Find the first match in the sorted list.
	for (it = apis.begin(); it != apis.end(); ++it)
		if ((*it).startsWith(function))
			break;

	QStringList cts;
	QString prev;

	while (it != apis.end() && (*it).startsWith(function))
	{
		if (maxnr > 0 && maxnr == cts.count())
			break;

		QString w = *it;

		// Remove any image ID used by auto-completion.
		w.replace(QRegExp("\\?[^(]*"),"");

		if (w.find('(') == function.length() && w.contains(',') >= commas && w != prev)
		{
			cts.append(w);
			prev = w;
		}

		++it;
	}

	// See if we want to add a down arrow.
	if (maxnr < 0 && cts.count() > 1)
	{
		// Remember the state so we can scroll through it later.
		ctlist = cts;
		ctcursor = 0;

		QString ct = cts[0];

		ct.prepend('\002');

		return ct;
	}

	ctlist.clear();

	return cts.join("\n");
}


// Return the next or previous call tip.
QString QextScintillaAPIs::callTipsNextPrev(int dir)
{
	QString ct;

	// Get the call tip.
	if (dir == 1 && ctcursor > 0)
		ct = ctlist[--ctcursor];
	else if (dir == 2 && ctcursor < ctlist.count() - 1)
		ct = ctlist[++ctcursor];

	// Add the arrows.
	if (!ct.isNull())
	{
		if (ctcursor < ctlist.count() - 1)
			ct.prepend('\002');

		if (ctcursor > 0)
			ct.prepend('\001');
	}

	return ct;
}


// Ensure the list is sorted.
void QextScintillaAPIs::ensureSorted()
{
	if (!sorted)
	{
		apis.sort();
		sorted = TRUE;
	}
}
