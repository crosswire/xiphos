/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
#ifndef _E_ENTRY_H_
#define _E_ENTRY_H_

#include <libgnomeui/gnome-canvas.h>
#include <gtk/gtktable.h>
#include <gnome-xml/tree.h>
#include "e-text.h"

BEGIN_GNOME_DECLS

#define E_ENTRY_TYPE        (e_entry_get_type ())
#define E_ENTRY(o)          (GTK_CHECK_CAST ((o), E_ENTRY_TYPE, EEntry))
#define E_ENTRY_CLASS(k)    (GTK_CHECK_CLASS_CAST((k), E_ENTRY_TYPE, EEntryClass))
#define E_IS_ENTRY(o)       (GTK_CHECK_TYPE ((o), E_ENTRY_TYPE))
#define E_IS_ENTRY_CLASS(k) (GTK_CHECK_CLASS_TYPE ((k), E_ENTRY_TYPE))

typedef struct {
	GtkTable parent;
	
	GnomeCanvas *canvas;
	EText *item;
} EEntry;

typedef struct {
	GtkTableClass parent_class;

	void (* changed) (EEntry *entry);
	void (* activate) (EEntry *entry);
} EEntryClass;

GtkType    e_entry_get_type   		    (void);

EEntry    *e_entry_construct  		    (EEntry *e_entry);
GtkWidget *e_entry_new        		    (void);

END_GNOME_DECLS

#endif /* _E_ENTRY_H_ */
