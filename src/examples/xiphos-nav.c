/*
 * Xiphos Bible Study Tool
 * xiphos.c -
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

#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ipc-gdbus.h"

int main(int argc, char **argv)
{
	GError *error = NULL;
	XiphosRemote *proxy;

	/* create proxy using gdbus-codegen generated code */
	proxy = xiphos_remote_proxy_new_for_bus_sync(
	    G_BUS_TYPE_SESSION,
	    G_DBUS_PROXY_FLAGS_NONE,
	    "org.xiphos.remote",
	    "/org/xiphos/remote/ipc",
	    NULL, /* GCancellable */
	    &error);

	if (error) {
		g_printerr("Failed to create proxy: %s\n", error->message);
		g_error_free(error);
		return 1;
	}

	/* attempt to set reference in running instance of Xiphos */
	xiphos_remote_call_set_current_reference_sync(proxy,
						      argv[1],
						      NULL, /* cancellable */
						      &error);

	/* if error, then Xiphos is not running, so we just start it and pass
	   it the arguments given to this program */
	if (error) {
		g_error_free(error);
		execvp("xiphos", argv);
	}

	g_object_unref(proxy);

	return 0;
}
