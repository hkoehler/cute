// This module implements the QextScintillaPrinter class.
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


#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qvaluestack.h>

#include "qextscintillaprinter.h"
#include "qextscintillabase.h"


// The ctor.
QextScintillaPrinter::QextScintillaPrinter(QPrinter::PrinterMode mode) :
						QPrinter(mode), mag(0),
						wrap(QextScintilla::WrapWord)
{
}


// The dtor.
QextScintillaPrinter::~QextScintillaPrinter()
{
}


// Format the page before the document text is drawn.
void QextScintillaPrinter::formatPage(QPainter &,bool,QRect &,int)
{
}


// Print a range of lines to a printer.
int QextScintillaPrinter::printRange(QextScintillaBase *qsb,int from,int to)
{
	// Sanity check.
	if (!qsb)
		return FALSE;

	// Setup the printing area.
	QPaintDeviceMetrics metrics(this);
	QRect def_area;

	def_area.setX(0);
	def_area.setY(0);
	def_area.setWidth(metrics.width());
	def_area.setHeight(metrics.height());

	// Get the page range.
	int pgFrom, pgTo;

	pgFrom = fromPage();
	pgTo = toPage();

	// Find the position range.
	long startPos, endPos;

	endPos = qsb -> SendScintilla(QextScintillaBase::SCI_GETLENGTH);

	startPos = (from > 0 ? qsb -> SendScintilla(QextScintillaBase::SCI_POSITIONFROMLINE,from) : 0);

	if (to >= 0)
	{
		long toPos = qsb -> SendScintilla(QextScintillaBase::SCI_POSITIONFROMLINE,to + 1);

		if (endPos > toPos)
			endPos = toPos;
	}

	if (startPos >= endPos)
		return FALSE;

	QPainter painter(this);
	bool reverse = (pageOrder() == LastPageFirst);
	bool needNewPage = FALSE;

	qsb -> SendScintilla(QextScintillaBase::SCI_SETPRINTMAGNIFICATION,mag);
	qsb -> SendScintilla(QextScintillaBase::SCI_SETPRINTWRAPMODE,wrap);

	for (int i = 1; i <= numCopies(); ++i)
	{
		// If we are printing in reverse page order then remember the
		// start position of each page.
		QValueStack<long> pageStarts;

		int currPage = 1;
		long pos = startPos;

		while (pos < endPos)
		{
			// See if we have finished the requested page range.
			if (pgTo > 0 && pgTo < currPage)
				break;

			// See if we are going to render this page, or just see
			// how much would fit onto it.
			bool render = FALSE;

			if (pgFrom == 0 || pgFrom <= currPage)
			{
				if (reverse)
					pageStarts.push(pos);
				else
				{
					render = TRUE;

					if (needNewPage)
					{
						if (!newPage())
							return FALSE;
					}
					else
						needNewPage = TRUE;
				}
			}

			QRect area = def_area;

			formatPage(painter,render,area,currPage);
			pos = qsb -> SendScintilla(QextScintillaBase::SCI_FORMATRANGE,render,&painter,area,pos,endPos);

			++currPage;
		}

		// All done if we are printing in normal page order.
		if (!reverse)
			continue;

		// Now go through each page on the stack and really print it.
		while (!pageStarts.isEmpty())
		{
			--currPage;

			long ePos = pos;
			pos = pageStarts.pop();

			if (needNewPage)
			{
				if (!newPage())
					return FALSE;
			}
			else
				needNewPage = TRUE;

			QRect area = def_area;

			formatPage(painter,TRUE,area,currPage);
			qsb -> SendScintilla(QextScintillaBase::SCI_FORMATRANGE,TRUE,&painter,area,pos,ePos);
		}
	}

	return TRUE;
}


// Set the print magnification in points.
void QextScintillaPrinter::setMagnification(int magnification)
{
	mag = magnification;
}


// Set the line wrap mode.
void QextScintillaPrinter::setWrapMode(QextScintilla::WrapMode wmode)
{
	wrap = wmode;
}
