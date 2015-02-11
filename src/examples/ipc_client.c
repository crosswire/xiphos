/*  Copyright (c) 2009 Matthew Talbert

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
*/

#include <dbus/dbus-glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipc-client-stub.h"

//this function will be called every time the user does a sidebar search
static gboolean
search_performed_signal_handler(DBusGProxy *proxy,
				char *results, gpointer user_data)
{
	char *ref;

	printf("number hits: %s\n", results);

	//method generated automatically in ipc-client-stub.h
	//this method can be called anywhere in your program
	org_xiphos_remote_get_next_search_reference(proxy, &ref, NULL);

	while (strcmp(ref, "XIPHOS_SEARCH_END")) {
		printf("ref: %s\n", ref);
		org_xiphos_remote_get_next_search_reference(proxy, &ref,
							    NULL);
	}

	return FALSE;
}

//this function will be called every time Xiphos navigates
static gboolean
navigation_performed_signal_handler(DBusGProxy *proxy,
				    char *reference, gpointer user_data)
{
	printf("new reference is: %s\n", reference);
	printf("now calling remote method just to be sure\n");

	char *ref;

	//method generated automatically in ipc-client-stub.h
	//can be called anywhere in your program
	org_xiphos_remote_get_current_reference(proxy, &ref, NULL);

	//of course, this doesn't actually *check* to see if they're the same :)
	printf("yep! Xiphos says the new reference is %s\n", ref);

	return FALSE;
}

int main(int argc, char **argv)
{
	DBusGConnection *bus;
	DBusGProxy *remote_object;
	GError *error = NULL;
	GMainLoop *mainloop;

//not necessary if using from a gtk/gnome program
#if !GLIB_CHECK_VERSION(2, 35, 0)
	g_type_init();
#endif

	mainloop = g_main_loop_new(NULL, FALSE);

	//get the "session" dbus
	bus = dbus_g_bus_get(DBUS_BUS_SESSION, &error);

	//conect to xiphos
	remote_object = dbus_g_proxy_new_for_name(bus,
						  "org.xiphos.remote",
						  "/org/xiphos/remote/ipc",
						  "org.xiphos.remote");

	//add and connect to the searchPerformedSignal
	dbus_g_proxy_add_signal(remote_object, "searchPerformedSignal",
				G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(remote_object, "searchPerformedSignal",
				    G_CALLBACK(search_performed_signal_handler),
				    NULL, NULL);

	//add and connect to the navigationSignal
	dbus_g_proxy_add_signal(remote_object, "navigationSignal",
				G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(remote_object, "navigationSignal",
				    G_CALLBACK(navigation_performed_signal_handler),
				    NULL, NULL);

	g_main_loop_run(mainloop);

	exit(0);
}
