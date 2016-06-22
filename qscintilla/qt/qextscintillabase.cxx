// This module implements the "official" low-level API.
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
#include <qclipboard.h>
#include <qscrollbar.h>
#include <qlayout.h>
#include <qcolor.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qpainter.h>
#include <qptrlist.h>

#include "qextscintillabase.h"
#include "ScintillaQt.h"


// The #defines in Scintilla.h and the enums in qextscintillabase.h conflict
// (because we want to use the same names) so we have to undefine those we use
// in this file.
#undef	SCI_SETCARETPERIOD
#undef	SCK_DOWN
#undef	SCK_UP
#undef	SCK_LEFT
#undef	SCK_RIGHT
#undef	SCK_HOME
#undef	SCK_END
#undef	SCK_PRIOR
#undef	SCK_NEXT
#undef	SCK_DELETE
#undef	SCK_INSERT
#undef	SCK_ESCAPE
#undef	SCK_BACK
#undef	SCK_TAB
#undef	SCK_RETURN
#undef	SCK_ADD
#undef	SCK_SUBTRACT
#undef	SCK_DIVIDE


// Remember if we have linked the lexers.
static bool lexersLinked = FALSE;

// The list of instances.
static QPtrList<QextScintillaBase> poolList;


// The ctor.
QextScintillaBase::QextScintillaBase(QWidget *parent,const char *name,WFlags f)
	: QWidget(parent,name,f)
{
	sci = 0;

	QGridLayout *layout = new QGridLayout(this,2,2);

	txtarea = new QWidget(this,0,WRepaintNoErase|WResizeNoErase);
	txtarea -> setMouseTracking(TRUE);
	txtarea -> setAcceptDrops(TRUE);
	layout -> addWidget(txtarea,0,0);

	vsb = new QScrollBar(Vertical,this);
	layout -> addWidget(vsb,0,1);
	connect(vsb,SIGNAL(valueChanged(int)),SLOT(handleVSb(int)));

	hsb = new QScrollBar(Horizontal,this);
	layout -> addWidget(hsb,1,0);
	connect(hsb,SIGNAL(valueChanged(int)),SLOT(handleHSb(int)));

	txtarea -> installEventFilter(this);

	setFocusPolicy(WheelFocus);

	sci = new ScintillaQt(this);

	SendScintilla(SCI_SETCARETPERIOD,QApplication::cursorFlashTime() / 2);

	// Make sure the lexers are linked in.
	if (!lexersLinked)
	{
		Scintilla_LinkLexers();

		lexersLinked = TRUE;
	}

	QClipboard *cb = QApplication::clipboard();

	if (cb -> supportsSelection())
		connect(cb,SIGNAL(selectionChanged()),SLOT(handleSelection()));

	// Add it to the pool.
	poolList.append(this);
}


// The dtor.
QextScintillaBase::~QextScintillaBase()
{
	// Remove it from the pool.
	poolList.remove(this);

	delete sci;
}


// Return an instance from the pool.
QextScintillaBase *QextScintillaBase::pool()
{
	return poolList.first();
}


// Send a message to the real Scintilla widget using the low level Scintilla
// API.
long QextScintillaBase::SendScintilla(unsigned int msg,unsigned long wParam,
				      long lParam)
{
	return sci -> WndProc(msg,wParam,lParam);
}


// Send a message to the real Scintilla widget that needs a TextRange
// structure.
long QextScintillaBase::SendScintilla(unsigned int msg,long cpMin,long cpMax,
				      char *lpstrText)
{
	TextRange tr;

	tr.chrg.cpMin = cpMin;
	tr.chrg.cpMax = cpMax;
	tr.lpstrText = lpstrText;

	return sci -> WndProc(msg,0,reinterpret_cast<long>(&tr));
}


// Send a message to the real Scintilla widget that needs a RangeToFormat
// structure.
long QextScintillaBase::SendScintilla(unsigned int msg,unsigned long wParam,
				      QPainter *hdc,const QRect &rc,
				      long cpMin,long cpMax)
{
	RangeToFormat rf;

	rf.hdc = rf.hdcTarget = reinterpret_cast<SurfaceID>(hdc);

	rf.rc.left = rc.left();
	rf.rc.top = rc.top();
	rf.rc.right = rc.right() + 1;
	rf.rc.bottom = rc.bottom() + 1;

	rf.chrg.cpMin = cpMin;
	rf.chrg.cpMax = cpMax;

	return sci -> WndProc(msg,wParam,reinterpret_cast<long>(&rf));
}


// Send a message to the real Scintilla widget that needs a colour.
long QextScintillaBase::SendScintilla(unsigned int msg,unsigned long wParam,
				      const QColor &col)
{
	long lParam = (col.blue() << 16) | (col.green() << 8) | col.red();

	return sci -> WndProc(msg,wParam,lParam);
}


// Send a message to the real Scintilla widget that needs a colour.
long QextScintillaBase::SendScintilla(unsigned int msg,const QColor &col)
{
	unsigned long wParam = (col.blue() << 16) | (col.green() << 8) | col.red();

	return sci -> WndProc(msg,wParam,0);
}


// Handle events on behalf of the text area.
bool QextScintillaBase::eventFilter(QObject *o,QEvent *e)
{
	if (o != txtarea)
		return QWidget::eventFilter(o,e);

	bool used = TRUE;

	switch (e -> type())
	{
	case QEvent::Paint:
		sci -> paintEvent(static_cast<QPaintEvent *>(e));
		break;

	case QEvent::Resize:
		sci -> ChangeSize();
		break;

	case QEvent::MouseButtonPress:
		mousePress(static_cast<QMouseEvent *>(e));
		break;

	case QEvent::MouseButtonRelease:
		mouseRelease(static_cast<QMouseEvent *>(e));
		break;

	case QEvent::MouseButtonDblClick:
		mouseDoubleClick(static_cast<QMouseEvent *>(e));
		break;

	case QEvent::MouseMove:
		mouseMove(static_cast<QMouseEvent *>(e));
		break;

	case QEvent::Wheel:
		mouseWheel(static_cast<QWheelEvent *>(e));
		break;

	case QEvent::ContextMenu:
		contextMenu(static_cast<QContextMenuEvent *>(e));
		break;

	case QEvent::DragEnter:
		sci -> dragEnterEvent(static_cast<QDragEnterEvent *>(e));
		break;

	case QEvent::DragMove:
		sci -> dragMoveEvent(static_cast<QDragMoveEvent *>(e));
		break;

	case QEvent::DragLeave:
		sci -> dragLeaveEvent(static_cast<QDragLeaveEvent *>(e));
		break;

	case QEvent::Drop:
		sci -> dropEvent(static_cast<QDropEvent *>(e));
		break;

	default:
		used = FALSE;
	}

	return used;
}


// Handle the timer on behalf of the ScintillaQt instance.
void QextScintillaBase::handleTimer()
{
	sci -> Tick();
}


// Handle the context menu on behalf of the ScintillaQt instance.
void QextScintillaBase::handlePopUp(int cmd)
{
	sci -> Command(cmd);
}


// Re-implemented to tell the widget it has the focus.
void QextScintillaBase::focusInEvent(QFocusEvent *)
{
	sci -> SetFocusState(true);
}


// Re-implemented to tell the widget it has lost the focus.
void QextScintillaBase::focusOutEvent(QFocusEvent *)
{
	sci -> SetFocusState(false);
}


// Handle a mouse button press.
void QextScintillaBase::mousePress(QMouseEvent *me)
{
	setFocus();

	Point pt(me -> x(),me -> y());

	switch (me -> button())
	{
	case LeftButton:
		{
			// Make sure Scintilla won't interpret this as a
			// double-click.
			sci -> lastClickTime = 0;
			unsigned thisClickTime = Platform::DoubleClickTime() + 1;

			bool shift = me -> state() & ShiftButton;
			bool ctrl = me -> state() & ControlButton;
			bool alt = me -> state() & AltButton;

			sci -> ButtonDown(pt,thisClickTime,shift,ctrl,alt);
			break;
		}

	case MidButton:
		{
			QClipboard *cb = QApplication::clipboard();

			if (cb -> supportsSelection())
			{
				cb -> setSelectionMode(TRUE);

				int pos = sci -> PositionFromLocation(pt);

				sci -> SetSelection(pos,pos);
				sci -> Paste();

				cb -> setSelectionMode(FALSE);
			}

			break;
		}

	default:
		break;
	}
}


// Handle a context menu event.
void QextScintillaBase::contextMenu(QContextMenuEvent *cme)
{
	contextMenuEvent(cme);

	if (!cme -> isConsumed())
		sci -> ContextMenu(Point(cme -> globalX(),cme -> globalY()));
}


// Handle a mouse button releases.
void QextScintillaBase::mouseRelease(QMouseEvent *me)
{
	if (sci -> HaveMouseCapture() && me -> button() == LeftButton)
	{
		bool ctrl = me -> state() & ControlButton;

		sci -> ButtonUp(Point(me -> x(),me -> y()),0,ctrl);
	}
}


// Handle a mouse move.
void QextScintillaBase::mouseMove(QMouseEvent *me)
{
	sci -> ButtonMove(Point(me -> x(),me -> y()));
}


// Handle a mouse wheel event.
void QextScintillaBase::mouseWheel(QWheelEvent *we)
{
	setFocus();

	if (we -> orientation() == Horizontal || we -> state() & ShiftButton)
		QApplication::sendEvent(hsb,we);
	else if (we -> orientation() == Vertical)
		QApplication::sendEvent(vsb,we);
}


// Handle a mouse button double click.
void QextScintillaBase::mouseDoubleClick(QMouseEvent *me)
{
	setFocus();

	if (me -> button() == LeftButton)
	{
		// Make sure Scintilla will interpret this as a double-click.
		sci -> lastClickTime = 0;
		unsigned thisClickTime = Platform::DoubleClickTime() - 1;

		bool shift = me -> state() & ShiftButton;
		bool ctrl = me -> state() & ControlButton;
		bool alt = me -> state() & AltButton;

		sci -> ButtonDown(Point(me -> x(),me -> y()),thisClickTime,shift,ctrl,alt);
	}
}


// Re-implemented to handle key press events.
void QextScintillaBase::keyPressEvent(QKeyEvent *ke)
{
	unsigned key;

	switch (ke -> key())
	{
	case Key_Down:
		key = SCK_DOWN;
		break;

	case Key_Up:
		key = SCK_UP;
		break;

	case Key_Left:
		key = SCK_LEFT;
		break;

	case Key_Right:
		key = SCK_RIGHT;
		break;

	case Key_Home:
		key = SCK_HOME;
		break;

	case Key_End:
		key = SCK_END;
		break;

	case Key_Prior:
		key = SCK_PRIOR;
		break;

	case Key_Next:
		key = SCK_NEXT;
		break;

	case Key_Delete:
		key = SCK_DELETE;
		break;

	case Key_Insert:
		key = SCK_INSERT;
		break;

	case Key_Escape:
		key = SCK_ESCAPE;
		break;

	case Key_Backspace:
		key = SCK_BACK;
		break;

	case Key_Tab:
		key = SCK_TAB;
		break;

	case Key_Return:
		key = SCK_RETURN;
		break;

	default:
		if (sci -> IsUnicodeMode())
		{
			// Work out if the original input was a single ASCII
			// key.
			if (ke -> text().length() == 1)
			{
				if ((key = ke -> text()[0].unicode()) >= 0x80)
					key = 0;
			}
			else
				key = 0;
		}
		else
		{
			key = ke -> ascii();

			if (key >= 0x01 && key <= 0x1f)
				key += 0x40;
		}
	}

	bool consumed = FALSE;

	if (key)
	{
		bool shift = ke -> state() & ShiftButton;
		bool ctrl = ke -> state() & ControlButton;
		bool alt = ke -> state() & AltButton;

		sci -> KeyDown(key,shift,ctrl,alt,&consumed);
	}
	else if (sci -> IsUnicodeMode())
	{
		if (ke -> text().length() > 0 && !ke -> text()[0].isNull())
		{
			QCString s = ke -> text().utf8();

			sci -> AddCharUTF(s.data(),s.length());

			consumed = TRUE;
		}
	}

	if (!consumed)
		ke -> ignore();
}


// Re-implemented to make sure tabs are passed to the editor.
bool QextScintillaBase::focusNextPrevChild(bool)
{
	return false;
}


// Handle the vertical scrollbar.
void QextScintillaBase::handleVSb(int val)
{
	sci -> ScrollTo(val);
}


// Handle the horizontal scrollbar.
void QextScintillaBase::handleHSb(int val)
{
	sci -> HorizontalScrollTo(val);
}


// Return the current prefered size.
QSize QextScintillaBase::sizeHint() const
{
	int height = sci -> vs.lineHeight * sci -> pdoc -> LinesTotal();

	if (sci -> horizontalScrollBarVisible)
		height += hsb -> sizeHint().height();

	return QSize(sci -> scrollWidth,height);
}


// Handle the selection changing.
void QextScintillaBase::handleSelection()
{
	if (!QApplication::clipboard() -> ownsSelection())
		sci -> UnclaimSelection();
}
