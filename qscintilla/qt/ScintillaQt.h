// The definition of the Qt specific subclass of ScintillaBase.
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


#ifndef SCINTILLAQT_H
#define	SCINTILLAQT_H


#include <qtimer.h>

// These are needed because scintilla class header files don't seem to manage
// their own dependencies properly.
#include <ctype.h>
#include <stdlib.h>
#include "Platform.h"
#include "Scintilla.h"
#include "SVector.h"
#include "CellBuffer.h"
#include "Document.h"
#include "Style.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Indicator.h"
#include "ViewStyle.h"
#include "KeyMap.h"
#include "ContractionState.h"
#include "Editor.h"
#include "AutoComplete.h"
#include "CallTip.h"
#include "SString.h"
#include "PropSet.h"
#include "Accessor.h"
#include "KeyWords.h"

#include "ScintillaBase.h"


class QPaintEvent;
class QextScintillaBase;
class QtCallTip;


class ScintillaQt : public ScintillaBase
{
	friend class QextScintillaBase;
	friend class QtCallTip;

public:
	ScintillaQt(QextScintillaBase *qsb_);
	virtual ~ScintillaQt();

	virtual sptr_t WndProc(unsigned int iMessage,uptr_t wParam,
			       sptr_t lParam);

private:
	QWidget *PWindow(Window &w)
	{
		return reinterpret_cast<QWidget *>(w.GetID());
	}

	void Initialise();
	void Finalise();
	void StartDrag();
	sptr_t DefWndProc(unsigned int,uptr_t,sptr_t);
	void SetTicking(bool);
	void SetMouseCapture(bool on);
	bool HaveMouseCapture();
	void SetVerticalScrollPos();
	void SetHorizontalScrollPos();
	bool ModifyScrollBars(int nMax,int nPage);
	void ReconfigureScrollBars();
	void NotifyChange();
	void NotifyParent(SCNotification scn);
	int KeyDefault(int key,int modifiers);
	void CopyToClipboard(const SelectionText &selectedText);
	void Copy();
	void Paste();
	void CreateCallTipWindow(PRectangle rc);
	void AddToPopUp(const char *label,int cmd = 0,bool enabled = true);
	void ClaimSelection();
	void UnclaimSelection();
	static sptr_t DirectFunction(ScintillaQt *sci,unsigned int iMessage,
				     uptr_t wParam,sptr_t lParam);

	QString textRange(const SelectionText *text);
	void paintEvent(QPaintEvent *pe);
	void dragEnterEvent(QDragEnterEvent *dee);
	void dragMoveEvent(QDragMoveEvent *dme);
	void dragLeaveEvent(QDragLeaveEvent *dle);
	void dropEvent(QDropEvent *de);

	bool capturedMouse;
	QextScintillaBase *qsb;
	QTimer qtimer;
};

#endif
