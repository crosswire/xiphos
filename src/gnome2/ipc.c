/*
 * Xiphos Bible Study Tool
 * ipc.c - Interprocess Communication - dbus integration
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include "gui/ipc.h"
#include "marshal.h"

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

#include "ipc-interface.h"
#include "main/url.hh"

static IpcObject *main_ipc_obj;

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
			 G_TYPE_NONE, 1, G_TYPE_STRING);
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

	dbus_g_object_type_install_info(IPC_TYPE_OBJECT,
					&dbus_glib_ipc_object_object_info);
}

/**
 * ipc_object_search_performed:
 * @obj: an #IpcObject
 * @search_term: the search term
 * @hits: number of search results
 * @error: GErorr
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

	GString *s = g_string_new("");
	g_string_printf(s, "%d", *hits);

	g_signal_emit(obj,
		      klass->signals[SEARCH_PERFORMED], 0, s->str, hits);
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
	obj->current_ref = g_strdup(reference);

	IpcObjectClass *klass = IPC_OBJECT_GET_CLASS(obj);

	g_signal_emit(obj, klass->signals[NAVIGATION], 0, reference);
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
IpcObject *ipc_init_dbus_connection(IpcObject *obj)
{
	DBusGConnection *bus = NULL;
	DBusGProxy *busProxy = NULL;
	obj = NULL;
	guint result;
	GError *error = NULL;

	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (error != NULL)
		return FALSE;

	busProxy = dbus_g_proxy_new_for_name(bus,
					     "org.freedesktop.DBus",
					     "/org/freedesktop/DBus",
					     "org.freedesktop.DBus");
	if (busProxy == NULL)
		return NULL;

	if (!dbus_g_proxy_call(busProxy,
			       "RequestName",
			       &error,
			       G_TYPE_STRING,
			       "org.xiphos.remote",
			       G_TYPE_UINT,
			       0,
			       G_TYPE_INVALID,
			       G_TYPE_UINT, &result, G_TYPE_INVALID)) {
		return NULL;
	}

	if (result != 1)
		return NULL;

	obj = g_object_new(IPC_TYPE_OBJECT, NULL);

	dbus_g_connection_register_g_object(bus,
					    "/org/xiphos/remote/ipc",
					    G_OBJECT(obj));

	main_ipc_obj = obj;

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
