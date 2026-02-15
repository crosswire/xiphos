/*
 * Xiphos Bible Study Tool
 * ipc.c - Interprocess Communication - dbus integration
 *
 * Copyright (C) 2000-2025 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/**
 * SECTION:ipc
 * @short_description: Interprocess Communication - dbus integration
 *
 * An #IpcObject is a gobject that publishes signals and methods over
 * dbus. It allows external control of Xiphos via dbus.
 *
 * sample client programs are in xiphos/src/examples/ipc_client.c
 * and xiphos/src/examples/ipc_client.py
 */

#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>
#include "gui/ipc.h"
#include "marshal.h"
#include "ipc-gdbus.h"

G_DEFINE_TYPE(IpcObject, ipc_object, G_TYPE_OBJECT)

gboolean ipc_object_set_current_reference(IpcObject *obj,
					  gchar *reference,
					  GError **error);
gboolean ipc_object_get_current_reference(IpcObject *obj,
					  gchar **reference,
					  GError **error);
gboolean ipc_object_get_next_search_reference(IpcObject *obj,
					      gchar **reference,
					      GError **error);

#include "main/url.hh"

static IpcObject *main_ipc_obj;
static XiphosRemote *ipc_skeleton;
static guint owner_id;

static void ipc_object_init(IpcObject *obj)
{
	g_assert(obj != NULL);
	obj->references = NULL;
	obj->current_ref = NULL;
}

static void ipc_object_class_init(IpcObjectClass *klass)
{

	/**
	 * IpcObject::search_performed_signal:
	 * @hits: the number of search results
	 *
	 * The search_performed_signal is emitted any time
	 * Xiphos performs a sidebar search; it is intended
	 * to be listened to and handled over dbus
	 *
	 * Since: 3.2
	 */
	klass->signals[SEARCH_PERFORMED] =
	    g_signal_new("search-performed-signal",
			 G_OBJECT_CLASS_TYPE(klass),
			 G_SIGNAL_RUN_LAST,
			 0,
			 NULL,
			 NULL,
			 ipc_marshal_VOID__STRING_INT,
			 G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_INT);
	/**
	 * IpcObject::navigation_signal:
	 * @reference: the new reference
	 *
	 * The navigation_signal is emitted any time Xiphos
	 * navigates to a new Bible reference; it is intended
	 * to be listened to and handled over dbus
	 *
	 * Since: 3.2
	 */
	klass->signals[NAVIGATION] =
	    g_signal_new("navigation-signal",
			 G_OBJECT_CLASS_TYPE(klass),
			 G_SIGNAL_RUN_LAST,
			 0,
			 NULL,
			 NULL,
			 g_cclosure_marshal_VOID__STRING,
			 G_TYPE_NONE, 1, G_TYPE_STRING);
}

/**
 * ipc_object_search_performed:
 * @obj: an #IpcObject
 * @search_term: the search term
 * @hits: number of search results
 * @error: GError
 *
 * makes the IpcObject emit a SEARCH_PERFORMED signal which is then
 * published over dbus
 *
 * Since: 3.2
 */
gboolean ipc_object_search_performed(IpcObject *obj,
				     const gchar *search_term,
				     const int *hits, GError **error)
{
	IpcObjectClass *klass = IPC_OBJECT_GET_CLASS(obj);

	/* Emit local GObject signal */
	g_signal_emit(obj, klass->signals[SEARCH_PERFORMED], 0,
		      search_term, *hits);

	/* Emit D-Bus signal using gdbus-codegen generated function */
	if (ipc_skeleton) {
		xiphos_remote_emit_search_performed_signal(ipc_skeleton,
							   search_term, *hits);
	}
	return FALSE;
}

/**
 * ipc_object_navigation_signal:
 * @obj: an #IpcObject
 * @reference: a gchar with the new reference
 * @error: GError (pass NULL)
 *
 * makes the IpcObject emit a NAVIGATION signal which is then
 * published over dbus
 *
 * Since: 3.2
 */

gboolean ipc_object_navigation_signal(IpcObject *obj,
				      const gchar *reference,
				      GError **error)
{
	g_free(obj->current_ref);
	obj->current_ref = g_strdup(reference);

	IpcObjectClass *klass = IPC_OBJECT_GET_CLASS(obj);

	/* Emit local GObject signal */
	g_signal_emit(obj, klass->signals[NAVIGATION], 0, reference);

	/* Emit D-Bus signal using gdbus-codegen generated function */
	if (ipc_skeleton) {
		xiphos_remote_emit_navigation_signal(ipc_skeleton, reference);
	}
	return FALSE;
}

/**
 * ipc_object_get_next_search_reference:
 * @obj: an #IpcObject
 * @reference: the return reference
 * @error: GError
 *
 * to be called from dbus; returns all the current search
 * results; returns XIPHOS_SEARCH_END when all have been retrieved
 *
 * Since: 3.2
 */

gboolean ipc_object_get_next_search_reference(IpcObject *obj,
					      gchar **reference,
					      GError **error)
{
	if (obj->references)
		*reference = g_strdup((gchar *)obj->references->data);
	else
		*reference = g_strdup("XIPHOS_SEARCH_END");

	obj->references = g_list_next(obj->references);
	return TRUE;
}

/**
 * ipc_object_set_current_reference:
 * @obj: an #IpcObject
 * @reference: the reference to set
 * @error: GError
 *
 * to be called from dbus; sets the current reference in Xiphos;
 * calls main_url_handler to handle the navigation
 *
 * Since: 3.2
 */
gboolean ipc_object_set_current_reference(IpcObject *obj,
					  gchar *reference,
					  GError **error)
{
	//easy route
	main_url_handler((const gchar *)reference, TRUE);
	//it should be done like this, probably
	/* g_signal_emit(obj,
	   "navigate-requested",
	   reference); */

	return TRUE;
}

/**
 * ipc_object_get_current_reference:
 * @obj: an #IpcObject
 * @reference: the return reference
 * @error: GError
 *
 * to be called from dbus; gets the current reference in Xiphos;
 *
 * Since: 3.2
 */
gboolean ipc_object_get_current_reference(IpcObject *obj,
					  gchar **reference,
					  GError **error)
{
	*reference = g_strdup(obj->current_ref);
	return TRUE;
}

/**
 * ipc_init_dbus_connection
 * @obj: an #IpcObject
 *
 * creates a new IpcObject and initiates the d-bus connection;
 * sets the single static object to this new object
 *
 * Since: 3.2
 */

/* D-Bus method handlers using gdbus-codegen skeleton */
static gboolean
on_handle_set_current_reference(XiphosRemote *skeleton,
				 GDBusMethodInvocation *invocation,
				 const gchar *reference,
				 gpointer user_data)
{
	IpcObject *obj = IPC_OBJECT(user_data);
	GError *error = NULL;

	ipc_object_set_current_reference(obj, (gchar *)reference, &error);
	xiphos_remote_complete_set_current_reference(skeleton, invocation);

	return TRUE;
}

static gboolean
on_handle_get_current_reference(XiphosRemote *skeleton,
				 GDBusMethodInvocation *invocation,
				 gpointer user_data)
{
	IpcObject *obj = IPC_OBJECT(user_data);
	gchar *reference = NULL;
	GError *error = NULL;

	ipc_object_get_current_reference(obj, &reference, &error);
	xiphos_remote_complete_get_current_reference(skeleton, invocation,
						     reference ? reference : "");
	g_free(reference);

	return TRUE;
}

static gboolean
on_handle_get_next_search_reference(XiphosRemote *skeleton,
				    GDBusMethodInvocation *invocation,
				    gpointer user_data)
{
	IpcObject *obj = IPC_OBJECT(user_data);
	gchar *reference = NULL;
	GError *error = NULL;

	ipc_object_get_next_search_reference(obj, &reference, &error);
	xiphos_remote_complete_get_next_search_reference(skeleton, invocation,
							 reference ? reference : "");
	g_free(reference);

	return TRUE;
}

static void
on_bus_acquired(GDBusConnection *connection,
		const gchar *name,
		gpointer user_data)
{
	IpcObject *obj = IPC_OBJECT(user_data);
	GError *error = NULL;

	/* Create skeleton from gdbus-codegen */
	ipc_skeleton = xiphos_remote_skeleton_new();

	/* Connect method handlers */
	g_signal_connect(ipc_skeleton, "handle-set-current-reference",
			 G_CALLBACK(on_handle_set_current_reference), obj);
	g_signal_connect(ipc_skeleton, "handle-get-current-reference",
			 G_CALLBACK(on_handle_get_current_reference), obj);
	g_signal_connect(ipc_skeleton, "handle-get-next-search-reference",
			 G_CALLBACK(on_handle_get_next_search_reference), obj);

	/* Export the interface */
	if (!g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(ipc_skeleton),
					     connection,
					     "/org/xiphos/remote/ipc",
					     &error)) {
		g_warning("Failed to export D-Bus interface: %s", error->message);
		g_error_free(error);
		g_object_unref(ipc_skeleton);
		ipc_skeleton = NULL;
	}
}

static void
on_name_acquired(GDBusConnection *connection,
		 const gchar *name,
		 gpointer user_data)
{
	/* Successfully acquired the name */
	g_debug("D-Bus name '%s' acquired", name);
}

static void
on_name_lost(GDBusConnection *connection,
	     const gchar *name,
	     gpointer user_data)
{
	if (connection == NULL) {
		g_warning("Failed to connect to D-Bus session bus");
	} else {
		g_warning("Failed to acquire D-Bus name '%s'", name);
	}
}

IpcObject *ipc_init_dbus_connection(IpcObject *obj)
{
	obj = g_object_new(IPC_TYPE_OBJECT, NULL);
	main_ipc_obj = obj;

	/* Request the D-Bus name */
	owner_id = g_bus_own_name(G_BUS_TYPE_SESSION,
				  "org.xiphos.remote",
				  G_BUS_NAME_OWNER_FLAGS_NONE,
				  on_bus_acquired,
				  on_name_acquired,
				  on_name_lost,
				  obj,
				  NULL);

	if (owner_id == 0) {
		g_warning("Failed to own D-Bus name");
		return NULL;
	}

	return obj;
}

/**
 * ipc_get_main_ipc:
 *
 * returns the single static IpcObject
 *
 * Since: 3.2
 */
IpcObject *ipc_get_main_ipc(void)
{
	return main_ipc_obj;
}
