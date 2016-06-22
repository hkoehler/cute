// This module implements the QextScintillaMacro class.
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


#include <string.h>

#include <qstring.h>

#include "qextscintillamacro.h"
#include "qextscintilla.h"


static QCString extract(const QCString &asc,int &start);
static int fromHex(unsigned char ch);


// The ctor.
QextScintillaMacro::QextScintillaMacro(QextScintilla *parent,const char *name)
			: QObject(parent,name), qsci(parent)
{
}


// The ctor that initialises the macro.
QextScintillaMacro::QextScintillaMacro(const QCString &asc,
				       QextScintilla *parent,const char *name)
			: QObject(parent,name), qsci(parent)
{
	load(asc);
}


// The dtor.
QextScintillaMacro::~QextScintillaMacro()
{
}


// Clear the contents of the macro.
void QextScintillaMacro::clear()
{
	macro.clear();
}


// Read a macro from a string.
bool QextScintillaMacro::load(const QCString &asc)
{
	bool rc = TRUE;

	macro.clear();

	int pos = 0;

	while (pos < asc.length())
	{
		QCString fld;
		Macro cmd;
		unsigned len;

		// Extract the 3 fixed fields.
		fld = extract(asc,pos);
		cmd.msg = fld.toUInt(&rc);

		if (!rc)
			break;

		fld = extract(asc,pos);
		cmd.wParam = fld.toULong(&rc);

		if (!rc)
			break;

		fld = extract(asc,pos);
		len = fld.toUInt(&rc);

		if (!rc)
			break;

		// Extract any text.
		if (len)
		{
			cmd.text.resize(len);
			fld = extract(asc,pos);

			char *dp = cmd.text.data();
			const char *sp = fld;

			if (!sp)
			{
				rc = FALSE;
				break;
			}

			while (len--)
			{
				unsigned char ch;

				ch = *sp++;

				if (ch == '"' || ch <= ' ' || ch >= 0x7f)
				{
					rc = FALSE;
					break;
				}

				if (ch == '\\')
				{
					int b1, b2;

					if ((b1 = fromHex(*sp++)) < 0 ||
					    (b2 = fromHex(*sp++)) < 0)
					{
						rc = FALSE;
						break;
					}

					ch = (b1 << 4) + b2;
				}

				*dp++ = ch;
			}

			if (!rc)
				break;
		}

		macro.append(cmd);
	}
		
	if (!rc)
		macro.clear();

	return rc;
}


// Write a macro to a string.
QCString QextScintillaMacro::save() const
{
	QCString ms;

	for (QValueList<Macro>::const_iterator it = macro.begin(); it != macro.end(); ++it)
	{
		if (!ms.isEmpty())
			ms += ' ';

		unsigned len = (*it).text.size();
		QCString m;

		m.sprintf("%u %lu %u",(*it).msg,(*it).wParam,len);

		if (len)
		{
			m += ' ';

			const char *cp = (*it).text.data();

			while (len--)
			{
				unsigned char ch = *cp++;

				if (ch == '\\' || ch == '"' || ch <= ' ' || ch >= 0x7f)
				{
					char buf[4];

					sprintf(buf,"\\%02x",ch);
					m += buf;
				}
				else
					m += ch;
			}
		}

		ms += m;
	}

	return ms;
}


// Play the macro.
void QextScintillaMacro::play()
{
	if (!qsci)
		return;

	for (QValueList<Macro>::const_iterator it = macro.begin(); it != macro.end(); ++it)
		qsci -> SendScintilla((*it).msg,(*it).wParam,(*it).text.data());
}


// Start recording.
void QextScintillaMacro::startRecording()
{
	if (!qsci)
		return;

	macro.clear();

	connect(qsci,
		SIGNAL(SCN_MACRORECORD(unsigned int,unsigned long,long)),
		SLOT(record(unsigned int,unsigned long,long)));

	qsci -> SendScintilla(QextScintillaBase::SCI_STARTRECORD);
}


// End recording.
void QextScintillaMacro::endRecording()
{
	if (!qsci)
		return;

	qsci -> SendScintilla(QextScintillaBase::SCI_STOPRECORD);
	qsci -> disconnect(this);
}


// Record a command.
void QextScintillaMacro::record(unsigned int msg,unsigned long wParam,
				long lParam)
{
	Macro m;

	m.msg = msg;
	m.wParam = wParam;

	// Determine commands which need special handling of the parameters.
	switch (msg)
	{
        case QextScintillaBase::SCI_ADDTEXT:
		m.text.duplicate(reinterpret_cast<const char *>(lParam),wParam);
		break;

        case QextScintillaBase::SCI_REPLACESEL:
		if (!macro.isEmpty() && macro.last().msg == QextScintillaBase::SCI_REPLACESEL)
		{
			const char *text = reinterpret_cast<const char *>(lParam);

			// This is the command used for ordinary user input so
			// it's a signifacant space reduction to append it to
			// the previous command.

			QByteArray &ba = macro.last().text;

			unsigned pos = ba.size() - 1;

			// Make room for the new text.
			ba.resize(ba.size() + strlen(text));

			// Copy it in.
			strcpy(ba.data() + pos,text);

			return;
		}

		/* Drop through. */

        case QextScintillaBase::SCI_INSERTTEXT:
        case QextScintillaBase::SCI_APPENDTEXT:
        case QextScintillaBase::SCI_SEARCHNEXT:
        case QextScintillaBase::SCI_SEARCHPREV:
		{
			const char *text = reinterpret_cast<const char *>(lParam);

			m.text.duplicate(text,strlen(text) + 1);
			break;
		}
	}

	macro.append(m);
}


// Extract a macro field starting at the given position.
static QCString extract(const QCString &asc,int &fstart)
{
	QCString f;

	if (fstart < asc.length())
	{
		int fend = asc.find(' ',fstart);

		if (fend < 0)
		{
			f = asc.mid(fstart);
			fstart = asc.length();
		}
		else
		{
			f = asc.mid(fstart,fend - fstart);
			fstart = fend + 1;
		}
	}

	return f;
}


// Return the given hex character as a binary.
static int fromHex(unsigned char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';

	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;

	return -1;
}
