/***************************************************************************
                          searchstuff.h  -  description
                             -------------------
    begin                : Wed Jul 26 2000
    copyright            : (C) 2000 by Terry Biggs
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
 
 
#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include "gs_gnomesword.h"

GList*    /* search Bible text or commentaries */
searchSWORD (GtkWidget *widget, SETTINGS *s);


#ifdef __cplusplus
}
#endif

