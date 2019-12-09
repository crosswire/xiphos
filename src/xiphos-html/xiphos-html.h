/* vi: set sw=4 ts=4 wrap ai: */
/*
 * xiphos-html.h: This file is part of xiphos
 *
 * Copyright (C) 2019 Wu Xiaotian <yetist@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * */

#ifndef __XIPHOS_HTML_H__
#define __XIPHOS_HTML_H__  1

#include <webkit2/webkit2.h>
#include "main/module_dialogs.h"

G_BEGIN_DECLS

#define XIPHOS_TYPE_HTML (xiphos_html_get_type ())
G_DECLARE_DERIVABLE_TYPE (XiphosHtml, xiphos_html, XIPHOS, HTML, WebKitWebView)

struct _XiphosHtmlClass
{
    WebKitWebViewClass     parent_class;

    /* Signals */
    void     (*uri_selected)        (XiphosHtml *view, gchar *uri, gboolean handled);
    gboolean (*frame_selected)      (XiphosHtml *view, gchar *uri, gboolean handled);
    // void     (*title_changed)       (XiphosHtml *view, const gchar *new_title);
    void     (*popupmenu_requested) (XiphosHtml *view, const gchar *link);
};

GType           xiphos_html_get_type       (void) G_GNUC_CONST;
GtkWidget*      xiphos_html_new            (DIALOG_DATA *dialog, gboolean is_dialog, gint pane);
void            xiphos_html_set_base_uri   (XiphosHtml *html, const gchar *uri);
void            xiphos_html_open_stream    (XiphosHtml *html, const gchar *mime);
void            xiphos_html_write          (XiphosHtml *html, const gchar *data);
void            xiphos_html_printf         (XiphosHtml *html, gchar *format, ...) G_GNUC_PRINTF(2, 3);
void            xiphos_html_close          (XiphosHtml *html);
void            xiphos_html_render_data    (XiphosHtml *html, const char *data);
void            xiphos_html_frames         (XiphosHtml *html, gboolean enable);
gboolean        xiphos_html_find           (XiphosHtml *html, const gchar *find_string);
gboolean        xiphos_html_find_again     (XiphosHtml *html, gboolean forward);
void            xiphos_html_jump_to_anchor (XiphosHtml *html, gchar *anchor);
void            xiphos_html_copy_selection (XiphosHtml *html);
void            xiphos_html_select_all     (XiphosHtml *html);
void            xiphos_html_print          (XiphosHtml *html);

G_END_DECLS

#endif /* __XIPHOS_HTML_H__ */
