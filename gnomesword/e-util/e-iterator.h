/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Authors:
 *   Chris Lahey <clahey@helixcode.com>
 *
 * Copyright (C) 2000 Helix Code, Inc.
 * Copyright (C) 1999 The Free Software Foundation
 */

#ifndef __E_ITERATOR_H__
#define __E_ITERATOR_H__

#include <time.h>
#include <gtk/gtk.h>
#include <stdio.h>

#define E_TYPE_ITERATOR            (e_iterator_get_type ())
#define E_ITERATOR(obj)            (GTK_CHECK_CAST ((obj), E_TYPE_ITERATOR, EIterator))
#define E_ITERATOR_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), E_TYPE_ITERATOR, EIteratorClass))
#define E_IS_ITERATOR(obj)         (GTK_CHECK_TYPE ((obj), E_TYPE_ITERATOR))
#define E_IS_ITERATOR_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), E_TYPE_ITERATOR))

typedef struct _EIterator EIterator;
typedef struct _EIteratorClass EIteratorClass;

struct _EIterator {
	GtkObject object;
};

struct _EIteratorClass {
	GtkObjectClass parent_class;

	/* Signals */
	void         (*invalidate) (EIterator  *iterator);
	
	/* Virtual functions */
	const void * (*get)        (EIterator  *iterator);
	void         (*reset)      (EIterator  *iterator);
	void         (*last)       (EIterator  *iterator);
	gboolean     (*next)       (EIterator  *iterator);
	gboolean     (*prev)       (EIterator  *iterator);
	void         (*delete)     (EIterator  *iterator);
	void         (*insert)     (EIterator  *iterator,
				    const void *object,
				    gboolean   	before);
	void         (*set)        (EIterator  *iterator,
				    const void *object);
	gboolean     (*is_valid)   (EIterator  *iterator);
};

const void    *e_iterator_get        (EIterator  *iterator);
void           e_iterator_reset      (EIterator  *iterator);
void           e_iterator_last       (EIterator  *iterator);
gboolean       e_iterator_next       (EIterator  *iterator);
gboolean       e_iterator_prev       (EIterator  *iterator);
void           e_iterator_delete     (EIterator  *iterator);
void           e_iterator_insert     (EIterator  *iterator,
				      const void *object,
				      gboolean    before);
void           e_iterator_set        (EIterator  *iterator, 
				      const void *object);
gboolean       e_iterator_is_valid   (EIterator  *iterator);

void           e_iterator_invalidate (EIterator  *iterator);

/* Standard Gtk function */
GtkType        e_iterator_get_type   (void);

#endif /* ! __E_ITERATOR_H__ */
