// This module implements the specialisation of QListBox that handles the
// Scintilla double-click callback.
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


#include <qapplication.h>
#include <qmap.h>
#include <qpixmap.h>

#include "SciListBox.h"


// The platform specific specialisation of the ListBox abstract class.

class ListBoxX : public ListBox
{
public:
	ListBoxX();
	virtual ~ListBoxX();

	CallBackAction cb_action;
	void *cb_data;

	virtual void SetFont(Font &font);
	virtual void Create(Window &parent,int,int,bool);
	virtual void SetAverageCharWidth(int);
	virtual void SetVisibleRows(int);
	virtual PRectangle GetDesiredRect();
	virtual int CaretFromEdge();
	virtual void Clear();
	virtual void Append(char *s,int type = -1);
	virtual int Length();
	virtual void Select(int n);
	virtual int GetSelection();
	virtual int Find(const char *prefix);
	virtual void GetValue(int n,char *value,int len);
	virtual void Sort();
	virtual void RegisterImage(int type,const char *xpm_data);
	virtual void ClearRegisteredImages();
	virtual void SetDoubleClickAction(CallBackAction action,void *data);

private:
	SciListBox *slb;

	typedef QMap<int,QPixmap> xpmMap;
	xpmMap xset;
};


ListBoxX::ListBoxX() : cb_action(0), cb_data(0), slb(0)
{
}


ListBoxX::~ListBoxX()
{
}


void ListBoxX::SetFont(Font &font)
{
	QFont *f = reinterpret_cast<QFont *>(font.GetID());

	if (f)
		slb -> setFont(*f);
}


void ListBoxX::Create(Window &parent,int,int,bool)
{
	// The parent we want is the QextScintillaBase, not the text area.
	id = slb = new SciListBox(reinterpret_cast<QWidget *>(parent.GetID()) -> parentWidget(),this);
}


void ListBoxX::SetAverageCharWidth(int)
{
	// We rely on QListBox::sizeHint() for the size of the list box rather
	// than make calculations based on the average character width and the
	// number of visible rows.
}


void ListBoxX::SetVisibleRows(int)
{
}


PRectangle ListBoxX::GetDesiredRect()
{
	PRectangle rc(0,0,100,100);

	if (slb)
	{
		QSize sh = slb -> sizeHint();

		rc.right = sh.width();
		rc.bottom = sh.height();
	}

	return rc;
}


int ListBoxX::CaretFromEdge()
{
	int dist = 0;

	// Find the width of the biggest image.
	for (xpmMap::Iterator it = xset.begin(); it != xset.end(); ++it)
	{
		int w = it.data().width();

		if (dist < w)
			dist = w;
	}

	if (slb)
		dist += slb -> frameWidth();

	// Fudge factor - adjust if required.
	dist += 3;

	return dist;
}


void ListBoxX::Clear()
{
	slb -> clear();
}


void ListBoxX::Append(char *s,int type)
{
	xpmMap::ConstIterator it;

	if (type < 0 || (it = xset.find(type)) == xset.end())
		slb -> insertItem(s);
	else
		slb -> insertItem(it.data(),s);
}


int ListBoxX::Length()
{
	return (slb ? slb -> numRows() : 0);
}


void ListBoxX::Select(int n)
{
	slb -> setSelected(n,TRUE);
}


int ListBoxX::GetSelection()
{
	return slb -> currentItem();
}


int ListBoxX::Find(const char *prefix)
{
	return slb -> index(slb -> findItem(prefix,Qt::CaseSensitive|Qt::BeginsWith));
}


void ListBoxX::GetValue(int n,char *value,int len)
{
	QString s = slb -> text(n);

	if (s.isNull() || len <= 0)
		value[0] = '\0';
	else
	{
		strncpy(value,s.latin1(),len - 1);
		value[len - 1] = '\0';
	}
}


void ListBoxX::Sort()
{
	slb -> sort();
}


void ListBoxX::RegisterImage(int type,const char *xpm_data)
{
	xset.insert(type,*reinterpret_cast<const QPixmap *>(xpm_data));
}


void ListBoxX::ClearRegisteredImages()
{
	xset.clear();
}


void ListBoxX::SetDoubleClickAction(CallBackAction action,void *data)
{
	cb_action = action;
	cb_data = data;
}


// The ListBox methods than need to be implemented explicitly.

ListBox::ListBox()
{
}


ListBox::~ListBox()
{
}


ListBox *ListBox::Allocate()
{
	return new ListBoxX();
}


SciListBox::SciListBox(QWidget *parent,ListBoxX *lbx_)
	: QListBox(parent,0,WType_Popup|WStyle_Customize|WStyle_NoBorder), lbx(lbx_)
{
	setFocusProxy(parent);

	setFrameShape(StyledPanel);
	setFrameShadow(Plain);

	connect(this,SIGNAL(doubleClicked(QListBoxItem *)),
		SLOT(handleDoubleClick(QListBoxItem *)));

	connect(this,SIGNAL(highlighted(QListBoxItem *)),
		SLOT(ensureCurrentVisible()));
}


SciListBox::~SciListBox()
{
	// Ensure that the main widget doesn't get a focus out event when this
	// is destroyed.
	setFocusProxy(0);
}


void SciListBox::handleDoubleClick(QListBoxItem *)
{
	if (lbx && lbx -> cb_action)
		lbx -> cb_action(lbx -> cb_data);
}
