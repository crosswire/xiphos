/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* ETextModel - Text item for evolution.
 * Copyright (C) 2000 Helix Code, Inc.
 *
 * Author: Chris Lahey <clahey@umich.edu>
 *
 * A majority of code taken from:
 *
 * Text item type for GnomeCanvas widget
 *
 * GnomeCanvas is basically a port of the Tk toolkit's most excellent
 * canvas widget.  Tk is copyrighted by the Regents of the University
 * of California, Sun Microsystems, and other parties.
 *
 * Copyright (C) 1998 The Free Software Foundation
 *
 * Author: Federico Mena <federico@nuclecu.unam.mx> */

#ifndef E_TEXT_MODEL_H
#define E_TEXT_MODEL_H

#include <gnome.h>


BEGIN_GNOME_DECLS

#define E_TYPE_TEXT_MODEL            (e_text_model_get_type ())
#define E_TEXT_MODEL(obj)            (GTK_CHECK_CAST ((obj), E_TYPE_TEXT_MODEL, ETextModel))
#define E_TEXT_MODEL_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), E_TYPE_TEXT_MODEL, ETextModelClass))
#define E_IS_TEXT_MODEL(obj)         (GTK_CHECK_TYPE ((obj), E_TYPE_TEXT_MODEL))
#define E_IS_TEXT_MODEL_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), E_TYPE_TEXT_MODEL))

typedef struct _ETextModel ETextModel;
typedef struct _ETextModelClass ETextModelClass;

struct _ETextModel {
	GtkObject item;

	char *text;			/* Text to display */
	int length;
};

struct _ETextModelClass {
	GtkObjectClass parent_class;

	/* Signal */
	void  (* changed)   (ETextModel *model);

	/* Virtual methods */
	char *(* get_text)    	(ETextModel *model);
	void  (* set_text)    	(ETextModel *model, gchar *text);
	void  (* insert)      	(ETextModel *model, gint position, gchar *text);
	void  (* insert_length) (ETextModel *model, gint position, gchar *text, gint length);
	void  (* delete)        (ETextModel *model, gint position, gint length);
};


/* Standard Gtk function */
GtkType e_text_model_get_type (void);
ETextModel *e_text_model_new(void);

void e_text_model_changed(ETextModel *model);
gchar *e_text_model_get_text(ETextModel *model);
void e_text_model_set_text(ETextModel *model, gchar *text);
void e_text_model_insert(ETextModel *model, gint position, gchar *text);
void e_text_model_insert_length(ETextModel *model, gint position, gchar *text, gint length);
void e_text_model_delete(ETextModel *model, gint position, gint length);


END_GNOME_DECLS

#endif
