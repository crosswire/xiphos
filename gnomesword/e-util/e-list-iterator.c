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

#include "e-list-iterator.h"
#include "e-list.h"

static void        e_list_iterator_init       (EListIterator *list);
static void        e_list_iterator_class_init (EListIteratorClass *klass);
		   
static void        e_list_iterator_invalidate (EIterator *iterator);
static gboolean    e_list_iterator_is_valid   (EIterator *iterator);
static void        e_list_iterator_set        (EIterator  *iterator,
	           			       const void *object);
static void        e_list_iterator_delete     (EIterator  *iterator);
static void        e_list_iterator_insert     (EIterator  *iterator,
		   			       const void *object,
		   			       gboolean    before);
static gboolean    e_list_iterator_prev       (EIterator  *iterator);
static gboolean    e_list_iterator_next       (EIterator  *iterator);
static void        e_list_iterator_reset      (EIterator *iterator);
static void        e_list_iterator_last       (EIterator *iterator);
static const void *e_list_iterator_get        (EIterator *iterator);
static void        e_list_iterator_destroy    (GtkObject *object);

#define PARENT_TYPE (e_iterator_get_type ())

static GtkObjectClass *parent_class;
#define PARENT_CLASS (E_LIST_ITERATOR_CLASS(parent_class))

/**
 * e_list_iterator_get_type:
 * @void: 
 * 
 * Registers the &EListIterator class if necessary, and returns the type ID
 * associated to it.
 * 
 * Return value: The type ID of the &EListIterator class.
 **/
GtkType
e_list_iterator_get_type (void)
{
	static GtkType type = 0;

	if (!type) {
		GtkTypeInfo info = {
			"EListIterator",
			sizeof (EListIterator),
			sizeof (EListIteratorClass),
			(GtkClassInitFunc) e_list_iterator_class_init,
			(GtkObjectInitFunc) e_list_iterator_init,
			NULL, /* reserved_1 */
			NULL, /* reserved_2 */
			(GtkClassInitFunc) NULL
		};

		type = gtk_type_unique (PARENT_TYPE, &info);
	}

	return type;
}

static void
e_list_iterator_class_init (EListIteratorClass *klass)
{
	GtkObjectClass *object_class;
	EIteratorClass *iterator_class;

	object_class = GTK_OBJECT_CLASS(klass);
	iterator_class = E_ITERATOR_CLASS(klass);

	parent_class = gtk_type_class (PARENT_TYPE);

	object_class->destroy = e_list_iterator_destroy;

	iterator_class->invalidate = e_list_iterator_invalidate;
	iterator_class->get        = e_list_iterator_get;
	iterator_class->reset      = e_list_iterator_reset;
	iterator_class->last       = e_list_iterator_last;
	iterator_class->next       = e_list_iterator_next;
	iterator_class->prev       = e_list_iterator_prev;
	iterator_class->delete     = e_list_iterator_delete;
	iterator_class->insert     = e_list_iterator_insert;
	iterator_class->set        = e_list_iterator_set;
	iterator_class->is_valid   = e_list_iterator_is_valid;
}



/**
 * e_list_iterator_init:
 */
static void
e_list_iterator_init (EListIterator *list)
{
}

EIterator *
e_list_iterator_new (EList *list)
{
	EListIterator *iterator = gtk_type_new(e_list_iterator_get_type());

	iterator->list = list;
	gtk_object_ref(GTK_OBJECT(list));
	iterator->iterator = list->list;

	return E_ITERATOR(iterator);
}

/*
 * Virtual functions: 
 */
static void
e_list_iterator_destroy (GtkObject *object)
{
	EListIterator *iterator = E_LIST_ITERATOR(object);
	e_list_remove_iterator(iterator->list, E_ITERATOR(iterator));
	gtk_object_unref(GTK_OBJECT(iterator->list));
}

static const void *
e_list_iterator_get      (EIterator *_iterator)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	if (iterator->iterator)
		return iterator->iterator->data;
	else
		return NULL;
}

static void
e_list_iterator_reset    (EIterator *_iterator)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	iterator->iterator = iterator->list->list;
}

static void
e_list_iterator_last     (EIterator *_iterator)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	iterator->iterator = g_list_last(iterator->list->list);
}

static gboolean
e_list_iterator_next     (EIterator *_iterator)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	if (iterator->iterator)
		iterator->iterator = g_list_next(iterator->iterator);
	return (iterator->iterator != NULL);
}

static gboolean
e_list_iterator_prev     (EIterator *_iterator)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	if (iterator->iterator)
		iterator->iterator = g_list_previous(iterator->iterator);
	return (iterator->iterator != NULL);
}

static void
e_list_iterator_insert   (EIterator  *_iterator,
			  const void *object,
			  gboolean    before)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	void *data;
	if (iterator->list->copy)
		data = iterator->list->copy(object, iterator->list->closure);
	else
		data = (void *) object;
	if (iterator->iterator) {
		if (before) {
			iterator->list->list = g_list_first(g_list_prepend(iterator->iterator, data));
			iterator->iterator = iterator->iterator->prev;
		} else {
			if (iterator->iterator->next)
				g_list_prepend(iterator->iterator->next, data);
			else
				g_list_append(iterator->iterator, data);
			iterator->iterator = iterator->iterator->next;
		}
		e_list_invalidate_iterators(iterator->list, E_ITERATOR(iterator));
	} else {
		if (before) {
			iterator->list->list = g_list_append(iterator->list->list, data);
			iterator->iterator = g_list_last(iterator->list->list);
		} else {
			iterator->list->list = g_list_prepend(iterator->list->list, data);
			iterator->iterator = iterator->list->list;
		}
		e_list_invalidate_iterators(iterator->list, E_ITERATOR(iterator));
	}
}

static void
e_list_iterator_delete   (EIterator *_iterator)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	if (iterator->iterator) {
		GList *temp = iterator->iterator->next;
		if (iterator->list->free)
			iterator->list->free(iterator->iterator->data, iterator->list->closure);
		iterator->list->list = g_list_remove_link(iterator->list->list, iterator->iterator);
		iterator->iterator = temp;
		e_list_invalidate_iterators(iterator->list, E_ITERATOR(iterator));
	}
}

static void
e_list_iterator_set      (EIterator  *_iterator,
			  const void *object)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	if (iterator->iterator) {
		if (iterator->list->free)
			iterator->list->free(iterator->iterator->data, iterator->list->closure);
		if (iterator->list->copy)
			iterator->iterator->data = iterator->list->copy(object, iterator->list->closure);
		else
			iterator->iterator->data = (void *) object;
	}
}

static gboolean
e_list_iterator_is_valid (EIterator *_iterator)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	return iterator->iterator != NULL;
}

static void
e_list_iterator_invalidate (EIterator *_iterator)
{
	EListIterator *iterator = E_LIST_ITERATOR(_iterator);
	iterator->iterator = NULL;
}
