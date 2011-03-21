/*
 * Xiphos Bible Study Tool
 * commentary_dialog.c - dialog for a commentary module
 *
 * Copyright (C) 2000-2010 Xiphos Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
 * A general compatibility layer that will buffer the application from the
 * direct effects of WebKit and Gecko being alternatively selected for
 * the rendering system.
 */
#ifdef USE_WEBKIT
  #include "webkit/wk-html.h"
  #define USE_XIPHOS_HTML
#else
  #ifdef USE_GTKMOZEMBED
    #ifdef WIN32
      #include "geckowin/gecko-html.h"
    #else
      #include "gecko/gecko-html.h"
    #endif
    #define USE_XIPHOS_HTML
  #else
    #ifdef XIPHOS_HTML_EXTERN_C
      #ifdef __cplusplus
        extern "C" {
      #endif
    #endif
    #include <gtkhtml/gtkhtml.h>
    #include "gui/html.h"
    #ifdef XIPHOS_HTML_EXTERN_C
      #ifdef __cplusplus
        }
      #endif
    #endif
  #endif
#endif

// Used in places like the ubiquitous _popmenu_requested_cb
#ifdef USE_WEBKIT
	typedef WkHtml XiphosHtml;
#else
  #ifdef USE_GTKMOZEMBED
	typedef GeckoHtml XiphosHtml;
  #endif
#endif

// Other layers of compatibility - pulled from display_info.c and other places
#ifdef USE_WEBKIT
	#define XIPHOS_HTML WK_HTML
	#define XIPHOS_HTML_NEW(a, b, c) wk_html_new()
	#define XIPHOS_HTML_COPY_SELECTION(text_html) wk_html_copy_selection(WK_HTML(text_html))
	#define XIPHOS_HTML_OPEN_STREAM(a, b) wk_html_open_stream(WK_HTML(a), b)
	#define XIPHOS_HTML_WRITE(a, b, c) wk_html_write(WK_HTML(a), b, c)
	#define XIPHOS_HTML_CLOSE(a) wk_html_close(WK_HTML(a))
	#define XIPHOS_HTML_FIND(a, b) wk_html_find(a, b)
	#define XIPHOS_HTML_FIND_AGAIN(a, b) wk_html_find_again(a, b)
	#define XIPHOS_HTML_JUMP_TO_ANCHOR wk_html_jump_to_anchor
	#define XIPHOS_HTML_PRINT_DOCUMENT(a, b, c) // TODO: Implement?
	#define XIPHOS_HTML_SHUTDOWN wk_html_shutdown
	#define XIPHOS_HTML_INITIALIZE wk_html_initialize
#else
  #ifdef USE_GTKMOZEMBED
	#define XIPHOS_HTML GECKO_HTML
	#define XIPHOS_HTML_NEW(a, b, c) gecko_html_new(a, b, c)
	#define XIPHOS_HTML_COPY_SELECTION(text_html) 	gecko_html_copy_selection(GECKO_HTML(text_html))
	#define XIPHOS_HTML_OPEN_STREAM(a, b) gecko_html_open_stream(GECKO_HTML(a), b)
	#define XIPHOS_HTML_WRITE(a, b, c) gecko_html_write(GECKO_HTML(a), b, c)
	#define XIPHOS_HTML_CLOSE(a) gecko_html_close(GECKO_HTML(a))
	#define XIPHOS_HTML_FIND(a, b) gecko_html_find(a, b)
	#define XIPHOS_HTML_FIND_AGAIN(a, b) gecko_html_find_again(a, b)
	#define XIPHOS_HTML_JUMP_TO_ANCHOR gecko_html_jump_to_anchor
	#define XIPHOS_HTML_PRINT_DOCUMENT(a, b, c) gecko_html_print_document(a, b, c)
	#define XIPHOS_HTML_SHUTDOWN gecko_html_shutdown
	#define XIPHOS_HTML_INITIALIZE gecko_html_initialize
  #endif
#endif
