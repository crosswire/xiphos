/***************************************************************************
                          thmlhtml.h  -  description
                             -------------------
    begin                : 1999-10-28

    copyright            : 1999 by Chris Little
    email                : chrislit@chiasma.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SW_THMLHTML_H
#define SW_THMLHTML_H

#include <swfilter.h>

#include <defs.h>

  /** this filter converts ThML text to HTML text
  */
class SWDLLEXPORT SW_ThMLHTML:public SWFilter
{
public:
  SW_ThMLHTML ();
  virtual char ProcessText (char *text, int maxlen = -1);
};

#endif
