#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>

static gboolean
search_performed_signal_handler (DBusGProxy *proxy,
				 const char *search_term,
				 gpointer user_data)
{
	printf ("Search was for %s\n", search_term);
	return FALSE;
}

int main (int argc, char **argv)
{
	DBusGConnection *bus;
	DBusGProxy *remote_object;
	GError *error = NULL;
	GMainLoop *mainloop;

	g_type_init ();

	mainloop = g_main_loop_new (NULL, FALSE);

	bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	remote_object = dbus_g_proxy_new_for_name (bus,
						   "org.xiphos.remote",
						   "/org/xiphos/remote/ipc",
						   "org.xiphos.remote");

	dbus_g_proxy_add_signal (remote_object, "searchPerformed",
				 G_TYPE_STRING, G_TYPE_INVALID);

	dbus_g_proxy_connect_signal (remote_object, "searchPerformed",
				     G_CALLBACK(search_performed_signal_handler),
				     NULL, NULL);
	g_main_loop_run (mainloop);

	exit (0);

}
		

	
