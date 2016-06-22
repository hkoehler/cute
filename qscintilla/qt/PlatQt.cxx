// This module implements the portability layer for the Qt port of Scintilla.
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


#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <qapplication.h>
#include <qwidget.h>
#include <qfont.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qlistbox.h>
#include <qpopupmenu.h>
#include <qdatetime.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qlibrary.h>

#include "Platform.h"
#include "XPM.h"

#include "qextscintillabase.h"


// Type convertors.
static QFont *PFont(FontID id)
{
	return reinterpret_cast<QFont *>(id);
}

static QPainter *PSurface(SurfaceID id)
{
	return reinterpret_cast<QPainter *>(id);
}

static QWidget *PWindow(WindowID id)
{
	return reinterpret_cast<QWidget *>(id);
}

static QPopupMenu *PMenu(MenuID id)
{
	return reinterpret_cast<QPopupMenu *>(id);
}


// Create a Point instance from a long value.
Point Point::FromLong(long lpoint)
{
	return Point(Platform::LowShortFromLong(lpoint),
		     Platform::HighShortFromLong(lpoint));
}


// Colour palette management.  The Qt interface to colours means this class
// doesn't have to do anything.
Palette::Palette()
{
	used = 0;
	allowRealization = false;
}

Palette::~Palette()
{
	Release();
}

void Palette::Release()
{
	used = 0;
}

void Palette::WantFind(ColourPair &cp,bool want)
{
	if (!want)
		cp.allocated.Set(cp.desired.AsLong());
}

void Palette::Allocate(Window &)
{
}


// Font management.
Font::Font() : id(0)
{
}

Font::~Font()
{
}

void Font::Create(const char *faceName,int,int size,bool bold,bool italic,bool)
{
	Release();

	QFont *f = new QFont();

	// If name of the font begins with a '-', assume, that it is an XLFD.
	if (faceName[0] == '-')
		f -> setRawName(faceName);
	else
	{
		f -> setFamily(faceName);
		f -> setPointSize(size);
		f -> setBold(bold);
		f -> setItalic(italic);
	}

	id = f;
}

void Font::Release()
{
	if (id)
	{
		delete PFont(id);
		id = 0;
	}
}


// A surface abstracts a place to draw.
class SurfaceImpl : public Surface
{
public:
	SurfaceImpl();
	virtual ~SurfaceImpl();

	void Init(WindowID);
	void Init(SurfaceID sid,WindowID);
	void InitPixMap(int width,int height,Surface *surface_,WindowID);

	void Release();
	bool Initialised() {return painter;}
	void PenColour(ColourAllocated fore);
	int LogPixelsY() {return 72;}
	int DeviceHeightFont(int points) {return points;}
	void MoveTo(int x_,int y_) {painter -> moveTo(x_,y_);}
	void LineTo(int x_,int y_) {painter -> lineTo(x_,y_);}
	void Polygon(Point *pts,int npts,ColourAllocated fore,
		     ColourAllocated back);
	void RectangleDraw(PRectangle rc,ColourAllocated fore,
			   ColourAllocated back);
	void FillRectangle(PRectangle rc,ColourAllocated back);
	void FillRectangle(PRectangle rc,Surface &surfacePattern);
	void RoundedRectangle(PRectangle rc,ColourAllocated fore,
			      ColourAllocated back);
	void Ellipse(PRectangle rc,ColourAllocated fore,ColourAllocated back);
	void Copy(PRectangle rc,Point from,Surface &surfaceSource);

	void DrawTextNoClip(PRectangle rc,Font &font_,int ybase,const char *s,
			    int len,ColourAllocated fore,ColourAllocated back);
	void DrawTextClipped(PRectangle rc,Font &font_,int ybase,const char *s,
			     int len,ColourAllocated fore,
			     ColourAllocated back);
	void DrawTextTransparent(PRectangle rc,Font &font_,int ybase,
				 const char *s,int len,ColourAllocated fore);
	void MeasureWidths(Font &font_,const char *s,int len,int *positions);
	int WidthText(Font &font_,const char *s,int len);
	int WidthChar(Font &font_,char ch);
	int Ascent(Font &font_);
	int Descent(Font &font_);
	int InternalLeading(Font &font_) {return 0;}
	int ExternalLeading(Font &font_);
	int Height(Font &font_);
	int AverageCharWidth(Font &font_) {return WidthChar(font_,'n');}

	int SetPalette(Palette *,bool) {return 0;}
	void SetClip(PRectangle rc);
	void FlushCachedState() {painter -> flush();}

	void SetUnicodeMode(bool unicodeMode_) {unicodeMode = unicodeMode_;}
	void SetDBCSMode(int codePage) {}

	void DrawXPM(PRectangle rc,const XPM *xpm);

private:
	void commonInit(QPainter *painter_,bool mypainter_);
	bool setFont(Font &font_);
	QString convertText(const char *s,int len);
	QColor convertColour(const ColourAllocated &col);

	bool unicodeMode;
	bool mypainter;
	QPainter *painter;
};

Surface *Surface::Allocate()
{
	return new SurfaceImpl;
}

SurfaceImpl::SurfaceImpl() : unicodeMode(false), mypainter(false), painter(0)
{
}

SurfaceImpl::~SurfaceImpl()
{
	Release();
}

void SurfaceImpl::commonInit(QPainter *painter_,bool mypainter_)
{
	Release();

	painter = painter_;
	mypainter = mypainter_;
}

void SurfaceImpl::Init(WindowID)
{
	commonInit(new QPainter(new QWidget()),true);
}

void SurfaceImpl::Init(SurfaceID sid,WindowID)
{
	commonInit(PSurface(sid),false);
}

void SurfaceImpl::InitPixMap(int width,int height,Surface *,WindowID)
{
	commonInit(new QPainter(new QPixmap(width,height)),true);
}

void SurfaceImpl::Release()
{
	if (painter && mypainter)
	{
		QPaintDevice *pd = painter -> device();

		delete painter;

		delete pd;
	}

	painter = 0;
}

void SurfaceImpl::PenColour(ColourAllocated fore)
{
	painter -> setPen(convertColour(fore));
}

void SurfaceImpl::Polygon(Point *pts,int npts,ColourAllocated fore,
			  ColourAllocated back)
{
	QPointArray qpts(npts);

	for (int i = 0; i < npts; ++i)
		qpts.setPoint(i,pts[i].x,pts[i].y);

	painter -> setPen(convertColour(fore));
	painter -> setBrush(convertColour(back));
	painter -> drawPolygon(qpts);
}

void SurfaceImpl::RectangleDraw(PRectangle rc,ColourAllocated fore,
				ColourAllocated back)
{
	painter -> setPen(convertColour(fore));
	painter -> setBrush(convertColour(back));
	painter -> drawRect(rc.left,rc.top,
			    rc.right - rc.left,rc.bottom - rc.top);
}

void SurfaceImpl::FillRectangle(PRectangle rc,ColourAllocated back)
{
	painter -> setPen(Qt::NoPen);
	painter -> setBrush(convertColour(back));
	painter -> drawRect(rc.left,rc.top,
			    rc.right - rc.left,rc.bottom - rc.top);
}

void SurfaceImpl::FillRectangle(PRectangle rc,Surface &surfacePattern)
{
	SurfaceImpl &si = static_cast<SurfaceImpl &>(surfacePattern);
	QPixmap *pm = static_cast<QPixmap *>(si.painter -> device());

	if (pm)
	{
		QBrush brsh(Qt::black,*pm);

		painter -> setBrush(brsh);
		painter -> drawRect(rc.left,rc.top,
				 rc.right - rc.left,rc.bottom - rc.top);
	}
	else
		FillRectangle(rc,ColourAllocated(0));
}

void SurfaceImpl::RoundedRectangle(PRectangle rc,ColourAllocated fore,
				   ColourAllocated back)
{
	painter -> setPen(convertColour(fore));
	painter -> setBrush(convertColour(back));
	painter -> drawRoundRect(rc.left,rc.top,
			      rc.right - rc.left,rc.bottom - rc.top);
}

void SurfaceImpl::Ellipse(PRectangle rc,ColourAllocated fore,
			  ColourAllocated back)
{
	painter -> setPen(convertColour(fore));
	painter -> setBrush(convertColour(back));
	painter -> drawEllipse(rc.left,rc.top,
			    rc.right - rc.left,rc.bottom - rc.top);
}

void SurfaceImpl::Copy(PRectangle rc,Point from,Surface &surfaceSource)
{
	SurfaceImpl &si = static_cast<SurfaceImpl &>(surfaceSource);
	QPaintDevice *spd = si.painter -> device();
	QPaintDevice *dpd = painter -> device();

	if (spd && dpd)
	{
		si.painter -> end();
		painter -> end();

		bitBlt(dpd,rc.left,rc.top,spd,from.x,from.y,
		       rc.right - rc.left,rc.bottom - rc.top);

		si.painter -> begin(spd);
		painter -> begin(dpd);
	}
}

void SurfaceImpl::DrawTextNoClip(PRectangle rc,Font &font_,int ybase,
				 const char *s,int len,ColourAllocated fore,
				 ColourAllocated back)
{
	FillRectangle(rc,back);
	DrawTextTransparent(rc,font_,ybase,s,len,fore);
}

void SurfaceImpl::DrawTextClipped(PRectangle rc,Font &font_,int ybase,
				  const char *s,int len,ColourAllocated fore,
				  ColourAllocated back)
{
	SetClip(rc);
	DrawTextNoClip(rc,font_,ybase,s,len,fore,back);
	painter -> setClipping(FALSE);
}

void SurfaceImpl::DrawTextTransparent(PRectangle rc,Font &font_,int ybase,
				      const char *s,int len,
				      ColourAllocated fore)
{
	QString qs = convertText(s,len);

	setFont(font_);
	painter -> setPen(convertColour(fore));
	painter -> drawText(rc.left,ybase,qs);
}

void SurfaceImpl::DrawXPM(PRectangle rc,const XPM *xpm)
{
	int x, y;
	const QPixmap &qpm = xpm -> Pixmap();

	x = rc.left + (rc.Width() - qpm.width()) / 2;
	y = rc.top + (rc.Height() - qpm.height()) / 2;

	painter -> drawPixmap(x,y,qpm);
}

void SurfaceImpl::MeasureWidths(Font &font_,const char *s,int len,
				int *positions)
{
	if (setFont(font_))
	{
		int totalWidth = 0, ui = 0;
		QString qs = convertText(s,len);
		QFontMetrics fm = painter -> fontMetrics();

		for (int i = 0; i < qs.length(); ++i)
		{
			totalWidth += fm.width(qs[i]);

			int l = (unicodeMode ? QString(qs[i]).utf8().length() : 1);

			while (l--)
				positions[ui++] = totalWidth;
		}
	}
	else
		for (int i = 0; i < len; ++i)
			positions[i] = i + 1;
}

int SurfaceImpl::WidthText(Font &font_,const char *s,int len)
{
	if (setFont(font_))
	{
		QString qs = convertText(s,len);

		return painter -> fontMetrics().width(qs,qs.length());
	}

	return 1;
}

int SurfaceImpl::WidthChar(Font &font_,char ch)
{
	if (setFont(font_))
		return painter -> fontMetrics().width(ch);

	return 1;
}

int SurfaceImpl::Ascent(Font &font_)
{
	if (setFont(font_))
		return painter -> fontMetrics().ascent();

	return 1;
}

int SurfaceImpl::Descent(Font &font_)
{
	// Qt doesn't include the baseline in the descent, so add it.

	if (setFont(font_))
		return painter -> fontMetrics().descent() + 1;

	return 1;
}

int SurfaceImpl::ExternalLeading(Font &font_)
{
	if (setFont(font_))
		return painter -> fontMetrics().leading();

	return 0;
}

int SurfaceImpl::Height(Font &font_)
{
	if (setFont(font_))
		return painter -> fontMetrics().height();

	return 1;
}

void SurfaceImpl::SetClip(PRectangle rc)
{
	painter -> setClipRect(rc.left,rc.top,
			       rc.right - rc.left,rc.bottom - rc.top);
}

// Set the painter font if there is one.  Return true if it was set.
bool SurfaceImpl::setFont(Font &font_)
{
	QFont *f = PFont(font_.GetID());

	if (f)
		painter -> setFont(*f);

	return f;
}

// Convert a Scintilla string to a Qt Unicode string.
QString SurfaceImpl::convertText(const char *s,int len)
{
	if (unicodeMode)
		return QString::fromUtf8(s,len);

	QString qs;

	qs.setLatin1(s,len);

	return qs;
}

// Convert a Scintilla colour to a Qt colour.
QColor SurfaceImpl::convertColour(const ColourAllocated &col)
{
	long c = col.AsLong();

	return QColor(c & 0xff,(c >> 8) & 0xff,(c >> 16) & 0xff);
}


// Window (widget) management.
Window::~Window()
{
}

void Window::Destroy()
{
	QWidget *w = PWindow(id);

	if (w)
	{
		delete w;
		id = 0;
	}
}

bool Window::HasFocus()
{
	return PWindow(id) -> hasFocus();
}

PRectangle Window::GetPosition()
{
	QWidget *w = PWindow(id);

	// Before any size allocated pretend its big enough not to be scrolled.
	PRectangle rc(0,0,5000,5000);

	if (w)
	{
		const QRect &r = w -> geometry();

		rc.left = r.left();
		rc.top = r.top();
		rc.right = r.right() + 1;
		rc.bottom = r.bottom() + 1;
	}

	return rc;
}

void Window::SetPosition(PRectangle rc)
{
	PWindow(id) -> setGeometry(rc.left,rc.top,
				   rc.right - rc.left,rc.bottom - rc.top);
}

void Window::SetPositionRelative(PRectangle rc,Window relativeTo)
{
	QWidget *rel = PWindow(relativeTo.id);
	QPoint pos = rel -> mapToGlobal(rel -> pos());

	int x = pos.x() + rc.left;
	int y = pos.y() + rc.top;

	PWindow(id) -> setGeometry(x,y,rc.right - rc.left,rc.bottom - rc.top);
}

PRectangle Window::GetClientPosition()
{
	return GetPosition();
}

void Window::Show(bool show)
{
	QWidget *w = PWindow(id);

	if (show)
		w -> show();
	else
		w -> hide();
}

void Window::InvalidateAll()
{
	QWidget *w = PWindow(id);

	if (w)
		w -> update();
}

void Window::InvalidateRectangle(PRectangle rc)
{
	QWidget *w = PWindow(id);

	if (w)
		w -> update(rc.left,rc.top,
			    rc.right - rc.left,rc.bottom - rc.top);
}

void Window::SetFont(Font &font)
{
	PWindow(id) -> setFont(*PFont(font.GetID()));
}

void Window::SetCursor(Cursor curs)
{
	Qt::CursorShape qc;

	switch (curs)
	{
	case cursorText:
		qc = Qt::IbeamCursor;
		break;

	case cursorUp:
		qc = Qt::UpArrowCursor;
		break;

	case cursorWait:
		qc = Qt::WaitCursor;
		break;

	case cursorHoriz:
		qc = Qt::SizeHorCursor;
		break;

	case cursorVert:
		qc = Qt::SizeVerCursor;
		break;

	case cursorHand:
		qc = Qt::PointingHandCursor;
		break;

	default:
		qc = Qt::ArrowCursor;
	}

	PWindow(id) -> setCursor(qc);
}

void Window::SetTitle(const char *s)
{
	PWindow(id) -> setCaption(s);
}


// Menu management.
Menu::Menu() : id(0)
{
}

void Menu::CreatePopUp()
{
	Destroy();
	id = new QPopupMenu();
}

void Menu::Destroy()
{
	QPopupMenu *m = PMenu(id);

	if (m)
	{
		delete m;
		id = 0;
	}
}

void Menu::Show(Point pt,Window &)
{
	PMenu(id) -> popup(QPoint(pt.x,pt.y));
}


class DynamicLibraryImpl : public DynamicLibrary
{
public:
	DynamicLibraryImpl(const char *modulePath)
	{
		m = new QLibrary(modulePath);
		m -> load();
	}

	virtual ~DynamicLibraryImpl()
	{
		if (m)
			delete m;
	}

	virtual Function FindFunction(const char *name)
	{
		if (m)
			return m -> resolve(name);

		return 0;
	}

	virtual bool IsValid()
	{
		return m && m -> isLoaded();
	}

private:
	QLibrary* m;
};

DynamicLibrary *DynamicLibrary::Load(const char *modulePath)
{
	return new DynamicLibraryImpl(modulePath);
}


// Elapsed time.  This implementation assumes that the maximum elapsed time is
// less than 48 hours.
ElapsedTime::ElapsedTime()
{
	QTime now = QTime::currentTime();

	bigBit = now.hour() * 60 * 60 + now.minute() * 60 + now.second();
	littleBit = now.msec();
}

double ElapsedTime::Duration(bool reset)
{
	long endBigBit, endLittleBit;
	QTime now = QTime::currentTime();

	endBigBit = now.hour() * 60 * 60 + now.minute() * 60 + now.second();
	endLittleBit = now.msec();

	double duration = endBigBit - bigBit;

	if (duration < 0 || (duration == 0 && endLittleBit < littleBit))
		duration += 24 * 60 * 60;

	duration += (endLittleBit - littleBit) / 1000.0;

	if (reset)
	{
		bigBit = endBigBit;
		littleBit = endLittleBit;
	}

	return duration;
}


// Manage system wide parameters.
ColourDesired Platform::Chrome()
{
	return ColourDesired(0xe0,0xe0,0xe0);
}

ColourDesired Platform::ChromeHighlight()
{
	return ColourDesired(0xff,0xff,0xff);
}

const char *Platform::DefaultFont()
{
	return QApplication::font().family();
}

int Platform::DefaultFontSize()
{
	return QApplication::font().pointSize();
}

unsigned int Platform::DoubleClickTime()
{
	return QApplication::doubleClickInterval();
}

bool Platform::MouseButtonBounce()
{
#if defined(Q_OS_WIN)
	return false;
#else
	return true;
#endif
}

void Platform::DebugDisplay(const char *s)
{
	qDebug("%s",s);
}

bool Platform::IsKeyDown(int)
{
	return false;
}

long Platform::SendScintilla(WindowID w,unsigned int msg,unsigned long wParam,
			     long lParam)
{
	return static_cast<QextScintillaBase *>(PWindow(w) -> parentWidget()) -> SendScintilla(msg,wParam,lParam);
}

long Platform::SendScintillaPointer(WindowID w,unsigned int msg,
				    unsigned long wParam,void *lParam)
{
	return static_cast<QextScintillaBase *>(PWindow(w) -> parentWidget()) -> SendScintilla(msg,wParam,reinterpret_cast<long>(lParam));
}

bool Platform::IsDBCSLeadByte(int codepage,char ch)
{
	// We don't support DBCS.
	return false;
}

int Platform::DBCSCharLength(int codePage,const char *s)
{
	// We don't support DBCS.
	return 1;
}

int Platform::DBCSCharMaxLength()
{
	// We don't support DBCS.
	return 2;
}

int Platform::Minimum(int a,int b)
{
	return (a < b) ? a : b;
}

int Platform::Maximum(int a,int b)
{
	return (a > b) ? a : b;
}

int Platform::Clamp(int val,int minVal,int maxVal)
{
	if (val > maxVal)
		val = maxVal;

	if (val < minVal)
		val = minVal;

	return val;
}


//#define TRACE

#ifdef TRACE
void Platform::DebugPrintf(const char *format, ...)
{
	char buffer[2000];
	va_list pArguments;

	va_start(pArguments,format);
	vsprintf(buffer,format,pArguments);
	va_end(pArguments);

	DebugDisplay(buffer);
}
#else
void Platform::DebugPrintf(const char *, ...)
{
}
#endif

static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_)
{
	bool ret = assertionPopUps;

	assertionPopUps = assertionPopUps_;

	return ret;
}

void Platform::Assert(const char *c,const char *file,int line)
{
	qFatal("Assertion [%s] failed at %s %d\n",c,file,line);
}
