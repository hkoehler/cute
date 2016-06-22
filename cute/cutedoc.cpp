/***************************************************************************
                          cutedoc.cpp  -  description
                             -------------------
    begin                : Sam Sep 28 23:53:49 CEST 2002
    copyright            : (C) 2002 by Heiko Köhler
    email                : heicom@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "cutedoc.h"

CUTEDoc::CUTEDoc()
{
  modified = false;
}

CUTEDoc::~CUTEDoc()
{
}

void CUTEDoc::newDoc()
{
}

bool CUTEDoc::save()
{
  return true;
}

bool CUTEDoc::saveAs(const QString &filename)
{
  return true;
}

bool CUTEDoc::load(const QString &filename)
{
  emit documentChanged();
  return true;
}

bool CUTEDoc::isModified() const
{
  return modified;
}
