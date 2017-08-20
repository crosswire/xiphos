/*
 * Xiphos Bible Study Tool
 * xiphos.c -
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

#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ipc-interface.h"

int main(int argc, char **argv)
{
	DBusGConnection *bus;
	DBusGProxy *remote_object;
	GError *error = NULL;

/* not necessary if using from a gtk/gnome program */
#if !GLIB_CHECK_VERSION(2, 35, 0)
	g_type_init();
#endif
	/* get the "session" dbus */
	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);

	/* connect to Xiphos */
	remote_object = dbus_g_proxy_new_for_name(bus,
						  "org.xiphos.remote",
						  "/org/xiphos/remote/ipc",
						  "org.xiphos.remote");

	/* attempt to set reference in running instance of Xiphos */
	org_xiphos_remote_set_current_reference(remote_object, argv[1],
						&error);

	/* if error, then Xiphos is not running, so we just start it and pass
	   it the arguments given to this program */
	if (error)
		execvp("xiphos", argv);

	return 1;
}
