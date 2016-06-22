// The implementation of the Qt specific subclass of ScintillaBase.
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
#include <qscrollbar.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qtimer.h>
#include <qclipboard.h>
#include <qdragobject.h>
#include <qpainter.h>

#include "qextscintillabase.h"
#include "ScintillaQt.h"


// We want to use the Scintilla notification names as Qt signal names.
#undef	SCEN_CHANGE
#undef	SCN_CALLTIPCLICK
#undef	SCN_CHARADDED
#undef	SCN_DOUBLECLICK
#undef	SCN_DWELLEND
#undef	SCN_DWELLSTART
#undef	SCN_HOTSPOTCLICK
#undef	SCN_HOTSPOTDOUBLECLICK
#undef	SCN_MACRORECORD
#undef	SCN_MARGINCLICK
#undef	SCN_MODIFIED
#undef	SCN_MODIFYATTEMPTRO
#undef	SCN_NEEDSHOWN
#undef	SCN_PAINTED
#undef	SCN_SAVEPOINTLEFT
#undef	SCN_SAVEPOINTREACHED
#undef	SCN_STYLENEEDED
#undef	SCN_UPDATEUI
#undef	SCN_USERLISTSELECTION
#undef	SCN_ZOOM

enum
{
	SCEN_CHANGE = 768,
	SCN_CALLTIPCLICK = 2021,
	SCN_CHARADDED = 2001,
	SCN_DOUBLECLICK = 2006,
	SCN_DWELLEND = 2017,
	SCN_DWELLSTART = 2016,
	SCN_HOTSPOTCLICK = 2019,
	SCN_HOTSPOTDOUBLECLICK = 2020,
	SCN_MACRORECORD = 2009,
	SCN_MARGINCLICK = 2010,
	SCN_MODIFIED = 2008,
	SCN_MODIFYATTEMPTRO = 2004,
	SCN_NEEDSHOWN = 2011,
	SCN_PAINTED = 2013,
	SCN_SAVEPOINTLEFT = 2003,
	SCN_SAVEPOINTREACHED = 2002,
	SCN_STYLENEEDED = 2000,
	SCN_UPDATEUI = 2007,
	SCN_USERLISTSELECTION = 2014,
	SCN_ZOOM = 2018
};


// The ctor.
ScintillaQt::ScintillaQt(QextScintillaBase *qsb_) :
				capturedMouse(false), qsb(qsb_)
{
	wMain = qsb -> txtarea;

	// We aren't a QObject so we use the API class to do QObject related
	// things for us.
	qsb -> connect(&qtimer,SIGNAL(timeout()),SLOT(handleTimer()));
	
	Initialise();
}


// The dtor.
ScintillaQt::~ScintillaQt()
{ 
}


// Initialise the instance.
void ScintillaQt::Initialise()
{
	SetTicking(true);
}


// Tidy up the instance.
void ScintillaQt::Finalise()
{
	SetTicking(false);
	ScintillaBase::Finalise();
}


// Start a drag.
void ScintillaQt::StartDrag()
{
	QDragObject *dobj = new QTextDrag(textRange(&drag),qsb -> txtarea);

	// Remove the dragged text if it was a move to another widget or
	// application.
	if (dobj -> drag() && dobj -> target() != qsb -> txtarea)
		ClearSelection();

	inDragDrop = false;
	SetDragPosition(invalidPosition);
}


// Handle a drag enter event.
void ScintillaQt::dragEnterEvent(QDragEnterEvent *dee)
{
	dragMoveEvent(dee);
}


// Handle a drag move event.
void ScintillaQt::dragMoveEvent(QDragMoveEvent *dme)
{
	dme -> acceptAction(QTextDrag::canDecode(dme));
	SetDragPosition(PositionFromLocation(Point(dme -> pos().x(),dme -> pos().y())));
}


// Handle a drag leave event.
void ScintillaQt::dragLeaveEvent(QDragLeaveEvent *dle)
{
	SetDragPosition(invalidPosition);
}


// Handle a drop event.
void ScintillaQt::dropEvent(QDropEvent *de)
{
	QString text;

	if (QTextDrag::decode(de,text))
	{
		bool moving = (de -> source() == qsb -> txtarea && de -> action() == QDropEvent::Move);

		de -> acceptAction();

		const char *s;
		QCString us;

		if (IsUnicodeMode())
		{
			us = text.utf8();
			s = us.data();
		}
		else
			s = text.latin1();

		DropAt(posDrop,s,moving,false);
		SetDragPosition(invalidPosition);
		Redraw();
	}
}


// Re-implement to trap certain messages.
sptr_t ScintillaQt::WndProc(unsigned int iMessage,uptr_t wParam,sptr_t lParam)
{
	switch (iMessage)
	{
	case SCI_GRABFOCUS:
		PWindow(wMain) -> setFocus();
		return 0;

	case SCI_GETDIRECTFUNCTION:
		return reinterpret_cast<sptr_t>(DirectFunction);
	
	case SCI_GETDIRECTPOINTER:
		return reinterpret_cast<sptr_t>(this);
	}

	return ScintillaBase::WndProc(iMessage,wParam,lParam);
}


// Windows nonsense.
sptr_t ScintillaQt::DefWndProc(unsigned int,uptr_t,sptr_t)
{
	return 0;
}


// Manage the timer.
void ScintillaQt::SetTicking(bool on)
{
	if (timer.ticking != on)
	{
		timer.ticking = on;

		if (timer.ticking)
			qtimer.start(timer.tickSize);
		else
			qtimer.stop();
	}

	timer.ticksToWait = caret.period;
}


// Grab or release the mouse (and keyboard).
void ScintillaQt::SetMouseCapture(bool on)
{
	if (mouseDownCaptures)
		if (on)
			PWindow(wMain) -> grabMouse();
		else
			PWindow(wMain) -> releaseMouse();

	capturedMouse = on;
}


// Return true if the mouse/keyboard are currently grabbed.
bool ScintillaQt::HaveMouseCapture()
{
	return capturedMouse;
}


// Set the position of the vertical scrollbar.
void ScintillaQt::SetVerticalScrollPos()
{
	qsb -> vsb -> setValue(topLine);
}


// Set the position of the horizontal scrollbar.
void ScintillaQt::SetHorizontalScrollPos()
{
	qsb -> hsb -> setValue(xOffset);
}


// Set the extent of the vertical and horizontal scrollbars and return true if
// the view needs re-drawing.
bool ScintillaQt::ModifyScrollBars(int nMax,int nPage)
{
	qsb -> vsb -> setMinValue(0);
	qsb -> vsb -> setMaxValue(nMax - nPage + 1);
	qsb -> vsb -> setLineStep(1);
	qsb -> vsb -> setPageStep(nPage);

	qsb -> hsb -> setMinValue(0);
	qsb -> hsb -> setMaxValue(scrollWidth);
	qsb -> hsb -> setPageStep(scrollWidth / 10);

	return true;
}


// Called after SCI_SETWRAPMODE and SCI_SETHSCROLLBAR.
void ScintillaQt::ReconfigureScrollBars()
{
	// Hide or show the scrollbars if needed.
	if (horizontalScrollBarVisible)
		qsb -> hsb -> show();
	else
		qsb -> hsb -> hide();

	if (verticalScrollBarVisible)
		qsb -> vsb -> show();
	else
		qsb -> vsb -> hide();
}


// Notify interested parties of any change in the document.
void ScintillaQt::NotifyChange()
{
	emit qsb -> SCEN_CHANGE();
}


// Notify interested parties of various events.  This is the main mapping
// between Scintilla notifications and Qt signals.
void ScintillaQt::NotifyParent(SCNotification scn)
{
	switch (scn.nmhdr.code)
	{
	case SCN_CALLTIPCLICK:
		emit qsb -> SCN_CALLTIPCLICK(scn.position);
		break;

	case SCN_CHARADDED:
		emit qsb -> SCN_CHARADDED(scn.ch);
		break;

	case SCN_DOUBLECLICK:
		emit qsb -> SCN_DOUBLECLICK();
		break;

	case SCN_DWELLEND:
		emit qsb -> SCN_DWELLEND(scn.position,scn.x,scn.y);
		break;

	case SCN_DWELLSTART:
		emit qsb -> SCN_DWELLSTART(scn.position,scn.x,scn.y);
		break;

	case SCN_HOTSPOTCLICK:
		emit qsb -> SCN_HOTSPOTCLICK(scn.position,scn.modifiers);
		break;

	case SCN_HOTSPOTDOUBLECLICK:
		emit qsb -> SCN_HOTSPOTDOUBLECLICK(scn.position,scn.modifiers);
		break;

	case SCN_MACRORECORD:
		emit qsb -> SCN_MACRORECORD(scn.message,scn.wParam,scn.lParam);
		break;

	case SCN_MARGINCLICK:
		emit qsb -> SCN_MARGINCLICK(scn.position,scn.modifiers,
					    scn.margin);
		break;

	case SCN_MODIFIED:
		emit qsb -> SCN_MODIFIED(scn.position,scn.modificationType,
					 scn.text,scn.length,scn.linesAdded,
					 scn.line,scn.foldLevelNow,
					 scn.foldLevelPrev);
		break;

	case SCN_MODIFYATTEMPTRO:
		emit qsb -> SCN_MODIFYATTEMPTRO();
		break;

	case SCN_NEEDSHOWN:
		emit qsb -> SCN_NEEDSHOWN(scn.position,scn.length);
		break;

	case SCN_PAINTED:
		emit qsb -> SCN_PAINTED();
		break;

	case SCN_SAVEPOINTLEFT:
		emit qsb -> SCN_SAVEPOINTLEFT();
		break;

	case SCN_SAVEPOINTREACHED:
		emit qsb -> SCN_SAVEPOINTREACHED();
		break;

	case SCN_STYLENEEDED:
		emit qsb -> SCN_STYLENEEDED(scn.position);
		break;

	case SCN_UPDATEUI:
		emit qsb -> SCN_UPDATEUI();
		break;

	case SCN_USERLISTSELECTION:
		emit qsb -> SCN_USERLISTSELECTION(scn.text,scn.wParam);
		break;

	case SCN_ZOOM:
		emit qsb -> SCN_ZOOM();
		break;

	default:
		qWarning("Unknown notification: %u",scn.nmhdr.code);
	}
}


// Handle a key that hasn't been filtered out as a command key.  Return 0 if we
// haven't handled it.
int ScintillaQt::KeyDefault(int key,int modifiers)
{
	if (modifiers & (SCI_CTRL | SCI_ALT))
		return 0;

	AddChar(key);

	return 1;
}


// Convert a text range to a QString.
QString ScintillaQt::textRange(const SelectionText *text)
{
	QString qs;

	if (text -> s)
		if (IsUnicodeMode())
			qs = QString::fromUtf8(text -> s);
		else
			qs.setLatin1(text -> s);

	return qs;
}


// Copy the selected text to the clipboard.
void ScintillaQt::CopyToClipboard(const SelectionText &selectedText)
{
	QApplication::clipboard() -> setText(textRange(&selectedText));
}


// Implement copy.
void ScintillaQt::Copy()
{
	if (currentPos != anchor)
	{
		SelectionText text;

		CopySelectionRange(&text);
		CopyToClipboard(text);
	}
}


// Implement paste.
void ScintillaQt::Paste()
{
	QString str = QApplication::clipboard() -> text();

	if (str.isEmpty())
		return;

	pdoc -> BeginUndoAction();

	ClearSelection();

	int len;

	if (IsUnicodeMode())
	{
		QCString s = str.utf8();

		len = s.length();

		if (len)
			pdoc -> InsertString(currentPos,s.data(),len);
	}
	else
	{
		const char *s = str.latin1();

		len = (s ? strlen(s) : 0);

		if (len)
			pdoc -> InsertString(currentPos,s,len);
	}

	SetEmptySelection(currentPos + len);

	pdoc -> EndUndoAction();

	NotifyChange();
	Redraw();
}


// A simple QWidget sub-class to implement a call tip.  No need to bother with
// all the moc stuff.
class QtCallTip : public QWidget
{
public:
	QtCallTip(QWidget *parent,ScintillaQt *sci_);
	~QtCallTip();

protected:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *me);

private:
	ScintillaQt *sci;
};


// Create a call tip.
QtCallTip::QtCallTip(QWidget *parent,ScintillaQt *sci_) :
		QWidget(parent,0,WType_Popup|WStyle_Customize|WStyle_NoBorder), sci(sci_)
{
	// Ensure that the main window keeps the focus (and the caret flashing)
	// when this is displayed.
	setFocusProxy(parent);
}


// Destroy a call tip.
QtCallTip::~QtCallTip()
{
	// Ensure that the main window doesn't receive a focus out event when
	// this is destroyed.
	setFocusProxy(0);
}


// Paint a call tip.
void QtCallTip::paintEvent(QPaintEvent *)
{
	Surface *surfaceWindow = Surface::Allocate();

	if (surfaceWindow)
	{
		QPainter p(this);

		surfaceWindow -> Init(&p,0);
		sci -> ct.PaintCT(surfaceWindow);
		surfaceWindow -> Release();

		delete surfaceWindow;
	}
}


// Handle a mouse press in a call tip.
void QtCallTip::mousePressEvent(QMouseEvent *me)
{
	Point pt;

	pt.x = me -> x();
	pt.y = me -> y();

	sci -> ct.MouseClick(pt);
	sci -> CallTipClick();
}


// Create a call tip window.
void ScintillaQt::CreateCallTipWindow(PRectangle rc)
{
	if (!ct.wCallTip.Created())
		ct.wCallTip = ct.wDraw = new QtCallTip(qsb,this);

	PWindow(ct.wCallTip) -> resize(rc.right - rc.left,rc.bottom - rc.top);
	ct.wCallTip.Show();
}


// Add an item to the right button menu.
void ScintillaQt::AddToPopUp(const char *label,int cmd,bool enabled)
{
	QPopupMenu *pm = static_cast<QPopupMenu *>(popup.GetID());

	if (label[0] != '\0')
	{
		QString tr_label = qApp -> translate("ContextMenu",label);

		pm -> insertItem(tr_label,qsb,SLOT(handlePopUp(int)),0,cmd);
		pm -> setItemEnabled(cmd,enabled);
	}
	else
		pm -> insertSeparator();
}


// Claim the selection.
void ScintillaQt::ClaimSelection()
{
	bool isSel = (currentPos != anchor);

	if (isSel)
	{
		QClipboard *cb = QApplication::clipboard();

		// If we support X11 style selection then make it available
		// now.
		if (cb -> supportsSelection())
		{
			SelectionText text;

			CopySelectionRange(&text);

			if (text.s)
			{
				cb -> setSelectionMode(TRUE);
				cb -> setText(text.s);
				cb -> setSelectionMode(FALSE);
			}
		}

		primarySelection = true;
	}
	else
		primarySelection = false;

	emit qsb -> QSCN_SELCHANGED(isSel);
}


// Unclaim the selection.
void ScintillaQt::UnclaimSelection()
{
	if (primarySelection)
	{
		primarySelection = false;
		qsb -> txtarea -> update();
	}
}


// Implemented to provide compatibility with the Windows version.
sptr_t ScintillaQt::DirectFunction(ScintillaQt *sciThis,unsigned int iMessage,
				   uptr_t wParam,sptr_t lParam)
{
	return sciThis -> WndProc(iMessage,wParam,lParam);
}


// Draw the contents of the widget.
void ScintillaQt::paintEvent(QPaintEvent *pe)
{
	bool isUnicodeMode = (pdoc && pdoc -> dbcsCodePage == SC_CP_UTF8);

	paintState = painting;

	const QRect &qr = pe -> rect();

	rcPaint.left = qr.left();
	rcPaint.top = qr.top();
	rcPaint.right = qr.right() + 1;
	rcPaint.bottom = qr.bottom() + 1;

	PRectangle rcText = GetTextRectangle();
	paintingAllText = rcPaint.Contains(rcText);

	Surface *sw = Surface::Allocate();

	if (sw)
	{
		QPainter painter(PWindow(wMain));

		sw -> Init(&painter,0);
		sw -> SetUnicodeMode(isUnicodeMode);
		Paint(sw,rcPaint);
		sw -> Release();
		delete sw;

		// If the painting area was insufficient to cover the new style
		// or brace highlight positions then repaint the whole thing.
		if (paintState == paintAbandoned)
			PWindow(wMain) -> update();
	}

	paintState = notPainting;
}
