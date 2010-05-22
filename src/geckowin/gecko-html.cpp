/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2004 Marco Pesenti Gritti
 * Copyright (C) 2005 Christian Persch
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Marco Pesenti Gritti <marco@gnome.org>
 */

#include <windows.h>
#include <ctype.h>
#include "gdk/gdkwin32.h"
#include "gtk/gtk.h"

#include <xpcom-config.h>
#include <config.h>

#include <swmgr.h>
#include <swmodule.h>
#include <url.h>
#include <swbuf.h>

#include "backend/sword_main.hh"

#include "main/display.hh"
#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/url.hh"

#include "gui/widgets.h"
#include "gui/utilities.h"

#include "geckowin/gecko-html.h"
#include "geckowin/signals.h"
#include "geckowin/BrowserEmbed.h"
#include "geckowin/WindowCreator.h"
#include "xulapp/nsXULAppAPI.h"
#include "Init.h"

#ifdef NS_HIDDEN
# undef NS_HIDDEN
#endif
#ifdef NS_IMPORT_
# undef NS_IMPORT_
#endif
#ifdef NS_EXPORT_
# undef NS_EXPORT_
#endif



extern gboolean shift_key_pressed;
extern gboolean in_url;

#define GECKO_HTML_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GECKO_TYPE_HTML, GeckoHtmlPriv))

struct _GeckoHtmlPriv {
	BrowserEmbed *embed;
	gchar *base_uri;
	gchar *anchor;
	gboolean frames_enabled;
	gboolean is_dialog;
	guint timeout;
	gint pane;
	DIALOG_DATA * dialog;
};

guint signals[LAST_SIGNAL] = { 0 };

static GObjectClass *parent_class = NULL;
static WindowCreator *creator = NULL;

static gint
handle_child_focus_in(GtkWidget *aWidget,
		      GdkEventMotion *event,
		      gpointer user_data)
{
	GeckoHtml *html = GECKO_HTML(aWidget);
	GS_message(("focus in"));
	html->priv->embed->ChildFocusIn();
	return FALSE;
}

static gint
handle_child_focus_out(GtkWidget *aWidget,
		       GdkEventFocus *aGdkFocusEvent,
		       GeckoHtml *embed)
{
	GS_message(("focus out"));
	embed->priv->embed->ChildFocusOut();
}

static void html_title(GeckoHtml * embed)
{
	GeckoHtml *html = GECKO_HTML(embed);
	wchar_t *new_title;

	html->priv->embed->getCurrentTitle(&new_title);

	if (new_title && *new_title != '\0') {
		g_signal_emit(html, signals[TITLE_CHANGED], 0, new_title);
		g_free(new_title);
	}
}

static gint html_dom_mouse_dbl_click(GeckoHtml * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->embed->ProcessMouseDblClickEvent(dom_event);
}

static gint html_dom_mouse_down(GeckoHtml * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
    	return html->priv->embed->ProcessMouseEvent(dom_event);
}


static gint html_dom_mouse_up(GeckoHtml * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->embed->ProcessMouseUpEvent(dom_event);
}


static void html_link_message(GeckoHtml * embed)
{
	gchar buf[500];
	GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	gchar *url = priv->embed->GetLinkMessage();
	
	GS_message(("html_link_message: url = %s",url));
	
	if (shift_key_pressed)
		return;
	
	if (!strlen(url)) { //* moved out of url - clear appbar - info viewer*
		gui_set_statusbar ("");
		in_url = FALSE;
		//*if(GPOINTER_TO_INT(data) == TEXT_TYPE)
		//	main_clear_viewer();*
	} else {		
		GString *url_clean = g_string_new(NULL);
		const gchar *url_chase;
		int i = 0;
		in_url = TRUE;	//* we need this for html_button_released *
		for (url_chase = url; *url_chase; ++url_chase) {
			switch (*url_chase) {
			case '/':
				if (i > 2)
				        g_string_append(url_clean, "%2F");
				else
					g_string_append_c(url_clean, *url_chase);
				++i;
				break;
			default:
					g_string_append_c(url_clean, *url_chase);
				break;
			}
		}
		GS_message(("\nurl:        %s\nurl_clean: %s",url,url_clean->str));
		if (priv->is_dialog) {
			if (main_dialogs_url_handler(priv->dialog, url_clean->str, FALSE)) {
				g_string_free(url_clean, TRUE);
				return;
			}
		} else {
			if (main_url_handler ((strncmp(url, "http", 4)
						    ? url_clean->str
						    : url),FALSE)) {
				g_string_free(url_clean, TRUE);
				return;
			}
		}
		g_string_free(url_clean, TRUE);
		if (*url == 'I') {
			return;
		} else if (*url == 'U') {
			++url;
			sprintf(buf, "%s %s", _("Unlock "), url);
		} else //* any other link *
			sprintf(buf, "%s", "");
		gui_set_statusbar (buf);
	}
}


static 
gint html_dom_mouse_over(GeckoHtml * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	return priv->embed->ProcessMouseOver(dom_event, 
					     priv->pane,
					     priv->is_dialog,
					     priv->dialog);
}


static
gint html_dom_mouse_out(GeckoHtml * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	if(priv->pane == VIEWER_TYPE)
		shift_key_pressed = FALSE;
	return 1; 
}

static
gint html_dom_key_down(GeckoHtml * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->embed->ProcessKeyDownEvent(dom_event);
}
static
gint html_dom_key_up(GeckoHtml * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->embed->ProcessKeyReleaseEvent(dom_event);
}

static gint html_open_uri(GeckoHtml * embed, const gchar * uri)
{
	GS_message(("in html_open_uri"));
	g_return_val_if_fail(uri != NULL, FALSE);

	GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	gchar *buf, *place;
	gchar tmpbuf[1023];
	gchar book[32];
	GString *tmpstr = g_string_new(NULL);

	// for some reason, `/' is not properly encoded for us as "%2F"
	// so we have to do it ourselves.  /mutter/ *ick*
	if ((place = strchr((char*)uri, '?'))) {	// url's beginning, as-is.
		strncpy(tmpbuf, uri, (++place)-uri);
		tmpbuf[place-uri] = '\0';
		tmpstr = g_string_append(tmpstr, tmpbuf);
		for (/* */ ; *place; ++place) {
			switch (*place) {
			case '/':
				tmpstr = g_string_append(tmpstr, "%2F"); break;
			case ':':
				tmpstr = g_string_append(tmpstr, "%3A"); break;
			case ' ':
				tmpstr = g_string_append(tmpstr, "%20"); break;
			default:
				tmpstr = g_string_append_c(tmpstr, *place);
			}
		}
		uri = tmpstr->str;
	}
	
	// prefixable link
	if(priv->pane == DIALOG_COMMENTARY_TYPE) { 
		strcpy(book, priv->dialog->key);
		*(strrchr(book, ' ')) = '\0';
		
		if ((buf = strstr((char*)uri, "&value="))) {
			buf += 7;
			place = buf;
	
			while (*buf && isdigit(*buf))
				buf++;
			if ((*buf == ':') || strncmp(buf, "%3A", 3) == 0) {
				/* url begins w/"digits:" only: fix */
				strncpy(tmpbuf, uri, place - uri);
				strcpy(tmpbuf+(place-uri), book);
				strcat(tmpbuf, place);
				uri = tmpbuf;
			}
		}
		
	}
	
	GS_message(("uri: %s", uri));
	if(priv->is_dialog)
		main_dialogs_url_handler(priv->dialog, uri, TRUE);
	else
		main_url_handler(uri, TRUE);

	g_string_free(tmpstr, TRUE);
	return TRUE;
}

void gecko_html_set_parent(GtkWidget *widget, GtkWidget *owner)
{
/* 	GeckoHtml *html = GECKO_HTML(widget); */

/* 	html->priv->embed = new BrowserEmbed(); */
/* 	long unsigned int hwnd = (long unsigned int)GDK_WINDOW_HWND(owner->window); */
/* 	html->priv->embed->init(hwnd, -50, -50, owner->allocation.width, */
/* 				owner->allocation.height, html); */
}
static void html_realize(GtkWidget * widget)
{
	GeckoHtml *html = GECKO_HTML(widget);

	GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

	GdkWindowAttr attributes;
	guint attributes_mask;

	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.event_mask = gtk_widget_get_events(widget) | GDK_ALL_EVENTS_MASK;
	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);

	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
	
	widget->window = gdk_window_new(
		gtk_widget_get_parent_window(widget),
		& attributes, attributes_mask
		);
	
	gdk_window_set_user_data(widget->window, html);

	widget->style = gtk_style_attach(widget->style, widget->window);
	gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
	//gtk_widget_realize(widget);

	while (gtk_events_pending())
	    gtk_main_iteration();

	nsresult rv;
	gint x, y, width, height;
	gdk_window_get_geometry(widget->window, &x, &y, &width, &height, NULL);
	long unsigned int hwnd = (long unsigned int)GDK_WINDOW_HWND(widget->window);
	GS_message(("handle is %d", (hwnd ? hwnd : 666)));
	GS_message(("pane is %d", html->priv->pane));

	rv = html->priv->embed->init(hwnd, attributes.x, attributes.y, widget->allocation.width,
				     widget->allocation.height, html);

	if (NS_FAILED(rv)) {
		GS_warning(("gecko initialization failed for %p\n",
			    (void *) html));
	}

	GtkWidget *child_widget = widget;
	g_signal_connect_object(G_OBJECT(child_widget),
				"focus-out-event",
				GTK_SIGNAL_FUNC(handle_child_focus_out),
				html,
				G_CONNECT_AFTER);
	g_signal_connect_object(G_OBJECT(child_widget),
				"motion-notify-event",
				GTK_SIGNAL_FUNC(handle_child_focus_in),
				html,
				G_CONNECT_AFTER);
}

static void html_init(GeckoHtml * html)
{
	GeckoHtmlPriv *priv;
	GeckoHtmlClass *klass;

	html->priv = priv = GECKO_HTML_GET_PRIVATE(html);

	priv->base_uri = NULL;
	priv->anchor = NULL;
	priv->timeout = 0;
	
	priv->embed = new BrowserEmbed();
	klass = GECKO_HTML_GET_CLASS(html);
}

static void html_dispose(GObject * object)
{
	GeckoHtml *html = GECKO_HTML(object);

	html->priv->embed->destroy();

	parent_class->dispose(object);
}

static void html_finalize(GObject * object)
{
	GeckoHtml *html = GECKO_HTML(object);
	GeckoHtmlPriv *priv = html->priv;

	delete priv->embed;

	if (priv->timeout)
		g_source_remove(priv->timeout);
	g_free(priv->base_uri);
	g_free(priv->anchor);

	parent_class->finalize(object);
}

static void html_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
    GeckoHtml *html;
    html = GECKO_HTML(widget);
   
    widget->allocation = *allocation;

    if (GTK_WIDGET_REALIZED(widget))
	{
	    gdk_window_move_resize(widget->window,
				   allocation->x, allocation->y,
				   allocation->width, allocation->height);

	    gint x, y, width, height;
	    //gdk_window_get_geometry(gtk_widget_get_parent_window(widget),
	    //		    &x, &y, &width, &height, NULL);
	    html->priv->embed->resize(allocation->x, allocation->y, allocation->width, allocation->height);
	}
}

static void html_class_init(GeckoHtmlClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
//	GtkMozEmbedClass *moz_embed_class = GTK_MOZ_EMBED_CLASS(klass);

	parent_class = (GObjectClass *) g_type_class_peek_parent(klass);

	object_class->finalize = html_finalize;
	object_class->dispose = html_dispose;

	widget_class->realize = html_realize;

	widget_class->size_allocate = html_size_allocate;

	klass->font_handler = 0;
	klass->color_handler = 0;
	klass->a11y_handler = 0;
	klass->link_message = html_link_message;


	signals[LINK_MESSAGE] =
	    g_signal_new("link_message",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass,
					 link_message),
			 NULL, NULL,
			 gtk_marshal_NONE__NONE,
			 G_TYPE_NONE, 0);

	signals[HTML_TITLE] = 
	    g_signal_new("html_title",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_FIRST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_title),
			 NULL, NULL,
			 gtk_marshal_NONE__NONE,
			 G_TYPE_NONE, 0);

	signals[HTML_OPEN_URI] = 
	    g_signal_new("html_open_uri",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_open_uri),
			 NULL, NULL,
			 gtk_marshal_VOID__STRING,
			 G_TYPE_BOOLEAN, 1, G_TYPE_CHAR);
	
	signals[HTML_DOM_KEY_DOWN] = 
	    g_signal_new("html_dom_key_down",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_dom_key_down),
			 NULL, NULL,
			 gtk_marshal_BOOL__POINTER,
			 G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
	
	signals[HTML_DOM_KEY_UP] =
	    g_signal_new("html_dom_key_up",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_dom_key_up),
			 NULL, NULL,
			 gtk_marshal_BOOL__POINTER,
			 G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);

	signals[HTML_DOM_MOUSE_DOWN] = 
	    g_signal_new("html_dom_mouse_down",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_dom_mouse_down),
			 NULL, NULL,
			 gtk_marshal_BOOL__POINTER,
			 G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);
	
	signals[HTML_DOM_MOUSE_UP] = 
	    g_signal_new("html_dom_mouse_up",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_dom_mouse_up),
			 NULL, NULL,
			 gtk_marshal_BOOL__POINTER,
			 G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);

	signals[HTML_DOM_MOUSE_DBL_CLICK] =
	    g_signal_new("html_dom_mouse_dbl_click",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_dom_mouse_dbl_click),
			 NULL, NULL,
			 gtk_marshal_BOOL__POINTER,
			 G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);

	signals[HTML_DOM_MOUSE_OVER] = 
	    g_signal_new("html_dom_mouse_over",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_dom_mouse_over),
			 NULL, NULL,
			 gtk_marshal_BOOL__POINTER,
			 G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);

	signals[HTML_DOM_MOUSE_OUT] = 
	    g_signal_new("html_dom_mouse_out",
			 G_TYPE_FROM_CLASS(klass),
			 G_SIGNAL_RUN_LAST,
			 G_STRUCT_OFFSET(GeckoHtmlClass, html_dom_mouse_out),
			 NULL, NULL,
			 gtk_marshal_BOOL__POINTER,
			 G_TYPE_BOOLEAN, 1, G_TYPE_POINTER);

			 
	signals[TITLE_CHANGED] =
		g_signal_new("title_changed",
			     G_TYPE_FROM_CLASS(klass),
			     G_SIGNAL_RUN_LAST,
			     G_STRUCT_OFFSET(GeckoHtmlClass,
					     title_changed),
			     NULL, NULL,
			     g_cclosure_marshal_VOID__STRING,
			     G_TYPE_NONE, 1, G_TYPE_STRING);

	signals[POPUPMENU_REQUESTED] =
		g_signal_new("popupmenu_requested",
			     G_TYPE_FROM_CLASS(klass),
			     G_SIGNAL_RUN_LAST,
			     G_STRUCT_OFFSET(GeckoHtmlClass,
					     popupmenu_requested),
			     NULL, NULL,
			     g_cclosure_marshal_VOID__STRING,
			     G_TYPE_NONE, 1, G_TYPE_STRING);

	g_type_class_add_private(klass, sizeof(GeckoHtmlPriv));
}

GType gecko_html_get_type(void)
{
	static GType type = 0;

	if (G_UNLIKELY(type == 0)) {
		static const GTypeInfo info = {
			sizeof(GeckoHtmlClass),
			NULL,
			NULL,
			(GClassInitFunc) html_class_init,
			NULL,
			NULL,
			sizeof(GeckoHtml),
			0,
			(GInstanceInitFunc) html_init,
		};

		type = g_type_register_static(GTK_TYPE_WIDGET,
					      "GeckoHtml",
					      &info, (GTypeFlags) 0);
	}

	return type;
}

GeckoHtml *gecko_html_new(DIALOG_DATA * dialog, gboolean is_dialog, gint pane)
{
	GeckoHtml *html;
	html = GECKO_HTML(g_object_new(GECKO_TYPE_HTML, NULL));
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	priv->pane = pane;
	priv->is_dialog = is_dialog;
	priv->dialog = dialog;
	
	//make scrollbar appear on the left for RTL locales
	GtkTextDirection dir = gtk_widget_get_direction(GTK_WIDGET (html));
	if (dir == GTK_TEXT_DIR_RTL)
	    priv->embed->SetRTL();
	return html;
}

void gecko_html_set_base_uri(GeckoHtml * html, const gchar * uri)
{
	GeckoHtmlPriv *priv;

	g_return_if_fail(GECKO_IS_HTML(html));

	//debug_print (DB_FUNCTION, "entering\n");
	//debug_print (DB_ARG, "  uri = \"%s\"\n", uri);

	priv = html->priv;

	if (priv->base_uri)
		g_free(priv->base_uri);

	priv->base_uri = g_strdup(uri);
}

void gecko_html_open_stream(GeckoHtml * html, const gchar * mime)
{
	GeckoHtmlPriv *priv;
	priv = html->priv;

	priv->embed->OpenStream("file:///", mime);

}

void gecko_html_write(GeckoHtml * html, const gchar * data, gint len)
{
	GeckoHtmlPriv *priv;
	priv = html->priv;

	if (len == -1)
		len = strlen(data);
	priv->embed->AppendToStream((const PRUint8*)data, len);
}

void gecko_html_frames(GeckoHtml * html, gboolean enable)
{
	html->priv->frames_enabled = enable;
}

void gecko_html_printf(GeckoHtml * html, char *format, ...)
{
	va_list args;
	gchar *string;

	g_return_if_fail(format != NULL);

	va_start(args, format);
	string = g_strdup_vprintf(format, args);
	va_end(args);

	gecko_html_write(html, string, -1);

	g_free(string);
}

/* Fire "children_changed::add" event to refresh "UI-Grab" window of GOK,
 * this event is not fired when using gtk_moz_embed_xxx_stream,
 * see Mozilla bug #293670.  Done in a timeout to allow mozilla to
 * actually draw to the screen */

static gboolean timeout_update_gok(GeckoHtml * html)
{
	//g_signal_emit_by_name(gtk_widget_get_accessible(GTK_WIDGET(html)),
	//		      "children_changed::add", -1, NULL, NULL);
	html->priv->timeout = 0;
	return FALSE;
}

void gecko_html_close(GeckoHtml * html)
{
	GeckoHtmlPriv *priv;
	priv = html->priv;

	priv->embed->CloseStream();
	
	//going to test to see if this is still needed
/*	html->priv->timeout = g_timeout_add(2000,
					    (GSourceFunc) timeout_update_gok,
					    html);
*/
}

void gecko_html_render_data(GeckoHtml *html, const char *data, guint32 len)
{
	GeckoHtmlPriv *priv;
	priv = html->priv;

	priv->embed->OpenStream("file:///sword", "text/html");
	priv->embed->AppendToStream((const PRUint8*) data, len);
	priv->embed->CloseStream();
}

gboolean gecko_html_find(GeckoHtml * html, const gchar * find_string)
{
	return html->priv->embed->Find(find_string);
}

gboolean gecko_html_find_again(GeckoHtml * html, gboolean forward)
{
	return html->priv->embed->FindAgain((PRBool) forward);
}

void
gecko_html_set_find_props(GeckoHtml * html,
			  const char *str, gboolean match_case, gboolean wrap)
{
	html->priv->embed->SetFindProperties((PRBool) match_case);
}

void gecko_html_jump_to_anchor(GeckoHtml * html, gchar * anchor)
{
	GeckoHtmlPriv *priv;

	g_return_if_fail(html != NULL);

	priv = html->priv;

	g_free(priv->anchor);
	priv->anchor = g_strdup(anchor);
	priv->embed->JumpToAnchor(anchor);
}

void gecko_html_copy_selection(GeckoHtml * html)
{
	html->priv->embed->DoCommand("cmd_copy");
}

void gecko_html_paste(GeckoHtml * html)
{
	html->priv->embed->DoCommand("cmd_paste");
}

void gecko_html_select_all(GeckoHtml * html)
{
	html->priv->embed->DoCommand("cmd_selectAll");
}

void gecko_html_select_none(GeckoHtml * html)
{
	html->priv->embed->DoCommand("cmd_selectNone");
}

#ifdef USE_GTKUPRINT
void
gecko_html_print(GeckoHtml * html, GeckoPrintInfo * info, gboolean preview,
		 gint * npages)
{
	html->priv->embed->Print(info, preview, npages);
}
#endif
void gecko_html_preview_navigate(GeckoHtml * html, gint page_no)
{
	//html->priv->embed->PrintPreviewNavigate(page_no);
}

void gecko_html_preview_end(GeckoHtml * html)
{
	//html->priv->embed->PrintPreviewEnd();
}

gboolean gecko_html_initialize(void)
{
	//nsresult rv;
	//rv = XRE_InitEmbedding(nsnull, nsnull, nsnull, nsnull, nsnull);
	gchar *xul_dir = g_win32_get_package_installation_directory_of_module(NULL);
	xul_dir = g_strconcat (xul_dir, "\0", NULL);
	xul_dir = g_build_filename (xul_dir, "bin\0");
	GS_message((xul_dir));
	init_xulrunner (xul_dir, xul_dir);	
	creator = new WindowCreator();
	creator->install();

}

void gecko_html_shutdown(void)
{
/*	GS_message(("gecko shutdown"));
	shutdown_xulrunner();
*/
}

void
gecko_html_print_document(GtkWindow * window, gchar * mod_name,
			  DIALOG_DATA * dialog)
{
#ifdef USE_GTKUPRINT	
	GtkWidget *gtk_win;
	GeckoHtml *html;
	GtkWidget *vbox = gtk_vbox_new(FALSE, FALSE);
    	SWDisplay *old_display = 0;
	SWDisplay *swdisplay = 0;
	SWMgr *mgr;
	if(dialog) {
		BackEnd *be = (BackEnd *)dialog->backend;
		mgr = be->get_mgr();
	}
	else 
		mgr = backend->get_mgr();
	
	SWModule *mod = mgr->Modules[mod_name];
	
	if (!mod)
		return;
    
	gtk_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	html = gecko_html_new(NULL,FALSE,8);
	gtk_container_add(GTK_CONTAINER(gtk_win), GTK_WIDGET(vbox));
	gtk_box_pack_end(GTK_BOX(vbox), GTK_WIDGET(html), TRUE, TRUE, 0);
	gtk_widget_show(gtk_win);
	gtk_widget_show(vbox);
	gtk_widget_show(GTK_WIDGET(html));
	gtk_widget_hide(gtk_win);
    
	old_display = mod->getDisplay();
    
	if (!strcmp(mod->Type(), TEXT_MODS))
		swdisplay = new GTKPrintChapDisp(GTK_WIDGET(html), backend);
	else 
		swdisplay = new GTKPrintEntryDisp(GTK_WIDGET(html), backend);
	
	mod->setDisplay(swdisplay);
	mod->Display();

	gecko_print_run(window, html, gtk_win, vbox);
    
    	mod->setDisplay(old_display);
	if (swdisplay)
		delete swdisplay;
#endif
}

void gecko_html_emit_link_message(GeckoHtml *html)
{
	g_signal_emit(G_OBJECT(html), signals[LINK_MESSAGE], 0);
}

void gecko_html_emit_title_changed(GeckoHtml *html)
{
	g_signal_emit(G_OBJECT(html), signals[HTML_TITLE], 0);
}

gboolean gecko_html_emit_uri_open(GeckoHtml *html, const gchar *uri)
{
	GS_message(("emit uri open %s", uri));
	gint return_val = FALSE;
	GS_message(("in html_open_uri"));
	g_return_val_if_fail(uri != NULL, FALSE);

	//GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	gchar *buf, *place;
	gchar tmpbuf[1023];
	gchar book[32];
	GString *tmpstr = g_string_new(NULL);

	// for some reason, `/' is not properly encoded for us as "%2F"
	// so we have to do it ourselves.  /mutter/ *ick*
	if ((place = strchr((char*)uri, '?'))) {	// url's beginning, as-is.
		strncpy(tmpbuf, uri, (++place)-uri);
		tmpbuf[place-uri] = '\0';
		tmpstr = g_string_append(tmpstr, tmpbuf);
		for (/* */ ; *place; ++place) {
			switch (*place) {
			case '/':
				tmpstr = g_string_append(tmpstr, "%2F"); break;
			case ':':
				tmpstr = g_string_append(tmpstr, "%3A"); break;
			case ' ':
				tmpstr = g_string_append(tmpstr, "%20"); break;
			default:
				tmpstr = g_string_append_c(tmpstr, *place);
			}
		}
		uri = tmpstr->str;
	}
	
	// prefixable link
	if(priv->pane == DIALOG_COMMENTARY_TYPE) { 
		strcpy(book, priv->dialog->key);
		*(strrchr(book, ' ')) = '\0';
		
		if ((buf = strstr((char*)uri, "&value="))) {
			buf += 7;
			place = buf;
	
			while (*buf && isdigit(*buf))
				buf++;
			if ((*buf == ':') || strncmp(buf, "%3A", 3) == 0) {
				/* url begins w/"digits:" only: fix */
				strncpy(tmpbuf, uri, place - uri);
				strcpy(tmpbuf+(place-uri), book);
				strcat(tmpbuf, place);
				uri = tmpbuf;
			}
		}
		
	}
	
	GS_message(("uri: %s", uri));
	if(priv->is_dialog)
		main_dialogs_url_handler(priv->dialog, uri, TRUE);
	else
		main_url_handler(uri, TRUE);

	g_string_free(tmpstr, TRUE);
	return TRUE;

}
