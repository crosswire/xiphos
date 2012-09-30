/*
 * Xiphos Bible Study Tool
 * wk-html.c - webkit-specific html support
 *
 * Copyright (C) 2010-2011 Xiphos Developer Team
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

#include <string.h>
#include <webkit/webkitwebframe.h>
#include <webkit/webkitnetworkrequest.h>
#include <webkit/webkitwebview.h>
#include <webkit/webkitwebsettings.h>

#include "main/url.hh"
#include "main/module_dialogs.h"

#include "wk-html.h"
#include "marshal.h"

#include "gui/dictlex.h"
#include "main/sword.h"

#define WK_HTML_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), WK_TYPE_HTML, WkHtmlPriv))

extern gboolean shift_key_pressed;
extern gboolean in_url;

static gchar *x_uri = NULL;
static gboolean db_click;

enum {
	URI_SELECTED,
	FRAME_SELECTED,
	TITLE_CHANGED,
	POPUPMENU_REQUESTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };
static GObjectClass *parent_class = NULL;

static gboolean button_release_handler (GtkWidget *widget,
					GdkEventButton *event)
{
	 GtkClipboard * clipboard;
         if(event->type ==  GDK_BUTTON_RELEASE && db_click) {
		GS_message((" button 1 = %s" , "double click!\n"));

		if(webkit_web_view_has_selection(WEBKIT_WEB_VIEW (widget))) {
			webkit_web_view_copy_clipboard (WEBKIT_WEB_VIEW (widget));

			clipboard = gtk_widget_get_clipboard (widget,
							      GDK_SELECTION_CLIPBOARD );

			gtk_clipboard_request_text  (clipboard,
						     gui_get_clipboard_text_for_lookup,
						     NULL);
		}
	}
	return FALSE;
}
static gboolean button_press_handler (GtkWidget *widget,
				      GdkEventButton *event)
{
	WkHtmlPriv *priv;
        priv = WK_HTML_GET_PRIVATE (WK_HTML(widget));

	if(event->type ==  GDK_2BUTTON_PRESS && !x_uri) {
		db_click = TRUE;
		return FALSE;
	}
	db_click = FALSE;

	if (event->button == 1)	{
		if (x_uri) {
			if ( priv->is_dialog )
				main_dialogs_url_handler(priv->dialog, x_uri, TRUE);
			else
				main_url_handler(x_uri, TRUE);
			return TRUE;
		}
		return FALSE;
	}
	if (event->button == 3)	{
		g_signal_emit(widget,
			signals[POPUPMENU_REQUESTED],
		      	0,
		      	priv->dialog,
		      	FALSE);
		return TRUE;
	}
	if (event->button == 2)	{
		/* ignore middle button */
		return TRUE;
	}
	return FALSE;
}

static void link_handler (GtkWidget *widget,
			  gchar     *title,
			  gchar     *uri,
			  gpointer   user_data)
{
	WkHtmlPriv *priv;
	priv = WK_HTML_GET_PRIVATE(WK_HTML(widget));
	GS_message(("html_link_message: uri = %s", (uri ? uri : "-none-")));

	if (shift_key_pressed)
		return;

	if(x_uri) {
		g_free(x_uri);
		x_uri = NULL;
	}
	if (uri) {
		x_uri = g_strdup(uri);
		in_url = 1;
	} else
		in_url = 0;

	g_signal_emit(widget,
		      signals[URI_SELECTED],
		      0,
		      uri,
		      FALSE);
	if (uri)  {
		if ( priv->is_dialog )
			main_dialogs_url_handler(priv->dialog, uri, FALSE);
		else
			main_url_handler(uri, FALSE);
	}
}

static void
html_realize (GtkWidget *widget)
{
//	WkHtml *html = WK_HTML (widget);
	GTK_WIDGET_CLASS (parent_class)->realize (widget);

	g_signal_connect (G_OBJECT (widget), "button-press-event",
			  G_CALLBACK (button_press_handler),
			  NULL);
	g_signal_connect (G_OBJECT (widget), "button-release-event",
			  G_CALLBACK (button_release_handler),
			  NULL);
	g_signal_connect (G_OBJECT (widget), "hovering-over-link",
			  G_CALLBACK (link_handler),
			  NULL);
}

static void
html_init (WkHtml *html)
{
	WkHtmlPriv *priv;

	html->priv = priv = WK_HTML_GET_PRIVATE (html);

	priv->base_uri = NULL;
	priv->anchor = NULL;
	priv->timeout = 0;
	priv->content = NULL;
	priv->mime = NULL;
	priv->initialised = FALSE;

	/* g_signal_connect (html, "navigation-policy-decision-requested", */
	/* 		  G_CALLBACK (html_open_uri), NULL); */
}

static void
html_dispose (GObject *object)
{
//	WkHtml *html = WK_HTML (object);
	parent_class->dispose (object);
}

static void
html_finalize (GObject *object)
{
	WkHtml *html = WK_HTML (object);
	WkHtmlPriv *priv = html->priv;

	if (priv->timeout)
		g_source_remove (priv->timeout);
	g_free (priv->base_uri);
	g_free (priv->anchor);

	parent_class->finalize (object);
}

static void
html_class_init (WkHtmlClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
//	WebKitWebViewClass *wc_class = WEBKIT_WEB_VIEW_CLASS (klass);

	parent_class = (GObjectClass *) g_type_class_peek_parent (klass);

	object_class->finalize = html_finalize;
	object_class->dispose = html_dispose;

	widget_class->realize = html_realize;

	signals[URI_SELECTED] =
		g_signal_new ("uri_selected",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (WkHtmlClass,
					       uri_selected),
			      NULL, NULL,
			      wk_marshal_VOID__POINTER_BOOLEAN,
			      G_TYPE_NONE,
			      2, G_TYPE_POINTER, G_TYPE_BOOLEAN);

	signals[FRAME_SELECTED] =
		g_signal_new ("frame_selected",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (WkHtmlClass,
					       frame_selected),
			      g_signal_accumulator_true_handled, NULL,
			      wk_marshal_BOOLEAN__POINTER_BOOLEAN,
			      G_TYPE_BOOLEAN,
			      2, G_TYPE_POINTER, G_TYPE_BOOLEAN);


	signals[POPUPMENU_REQUESTED] =
		g_signal_new ("popupmenu_requested",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (WkHtmlClass,
					       popupmenu_requested),
			      NULL, NULL,

			      wk_marshal_VOID__POINTER_BOOLEAN,
			      G_TYPE_NONE,
			      2, G_TYPE_POINTER, G_TYPE_BOOLEAN);
			   //   g_cclosure_marshal_VOID__VOID,
			   //    G_TYPE_NONE,
			   //    1,G_TYPE_POINTER
		          //   );

	g_type_class_add_private (klass, sizeof (WkHtmlPriv));
}

GType
wk_html_get_type (void)
{
	static GType type = 0;

	if (G_UNLIKELY(type == 0)) {
		static const GTypeInfo info = {
		    sizeof (WkHtmlClass),
		    NULL,
		    NULL,
		    (GClassInitFunc) html_class_init,
		    NULL,
		    NULL,
		    sizeof (WkHtml),
		    0,
		    (GInstanceInitFunc) html_init,
		};

		type = g_type_register_static(WEBKIT_TYPE_WEB_VIEW,
					      "WkHtml",
					      &info, (GTypeFlags) 0);
	}

	return type;
}

void
wk_html_set_base_uri (WkHtml *html, const gchar *uri)
{
	WkHtmlPriv *priv;

	g_return_if_fail(WK_HTML_IS_HTML (html));

	priv = html->priv;

	if (priv->base_uri)
		g_free (priv->base_uri);

	priv->base_uri = g_strdup("file://");
}

void
wk_html_open_stream (WkHtml *html, const gchar *mime)
{
	wk_html_set_base_uri (html, NULL);
	webkit_web_view_set_transparent(WEBKIT_WEB_VIEW(html), TRUE);

	html->priv->frames_enabled = FALSE;
	g_free (html->priv->content);
	html->priv->content = NULL;
	g_free (html->priv->mime);
	html->priv->mime = g_strdup(mime);
}

void
wk_html_write (WkHtml *html, const gchar *data, gint len)
{
	gchar *tmp = NULL;

	if (len == -1) len = strlen (data);

	if (html->priv->content) {
		tmp = g_strjoin (NULL, html->priv->content, data, NULL);
		g_free (html->priv->content);
		html->priv->content = tmp;
	} else {
		html->priv->content = g_strdup (data);
	}
}

void
wk_html_frames (WkHtml *html, gboolean enable)
{
	html->priv->frames_enabled = enable;

}

void
wk_html_printf (WkHtml *html, char *format, ...)
{
	va_list  args;
	gchar   *string;

	g_return_if_fail (format != NULL);

	va_start (args, format);
	string = g_strdup_vprintf (format, args);
	va_end (args);

	wk_html_write (html, string, -1);

	g_free (string);
}


void
wk_html_close (WkHtml *html)
{

	if (!html->priv->initialised) {
		html->priv->initialised = TRUE;
		webkit_web_view_set_maintains_back_forward_list (WEBKIT_WEB_VIEW (html), FALSE);
	}

	webkit_web_view_load_string (WEBKIT_WEB_VIEW (html),
				     html->priv->content,
				     html->priv->mime,
				     NULL,
				     html->priv->base_uri);
	g_free (html->priv->content);
	html->priv->content = NULL;
	g_free (html->priv->mime);
	html->priv->mime = NULL;

}

void
wk_html_render_data(WkHtml *html, const char *data, guint32 len)
{
	printf("in render_data, data is %s", data);
	wk_html_open_stream(html, "text/html");
	wk_html_write(html, data, len);
	wk_html_close(html);
}

gboolean
wk_html_find (WkHtml    *html,
	      const gchar *find_string)
{
	if (html->priv->find_string)
		g_free(html->priv->find_string);
	html->priv->find_string = g_strdup (find_string);
	return webkit_web_view_search_text (WEBKIT_WEB_VIEW (html),
					    find_string, FALSE,
					    TRUE, TRUE);
}

gboolean
wk_html_find_again (WkHtml    *html,
		    gboolean     forward)
{
	return webkit_web_view_search_text (WEBKIT_WEB_VIEW (html),
					    html->priv->find_string,
					    FALSE,
					    forward, TRUE);
}

void
wk_html_set_find_props (WkHtml    *html,
			const char  *str,
			gboolean     match_case,
			gboolean     wrap)
{
	/* Empty */
}

void
wk_html_jump_to_anchor (WkHtml    *html,
			gchar       *anchor)
{
	WkHtmlPriv *priv;

	g_return_if_fail (html != NULL);

	priv = html->priv;

	g_free (priv->anchor);
	priv->anchor = g_strdup (anchor);
}

/*
void ClipboardTextReceivedFunc (GtkClipboard *clipboard,
                                                         const gchar *text,
                                                         gpointer data)
{

	GS_message(("clipboard text = %s",text));
}
*/

void
wk_html_copy_selection (WkHtml *html)
{
	if(webkit_web_view_has_selection(WEBKIT_WEB_VIEW (html)))
		webkit_web_view_copy_clipboard (WEBKIT_WEB_VIEW (html));
}

void
wk_html_select_all (WkHtml *html)
{
	webkit_web_view_select_all (WEBKIT_WEB_VIEW (html));
}

void
wk_html_print (WkHtml *html)
{
	webkit_web_frame_print (webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW (html)));
	//webkit_web_view_execute_script (WEBKIT_WEB_VIEW (html), "print();");
}

gboolean
wk_html_initialize (void)
{
	return TRUE;
}

void
wk_html_shutdown (void)
{
	/* Empty */
}
