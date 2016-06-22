/***************************************************************************
                          pyqt_api.cpp  -  python API to gui objects
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
#include "Python.h"
#include <qapplication.h>
#include <qsizepolicy.h>
#include "cutehook.h"
#include "pyqt_api.h"

// default python type definition
// assumes fixed dealloc and getattr method names
#define TYPEDEF(x) PyTypeObject x::x ## Type = { \
    PyObject_HEAD_INIT(&PyType_Type) \
    0,          /*ob_size*/ \
    #x,         /*tp_name*/ \
    sizeof(x),  /*tp_basicsize*/ \
    0,          /*tp_itemsize*/ \
    /* methods */ \
    (destructor)dealloc, /*tp_dealloc*/ \
    0,          /*tp_print*/ \
    getattr,    /*tp_getattr*/ \
    0,          /*tp_setattr*/ \
    0,          /*tp_compare*/ \
    0,          /*tp_repr*/ \
    0,          /*tp_as_number*/ \
    0,          /*tp_as_sequence*/ \
    0,          /*tp_as_mapping*/ \
    0,          /*tp_hash*/ \
}

//////////////////////////////////////////////////////////////////
// cuteQDialog
//////////////////////////////////////////////////////////////////

void cuteQDialog::dealloc(cuteQDialog *self)
{
    delete self->wrapped;
    PyMem_DEL(self);
}

/* methods */
PyObject *cuteQDialog::show(cuteQDialog *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) return NULL;
    self->wrapped->show();
    return Py_BuildValue("");
}

PyObject *cuteQDialog::close(cuteQDialog *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) return NULL;
    self->wrapped->close();
    return Py_BuildValue("");
}

PyObject *cuteQDialog::setMaximumSize(cuteQDialog *self, PyObject *args)
{
    int width, height;
    if (!PyArg_ParseTuple(args, "ii", &width, &height)) return NULL;
    self->wrapped->setMaximumSize(width, height);
    return Py_BuildValue("");
}

PyObject *cuteQDialog::setMinimumSize(cuteQDialog *self, PyObject *args)
{
    int width, height;
    if (!PyArg_ParseTuple(args, "ii", &width, &height)) return NULL;
    self->wrapped->setMinimumSize(width, height);
    return Py_BuildValue("");
}

struct PyMethodDef cuteQDialog::methods[] = {
    {"show", (PyCFunction)show, METH_VARARGS},
    {"close", (PyCFunction)close, METH_VARARGS},
    {"setMaximumSize", (PyCFunction)setMaximumSize, METH_VARARGS},
    {"setMinimumSize", (PyCFunction)setMinimumSize, METH_VARARGS},
    { NULL, NULL }
};

PyObject *cuteQDialog::getattr(PyObject *self, char *name)
{
    return(Py_FindMethod(methods, self, name));
}

TYPEDEF(cuteQDialog);   /* cuteQDialogType */

cuteQDialog *cuteQDialog::create(QWidget *parent, const char *name, bool modal)
{
    cuteQDialog *self;
    QDialog *dialog;
    self = PyObject_NEW(cuteQDialog, &cuteQDialogType);
    if (self == NULL) return NULL;
    dialog = new QDialog(parent, name, modal, Qt::WStyle_Dialog | Qt::WStyle_Customize | Qt::WStyle_NormalBorder | Qt::WStyle_Title | Qt::WStyle_SysMenu);
    dialog->setModal(modal);
    dialog->setSizeGripEnabled(TRUE);
    self->wrapped = dialog;
    return self;
}

PyObject *cuteQDialog::newDialog(PyObject *self, PyObject *args)
{
    int modal;

    if (!PyArg_ParseTuple(args, "i", &modal)) {
        return NULL;
    }

    return (PyObject *)create(qApp->mainWidget(), NULL, modal);
}

//////////////////////////////////////////////////////////////////
// cuteQGridLayout
//////////////////////////////////////////////////////////////////

void cuteQGridLayout::dealloc(cuteQGridLayout *self)
{
    //delete self->wrapped;
    PyMem_DEL(self);
}

/* methods */
PyObject *cuteQGridLayout::addWidget(cuteQGridLayout *self, PyObject *args)
{
    cuteQWidget *w;
    int row, col;
    if (!PyArg_ParseTuple(args, "Oii", &w, &row, &col)) return NULL;
    ((QGridLayout *)self->wrapped)->addWidget(w->wrapped, row, col);
    return Py_BuildValue("");
}

PyObject *cuteQGridLayout::addMultiCellWidget(cuteQGridLayout *self, PyObject *args)
{
    cuteQWidget *w;
    int fromRow, toRow, fromCol, toCol;
    if (!PyArg_ParseTuple(args, "Oiiii", &w, &fromRow, &toRow, &fromCol, &toCol)) return NULL;
    ((QGridLayout *)self->wrapped)->addMultiCellWidget(w->wrapped, fromRow, toRow, fromCol, toCol);
    return Py_BuildValue("");
}

PyObject *cuteQGridLayout::addItem(cuteQGridLayout *self, PyObject *args)
{
    cuteQLayoutItem *i;
    int row, col;
    if (!PyArg_ParseTuple(args, "Oii", &i, &row, &col)) return NULL;
    ((QGridLayout *)self->wrapped)->addItem(i->wrapped, row, col);
    return Py_BuildValue("");
}

struct PyMethodDef cuteQGridLayout::methods[] = {
    {"addWidget", (PyCFunction)addWidget, METH_VARARGS},
    {"addMultiCellWidget", (PyCFunction)addMultiCellWidget, METH_VARARGS},
    {"addItem", (PyCFunction)addItem, METH_VARARGS},
    { NULL, NULL }
};

PyObject *cuteQGridLayout::getattr(PyObject *self, char *name)
{
    return(Py_FindMethod(methods, self, name));
}

TYPEDEF(cuteQGridLayout);   /* cuteQGridLayoutType */

cuteQGridLayout *cuteQGridLayout::create(QWidget *parent, int nRows, int nCols, int margin, int space, const char *name)
{
    cuteQGridLayout *self;
    self = PyObject_NEW(cuteQGridLayout, &cuteQGridLayoutType);
    if (self == NULL) return NULL;
    self->wrapped = new QGridLayout(parent, nRows, nCols, margin, space, name);
    return self;
}

PyObject *cuteQGridLayout::newGridLayout(PyObject *self, PyObject *args)
{
    cuteQWidget *parent;
    int nRows, nCols, margin, space;

    if (!PyArg_ParseTuple(args, "Oiiii", &parent, &nRows, &nCols, &margin, &space)) {
        return NULL;
    }

    return (PyObject *)create(parent->wrapped, nRows, nCols, margin, space, NULL);
}

//////////////////////////////////////////////////////////////////
// cuteQLabel
//////////////////////////////////////////////////////////////////

void cuteQLabel::dealloc(cuteQLabel *self)
{
    //delete self->wrapped;
    PyMem_DEL(self);
}

/* methods */
PyObject *cuteQLabel::setText(cuteQLabel *self, PyObject *args)
{
    char *text;
    if (!PyArg_ParseTuple(args, "s", &text)) return NULL;
    ((QLabel *)self->wrapped)->setText(text);
    return Py_BuildValue("");
}

struct PyMethodDef cuteQLabel::methods[] = {
    {"setText", (PyCFunction)setText, METH_VARARGS},
    { NULL, NULL }
};

PyObject *cuteQLabel::getattr(PyObject *self, char *name)
{
    return(Py_FindMethod(methods, self, name));
}

TYPEDEF(cuteQLabel); /* cuteQLabelType */

cuteQLabel *cuteQLabel::create(const QString &text, QWidget *parent, const char *name)
{
    cuteQLabel *self;
    QLabel *label;
    self = PyObject_NEW(cuteQLabel, &cuteQLabelType);
    if (self == NULL) return NULL;
    label = new QLabel(text, parent, name);

    self->wrapped = label;    
    return self;
}

PyObject *cuteQLabel::newLabel(PyObject *self, PyObject *args)
{
    cuteQWidget *parent;
    char *text;

    if (!PyArg_ParseTuple(args, "Os", &parent, &text)) {
        return NULL;
    }

    return (PyObject *)create(text, parent->wrapped, NULL);
}

//////////////////////////////////////////////////////////////////
// cuteQPushButton
//////////////////////////////////////////////////////////////////

void cuteQPushButton::dealloc(cuteQPushButton *self)
{
    //delete self->wrapped;
    PyMem_DEL(self);
}

/* methods */
PyObject *cuteQPushButton::setText(cuteQPushButton *self, PyObject *args)
{
    char *text;
    if (!PyArg_ParseTuple(args, "s", &text)) return NULL;
    ((QPushButton *)self->wrapped)->setText(text);
    return Py_BuildValue("");
}

PyObject *cuteQPushButton::hook(cuteQPushButton *self, PyObject *args)
{
    PyObject *cmd;
    char *hookType;
    if (!PyArg_ParseTuple(args, "Os", &cmd, &hookType)) return NULL;
    if(!PyCallable_Check(cmd)){
        PyErr_SetString(PyExc_TypeError, "first argument not callable");
        return NULL;
    }
    CUTEHook *pyHook = new CUTEHook(cmd, self->wrapped, NULL);
    if (pyHook->attachHook(hookType, self->wrapped)) {
        delete pyHook;
        pyHook = NULL;
    }
    return PyCObject_FromVoidPtr(pyHook, NULL);
}

struct PyMethodDef cuteQPushButton::methods[] = {
    {"setText", (PyCFunction)setText, METH_VARARGS},
    {"hook", (PyCFunction)hook, METH_VARARGS},
    { NULL, NULL }
};

PyObject *cuteQPushButton::getattr(PyObject *self, char *name)
{
    return(Py_FindMethod(methods, self, name));
}

TYPEDEF(cuteQPushButton); /* cuteQPushButtonType */

cuteQPushButton *cuteQPushButton::create(const QString &text, QWidget *parent, const char *name)
{
    cuteQPushButton *self;
    QPushButton *pushButton;
    self = PyObject_NEW(cuteQPushButton, &cuteQPushButtonType);
    if (self == NULL) return NULL;
    pushButton = new QPushButton(text, parent, name);
    
    CUTEHook::addHookType("clicked", pushButton, SIGNAL(clicked()));

    self->wrapped = pushButton;    
    return self;
}

PyObject *cuteQPushButton::newPushButton(PyObject *self, PyObject *args)
{
    cuteQWidget *parent;
    char *text;

    if (!PyArg_ParseTuple(args, "Os", &parent, &text)) {
        return NULL;
    }

    return (PyObject *)create(text, parent->wrapped, NULL);
}

//////////////////////////////////////////////////////////////////
// cuteQCheckBox
//////////////////////////////////////////////////////////////////

void cuteQCheckBox::dealloc(cuteQCheckBox *self)
{
    //delete self->wrapped;
    PyMem_DEL(self);
}

/* methods */
PyObject *cuteQCheckBox::setText(cuteQCheckBox *self, PyObject *args)
{
    char *text;
    if (!PyArg_ParseTuple(args, "s", &text)) return NULL;
    ((QCheckBox *)self->wrapped)->setText(text);
    return Py_BuildValue("");
}

PyObject *cuteQCheckBox::isChecked(cuteQCheckBox *self, PyObject *args)
{
    if (!PyArg_ParseTuple(args, "")) return NULL;
    return Py_BuildValue("i", (int)((QCheckBox *)self->wrapped)->isChecked());
}

struct PyMethodDef cuteQCheckBox::methods[] = {
    {"setText", (PyCFunction)setText, METH_VARARGS},
    {"isChecked", (PyCFunction)isChecked, METH_VARARGS},
    { NULL, NULL }
};

PyObject *cuteQCheckBox::getattr(PyObject *self, char *name)
{
    return(Py_FindMethod(methods, self, name));
}

TYPEDEF(cuteQCheckBox); /* cuteQCheckBoxType */

cuteQCheckBox *cuteQCheckBox::create(const QString &text, QWidget *parent, const char *name)
{
    cuteQCheckBox *self;
    QCheckBox *checkBox;
    self = PyObject_NEW(cuteQCheckBox, &cuteQCheckBoxType);
    if (self == NULL) return NULL;
    checkBox = new QCheckBox(text, parent, name);

    self->wrapped = checkBox;    
    return self;
}

PyObject *cuteQCheckBox::newCheckBox(PyObject *self, PyObject *args)
{
    cuteQWidget *parent;
    char *text;

    if (!PyArg_ParseTuple(args, "Os", &parent, &text)) {
        return NULL;
    }

    return (PyObject *)create(text, parent->wrapped, NULL);
}

//////////////////////////////////////////////////////////////////
// cuteQTextEdit
//////////////////////////////////////////////////////////////////

void cuteQTextEdit::dealloc(cuteQTextEdit *self)
{
    //delete self->wrapped;
    PyMem_DEL(self);
}

/* methods */
PyObject *cuteQTextEdit::text(cuteQTextEdit *self, PyObject *args)
{
    char *text;
    if (!PyArg_ParseTuple(args, "")) return NULL;
    return Py_BuildValue("s", (const char *)((QTextEdit *)self->wrapped)->text());
}

struct PyMethodDef cuteQTextEdit::methods[] = {
    {"text", (PyCFunction)text, METH_VARARGS},
    { NULL, NULL }
};

PyObject *cuteQTextEdit::getattr(PyObject *self, char *name)
{
    return(Py_FindMethod(methods, self, name));
}

TYPEDEF(cuteQTextEdit); /* cuteQTextEditType */

cuteQTextEdit *cuteQTextEdit::create(const QString &text, QWidget *parent, const char *name)
{
    cuteQTextEdit *self;
    QTextEdit *textEdit;
    self = PyObject_NEW(cuteQTextEdit, &cuteQTextEditType);
    if (self == NULL) return NULL;
    textEdit = new QTextEdit(parent, name);
    //textEdit->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    textEdit->setText(text);
    
    self->wrapped = textEdit;    
    return self;
}

PyObject *cuteQTextEdit::newTextEdit(PyObject *self, PyObject *args)
{
    cuteQWidget *parent;
    char *text;

    if (!PyArg_ParseTuple(args, "Os", &parent, &text)) {
        return NULL;
    }

    return (PyObject *)create(text, parent->wrapped, NULL);
}

//////////////////////////////////////////////////////////////////
// cuteQSpacerItem
//////////////////////////////////////////////////////////////////

void cuteQSpacerItem::dealloc(cuteQSpacerItem *self)
{
    //delete self->wrapped;
    PyMem_DEL(self);
}

/* methods */
struct PyMethodDef cuteQSpacerItem::methods[] = {
    /* none */
    { NULL, NULL }
};

PyObject *cuteQSpacerItem::getattr(PyObject *self, char *name)
{
    return(Py_FindMethod(methods, self, name));
}

TYPEDEF(cuteQSpacerItem); /* cuteQSpacerItemType */

cuteQSpacerItem *cuteQSpacerItem::create(int w, int h, QSizePolicy::SizeType hData, QSizePolicy::SizeType vData)
{
    cuteQSpacerItem *self;
    QSpacerItem *spacerItem;
    self = PyObject_NEW(cuteQSpacerItem, &cuteQSpacerItemType);
    if (self == NULL) return NULL;
    spacerItem = new QSpacerItem(w, h, hData, vData);
    
    self->wrapped = spacerItem;    
    return self;
}

PyObject *cuteQSpacerItem::newSpacerItem(PyObject *self, PyObject *args)
{
    int w, h, hData, vData;

    if (!PyArg_ParseTuple(args, "iiii", &w, &h, &hData, &vData)) {
        return NULL;
    }

    return (PyObject *)create(w, h, (QSizePolicy::SizeType)hData, (QSizePolicy::SizeType)vData);
}
