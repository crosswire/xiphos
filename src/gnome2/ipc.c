#include <glib.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include "gui/ipc.h"
#include "marshal.h"

G_DEFINE_TYPE(IpcObject, ipc_object, G_TYPE_OBJECT)


gboolean ipc_object_set_current_reference(IpcObject* obj,
					  gchar* reference,
					  GError** error);
gboolean ipc_object_get_current_reference(IpcObject* obj,
					  gchar* reference,
					  GError** error);
gboolean ipc_object_get_next_search_reference(IpcObject* obj, 
				       gchar* reference,
				       GError** error);

#include "ipc-interface.h"
#include "main/url.hh"

static IpcObject *main_ipc_obj;

static void ipc_object_init(IpcObject* obj) {
	g_assert(obj != NULL);
	obj->references = NULL;
	obj->current_ref = NULL;
}

static void ipc_object_class_init(IpcObjectClass* klass) {

	klass->signals[SEARCH_PERFORMED] = 
		g_signal_new ("search_performed_signal",
			      G_OBJECT_CLASS_TYPE(klass),
			      G_SIGNAL_RUN_LAST,
			      0,
			      NULL,
			      NULL,
			      ipc_marshal_VOID__STRING_INT,
			      G_TYPE_NONE,
			      1,
			      G_TYPE_STRING);
	klass->signals[NAVIGATION] =
		g_signal_new ("navigation_signal",
			      G_OBJECT_CLASS_TYPE(klass),
			      G_SIGNAL_RUN_LAST,
			      0,
			      NULL,
			      NULL,
			      g_cclosure_marshal_VOID__STRING,
			      G_TYPE_NONE,
			      1,
			      G_TYPE_STRING);

	dbus_g_object_type_install_info(IPC_TYPE_OBJECT,
					&dbus_glib_ipc_object_object_info);
}

gboolean ipc_object_search_performed(IpcObject* obj,
				     const gchar* search_term,
				     const int* hits,
				     GError **error)
 {
	 g_print ("search performed signal");
	 
	 IpcObjectClass* klass = IPC_OBJECT_GET_CLASS(obj);

	 g_signal_emit(obj,
		       klass->signals[SEARCH_PERFORMED],
		       0,
		       search_term, hits);
	return FALSE;
}

gboolean ipc_object_navigation_signal (IpcObject* obj,
					const gchar* reference,
					GError** error)
{
	obj->current_ref = reference;
	g_print ("navigation performed signal");

	IpcObjectClass* klass = IPC_OBJECT_GET_CLASS(obj);

	g_signal_emit(obj,
		      klass->signals[NAVIGATION],
		      0,
		      reference);
	return FALSE;
}

gboolean ipc_object_get_next_search_reference(IpcObject* obj, 
				       gchar* reference,
				       GError** error)
{
	obj->references = g_list_next(obj->references);
	if (obj->references)
		reference = (gchar*)obj->references->data;
	else
		reference = NULL;

	return TRUE;
}

gboolean ipc_object_set_current_reference(IpcObject* obj,
					  gchar* reference,
					  GError** error)
{
	//easy route
	main_url_handler((const gchar*)reference, TRUE);
	//it should be done like this, probably
	/* g_signal_emit(obj,
		      "navigate-requested",
		      reference); */

	return TRUE;
}

gboolean ipc_object_get_current_reference(IpcObject* obj,
					  gchar* reference,
					  GError** error)
{
	reference = obj->current_ref;
	return TRUE;
}

IpcObject* ipc_init_dbus_connection(IpcObject* obj)
{
	DBusGConnection* bus = NULL;
	DBusGProxy* busProxy = NULL;
	obj = NULL;
	guint result;
	GError* error = NULL;

	g_print("Connecting to the Session D-Bus\n");
	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);
	if (error != NULL)
		return FALSE;
	
	g_print("Registering /org/xiphos/remote\n");

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
			       G_TYPE_UINT,
			       &result,
			       G_TYPE_INVALID)) {
		return NULL;
	}

	g_print("RequestName returned %d.\n", result);
					     
	if (result != 1)
		return NULL;
	
	obj = g_object_new(IPC_TYPE_OBJECT, NULL);

	dbus_g_connection_register_g_object(bus,
					    "/org/xiphos/remote/ipc",
					    G_OBJECT(obj));

	main_ipc_obj = obj;
	
	return obj;
}

IpcObject* ipc_get_main_ipc()
{
	return main_ipc_obj;
}

	
