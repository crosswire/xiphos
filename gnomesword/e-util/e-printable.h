/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * e-printable.h: an object printer.
 *
 * Author:
 *   Christopher James Lahey <clahey@helixcode.com>
 *
 * (C) 2000 Helix Code, Inc.
 */
#ifndef _E_PRINTABLE_H_
#define _E_PRINTABLE_H_

#include <gtk/gtkobject.h>
#include <libgnomeprint/gnome-print.h>

#define E_PRINTABLE_TYPE        (e_printable_get_type ())
#define E_PRINTABLE(o)          (GTK_CHECK_CAST ((o), E_PRINTABLE_TYPE, EPrintable))
#define E_PRINTABLE_CLASS(k)    (GTK_CHECK_CLASS_CAST((k), E_PRINTABLE_TYPE, EPrintableClass))
#define E_IS_PRINTABLE(o)       (GTK_CHECK_TYPE ((o), E_PRINTABLE_TYPE))
#define E_IS_PRINTABLE_CLASS(k) (GTK_CHECK_CLASS_TYPE ((k), E_PRINTABLE_TYPE))

typedef struct {
	GtkObject   base;
} EPrintable;

typedef struct {
	GtkObjectClass parent_class;

	/*
	 * Signals
	 */

	void        (*print_page)  (EPrintable *etm, GnomePrintContext *context, gdouble width, gdouble height, gboolean quantized);
	gboolean    (*data_left)   (EPrintable *etm);
	void        (*reset)       (EPrintable *etm);
	gdouble     (*height)      (EPrintable *etm, GnomePrintContext *context, gdouble width, gdouble max_height, gboolean quantized);

	/* e_printable_will_fit (ep, ...) should be equal in value to
	 * (e_printable_print_page (ep, ...),
	 * !e_printable_data_left(ep)) except that the latter has the
	 * side effect of doing the printing and advancing the
	 * position of the printable.
	 */

	gboolean    (*will_fit)    (EPrintable *etm, GnomePrintContext *context, gdouble width, gdouble max_height, gboolean quantized);
} EPrintableClass;

GtkType     e_printable_get_type (void);

EPrintable *e_printable_new                 (void);
	
/*
 * Routines for emitting signals on the e_table */
void        e_printable_print_page          (EPrintable        *e_printable,
					     GnomePrintContext *context,
					     gdouble            width,
					     gdouble            height,
					     gboolean           quantized);
gboolean    e_printable_data_left           (EPrintable        *e_printable);
void        e_printable_reset               (EPrintable        *e_printable);
gdouble     e_printable_height              (EPrintable        *e_printable,
					     GnomePrintContext *context,
					     gdouble            width,
					     gdouble            max_height,
					     gboolean           quantized);
gboolean    e_printable_will_fit            (EPrintable        *e_printable,
					     GnomePrintContext *context,
					     gdouble            width,
					     gdouble            max_height,
					     gboolean           quantized);

#endif /* _E_PRINTABLE_H_ */
