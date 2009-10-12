#ifndef __XIPHOS_IPC_H__
#define __XIPHOS_IPC_H__

#include <glib.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

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

typedef enum {
	SEARCH_PERFORMED,
	LAST_SIGNAL
} IpcSignalNumber;

typedef struct _IpcObject IpcObject;
typedef struct _IpcObjectClass IpcObjectClass;

struct _IpcObject {
	GObject parent;
	GList *references;
};

struct _IpcObjectClass {
	GObjectClass parent;
	guint signals[LAST_SIGNAL];
};

GType ipc_object_get_type(void);

gboolean ipc_object_search_performed (IpcObject* obj, 
				      const gchar* reference,
				      GError** error);
IpcObject* ipc_init_dbus_connection(IpcObject* obj);
IpcObject* ipc_get_main_ipc();

G_END_DECLS

#endif /* __XIPHOS_IPC_H__ */
