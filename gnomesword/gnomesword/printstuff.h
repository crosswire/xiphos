/***************************************************************************
                          printstuff.h  -  description
                             -------------------
    begin                : 2000-06-08
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef __PRINTSTUFF_H__
#define __PRINTSTUFF_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	PRINT_ORIENT_LANDSCAPE,
	PRINT_ORIENT_PORTRAIT
} PrintOrientation;

void
file_print			(GtkWidget *widget,
					gchar *currentfilename,
					gint file_printpreview);


#ifdef __cplusplus
}
#endif
 
#endif /* __PRINTSTUFF_H__ */