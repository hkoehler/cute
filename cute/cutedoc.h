/***************************************************************************
                          cutedoc.h  -  description
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
#ifndef CUTEDOC_H
#define CUTEDOC_H

// include files for QT
#include <qobject.h>

// application specific includes

/**
  * the Document Class
  */

class CUTEDoc : public QObject
{
  Q_OBJECT

  public:
    CUTEDoc();
    ~CUTEDoc();
    void newDoc();
    bool save();
    bool saveAs(const QString &filename);
    bool load(const QString &filename);
    bool isModified() const;

  signals:
    void documentChanged();

  protected:
    bool modified;
};

#endif
