#include <glib.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>


typedef enum {
	SEARCH_PERFORMED,
	LAST_SIGNAL
} IpcSignalNumber;

typedef struct {
	GObject parent;
	GList *references;
} IpcObject;

typedef struct {
	GObjectClass parent;
	guint signals[LAST_SIGNAL];
} IpcObjectClass;

GType ipc_object_get_type(void);

#define IPC_TYPE_OBJECT             (ipc_object_get_type())
#define IPC_OBJECT(object) \
	(G_TYPE_CHECK_INSTANCE_CAST ((object), \
	 IPC_TYPE_OBJECT, IpcObject))
#define IPC_OBJECT_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST ((klass), \
	 IPC_TYPE_OBJECT, IpcObjectClass))
#define IPC_IS_OBJECT(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), \
	 IPC_TYPE_OBJECT))
#define IPC_IS_OBJECT_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE ((klass), \
	 IPC_TYPE_OBJECT))
#define IPC_OBJECT_GET_CLASS(obj) \
	(G_TYPE_INSTANCE_GET_CLASS ((obj), \
	 IPC_TYPE_OBJECT, IpcObjectClass))

G_DEFINE_TYPE(IpcObject, ipc_object, G_TYPE_OBJECT)

gboolean ipc_object_search_performed (IpcObject* obj, char* reference,
				      GError** error);

gboolean ipc_object_navigate(IpcObject* obj, gchar* reference,
			     GError** error);
gboolean ipc_object_get_next_reference(IpcObject* obj, gchar* reference,
				       GError** error);

#include "ipc-server-stub.h"

static void ipc_object_init(IpcObject* obj) {
	g_assert(obj != NULL);
	obj->references = NULL;
}

static void ipc_object_class_init(IpcObjectClass* klass) {

	klass->signals[SEARCH_PERFORMED] = 
		g_signal_new ("search_performed",
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

static void ipc_object_search_performed(IpcObject* obj,
					const gchar* reference,
					GError **error)
 {
	IpcObjectClass* klass = IPC_OBJECT_GET_KLASS(obj);

	g_signal_emit(obj,
		      klass->signals[SEARCH_PERFORMED],
		      0,
		      reference);
}


gboolean ipc_object_get_next_reference(IpcObject* obj, gchar* reference,
				       GError** error) 
{
	obj->references = g_list_next(obj->references);
	if (references)
		reference = (gchar*)references->data;
	else
		reference = NULL;
}

gboolean ipc_object_navigate(IpcObject* obj, gchar* reference,
			     GError** error)
{
	//easy route
	main_url_handler((const gchar*)reference, TRUE);
	//it should be done like this, probably
	/* g_signal_emit(obj,
		      "navigate-requested",
		      reference); */
}
		
gboolean ipc_init_dbus_connection(IpcObject* ob)
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
		return FALSE;

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
		return FALSE;
	}

	g_print("RequestName returned %d.\n", result);
					     
	if (result != 1)
		return FALSE;
	
	obj = g_object_new(IPC_TYPE_OBJECT, NULL);

	dbus_g_connection_register_g_object(bus,
					    "/org/xiphos/remote/ipc",
					    G_OBJECT(obj));

	return TRUE;
}
	

	
