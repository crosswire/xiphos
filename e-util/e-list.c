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

#include "e-list.h"
#include "e-list-iterator.h"

#define ECL_CLASS(object) (E_LIST_CLASS(GTK_OBJECT((object))->klass))

static void e_list_init (EList *list);
static void e_list_class_init (EListClass *klass);
static void e_list_destroy (GtkObject *object);

#define PARENT_TYPE (gtk_object_get_type ())

static GtkObjectClass *parent_class;

/**
 * e_list_get_type:
 * @void: 
 * 
 * Registers the &EList class if necessary, and returns the type ID
 * associated to it.
 * 
 * Return value: The type ID of the &EList class.
 **/
GtkType
e_list_get_type (void)
{
	static GtkType type = 0;

	if (!type) {
		GtkTypeInfo info = {
			"EList",
			sizeof (EList),
			sizeof (EListClass),
			(GtkClassInitFunc) e_list_class_init,
			(GtkObjectInitFunc) e_list_init,
			NULL, /* reserved_1 */
			NULL, /* reserved_2 */
			(GtkClassInitFunc) NULL
		};

		type = gtk_type_unique (PARENT_TYPE, &info);
	}

	return type;
}

static void
e_list_class_init (EListClass *klass)
{
	GtkObjectClass *object_class;

	object_class = GTK_OBJECT_CLASS(klass);

	parent_class = gtk_type_class (PARENT_TYPE);

	object_class->destroy = e_list_destroy;
}

/**
 * e_list_init:
 */
static void
e_list_init (EList *list)
{
	list->list = NULL;
	list->iterators = NULL;
}

EList *
e_list_new          (EListCopyFunc copy, EListFreeFunc free, void *closure)
{
	EList *list = gtk_type_new(e_list_get_type());
	list->copy    = copy;
	list->free    = free;
	list->closure = closure;
	return list;
}

EList *
e_list_duplicate    (EList *old)
{
	EList *list = gtk_type_new(e_list_get_type());

	list->copy    = old->copy;
	list->free    = old->free;
	list->closure = old->closure;
	list->list    = g_list_copy(old->list);
	if (list->copy) {
		GList *listlist;
		for (listlist = list->list; listlist; listlist = listlist->next) {
			listlist->data = list->copy (listlist->data, list->closure);
		}
	}
	return list;
}

EIterator *
e_list_get_iterator (EList *list)
{
	EIterator *iterator = e_list_iterator_new(list);
	list->iterators = g_list_append(list->iterators, iterator);
	return iterator;
}

int
e_list_length (EList *list)
{
	return g_list_length(list->list);
}

void
e_list_append       (EList *list, const void *data)
{
	e_list_invalidate_iterators(list, NULL);
	if (list->copy)
		list->list = g_list_append(list->list, list->copy(data, list->closure));
	else
		list->list = g_list_append(list->list, (void *) data);
}

void
e_list_invalidate_iterators (EList *list, EIterator *skip)
{
	GList *iterators = list->iterators;
	for (; iterators; iterators = iterators->next) {
		if (iterators->data != skip) {
			e_iterator_invalidate(E_ITERATOR(iterators->data));
		}
	}
}

void
e_list_remove_iterator (EList *list, EIterator *iterator)
{
	list->iterators = g_list_remove(list->iterators, iterator);
}

/* 
 * Virtual functions 
 */
static void
e_list_destroy (GtkObject *object)
{
	EList *list = E_LIST(object);
	g_list_foreach(list->list, (GFunc) list->free, list->closure);
	g_list_free(list->list);
}
