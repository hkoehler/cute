// This defines the interface to the QextScintillaDocument class.
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


#ifndef QEXTSCINTILLADOCUMENT_H
#define QEXTSCINTILLADOCUMENT_H


#include <qextscintillaglobal.h>


class QextScintillaBase;
class QextScintillaDocumentP;


//! \brief The QextScintillaDocument class represents a document to be editted.
//!
//! It is an opaque class that can be attached to multiple instances of
//! QextScintilla to create different simultaneous views of the same document.
//! QextScintillaDocument uses implicit sharing so that copying class instances
//! is a cheap operation.
class QEXTSCINTILLA_EXPORT QextScintillaDocument
{
public:
	//! Create a new unattached document.
	QextScintillaDocument();
	virtual ~QextScintillaDocument();

	QextScintillaDocument(const QextScintillaDocument &);
	QextScintillaDocument &operator=(const QextScintillaDocument &);

private:
	friend class QextScintilla;

	void attach(const QextScintillaDocument &that);
	void detach();
	void display(QextScintillaBase *qsb,const QextScintillaDocument *from);
	void undisplay(QextScintillaBase *qsb);

	QextScintillaDocumentP *pdoc;
};

#endif
