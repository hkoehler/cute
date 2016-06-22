// This module defines interface to the QextScintillaPrinter class.
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


#ifndef QEXTSCINTILLAPRINTER_H
#define QEXTSCINTILLAPRINTER_H


#include <qprinter.h>

#include <qextscintillaglobal.h>
#include <qextscintilla.h>


class QRect;
class QPainter;
class QextScintillaBase;


//! \brief The QextScintillaPrinter class is a sub-class of the Qt QPrinter
//! class that is able to print the text of a Scintilla document.
//!
//! The class can be further sub-classed to alter to layout of the text, adding
//! headers and footers for example.
class QEXTSCINTILLA_EXPORT QextScintillaPrinter : public QPrinter
{
public:
	//! Constructs a printer paint device with mode \a mode.
	QextScintillaPrinter(PrinterMode mode = ScreenResolution);

	//! Destroys the QextScintillaPrinter instance.
	~QextScintillaPrinter();

	//! Format a page, by adding headers and footers for example, before
	//! the document text is drawn on it.  \a painter is the painter to be
	//! used to add customised text and graphics.  \a drawing is TRUE if
	//! the page is actually being drawn rather than being sized.
	//! \a painter drawing methods must only be called when \a drawing is
	//! TRUE.  \a area is the area of the page that will be used to draw
	//! the text.  This should be modified if it is necessary to reserve
	//! space for any customised text or graphics.  By default the area is
	//! relative to the printable area of the page.  Use
	//! QPrinter::setFullPage() because calling printRange() if you want to
	//! try and print over the whole page.  \a pagenr is the number of the
	//! page.  The first page is numbered 1.
	virtual void formatPage(QPainter &painter,bool drawing,QRect &area,
				int pagenr);

	//! Return the number of points to add to each font when printing.
	//!
	//! \sa setMagnification()
	int magnification() const {return mag;}

	//! Sets the number of points to add to each font when printing to
	//! \a magnification.
	//!
	//! \sa magnification()
	virtual void setMagnification(int magnification);

	//! Print a range of lines from the Scintilla instance \a qsb.  \a from
	//! is the first line to print and a negative value signifies the first
	//! line of text.  \a to is the last line to print and a negative value
	//! signifies the last line of text.  TRUE is returned if there was no
	//! error.
	virtual int printRange(QextScintillaBase *qsb,int from = -1,
			       int to = -1);

	//! Return the line wrap mode used when printing.  The default is
	//! QextScintilla::WrapWord.
	//!
	//! \sa setWrapMode()
	QextScintilla::WrapMode wrapMode() const {return wrap;}

	//! Sets the line wrap mode used when printing to \a wmode.
	//!
	//! \sa wrapMode()
	virtual void setWrapMode(QextScintilla::WrapMode wmode);

private:
	int mag;
	QextScintilla::WrapMode wrap;

#if defined(Q_DISABLE_COPY)
	QextScintillaPrinter(const QextScintillaPrinter &);
	QextScintillaPrinter &operator=(const QextScintillaPrinter &);
#endif
};

#endif
