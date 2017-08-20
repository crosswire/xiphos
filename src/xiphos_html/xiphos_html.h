/*
 * Xiphos Bible Study Tool
 * xiphos_html.h - toolkit-generalized html support
 *
 * Copyright (C) 2010-2017 Xiphos Developer Team
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

#include <config.h>

/**
 * A general compatibility layer that will buffer the application from the
 * direct effects of WebKit and Gecko being alternatively selected for
 * the rendering system.
 */
#include "webkit/wk-html.h"

// Used in places like the ubiquitous _popmenu_requested_cb
typedef WkHtml XiphosHtml;
typedef WkHtmlPriv XiphosHtmlPriv;

// Other layers of compatibility - pulled from display_info.c and other places
#define XIPHOS_HTML WK_HTML
#define XIPHOS_HTML_NEW(a, b, c) xiphos_html_new(a, b, c)
#define XIPHOS_HTML_COPY_SELECTION(text_html) wk_html_copy_selection(WK_HTML(text_html))
#define XIPHOS_HTML_OPEN_STREAM(a, b) wk_html_open_stream(WK_HTML(a), b)
#define XIPHOS_HTML_WRITE(a, b, c) wk_html_write(WK_HTML(a), b, c)
#define XIPHOS_HTML_CLOSE(a) wk_html_close(WK_HTML(a))
#define XIPHOS_HTML_FIND(a, b) wk_html_find(a, b)
#define XIPHOS_HTML_FIND_AGAIN(a, b) wk_html_find_again(a, b)
#define XIPHOS_HTML_JUMP_TO_ANCHOR wk_html_jump_to_anchor
#define XIPHOS_HTML_PRINT_DOCUMENT(a) wk_html_print(a) // TODO: Implement?
#define XIPHOS_HTML_SHUTDOWN wk_html_shutdown
#define XIPHOS_HTML_INITIALIZE wk_html_initialize

#define XIPHOS_TYPE_HTML WK_TYPE_HTML
#define XIPHOS_HTML_GET_PRIVATE(object) (G_TYPE_INSTANCE_GET_PRIVATE((object), WK_TYPE_HTML, WkHtmlPriv))

XiphosHtml *xiphos_html_new(DIALOG_DATA *dialog, gboolean is_dialog,
			    gint pane);
