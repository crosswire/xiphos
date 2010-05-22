/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2001-2002 Mikael Hallendal <micke@imendio.com>
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
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Author: Mikael Hallendal <micke@imendio.com>
 */

#ifndef __GECKO_HTML_H__
#define __GECKO_HTML_H__

#include <gtk/gtk.h>
#include "main/module_dialogs.h"

G_BEGIN_DECLS

#define GECKO_TYPE_HTML         (gecko_html_get_type ())
#define GECKO_HTML(o)           (GTK_CHECK_CAST ((o), GECKO_TYPE_HTML, GeckoHtml))
#define GECKO_HTML_CLASS(k)     (GTK_CHECK_FOR_CAST((k), GECKO_TYPE_HTML, GeckoHtmlClass))
#define GECKO_IS_HTML(o)        (GTK_CHECK_TYPE ((o), GECKO_TYPE_HTML))
#define GECKO_IS_HTML_CLASS(k)  (GTK_CHECK_CLASS_TYPE ((k), GECKO_TYPE_HTML))
#define GECKO_HTML_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GECKO_TYPE_HTML, GeckoHtmlClass))

typedef struct _GeckoHtml        GeckoHtml;
typedef struct _GeckoHtmlClass   GeckoHtmlClass;
typedef struct _GeckoHtmlPriv    GeckoHtmlPriv;

struct _GeckoHtml {
	GtkWidget widget;

	GeckoHtmlPriv *priv;
};

struct _GeckoHtmlClass {
        GtkWidgetClass parent_class;

	guint font_handler;
	guint color_handler;
	guint a11y_handler;

	/* Signals */
	void (*uri_selected)   (GeckoHtml    *view,
				gchar       *uri,
				gboolean     handled);
	gboolean (*frame_selected) (GeckoHtml    *view,
				gchar       *uri,
				gboolean    handled);
	void (*title_changed)  (GeckoHtml    *view,
				const gchar *new_title);
	void (*popupmenu_requested) (GeckoHtml *view,
				     const gchar *link);
	void (*link_message) (GeckoHtml *view);

	void (*html_title) (GeckoHtml *view);

	gint (*html_dom_key_down) (GeckoHtml *view, gpointer dom_event);

	gint (*html_dom_key_up) (GeckoHtml *view, gpointer dom_event);

	gint (*html_dom_mouse_down) (GeckoHtml *view, gpointer dom_event);

	gint (*html_dom_mouse_up) (GeckoHtml *view, gpointer dom_event);

	gint (*html_dom_mouse_out) (GeckoHtml *view, gpointer dom_event);

	gint (*html_dom_mouse_over) (GeckoHtml *view, gpointer dom_event);

	gint (*html_dom_mouse_dbl_click) (GeckoHtml *view, gpointer dom_event);

	gint (*html_open_uri) (GeckoHtml *view, const gchar *uri);

};

GType           gecko_html_get_type       (void);
GeckoHtml *     gecko_html_new            (DIALOG_DATA * dialog,
					   gboolean is_dialog,
					   gint pane);
void gecko_html_set_parent(GtkWidget *widget, GtkWidget *owner);

void            gecko_html_set_base_uri   (GeckoHtml    *html,
					  const gchar *uri);
void            gecko_html_open_stream    (GeckoHtml    *html,
					  const gchar *mime);
void            gecko_html_write          (GeckoHtml    *html,
					  const gchar *data,
					  gint         len);
void            gecko_html_printf         (GeckoHtml    *html,
					  char        *format,
					  ...) G_GNUC_PRINTF (2,3);
void            gecko_html_close          (GeckoHtml    *html);

void            gecko_html_render_data    (GeckoHtml    *html,
					   const char *data,
					   guint32 len);

void            gecko_html_frames         (GeckoHtml    *html,
					  gboolean     enable);

gboolean        gecko_html_find           (GeckoHtml    *html,
					  const gchar *str);

gboolean	gecko_html_find_again	 (GeckoHtml    *html,
					  gboolean     forward);

void		gecko_html_set_find_props (GeckoHtml    *html,
					  const char  *str,
					  gboolean     match_case,
					  gboolean     wrap);

void            gecko_html_jump_to_anchor (GeckoHtml    *html,
					  gchar       *anchor);

void            gecko_html_copy_selection (GeckoHtml    *html);
void            gecko_html_paste(GeckoHtml * html);

void            gecko_html_select_all     (GeckoHtml    *html);
void            gecko_html_select_none(GeckoHtml * html);

void            gecko_html_preview_end    (GeckoHtml    *html);
void            gecko_html_preview_navigate (GeckoHtml *html,
					    gint page_no);
gboolean        gecko_html_initialize     (void);
void            gecko_html_shutdown       (void);

void            gecko_html_print_document (GtkWindow * window,
					   gchar * mod_name,
					   DIALOG_DATA * dialog);

void gecko_html_emit_link_message(GeckoHtml *html);

void gecko_html_emit_title_changed(GeckoHtml *html);

gboolean gecko_html_emit_uri_open(GeckoHtml *html, const gchar *uri);

G_END_DECLS

#endif /* __GECKO_HTML_H__ */
