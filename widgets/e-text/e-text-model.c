/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* ETextModel - Text item model for evolution.
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

#include <config.h>
#include <ctype.h>
#include "e-text-model.h"

enum {
	E_TEXT_MODEL_CHANGED,
	E_TEXT_MODEL_LAST_SIGNAL
};

static guint e_text_model_signals[E_TEXT_MODEL_LAST_SIGNAL] = { 0 };

static void e_text_model_class_init (ETextModelClass *class);
static void e_text_model_init (ETextModel *model);
static void e_text_model_destroy (GtkObject *object);

static gchar *e_text_model_real_get_text(ETextModel *model);
static void e_text_model_real_set_text(ETextModel *model, gchar *text);
static void e_text_model_real_insert(ETextModel *model, gint postion, gchar *text);
static void e_text_model_real_insert_length(ETextModel *model, gint postion, gchar *text, gint length);
static void e_text_model_real_delete(ETextModel *model, gint postion, gint length);

static GtkObject *parent_class;



/**
 * e_text_model_get_type:
 * @void: 
 * 
 * Registers the &ETextModel class if necessary, and returns the type ID
 * associated to it.
 * 
 * Return value: The type ID of the &ETextModel class.
 **/
GtkType
e_text_model_get_type (void)
{
	static GtkType model_type = 0;

	if (!model_type) {
		GtkTypeInfo model_info = {
			"ETextModel",
			sizeof (ETextModel),
			sizeof (ETextModelClass),
			(GtkClassInitFunc) e_text_model_class_init,
			(GtkObjectInitFunc) e_text_model_init,
			NULL, /* reserved_1 */
			NULL, /* reserved_2 */
			(GtkClassInitFunc) NULL
		};

		model_type = gtk_type_unique (gtk_object_get_type (), &model_info);
	}

	return model_type;
}

/* Class initialization function for the text item */
static void
e_text_model_class_init (ETextModelClass *klass)
{
	GtkObjectClass *object_class;

	object_class = (GtkObjectClass *) klass;

	parent_class = gtk_type_class (gtk_object_get_type ());

	e_text_model_signals[E_TEXT_MODEL_CHANGED] =
		gtk_signal_new ("changed",
				GTK_RUN_LAST,
				object_class->type,
				GTK_SIGNAL_OFFSET (ETextModelClass, changed),
				gtk_marshal_NONE__NONE,
				GTK_TYPE_NONE, 0);
	
	gtk_object_class_add_signals (object_class, e_text_model_signals, E_TEXT_MODEL_LAST_SIGNAL);
	
	klass->changed = NULL;
	klass->get_text = e_text_model_real_get_text;
	klass->set_text = e_text_model_real_set_text;
	klass->insert = e_text_model_real_insert;
	klass->insert_length = e_text_model_real_insert_length;
	klass->delete = e_text_model_real_delete;
	
	object_class->destroy = e_text_model_destroy;
}

/* Object initialization function for the text item */
static void
e_text_model_init (ETextModel *model)
{
	model->text = NULL;
}

/* Destroy handler for the text item */
static void
e_text_model_destroy (GtkObject *object)
{
	ETextModel *model;

	g_return_if_fail (object != NULL);
	g_return_if_fail (E_IS_TEXT_MODEL (object));

	model = E_TEXT_MODEL (object);

	if (model->text)
		g_free (model->text);

	if (GTK_OBJECT_CLASS (parent_class)->destroy)
		(* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static gchar *
e_text_model_real_get_text(ETextModel *model)
{
	if (model->text)
		return model->text;
	else
		return "";
}

static void
e_text_model_real_set_text(ETextModel *model, gchar *text)
{
	if (model->text)
		g_free(model->text);
	model->text = g_strdup(text);
	e_text_model_changed(model);
}

static void
e_text_model_real_insert(ETextModel *model, gint position, gchar *text)
{
	gchar *temp = g_strdup_printf("%.*s%s%s", position, model->text, text, model->text + position);
	if (model->text)
		g_free(model->text);
	model->text = temp;
	e_text_model_changed(model);
}

static void
e_text_model_real_insert_length(ETextModel *model, gint position, gchar *text, gint length)
{
	gchar *temp = g_strdup_printf("%.*s%.*s%s", position, model->text, length, text, model->text + position);
	if (model->text)
		g_free(model->text);
	model->text = temp;
	e_text_model_changed(model);
}

static void
e_text_model_real_delete(ETextModel *model, gint position, gint length)
{
	memmove(model->text + position, model->text + position + length, strlen(model->text + position + length) + 1);
	e_text_model_changed(model);
}

void
e_text_model_changed(ETextModel *model)
{
	g_return_if_fail (model != NULL);
	g_return_if_fail (E_IS_TEXT_MODEL (model));

	gtk_signal_emit (GTK_OBJECT (model),
			 e_text_model_signals [E_TEXT_MODEL_CHANGED]);
}

gchar *
e_text_model_get_text(ETextModel *model)
{
	g_return_val_if_fail (model != NULL, NULL);
	g_return_val_if_fail (E_IS_TEXT_MODEL (model), NULL);

	if ( E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->get_text )
		return E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->get_text(model);
	else
		return "";
}

void
e_text_model_set_text(ETextModel *model, gchar *text)
{
	g_return_if_fail (model != NULL);
	g_return_if_fail (E_IS_TEXT_MODEL (model));

	if ( E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->set_text )
		E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->set_text(model, text);
}

void
e_text_model_insert(ETextModel *model, gint position, gchar *text)
{
	g_return_if_fail (model != NULL);
	g_return_if_fail (E_IS_TEXT_MODEL (model));

	if ( E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->insert )
		E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->insert(model, position, text);
}

void
e_text_model_insert_length(ETextModel *model, gint position, gchar *text, gint length)
{
	g_return_if_fail (model != NULL);
	g_return_if_fail (E_IS_TEXT_MODEL (model));

	if ( E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->insert_length )
		E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->insert_length(model, position, text, length);
}

void
e_text_model_delete(ETextModel *model, gint position, gint length)
{
	g_return_if_fail (model != NULL);
	g_return_if_fail (E_IS_TEXT_MODEL (model));

	if ( E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->delete )
		E_TEXT_MODEL_CLASS(GTK_OBJECT(model)->klass)->delete(model, position, length);
}

ETextModel *
e_text_model_new(void)
{
	ETextModel *model = gtk_type_new (e_text_model_get_type ());
	model->text = g_strdup("");
	return model;
}
