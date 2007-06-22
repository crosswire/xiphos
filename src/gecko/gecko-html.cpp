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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Marco Pesenti Gritti <marco@gnome.org>
 */

#include <mozilla-config.h>
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

#include "gecko/gecko-html.h"
#include "gecko/gecko-services.h"
#include "gecko/gecko-utils.h"
#include "gecko/Yelper.h"

extern gboolean shift_key_presed;
extern gboolean in_url;

#define GECKO_HTML_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GECKO_TYPE_HTML, GeckoHtmlPriv))

struct _GeckoHtmlPriv {
	Yelper *yelper;
	gchar *base_uri;
	gchar *anchor;
	gboolean frames_enabled;
	gboolean is_dialog;
	guint timeout;
	gint pane;
	DIALOG_DATA * dialog;
};

static void html_set_fonts(void);
static void html_set_colors(void);
static void html_set_a11y(void);

enum {
	URI_SELECTED,
	FRAME_SELECTED,
	TITLE_CHANGED,
	POPUPMENU_REQUESTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static GObjectClass *parent_class = NULL;

static void html_title(GtkMozEmbed * embed)
{
	GeckoHtml *html = GECKO_HTML(embed);
	char *new_title;

	new_title = gtk_moz_embed_get_title(embed);

	if (new_title && *new_title != '\0') {
		g_signal_emit(html, signals[TITLE_CHANGED], 0, new_title);
		g_free(new_title);
	}
}

static gint html_dom_mouse_dbl_click(GtkMozEmbed * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->yelper->ProcessMouseDblClickEvent(dom_event);
}

static gint html_dom_mouse_down(GtkMozEmbed * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->yelper->ProcessMouseEvent(dom_event);
}


static gint html_dom_mouse_up(GtkMozEmbed * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->yelper->ProcessMouseUpEvent(dom_event);
}


static void html_link_message(GtkMozEmbed * embed)
{
	gchar buf[500];
	gchar *url = gtk_moz_embed_get_link_message(embed);
	GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	    
	
	GS_message(("html_link_message: url = %s",url));
	
	if(shift_key_presed)
		return;
	
	if (!strlen(url)) { //* moved out of url - clear appbar - info viewer*
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), "");
		in_url = FALSE;
		//*if(GPOINTER_TO_INT(data) == TEXT_TYPE)
		//	main_clear_viewer();*
	} else {		
		GString *url_clean = g_string_new(NULL);
		const gchar *url_chase;
		int i = 0;
		for (url_chase = url; *url_chase; ++url_chase) {
			switch (*url_chase) {
			case '/':
				if(i > 2)
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
		in_url = TRUE;	//* we need this for html_button_released *
		if(priv->is_dialog) {
			if(main_dialogs_url_handler(priv->dialog, url_clean->str, FALSE)) {	
				g_string_free(url_clean, TRUE);	
				return;
			}
			
		} else {
			if(main_url_handler_gecko(url_clean->str)) {	
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
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), buf);
	}
}


static 
gint html_dom_mouse_over(GtkMozEmbed * embed, gpointer dom_event)
{
/*	GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	return html->priv->yelper->ProcessMouseOver(dom_event, 
						    priv->pane,
						    priv->is_dialog,
						    priv->dialog);*/
	return 0;
}

static
gint html_dom_mouse_out(GtkMozEmbed * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	if(priv->pane == VIEWER_TYPE)
		shift_key_presed = FALSE;
	return 1; 
}

static
gint html_dom_key_down(GtkMozEmbed * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->yelper->ProcessKeyDownEvent(embed, dom_event);
}
static
gint html_dom_key_up(GtkMozEmbed * embed, gpointer dom_event)
{
	GeckoHtml *html = GECKO_HTML(embed);
	return html->priv->yelper->ProcessKeyReleaseEvent(embed, dom_event);
}

static gint html_open_uri(GtkMozEmbed * embed, const gchar * uri)
{
	g_return_val_if_fail(uri != NULL, FALSE);
	GeckoHtml *html = GECKO_HTML(embed);
	GeckoHtmlPriv *priv = GECKO_HTML_GET_PRIVATE(html);
	
	// prefixable link
	if(priv->pane == DIALOG_COMMENTARY_TYPE) { 
		gchar *buf, *place;
		gchar tmpbuf[1023];
		gchar book[32];
		strcpy(book, priv->dialog->key);
		*(strrchr(book, ' ')) = '\0';
		
		if (buf = strstr(uri, "&value=")) {
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
	return TRUE;
}

static void html_realize(GtkWidget * widget)
{
	GeckoHtml *html = GECKO_HTML(widget);

	GTK_WIDGET_CLASS(parent_class)->realize(widget);

	nsresult rv;
	rv = html->priv->yelper->Init();

	if (NS_FAILED(rv)) {
		GS_warning(("gecko initialization failed for %p\n",
			    (void *) html));
	}
}

static void html_init(GeckoHtml * html)
{
	GeckoHtmlPriv *priv;
	GeckoHtmlClass *klass;

	html->priv = priv = GECKO_HTML_GET_PRIVATE(html);

	priv->base_uri = NULL;
	priv->anchor = NULL;
	priv->timeout = 0;

	priv->yelper = new Yelper(GTK_MOZ_EMBED(html));
	klass = GECKO_HTML_GET_CLASS(html);
/*	if (!klass->font_handler) {
		klass->font_handler =
		   gecko_settings_notify_add (GECKO_SETTINGS_INFO_FONTS,
		   (GHookFunc) html_set_fonts,
		   NULL);
		   html_set_fonts (); 
	}
	if (!klass->color_handler) {
		klass->color_handler =
		   gecko_settings_notify_add (GS_SETTINGS_INFO_COLOR,
		   (GHookFunc) html_set_colors,
		   NULL);
		   html_set_colors (); 
	}
	if (!klass->a11y_handler) {
		klass->a11y_handler =
		   gecko_settings_notify_add (GS_SETTINGS_INFO_A11Y,
		   (GHookFunc) html_set_a11y,
		   NULL);
		   html_set_a11y (); 
	} */
}

static void html_dispose(GObject * object)
{
	GeckoHtml *html = GECKO_HTML(object);

	html->priv->yelper->Destroy();

	parent_class->dispose(object);
}

static void html_finalize(GObject * object)
{
	GeckoHtml *html = GECKO_HTML(object);
	GeckoHtmlPriv *priv = html->priv;

	delete priv->yelper;

	if (priv->timeout)
		g_source_remove(priv->timeout);
	g_free(priv->base_uri);
	g_free(priv->anchor);

	parent_class->finalize(object);
}

static void html_class_init(GeckoHtmlClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
	GtkMozEmbedClass *moz_embed_class = GTK_MOZ_EMBED_CLASS(klass);

	parent_class = (GObjectClass *) g_type_class_peek_parent(klass);

	object_class->finalize = html_finalize;
	object_class->dispose = html_dispose;

	widget_class->realize = html_realize;

	moz_embed_class->title = html_title;
	moz_embed_class->dom_mouse_over = html_dom_mouse_over;
	moz_embed_class->dom_mouse_down = html_dom_mouse_down;
	moz_embed_class->dom_mouse_up = html_dom_mouse_up;
	moz_embed_class->dom_mouse_out = html_dom_mouse_out;
	//moz_embed_class->dom_mouse_click = html_dom_mouse_click;
	moz_embed_class->dom_mouse_dbl_click = html_dom_mouse_dbl_click;
	moz_embed_class->open_uri = html_open_uri;
	moz_embed_class->link_message = html_link_message;  
	moz_embed_class->dom_key_down = html_dom_key_down;
	moz_embed_class->dom_key_up = html_dom_key_up;
	klass->font_handler = 0;
	klass->color_handler = 0;
	klass->a11y_handler = 0;

/*	signals[URI_SELECTED] = g_signal_new("uri_selected", G_TYPE_FROM_CLASS(klass), 
			     G_SIGNAL_RUN_LAST, 
			     G_STRUCT_OFFSET(GeckoHtmlClass, uri_selected), 
			     NULL, NULL, 
			     NULL,	//gs_marshal_VOID__POINTER_BOOLEAN,
			     G_TYPE_NONE,
			     2, G_TYPE_POINTER,
			     G_TYPE_BOOLEAN);
*/
/*
	signals[FRAME_SELECTED] = g_signal_new("frame_selected", 
			       G_TYPE_FROM_CLASS(klass), 
			       G_SIGNAL_RUN_LAST, 
			       G_STRUCT_OFFSET(GeckoHtmlClass, frame_selected), 
			       g_signal_accumulator_true_handled, 
			       NULL, 
			       NULL,	//gs_marshal_BOOLEAN__POINTER_BOOLEAN,
			       G_TYPE_BOOLEAN,
			       2, G_TYPE_POINTER,
			       G_TYPE_BOOLEAN);
*/
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

		type = g_type_register_static(GTK_TYPE_MOZ_EMBED,
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
	// debug_print (DB_FUNCTION, "entering\n");

	// html->priv->frames_enabled = FALSE;
	gtk_moz_embed_open_stream(GTK_MOZ_EMBED(html), "file:///",	//html->priv->base_uri,
				  mime);
}

void gecko_html_write(GeckoHtml * html, const gchar * data, gint len)
{
	if (len == -1)
		len = strlen(data);
	gtk_moz_embed_append_data(GTK_MOZ_EMBED(html), data, len);
	GS_message(("html_write: \n\tlength = %d \n\tdata: %s",len,data));
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
	g_signal_emit_by_name(gtk_widget_get_accessible(GTK_WIDGET(html)),
			      "children_changed::add", -1, NULL, NULL);
	html->priv->timeout = 0;
	return FALSE;
}

void gecko_html_close(GeckoHtml * html)
{
//    debug_print (DB_FUNCTION, "entering\n");
	gtk_moz_embed_close_stream(GTK_MOZ_EMBED(html));
	html->priv->timeout = g_timeout_add(2000,
					    (GSourceFunc) timeout_update_gok,
					    html);
}

gboolean gecko_html_find(GeckoHtml * html, const gchar * find_string)
{
	return html->priv->yelper->Find(find_string);
}

gboolean gecko_html_find_again(GeckoHtml * html, gboolean forward)
{
	return html->priv->yelper->FindAgain((PRBool) forward);
}

void
gecko_html_set_find_props(GeckoHtml * html,
			  const char *str, gboolean match_case, gboolean wrap)
{
	html->priv->yelper->SetFindProperties(str, (PRBool) match_case,
					      (PRBool) wrap);
}

void gecko_html_jump_to_anchor(GeckoHtml * html, gchar * anchor)
{
	GeckoHtmlPriv *priv;

	g_return_if_fail(html != NULL);

	priv = html->priv;

	g_free(priv->anchor);
	priv->anchor = g_strdup(anchor);
	priv->yelper->JumpToAnchor(anchor);
}

void gecko_html_copy_selection(GeckoHtml * html)
{
	html->priv->yelper->DoCommand("cmd_copy");
}

void gecko_html_paste(GeckoHtml * html)
{
	html->priv->yelper->DoCommand("cmd_paste");
}

void gecko_html_select_all(GeckoHtml * html)
{
	html->priv->yelper->DoCommand("cmd_selectAll");
}

void gecko_html_select_none(GeckoHtml * html)
{
	html->priv->yelper->DoCommand("cmd_selectNone");
}

#ifdef USE_GTKUPRINT
void
gecko_html_print(GeckoHtml * html, GeckoPrintInfo * info, gboolean preview,
		 gint * npages)
{
	html->priv->yelper->Print(info, preview, npages);
}
#endif
void gecko_html_preview_navigate(GeckoHtml * html, gint page_no)
{
	html->priv->yelper->PrintPreviewNavigate(page_no);
}

void gecko_html_preview_end(GeckoHtml * html)
{
	html->priv->yelper->PrintPreviewEnd();
}

static void html_set_fonts(void)
{
/*	  gchar *font;

	   font = gecko_settings_get_font (GECKO_FONT_VARIABLE);
	   gecko_set_font (GECKO_FONT_VARIABLE, font);
	   g_free (font);

	   font = gecko_settings_get_font (GECKO_FONT_FIXED);
	   gecko_set_font (GECKO_FONT_FIXED, font);
	   g_free (font); */
}

static void html_set_colors(void)
{
	/*   const gchar *color;

	   color = gs_settings_get_color (GS_COLOR_FG);
	   gs_gecko_set_color (GS_COLOR_FG, color);

	   color = gs_settings_get_color (GS_COLOR_BG);
	   gs_gecko_set_color (GS_COLOR_BG, color);

	   color = gs_settings_get_color (GS_COLOR_ANCHOR);
	   gs_gecko_set_color (GS_COLOR_ANCHOR, color); */
}

static void html_set_a11y(void)
{
/*    gboolean caret;

    caret = gs_settings_get_caret ();
    gs_gecko_set_caret (caret);*/
}

gboolean gecko_html_initialize(void)
{
	return gecko_init();
}

void gecko_html_shutdown(void)
{
	GS_message(("gecko shutdown"));
	gecko_shutdown();
}

void
gecko_html_print_document(GtkWindow * window, gchar * mod_name,
			  gpointer print_html)
{
#ifdef USE_GTKUPRINT	
	GtkWidget *gtk_win;
	GeckoHtml *html;
	GtkWidget *vbox = gtk_vbox_new(FALSE, FALSE);
	SWDisplay *swdisplay = 0;
	SWMgr *mgr = backend->get_display_mgr();
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
	
	if (!strcmp(mod->Type(), TEXT_MODS))
		swdisplay = new GTKPrintChapDisp(GTK_WIDGET(html), backend);
	else 
		swdisplay = new GTKPrintEntryDisp(GTK_WIDGET(html), backend);
	
	mod->setDisplay(swdisplay);
	mod->Display();

	gecko_print_run(window, html, gtk_win, vbox);
	if (swdisplay)
		delete swdisplay;
#endif
}
