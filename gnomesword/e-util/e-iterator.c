/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Authors: 
 *   Christopher James Lahey <clahey@umich.edu>
 *
 * Copyright (C) 2000 Helix Code, Inc.
 * Copyright (C) 1999 The Free Software Foundation
 */

#include <config.h>
#include <gtk/gtk.h>

#include "e-iterator.h"

#define ECI_CLASS(object) (E_ITERATOR_CLASS(GTK_OBJECT((object))->klass))

static void e_iterator_init (EIterator *card);
static void e_iterator_class_init (EIteratorClass *klass);

#define PARENT_TYPE (gtk_object_get_type ())

static GtkObjectClass *parent_class;

enum {
	INVALIDATE,
	LAST_SIGNAL
};

static guint e_iterator_signals [LAST_SIGNAL] = { 0, };

/**
 * e_iterator_get_type:
 * @void: 
 * 
 * Registers the &EIterator class if necessary, and returns the type ID
 * associated to it.
 * 
 * Return value: The type ID of the &EIterator class.
 **/
GtkType
e_iterator_get_type (void)
{
	static GtkType type = 0;

	if (!type) {
		GtkTypeInfo info = {
			"EIterator",
			sizeof (EIterator),
			sizeof (EIteratorClass),
			(GtkClassInitFunc) e_iterator_class_init,
			(GtkObjectInitFunc) e_iterator_init,
			NULL, /* reserved_1 */
			NULL, /* reserved_2 */
			(GtkClassInitFunc) NULL
		};

		type = gtk_type_unique (PARENT_TYPE, &info);
	}

	return type;
}

static void
e_iterator_class_init (EIteratorClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS(klass);

	parent_class = gtk_type_class (PARENT_TYPE);

	e_iterator_signals [INVALIDATE] =
		gtk_signal_new ("invalidate",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (EIteratorClass, invalidate),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);

	gtk_object_class_add_signals (object_class, e_iterator_signals, LAST_SIGNAL);

	klass->invalidate = NULL;
	klass->get        = NULL;
	klass->reset      = NULL;
	klass->last       = NULL;
	klass->next       = NULL;
	klass->prev       = NULL;
	klass->delete     = NULL;
	klass->insert     = NULL;
	klass->set        = NULL;
	klass->is_valid   = NULL;
}

/**
 * e_iterator_init:
 */
static void
e_iterator_init (EIterator *card)
{
}

/*
 * Virtual functions: 
 */
const void *
e_iterator_get      (EIterator *iterator)
{
	if (ECI_CLASS(iterator)->get)
		return ECI_CLASS(iterator)->get(iterator);
	else
		return NULL;
}

void
e_iterator_reset    (EIterator *iterator)
{
	if (ECI_CLASS(iterator)->reset)
		ECI_CLASS(iterator)->reset(iterator);
}

void
e_iterator_last     (EIterator *iterator)
{
	if (ECI_CLASS(iterator)->last)
		ECI_CLASS(iterator)->last(iterator);
}

gboolean
e_iterator_next     (EIterator *iterator)
{
	if (ECI_CLASS(iterator)->next)
		return ECI_CLASS(iterator)->next(iterator);
	else
		return FALSE;
}

gboolean
e_iterator_prev     (EIterator *iterator)
{
	if (ECI_CLASS(iterator)->prev)
		return ECI_CLASS(iterator)->prev(iterator);
	else
		return FALSE;
}

void
e_iterator_delete   (EIterator *iterator)
{
	if (ECI_CLASS(iterator)->delete)
		ECI_CLASS(iterator)->delete(iterator);
}

void           e_iterator_insert     (EIterator  *iterator,
				      const void *object,
				      gboolean    before)
{
	if (ECI_CLASS(iterator)->insert)
		ECI_CLASS(iterator)->insert(iterator, object, before);
}

void
e_iterator_set      (EIterator *iterator,
			  const void    *object)
{
	if (ECI_CLASS(iterator)->set)
		ECI_CLASS(iterator)->set(iterator, object);
}

gboolean
e_iterator_is_valid (EIterator *iterator)
{
	if (ECI_CLASS(iterator)->is_valid)
		return ECI_CLASS(iterator)->is_valid(iterator);
	else
		return FALSE;
}

void
e_iterator_invalidate (EIterator *iterator)
{
	g_return_if_fail (iterator != NULL);
	g_return_if_fail (E_IS_ITERATOR (iterator));

	gtk_signal_emit (GTK_OBJECT (iterator),
			 e_iterator_signals [INVALIDATE]);
}
