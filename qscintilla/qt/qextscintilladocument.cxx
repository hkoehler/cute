// This module implements the QextScintillaDocument class.
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


#include "qextscintilladocument.h"
#include "qextscintillabase.h"


// This internal class encapsulates the underlying document and is shared by
// QextScintillaDocument instances.
class QextScintillaDocumentP
{
public:
	QextScintillaDocumentP() : doc(0), nr_displays(0), nr_attaches(1) {}

	long doc;			// The Scintilla document.
	int nr_displays;		// The number of displays.
	int nr_attaches;		// The number of attaches.
};


// The ctor.
QextScintillaDocument::QextScintillaDocument()
{
	pdoc = new QextScintillaDocumentP();
}


// The dtor.
QextScintillaDocument::~QextScintillaDocument()
{
	detach();
}


// The copy ctor.
QextScintillaDocument::QextScintillaDocument(const QextScintillaDocument &that)
{
	attach(that);
}


// The assignment operator.
QextScintillaDocument &QextScintillaDocument::operator=(const QextScintillaDocument &that)
{
	if (pdoc != that.pdoc)
	{
		detach();
		attach(that);
	}

	return *this;
}


// Attach an existing document to this one.
void QextScintillaDocument::attach(const QextScintillaDocument &that)
{
	++that.pdoc -> nr_attaches;
	pdoc = that.pdoc;
}


// Detach the underlying document.
void QextScintillaDocument::detach()
{
	if (!pdoc)
		return;

	if (--pdoc -> nr_attaches == 0)
	{
		if (pdoc -> doc && pdoc -> nr_displays == 0)
		{
			QextScintillaBase *qsb = QextScintillaBase::pool();

			// Release the explicit reference to the document.  If
			// the pool is empty then we just accept the memory
			// leak.
			if (qsb)
				qsb -> SendScintilla(QextScintillaBase::SCI_RELEASEDOCUMENT,0,pdoc -> doc);
		}

		delete pdoc;
	}

	pdoc = 0;
}


// Undisplay and detach the underlying document.
void QextScintillaDocument::undisplay(QextScintillaBase *qsb)
{
	if (--pdoc -> nr_attaches == 0)
		delete pdoc;
	else if (--pdoc -> nr_displays == 0)
	{
		// Create an explicit reference to the document to keep it
		// alive.
		qsb -> SendScintilla(QextScintillaBase::SCI_ADDREFDOCUMENT,0,pdoc -> doc);
	}

	pdoc = 0;
}


// Display the underlying document.
void QextScintillaDocument::display(QextScintillaBase *qsb,const QextScintillaDocument *from)
{
	long ndoc;

	if (from)
	{
		ndoc = from -> pdoc -> doc;
		qsb -> SendScintilla(QextScintillaBase::SCI_SETDOCPOINTER,0,ndoc);
	}
	else
		ndoc = qsb -> SendScintilla(QextScintillaBase::SCI_GETDOCPOINTER);

	pdoc -> doc = ndoc;
	++pdoc -> nr_displays;
}
