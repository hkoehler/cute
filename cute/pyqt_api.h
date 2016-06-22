/***************************************************************************
                          pyqt_api.h  -  python API to gui objects
                             -------------------
    begin                : Wed, 23 Jun 2004
    copyright            : (C) 2004 by Nick Thompson
    email                : nickthompson@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __PYQT_API_H__
#define __PYQT_API_H__

#undef slots
#include <Python.h>
#define slots

#include <qwidget.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qtextedit.h>

//////////////////////////////////////////////////////////////////
// cuteQWidget
//////////////////////////////////////////////////////////////////

class cuteQWidget {
    PyObject_HEAD
public:
    QWidget *wrapped;
};

//////////////////////////////////////////////////////////////////
// cuteQLayoutItem
//////////////////////////////////////////////////////////////////

class cuteQLayoutItem {
    PyObject_HEAD
public:
    QLayoutItem *wrapped;
};

//////////////////////////////////////////////////////////////////
// cuteQDialog
//////////////////////////////////////////////////////////////////

class cuteQDialog : cuteQWidget {
public:
    static PyObject *newDialog(PyObject *self, PyObject *args);
private:
    //QDialog *wrapped;

    static void dealloc(cuteQDialog *self);
    static PyObject *show(cuteQDialog *self, PyObject *args);
    static PyObject *close(cuteQDialog *self, PyObject *args);
    static PyObject *setMaximumSize(cuteQDialog *self, PyObject *args);
    static PyObject *setMinimumSize(cuteQDialog *self, PyObject *args);
    static PyObject *getattr(PyObject *self, char *name);
    static cuteQDialog *create(QWidget *parent, const char *name, bool modal);

    static struct PyMethodDef methods[];
    static PyTypeObject cuteQDialogType;
};

//////////////////////////////////////////////////////////////////
// cuteQGridLayout
//////////////////////////////////////////////////////////////////

class cuteQGridLayout {
    PyObject_HEAD
public:
    static PyObject *newGridLayout(PyObject *self, PyObject *args);
private:
    QGridLayout *wrapped;

    static void dealloc(cuteQGridLayout *self);
    static PyObject *addWidget(cuteQGridLayout *self, PyObject *args);
    static PyObject *addMultiCellWidget(cuteQGridLayout *self, PyObject *args);
    static PyObject *addItem(cuteQGridLayout *self, PyObject *args);
    static PyObject *getattr(PyObject *self, char *name);
    static cuteQGridLayout *create(QWidget *parent, int nRows, int nCols, int margin, int space, const char *name);

    static struct PyMethodDef methods[];
    static PyTypeObject cuteQGridLayoutType;
};

//////////////////////////////////////////////////////////////////
// cuteQLabel
//////////////////////////////////////////////////////////////////

class cuteQLabel : cuteQWidget {
public:
    static PyObject *newLabel(PyObject *self, PyObject *args);
private:
    //QLabel *wrapped;

    static void dealloc(cuteQLabel *self);
    static PyObject *setText(cuteQLabel *self, PyObject *args);
    static PyObject *getattr(PyObject *self, char *name);
    static cuteQLabel *create(const QString &text, QWidget *parent, const char *name);

    static struct PyMethodDef methods[];
    static PyTypeObject cuteQLabelType;
};

//////////////////////////////////////////////////////////////////
// cuteQPushButton
//////////////////////////////////////////////////////////////////

class cuteQPushButton : cuteQWidget {
public:
    static PyObject *newPushButton(PyObject *self, PyObject *args);
private:
    //QPushButton *wrapped;

    static void dealloc(cuteQPushButton *self);
    static PyObject *setText(cuteQPushButton *self, PyObject *args);
    static PyObject *hook(cuteQPushButton *self, PyObject *args);
    static PyObject *getattr(PyObject *self, char *name);
    static cuteQPushButton *create(const QString &text, QWidget *parent, const char *name);

    static struct PyMethodDef methods[];
    static PyTypeObject cuteQPushButtonType;
};

//////////////////////////////////////////////////////////////////
// cuteQCheckBox
//////////////////////////////////////////////////////////////////

class cuteQCheckBox : cuteQWidget {
public:
    static PyObject *newCheckBox(PyObject *self, PyObject *args);
private:
    //QCheckBox *wrapped;

    static void dealloc(cuteQCheckBox *self);
    static PyObject *setText(cuteQCheckBox *self, PyObject *args);
    static PyObject *isChecked(cuteQCheckBox *self, PyObject *args);
    static PyObject *getattr(PyObject *self, char *name);
    static cuteQCheckBox *create(const QString &text, QWidget *parent, const char *name);

    static struct PyMethodDef methods[];
    static PyTypeObject cuteQCheckBoxType;
};

//////////////////////////////////////////////////////////////////
// cuteQTextEdit
//////////////////////////////////////////////////////////////////

class cuteQTextEdit : cuteQWidget {
public:
    static PyObject *newTextEdit(PyObject *self, PyObject *args);
private:
    //QTextEdit *wrapped;

    static void dealloc(cuteQTextEdit *self);
    static PyObject *text(cuteQTextEdit *self, PyObject *args);
    static PyObject *getattr(PyObject *self, char *name);
    static cuteQTextEdit *create(const QString &text, QWidget *parent, const char *name);

    static struct PyMethodDef methods[];
    static PyTypeObject cuteQTextEditType;
};

//////////////////////////////////////////////////////////////////
// cuteQSpacerItem
//////////////////////////////////////////////////////////////////

class cuteQSpacerItem : cuteQLayoutItem {
public:
    static PyObject *newSpacerItem(PyObject *self, PyObject *args);
private:
    static void dealloc(cuteQSpacerItem *self);
    static PyObject *getattr(PyObject *self, char *name);
    static cuteQSpacerItem *create(int w, int h, QSizePolicy::SizeType hData, QSizePolicy::SizeType vData);

    static struct PyMethodDef methods[];
    static PyTypeObject cuteQSpacerItemType;
};

// new methods for Py/Qt integration
#define PYQT_METHOD_DEF {"newDialog", cuteQDialog::newDialog, METH_VARARGS, "Create a dialog object"},\
    {"newLabel", cuteQLabel::newLabel, METH_VARARGS, "Create a push button object"},\
    {"newPushButton", cuteQPushButton::newPushButton, METH_VARARGS, "Create a push button object"},\
    {"newCheckBox", cuteQCheckBox::newCheckBox, METH_VARARGS, "Create a check box object"},\
    {"newGridLayout", cuteQGridLayout::newGridLayout, METH_VARARGS, "Create a grid layout object"},\
    {"newTextEdit", cuteQTextEdit::newTextEdit, METH_VARARGS, "Create a text edit object"},\
    {"newSpacerItem", cuteQSpacerItem::newSpacerItem, METH_VARARGS, "Create a spacer item object"},

#endif 
