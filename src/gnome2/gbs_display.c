/*
 * GnomeSword Bible Study Tool
 * gnome/gbs_display.c - support for displaying General Book Format modules
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gtkhtml/gtkhtml.h>

 
#include "gui/gbs_display.h"
#include "gui/gbs.h"
#include "gui/utilities.h"

#include "main/gbs.h"
#include "main/sword.h"


 
/******************************************************************************
 * Name
 *  set_gobal_options
 *
 * Synopsis
 *   #include ".h"
 *
 *  void 	
 *
 * Description
 *   set module global options 
 *
 * Return value
 *   void
 */

static void set_global_options(GBS_GLOBALS *bgo)
{
	set_global_option(3, "Strong's Numbers", bgo->strongs);
	set_global_option(3, "Morphological Tags", bgo->morphs);
	set_global_option(3, "Footnotes", bgo->footnotes);
	set_global_option(3, "Greek Accents", bgo->greekaccents);
	set_global_option(3, "Lemmas", bgo->lemmas);
	set_global_option(3, "Cross-references", bgo->scripturerefs);
	set_global_option(3, "Hebrew Vowel Points", bgo->hebrewpoints);
	set_global_option(3, "Hebrew Cantillation", bgo->hebrewcant);
	set_global_option(3, "Headings", bgo->headings);
	set_global_option(3, "Words of Christ in Red", bgo->words_in_red);
/*	if (tgs->variants_primary)
		set_text_global_option("Textual Variants",
				       "Primary Reading");
	else if (tgs->variants_secondary)
		set_text_global_option("Textual Variants",
				       "Secondary Reading");
	else
		set_text_global_option("Textual Variants",
				       "All Readings");*/
}



/******************************************************************************
 * Name
 *   gbs_entry
 *
 * Synopsis
 *   #include "gui/gbs_display.h"
 *
 *   GString *gbs_entry(unsigned long offset, GBS_DATA * gbs, 
 *		  	 gchar * title, gchar * text, MOD_FONT * mf)
 *
 * Description
 *   
 *
 * Return value
 *   GString
 */

static GString *gbs_entry(unsigned long offset, GBS_DATA * gbs,
			   gchar * title, gchar * text, MOD_FONT * mf)
{
	GString *str = NULL;
	gboolean use_gtkhtml_font = FALSE;
	gchar *use_font = NULL;
	gchar *use_font_size = NULL;
	gchar *div_align = NULL;

	use_font = g_strdup(mf->old_font);
	
	if (use_font) {
		if (!strncmp(use_font, "none", 4))
			use_gtkhtml_font = TRUE;
		else
			use_gtkhtml_font = FALSE;

	} else {
		use_gtkhtml_font = TRUE;

	}
	use_font_size = g_strdup(mf->old_font_size);
	
	if (gbs->is_rtol)
		div_align = "<div align=\"right\">";
	else
		div_align = "<div align=\"left\">";	
	
	str = g_string_new("");
	
	if(use_gtkhtml_font)
		g_string_sprintf(str,
			 "&nbsp; %s<a href=\"%lu\" name=\"%lu\">%s</a>"
			"%s<br></div>",
			 div_align, offset, offset, 
			title, text);
	else
		g_string_sprintf(str,
			"&nbsp; %s<a href=\"%lu\" name=\"%lu\">%s</a>"
			"<font face=\"%s\" size=\"%s\">%s</font><br></div>",
			div_align, offset, offset, 
			title, use_font,use_font_size,text);
	g_free(use_font);
	g_free(use_font_size);
	return str;
}


/******************************************************************************
 * Name
 *   gbs_display
 *
 * Synopsis
 *   #include "gui/gbs_display.h"
 *
 *   void gbs_display(GBS_DATA * gbs, gchar * anchor, gint level,
 *		 gboolean is_leaf)if (gbs->is_rtol)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gbs_display(GBS_DATA * gbs, gchar * anchor, gint level,
		 gboolean is_leaf)
{
	GString *str, *str_tmp;
	gchar *text = NULL;
	gchar *tmpbuf = NULL;
	unsigned long offset;
	MOD_FONT *mf;
	GtkHTMLStreamStatus status1 = 0;
	GtkHTMLStream *htmlstream;

	mf = get_font(gbs->mod_name);
	
	if (gbs->display_level && (level > gbs->display_level)
	    && is_leaf) {
		gbs_treekey_get_parent(gbs->offset);
		gbs->offset = gbs_get_treekey_offset();
	}

	if (gbs->is_rtol)
		str = g_string_new(HTML_START "<body dir=\"rtl\">");
	else
		str = g_string_new(HTML_START "<body>");

	set_global_options(gbs->bgo);
	text = get_text_from_offset(gbs->mod_name, gbs->offset);
	tmpbuf = gbs_get_treekey_local_name(gbs->offset);
	str_tmp = gbs_entry(gbs->offset, gbs, tmpbuf, text, mf);
	str = g_string_append(str, str_tmp->str);
	g_string_free(str_tmp, TRUE);

	if (text)
		g_free(text);
	if (tmpbuf)
		g_free(tmpbuf);

	if (gbs->display_level && level >= gbs->display_level) {
		if (gbs_treekey_first_child(gbs->offset)) {
			offset = gbs_get_treekey_offset();
			tmpbuf = gbs_get_treekey_local_name(offset);
			text = get_text_from_offset(gbs->mod_name,
						    offset);
			str_tmp =
			    gbs_entry(offset, gbs, tmpbuf, text, mf);
			str = g_string_append(str, str_tmp->str);
			g_string_free(str_tmp, TRUE);
			if (text)
				g_free(text);
			if (tmpbuf)
				g_free(tmpbuf);
			while (treekey_next_sibling(offset)) {
				offset = gbs_get_treekey_offset();
				tmpbuf =
				    gbs_get_treekey_local_name(offset);
				text =
				    get_text_from_offset(gbs->mod_name,
							 offset);
				str_tmp =
				    gbs_entry(offset, gbs,
					       tmpbuf, text, mf);
				str =
				    g_string_append(str, str_tmp->str);
				g_string_free(str_tmp, TRUE);
				if (text)
					g_free(text);
				if (tmpbuf)
					g_free(tmpbuf);
			}
		}
	}

	str = g_string_append(str, "</body></html>");
	htmlstream = gtk_html_begin_content(GTK_HTML(gbs->html),
					    "text/html; charset=utf-8");
	if (str->len) {
		gtk_html_write(GTK_HTML(gbs->html), htmlstream,
			       str->str, str->len);
	}
	gtk_html_end(GTK_HTML(gbs->html), htmlstream, status1);
	gtk_html_jump_to_anchor(GTK_HTML(gbs->html), anchor);
	
	/* andyp - inserted for debugging, remove */
	g_print(str->str); 

	//gtk_html_set_editable(html, was_editable); 
	g_string_free(str, TRUE);
	free_font(mf);
}
