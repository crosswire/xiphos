/***************************************************************************
                          rwphtml.h  -  description
                             -------------------
    begin                : Tue Jun 15 15:34:26 CEST 1999

    copyright            : (C) 1999 by Torsten Uhlmann
    email                : TUhlmann@gmx.de
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

#ifndef GS_RWPHTML_H
#define GS_RWPHTML_H

#include <swfilter.h>

class GS_RWPHTML : public SWFilter {
public:
	GS_RWPHTML();
	virtual char ProcessText(char *text, int maxlen, const SWKey *key);
};

#endif
