#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipc-client-stub.h"

int main (int argc, char **argv)
{
	DBusGConnection *bus;
	DBusGProxy *remote_object;
	GError *error = NULL;

	/* not necessary if using from a gtk/gnome program */
	g_type_init ();

	/* get the "session" dbus */
	bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	/* connect to Xiphos */
	remote_object = dbus_g_proxy_new_for_name (bus,
						   "org.xiphos.remote",
						   "/org/xiphos/remote/ipc",
						   "org.xiphos.remote");

	/* attempt to set reference in running instance of Xiphos */
	org_xiphos_remote_set_current_reference(remote_object, argv[1], &error);

	/* if error, then Xiphos is not running, so we just start it and pass
	   it the arguments given to this program */
	if (error)
		execvp("xiphos", argv);

	return 1;
}



