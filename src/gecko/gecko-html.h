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
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Mikael Hallendal <micke@imendio.com>
 */

#ifndef __GECKO_HTML_H__
#define __GECKO_HTML_H__

#include <gtkmozembed.h>

#include "gecko/gecko-print.h"

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
	GtkMozEmbed   parent;

	GeckoHtmlPriv *priv;
};

struct _GeckoHtmlClass {
        GtkMozEmbedClass parent_class;

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

};

GType           gecko_html_get_type       (void);
GeckoHtml *        gecko_html_new            (gint pane);

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
void            gecko_html_print          (GeckoHtml    *html,
					  GeckoPrintInfo *info,
					  gboolean preview,
					  gint *npages);
void            gecko_html_preview_end    (GeckoHtml    *html);
void            gecko_html_preview_navigate (GeckoHtml *html,
					    gint page_no);
gboolean        gecko_html_initialize     (void);
void            gecko_html_shutdown       (void); 

void            gecko_html_print_document (GtkWindow * window, gchar * mod_name, gpointer print_html);
G_END_DECLS

#endif /* __GECKO_HTML_H__ */
