// This module implements the QextScintillaLexer class.
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


#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>

#include "qextscintillalexer.h"
#include "qextscintilla.h"


// The ctor.
QextScintillaLexer::QextScintillaLexer(QObject *parent,const char *name)
	: QObject(parent,name), autoIndStyle(-1)
{
#if defined(Q_OS_WIN)
	defFont = QFont("Verdana",10);
#else
	defFont = QFont("lucidatypewriter",12);
#endif
}


// The dtor.
QextScintillaLexer::~QextScintillaLexer()
{
}


// Default implementation to return the list of keywords that can start a
// block.
const char *QextScintillaLexer::blockStartKeyword(int *) const
{
	return 0;
}


// Default implementation to return the list of characters that can start a
// block.
const char *QextScintillaLexer::blockStart(int *) const
{
	return 0;
}


// Default implementation to return the list of characters that can end a
// block.
const char *QextScintillaLexer::blockEnd(int *) const
{
	return 0;
}


// Default implementation to return the style used for braces.
int QextScintillaLexer::braceStyle() const
{
	return -1;
}


// Default implementation to return the number of lines to look back when
// auto-indenting.
int QextScintillaLexer::blockLookback() const
{
	return 20;
}


// Default implementation to return the number of style bits needed.
int QextScintillaLexer::styleBits() const
{
	return 5;
}


// Default implementation to return the characters that make up a word.
const char *QextScintillaLexer::wordCharacters() const
{
	return 0;
}


// Returns the foreground colour of the text for a style.
QColor QextScintillaLexer::color(int) const
{
	return Qt::black;
}


// Returns the end-of-line fill for a style.
bool QextScintillaLexer::eolFill(int) const
{
	return FALSE;
}


// Returns the font for a style.
QFont QextScintillaLexer::font(int) const
{
	return defaultFont();
}


// Returns the set of keywords.
const char *QextScintillaLexer::keywords(int) const
{
	return 0;
}


// Returns the background colour of the text for a style.
QColor QextScintillaLexer::paper(int) const
{
	return Qt::white;
}


// Returns the default font for all styles.
QFont QextScintillaLexer::defaultFont() const
{
	return defFont;
}


// Sets the default font for all styles.
void QextScintillaLexer::setDefaultFont(const QFont &f)
{
	defFont = f;
}


// Read properties from the settings.
bool QextScintillaLexer::readProperties(QSettings &,const QString &)
{
	return TRUE;
}


// Refresh all properties.
void QextScintillaLexer::refreshProperties()
{
}


// Write properties to the settings.
bool QextScintillaLexer::writeProperties(QSettings &,const QString &) const
{
	return TRUE;
}


// Restore the user settings.
bool QextScintillaLexer::readSettings(QSettings &qs,const char *prefix)
{
	bool ok, flag, rc = TRUE;
	int num;
	QString key;

	// Read the styles.
	for (int i = 0; i < 128; ++i)
	{
		// Ignore invalid styles.
		if (description(i).isNull())
			continue;

		key.sprintf("%s/%s/style%d/",prefix,language(),i);

		// Read the foreground colour.
		num = qs.readNumEntry(key + "color",0,&ok);

		if (ok)
			setColor(QColor((num >> 16) & 0xff,(num >> 8) & 0xff,num & 0xff),i);
		else
			rc = FALSE;

		// Read the end-of-line fill.
		flag = qs.readBoolEntry(key + "eolfill",0,&ok);

		if (ok)
			setEolFill(flag,i);
		else
			rc = FALSE;

		// Read the font
		QStringList fdesc;

		fdesc = qs.readListEntry(key + "font",',',&ok);

		if (ok && fdesc.count() == 5)
		{
			QFont f;

			f.setFamily(fdesc[0]);
			f.setPointSize(fdesc[1].toInt());
			f.setBold(fdesc[2].toInt());
			f.setItalic(fdesc[3].toInt());
			f.setUnderline(fdesc[4].toInt());

			setFont(f,i);
		}
		else
			rc = FALSE;

		// Read the background colour.
		num = qs.readNumEntry(key + "paper",0,&ok);

		if (ok)
			setPaper(QColor((num >> 16) & 0xff,(num >> 8) & 0xff,num & 0xff),i);
		else
			rc = FALSE;
	}

	// Read the properties.
	key.sprintf("%s/%s/properties/",prefix,language());

	if (!readProperties(qs,key))
		rc = FALSE;

	refreshProperties();

	// Read the rest.
	key.sprintf("%s/%s/",prefix,language());

	num = qs.readNumEntry(key + "autoindentstyle",0,&ok);

	if (ok)
		autoIndStyle = num;
	else
		rc = FALSE;

	return rc;
}


// Save the user settings.
bool QextScintillaLexer::writeSettings(QSettings &qs,const char *prefix) const
{
	bool rc = TRUE;
	QString key;

	// Write the styles.
	for (int i = 0; i < 128; ++i)
	{
		// Ignore invalid styles.
		if (description(i).isNull())
			continue;

		int num;
		QColor c;

		key.sprintf("%s/%s/style%d/",prefix,language(),i);

		// Write the foreground colour.
		c = color(i);
		num = (c.red() << 16) | (c.green() << 8) | c.blue();

		if (!qs.writeEntry(key + "color",num))
			rc = FALSE;

		// Write the end-of-line fill.
		if (!qs.writeEntry(key + "eolfill",eolFill(i)))
			rc = FALSE;

		// Write the font
		QStringList fdesc;
		QString fmt("%1");
		QFont f;

		f = font(i);

		fdesc += f.family();
		fdesc += fmt.arg(f.pointSize());
		fdesc += fmt.arg(f.bold());
		fdesc += fmt.arg(f.italic());
		fdesc += fmt.arg(f.underline());

		if (!qs.writeEntry(key + "font",fdesc,','))
			rc = FALSE;

		// Write the background colour.
		c = paper(i);
		num = (c.red() << 16) | (c.green() << 8) | c.blue();

		if (!qs.writeEntry(key + "paper",num))
			rc = FALSE;
	}

	// Write the properties.
	key.sprintf("%s/%s/properties/",prefix,language());

	if (!writeProperties(qs,key))
		rc = FALSE;

	// Write the rest.
	key.sprintf("%s/%s/",prefix,language());

	if (!qs.writeEntry(key + "autoindentstyle",autoIndStyle))
		rc = FALSE;

	return rc;
}


// Return the auto-indentation style.
int QextScintillaLexer::autoIndentStyle()
{
	// We can't do this in the ctor because we want the virtuals to work.
	if (autoIndStyle < 0)
		autoIndStyle = (blockStartKeyword() || blockStart() || blockEnd()) ?
					0 : QextScintilla::AiMaintain;

	return autoIndStyle;
}


// Set the auto-indentation style.
void QextScintillaLexer::setAutoIndentStyle(int autoindentstyle)
{
	autoIndStyle = autoindentstyle;
}


// Set the foreground colour for a style.
void QextScintillaLexer::setColor(const QColor &c,int style)
{
	emit colorChanged(c,style);
}


// Set the end-of-line fill for a style.
void QextScintillaLexer::setEolFill(bool eolfill,int style)
{
	emit eolFillChanged(eolfill,style);
}


// Set the font for a style.
void QextScintillaLexer::setFont(const QFont &f,int style)
{
	emit fontChanged(f,style);
}


// Set the background colour for a style.
void QextScintillaLexer::setPaper(const QColor &c,int style)
{
	emit paperChanged(c,style);
}
