/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * e-printable.c: an object printer.
 *
 * Author:
 *   Christopher James Lahey <clahey@helixcode.com>
 *
 * (C) 2000 Helix Code, Inc.
 */
#include <config.h>
#include <gtk/gtksignal.h>
#include "e-util.h"
#include "e-printable.h"

#define EP_CLASS(e) ((EPrintableClass *)((GtkObject *)e)->klass)

#define PARENT_TYPE gtk_object_get_type ()
					  

static GtkObjectClass *e_printable_parent_class;

enum {
	PRINT_PAGE,
	DATA_LEFT,
	RESET,
	HEIGHT,
	WILL_FIT,
	LAST_SIGNAL
};

static guint e_printable_signals [LAST_SIGNAL] = { 0, };

static void
e_printable_class_init (GtkObjectClass *object_class)
{
	EPrintableClass *klass = E_PRINTABLE_CLASS(object_class);
	e_printable_parent_class = gtk_type_class (PARENT_TYPE);
	
	e_printable_signals [PRINT_PAGE] =
		gtk_signal_new ("print_page",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (EPrintableClass, print_page),
				e_marshal_NONE__OBJECT_DOUBLE_DOUBLE_BOOL,
				GTK_TYPE_NONE, 4, GTK_TYPE_OBJECT, GTK_TYPE_DOUBLE, GTK_TYPE_DOUBLE, GTK_TYPE_BOOL);

	e_printable_signals [DATA_LEFT] =
		gtk_signal_new ("data_left",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (EPrintableClass, data_left),
				gtk_marshal_BOOL__NONE,
				GTK_TYPE_BOOL, 0, GTK_TYPE_NONE);

	e_printable_signals [RESET] =
		gtk_signal_new ("reset",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (EPrintableClass, reset),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0, GTK_TYPE_NONE);

	e_printable_signals [HEIGHT] =
		gtk_signal_new ("height",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (EPrintableClass, height),
				e_marshal_DOUBLE__OBJECT_DOUBLE_DOUBLE_BOOL,
				GTK_TYPE_DOUBLE, 4, GTK_TYPE_OBJECT, GTK_TYPE_DOUBLE, GTK_TYPE_DOUBLE, GTK_TYPE_BOOL);

	e_printable_signals [WILL_FIT] =
		gtk_signal_new ("will_fit",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (EPrintableClass, will_fit),
				e_marshal_BOOL__OBJECT_DOUBLE_DOUBLE_BOOL,
				GTK_TYPE_BOOL, 4, GTK_TYPE_OBJECT, GTK_TYPE_DOUBLE, GTK_TYPE_DOUBLE, GTK_TYPE_BOOL);

	gtk_object_class_add_signals (object_class, e_printable_signals, LAST_SIGNAL);

	klass->print_page = NULL;    
	klass->data_left = NULL;
	klass->reset = NULL;
	klass->height = NULL;
	klass->will_fit = NULL;
}


guint
e_printable_get_type (void)
{
  static guint type = 0;

  if (!type)
    {
      GtkTypeInfo info =
      {
	"EPrintable",
	sizeof (EPrintable),
	sizeof (EPrintableClass),
	(GtkClassInitFunc) e_printable_class_init,
	NULL,
	/* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      type = gtk_type_unique (gtk_object_get_type (), &info);
    }

  return type;
}

EPrintable *
e_printable_new(void)
{
	return E_PRINTABLE(gtk_type_new(e_printable_get_type()));
}

void
e_printable_print_page          (EPrintable        *e_printable,
				 GnomePrintContext *context,
				 gdouble            width,
				 gdouble            height,
				 gboolean           quantized)
{
	g_return_if_fail (e_printable != NULL);
	g_return_if_fail (E_IS_PRINTABLE (e_printable));
	
	gtk_signal_emit (GTK_OBJECT (e_printable),
			 e_printable_signals [PRINT_PAGE],
			 context,
			 width,
			 height,
			 quantized);
}

gboolean
e_printable_data_left           (EPrintable        *e_printable)
{
        gboolean ret_val;

	g_return_val_if_fail (e_printable != NULL, FALSE);
	g_return_val_if_fail (E_IS_PRINTABLE (e_printable), FALSE);

	gtk_signal_emit (GTK_OBJECT (e_printable),
			 e_printable_signals [DATA_LEFT],
			 &ret_val);

	return ret_val;
}

void
e_printable_reset               (EPrintable        *e_printable)
{
	g_return_if_fail (e_printable != NULL);
	g_return_if_fail (E_IS_PRINTABLE (e_printable));
	
	gtk_signal_emit (GTK_OBJECT (e_printable),
			 e_printable_signals [RESET]);
}

gdouble
e_printable_height              (EPrintable        *e_printable,
				 GnomePrintContext *context,
				 gdouble            width,
				 gdouble            max_height,
				 gboolean           quantized)
{
        gdouble ret_val;

	g_return_val_if_fail (e_printable != NULL, -1);
	g_return_val_if_fail (E_IS_PRINTABLE (e_printable), -1);

	gtk_signal_emit (GTK_OBJECT (e_printable),
			 e_printable_signals [HEIGHT],
			 context,
			 width,
			 max_height,
			 quantized,
			 &ret_val);

	return ret_val;
}

gboolean
e_printable_will_fit            (EPrintable        *e_printable,
				 GnomePrintContext *context,
				 gdouble            width,
				 gdouble            max_height,
				 gboolean           quantized)
{
        gboolean ret_val;

	g_return_val_if_fail (e_printable != NULL, -1);
	g_return_val_if_fail (E_IS_PRINTABLE (e_printable), -1);

	gtk_signal_emit (GTK_OBJECT (e_printable),
			 e_printable_signals [WILL_FIT],
			 context,
			 width,
			 max_height,
			 quantized,
			 &ret_val);

	return ret_val;
}
