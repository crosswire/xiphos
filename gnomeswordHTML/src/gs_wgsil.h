/***************************************************************************
                          gs_wgsil.h  -  description
                             -------------------
    begin                : Sat. April 27, 2001
    copyright            : (C) 2001 Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* $Header$ */
/* $Revision$ */

#ifndef GS_WGSIL_H
#define GS_WGSIL_H

#include <swfilter.h>

#include <defs.h>

  /** This filter converts GBF Text into HTML
  */
class SWDLLEXPORT GS_WGSIL:public SWFilter
{
public:
  GS_WGSIL ();
  virtual char ProcessText (char *text, int maxlen, const SWKey * key);
};

#endif
