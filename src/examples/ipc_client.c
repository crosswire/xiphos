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

#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ipc-gdbus.h"

static XiphosRemote *remote_object = NULL;

/* this function will be called every time the user does a sidebar search */
static void
search_performed_signal_handler(XiphosRemote *proxy,
				const gchar *search_term,
				gint hits,
				gpointer user_data)
{
	GError *error = NULL;
	gchar *ref = NULL;

	printf("Search: %s, number hits: %d\n", search_term, hits);

	/* get search results using type-safe generated method */
	xiphos_remote_call_get_next_search_reference_sync(remote_object,
							  &ref,
							  NULL,
							  &error);
	if (error) {
		g_printerr("Error calling getNextSearchReference: %s\n", error->message);
		g_error_free(error);
		return;
	}

	while (strcmp(ref, "XIPHOS_SEARCH_END")) {
		printf("ref: %s\n", ref);
		g_free(ref);
		ref = NULL;

		xiphos_remote_call_get_next_search_reference_sync(remote_object,
								  &ref,
								  NULL,
								  &error);
		if (error) {
			g_printerr("Error getting next reference: %s\n", error->message);
			g_error_free(error);
			break;
		}
	}

	g_free(ref);
}

/* this function will be called every time Xiphos navigates */
static void
navigation_performed_signal_handler(XiphosRemote *proxy,
				    const gchar *reference,
				    gpointer user_data)
{
	GError *error = NULL;
	gchar *ref = NULL;

	printf("new reference is: %s\n", reference);
	printf("now calling remote method just to be sure\n");

	/* verify by calling the remote method using type-safe generated function */
	xiphos_remote_call_get_current_reference_sync(remote_object,
						      &ref,
						      NULL,
						      &error);
	if (error) {
		g_printerr("Error calling getCurrentReference: %s\n", error->message);
		g_error_free(error);
		return;
	}

	/* of course, this doesn't actually *check* to see if they're the same :) */
	printf("yep! Xiphos says the new reference is %s\n", ref);

	g_free(ref);
}

int main(int argc, char **argv)
{
	GError *error = NULL;
	GMainLoop *mainloop;

	mainloop = g_main_loop_new(NULL, FALSE);

	/* create proxy using gdbus-codegen generated code */
	remote_object = xiphos_remote_proxy_new_for_bus_sync(
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

	/* connect to signals using type-safe generated signal handlers */
	g_signal_connect(remote_object, "search-performed-signal",
			 G_CALLBACK(search_performed_signal_handler), NULL);
	g_signal_connect(remote_object, "navigation-signal",
			 G_CALLBACK(navigation_performed_signal_handler), NULL);

	g_main_loop_run(mainloop);

	g_object_unref(remote_object);

	exit(0);
}
