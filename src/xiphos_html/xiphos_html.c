/*
 * Xiphos Bible Study Tool
 * xiphos_html.c - toolkit-generalized html support
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

#include "xiphos_html.h"

#ifndef GTKHTML
XiphosHtml *xiphos_html_new(DIALOG_DATA * dialog, gboolean is_dialog, gint pane)
{
	XiphosHtml *html;
	html = XIPHOS_HTML(g_object_new(XIPHOS_TYPE_HTML, NULL));
	XiphosHtmlPriv *priv = XIPHOS_HTML_GET_PRIVATE(html);

#if !defined(USE_WEBKIT) && defined(USE_GTKMOZEMBED)
	/* gecko, which has weird "structure privacy" needs */
	gecko_html_set_parms(html, priv, dialog, is_dialog, pane);
#else
	/* webkit or gtkhtml */
	priv->pane = pane;
	priv->is_dialog = is_dialog;
	priv->dialog = dialog;
#endif /* USE_WEBKIT || !USE_GTKMOZEMBED */
	
	return html;
}
#endif /* !GTKHTML */
