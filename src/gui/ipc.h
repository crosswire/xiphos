/*
 * Xiphos Bible Study Tool
 * ipc.h - Interprocess Communication - dbus integration
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

#ifndef __XIPHOS_IPC_H__
#define __XIPHOS_IPC_H__

#include <glib.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS
#define IPC_TYPE_OBJECT (ipc_object_get_type())
#define IPC_OBJECT(object)                    \
	(G_TYPE_CHECK_INSTANCE_CAST((object), \
				    IPC_TYPE_OBJECT, IpcObject))
#define IPC_OBJECT_CLASS(klass)           \
	(G_TYPE_CHECK_CLASS_CAST((klass), \
				 IPC_TYPE_OBJECT, IpcObjectClass))
#define IPC_IS_OBJECT(klass)              \
	(G_TYPE_CHECK_CLASS_TYPE((klass), \
				 IPC_TYPE_OBJECT))
#define IPC_IS_OBJECT_CLASS(klass)        \
	(G_TYPE_CHECK_CLASS_TYPE((klass), \
				 IPC_TYPE_OBJECT))
#define IPC_OBJECT_GET_CLASS(obj)         \
	(G_TYPE_INSTANCE_GET_CLASS((obj), \
				   IPC_TYPE_OBJECT, IpcObjectClass))
typedef enum {
	SEARCH_PERFORMED,
	NAVIGATION,
	LAST_SIGNAL
} IpcSignalNumber;

typedef struct _IpcObject IpcObject;
typedef struct _IpcObjectClass IpcObjectClass;

struct _IpcObject
{
	GObject parent;
	GList *references;
	gchar *current_ref;
};

struct _IpcObjectClass
{
	GObjectClass parent;
	guint signals[LAST_SIGNAL];
};

GType ipc_object_get_type(void);

gboolean ipc_object_search_performed(IpcObject *obj,
				     const gchar *search_term,
				     const int *hits, GError **error);
gboolean ipc_object_navigation_signal(IpcObject *obj,
				      const gchar *reference,
				      GError **error);

IpcObject *ipc_init_dbus_connection(IpcObject *obj);
IpcObject *ipc_get_main_ipc(void);

G_END_DECLS
#endif /* __XIPHOS_IPC_H__ */
