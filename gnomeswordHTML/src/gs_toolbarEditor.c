/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          gs_toolbarEditor.c  -  description
                             -------------------
    begin                : Fri Apr 13 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */
#include <gnome.h>
#include <gtkhtml/gtkhtml.h> 
#include <gtkhtml/htmlcolor.h> 
#include <gtkhtml/htmlcolorset.h> 
#include <gtkhtml/htmlengine-edit-fontstyle.h> 
#include <gtkhtml/htmlsettings.h> 
#include <gal/widgets/widget-color-combo.h>

#include "gs_html.h"
#include "support.h"

static   GtkWidget *btnBold;
static   GtkWidget *btnItalic;
static   GtkWidget *btnUnderline;
static   GtkWidget *btnStrikeout;
static  GtkWidget *btnAlignLeft;
static  GtkWidget *btnAlignCenter;
static  GtkWidget *btnAlignRight;
static  GtkWidget *font_size_menu;
static  GtkWidget *combo;

gboolean block_font_style_change;


static void
on_btnBold_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	boldHTML( lookup_widget(GTK_WIDGET(togglebutton) ,"btnBold"), 
			lookup_widget( GTK_WIDGET(togglebutton),(gchar *) user_data));
}

static void
on_btnItalic_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	italicHTML(GTK_WIDGET(togglebutton) , 
			lookup_widget( GTK_WIDGET(togglebutton),(gchar *) user_data));
}

static void
on_btnUnderline_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	underlineHTML(GTK_WIDGET(togglebutton) , 
			lookup_widget( GTK_WIDGET(togglebutton),(gchar *) user_data));
}


static void
on_btnStrikeout_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


static void
on_btnAlignLeft_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	/* If the button is not active at this point, it means that the user clicked on
           some other button in the radio group.  */
	if (! gtk_toggle_button_get_active (togglebutton))
		return;

	gtk_html_set_paragraph_alignment (GTK_HTML (lookup_widget( GTK_WIDGET(togglebutton),(gchar *) user_data)),
					  GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT);
}


static void
on_btnAlignCenter_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	/* If the button is not active at this point, it means that the user clicked on
           some other button in the radio group.  */
	if (! gtk_toggle_button_get_active (togglebutton))
		return;

	gtk_html_set_paragraph_alignment (GTK_HTML (lookup_widget( GTK_WIDGET(togglebutton),(gchar *) user_data)),
					  GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER);
}


static void
on_btnAlignRight_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	/* If the button is not active at this point, it means that the user clicked on
           some other button in the radio group.  */
	if (! gtk_toggle_button_get_active (togglebutton))
		return;

	gtk_html_set_paragraph_alignment (GTK_HTML (lookup_widget( GTK_WIDGET(togglebutton),(gchar *) user_data)),
					  GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT);

}


static void
on_btnIndent_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_html_modify_indent_by_delta (GTK_HTML (lookup_widget( GTK_WIDGET(button),(gchar *) user_data)), +1);
}


static void
on_btnUnIndent_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_html_modify_indent_by_delta (GTK_HTML (lookup_widget( GTK_WIDGET(button),(gchar *) user_data)), -1);
}

static void
insertion_font_style_changed_cb (GtkHTML *widget, GtkHTMLFontStyle font_style, gpointer user_data)
{
	block_font_style_change = TRUE;
	if (font_style & GTK_HTML_FONT_STYLE_BOLD)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btnBold), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btnBold), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_ITALIC)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btnItalic), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btnItalic), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_UNDERLINE)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btnUnderline), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btnUnderline), FALSE);

	if (font_style & GTK_HTML_FONT_STYLE_STRIKEOUT)
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btnStrikeout), TRUE);
	else
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btnStrikeout), FALSE);
	block_font_style_change = FALSE;
}

static void
safe_set_active (GtkWidget *widget,
		 gpointer data)
{
	GtkObject *object;
	GtkToggleButton *toggle_button;

	object = GTK_OBJECT (widget);
	toggle_button = GTK_TOGGLE_BUTTON (widget);

	gtk_signal_handler_block_by_data (object, data);
	gtk_toggle_button_set_active (toggle_button, TRUE);
	gtk_signal_handler_unblock_by_data (object, data);
}

static void
paragraph_alignment_changed_cb (GtkHTML *widget,
				GtkHTMLParagraphAlignment alignment,
				gpointer user_data)
{
	switch (alignment) {
	case GTK_HTML_PARAGRAPH_ALIGNMENT_LEFT:
		safe_set_active (btnAlignLeft, user_data);
		break;
	case GTK_HTML_PARAGRAPH_ALIGNMENT_CENTER:
		safe_set_active (btnAlignCenter, user_data);
		break;
	case GTK_HTML_PARAGRAPH_ALIGNMENT_RIGHT:
		safe_set_active (btnAlignRight, user_data);
		break;
	default:
		g_warning ("Unknown GtkHTMLParagraphAlignment %d.", alignment);
	}
}

/******************************************************************************
 *
 ******************************************************************************/
static void
color_changed (GtkWidget *w, GdkColor *gdk_color, GtkHTML *html)
{
	HTMLColor *color = gdk_color
		&& gdk_color != &html_colorset_get_color (html->engine->settings->color_set, HTMLTextColor)->color
		? html_color_new_from_gdk_color (gdk_color) : NULL;

	gtk_html_set_color (html, color);
	if (color)
		html_color_unref (color);
}

static void
unset_focus (GtkWidget *w, gpointer data)
{
	GTK_WIDGET_UNSET_FLAGS (w, GTK_CAN_FOCUS);
}

inline static void
set_color_combo (GtkHTML *html, gpointer user_data)
{
	color_combo_set_color (COLOR_COMBO (combo),
			       &html_colorset_get_color_allocated (html->engine->painter, HTMLTextColor)->color);
}

static void
realize_engine (GtkHTML *html, gpointer user_data)
{
	set_color_combo (html, user_data);
	gtk_signal_disconnect_by_func (GTK_OBJECT (html), realize_engine,user_data );
}

static void
load_done (GtkHTML *html, gpointer user_data)
{
	set_color_combo (html, NULL);
}

static GtkWidget *
setup_color_combo (GtkHTML *html)
{
	HTMLColor *color;

	color = html_colorset_get_color (html->engine->settings->color_set, HTMLTextColor);
	if (GTK_WIDGET_REALIZED (html))
		html_color_alloc (color, html->engine->painter);
	else
		gtk_signal_connect (GTK_OBJECT (html), "realize", realize_engine, NULL);
        gtk_signal_connect (GTK_OBJECT (html), "load_done", GTK_SIGNAL_FUNC (load_done), NULL);

	combo = color_combo_new (NULL, _("Automatic"), &color->color, "toolbar_text");
	GTK_WIDGET_UNSET_FLAGS (combo, GTK_CAN_FOCUS);
	gtk_container_forall (GTK_CONTAINER (combo), unset_focus, NULL);
        gtk_signal_connect (GTK_OBJECT (combo), "changed", GTK_SIGNAL_FUNC (color_changed), 
		html);

	gtk_widget_show_all (combo);
	return combo;
}

/******************************************************************************
 * Paragraph style option menu.  
 ******************************************************************************/
static struct {
	GtkHTMLParagraphStyle style;
	const gchar *description;
} paragraph_style_items[] = {
	{ GTK_HTML_PARAGRAPH_STYLE_NORMAL, N_("Normal") },
	{ GTK_HTML_PARAGRAPH_STYLE_H1, N_("Header 1") },
	{ GTK_HTML_PARAGRAPH_STYLE_H2, N_("Header 2") },
	{ GTK_HTML_PARAGRAPH_STYLE_H3, N_("Header 3") },
	{ GTK_HTML_PARAGRAPH_STYLE_H4, N_("Header 4") },
	{ GTK_HTML_PARAGRAPH_STYLE_H5, N_("Header 5") },
	{ GTK_HTML_PARAGRAPH_STYLE_H6, N_("Header 6") },
	{ GTK_HTML_PARAGRAPH_STYLE_ADDRESS, N_("Address") },
	{ GTK_HTML_PARAGRAPH_STYLE_PRE, N_("Pre") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMDIGIT, N_("List item (digit)") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMDOTTED, N_("List item (unnumbered)") },
	{ GTK_HTML_PARAGRAPH_STYLE_ITEMROMAN, N_("List item (roman)") },
	{ GTK_HTML_PARAGRAPH_STYLE_NORMAL, NULL },
};

static void
paragraph_style_changed_cb (GtkHTML *html,
			    GtkHTMLParagraphStyle style,
			    gpointer data)
{
	GtkOptionMenu *option_menu;
	guint i;

	option_menu = GTK_OPTION_MENU (data);

	for (i = 0; paragraph_style_items[i].description != NULL; i++) {
		if (paragraph_style_items[i].style == style) {
			gtk_option_menu_set_history (option_menu, i);
			return;
		}
	}

	g_warning ("Editor component toolbar: unknown paragraph style %d", style);
}

static void
paragraph_style_menu_item_activated_cb (GtkWidget *widget,
					gpointer data)
{
	GtkHTMLParagraphStyle style;
	GtkHTML *html;

	html = GTK_HTML (data);
	style = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (widget), "paragraph_style_value"));

	/* g_warning ("Setting paragraph style to %d.", style); */

	gtk_html_set_paragraph_style (html, style);
}

static GtkWidget *
setup_paragraph_style_option_menu (GtkHTML *html)
{
	GtkWidget *option_menu;
	GtkWidget *menu;
	guint i;

	option_menu = gtk_option_menu_new ();
	menu = gtk_menu_new ();

	for (i = 0; paragraph_style_items[i].description != NULL; i++) {
		GtkWidget *menu_item;

		menu_item = gtk_menu_item_new_with_label (_(paragraph_style_items[i].description));
		gtk_widget_show (menu_item);

		gtk_menu_append (GTK_MENU (menu), menu_item);

		gtk_object_set_data (GTK_OBJECT (menu_item), "paragraph_style_value",
				     GINT_TO_POINTER (paragraph_style_items[i].style));
		gtk_signal_connect (GTK_OBJECT (menu_item), "activate",
				    GTK_SIGNAL_FUNC (paragraph_style_menu_item_activated_cb),
				    html);
	}

	gtk_option_menu_set_menu (GTK_OPTION_MENU (option_menu), menu);

	gtk_signal_connect (GTK_OBJECT (html), "current_paragraph_style_changed",
			    GTK_SIGNAL_FUNC (paragraph_style_changed_cb), option_menu);

	gtk_widget_show (option_menu);

	return option_menu;
}

static void
set_font_size (GtkWidget *w, GtkHTML *html)
{
	GtkHTMLFontStyle style = GTK_HTML_FONT_STYLE_SIZE_1 + GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (w),
												    "size"));

	if (!block_font_style_change)
		gtk_html_set_font_style (html, GTK_HTML_FONT_STYLE_MAX & ~GTK_HTML_FONT_STYLE_SIZE_MASK, style);
}

static void
font_size_changed (GtkWidget *w, GtkHTMLParagraphStyle style, GtkHTML *html)
{
	if (style == GTK_HTML_FONT_STYLE_DEFAULT)
		style = GTK_HTML_FONT_STYLE_SIZE_3;
	block_font_style_change = TRUE;
	gtk_option_menu_set_history (GTK_OPTION_MENU (font_size_menu),
				     (style & GTK_HTML_FONT_STYLE_SIZE_MASK) - GTK_HTML_FONT_STYLE_SIZE_1);
	block_font_style_change = FALSE;
}

static GtkWidget *
setup_font_size_option_menu (GtkHTML*html)
{
	GtkWidget *option_menu;
	GtkWidget *menu;
	guint i;
	gchar size [3];

	font_size_menu = option_menu = gtk_option_menu_new ();

	menu = gtk_menu_new ();
	size [2] = 0;

	for (i = 0; i < GTK_HTML_FONT_STYLE_SIZE_MAX; i++) {
		GtkWidget *menu_item;

		size [0] = (i>1) ? '+' : '-';
		size [1] = '0' + ((i>1) ? i - 2 : 2 - i);

		menu_item = gtk_menu_item_new_with_label (size);
		gtk_widget_show (menu_item);

		gtk_menu_append (GTK_MENU (menu), menu_item);

		gtk_object_set_data (GTK_OBJECT (menu_item), "size",
				     GINT_TO_POINTER (i));
		gtk_signal_connect (GTK_OBJECT (menu_item), "activate",
				    GTK_SIGNAL_FUNC (set_font_size), 
					html);
	}

	gtk_option_menu_set_menu (GTK_OPTION_MENU (option_menu), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (option_menu), 2);

	gtk_signal_connect (GTK_OBJECT (html), "insertion_font_style_changed",
			    GTK_SIGNAL_FUNC (font_size_changed), 
					html);

	gtk_widget_show (option_menu);

	return option_menu;
}


GtkWidget *HTMLtoolbar_create(GtkWidget *mainwindow, gchar *html)
{
  
  GtkWidget *tbHTMLEdit;
/*  GtkWidget *paragraph_style_menu;
  GtkWidget *paragraph_style_menu_menu;
  GtkWidget *glade_menuitem;
  GtkWidget *font_size_menu;
  GtkWidget *font_size_menu_menu; */
  GtkWidget *vseparator14;
  GtkWidget *vseparator15;
  GtkWidget *btnIndent;
  GtkWidget *btnUnIndent;
  GtkWidget *tmp_toolbar_icon;
	
 tbHTMLEdit = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (tbHTMLEdit);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "tbHTMLEdit", tbHTMLEdit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (tbHTMLEdit);
  gtk_box_pack_start (GTK_BOX (lookup_widget(mainwindow,"vbox32")), tbHTMLEdit, FALSE, FALSE, 0);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (tbHTMLEdit), GTK_RELIEF_NONE);

	gtk_toolbar_prepend_widget (GTK_TOOLBAR (tbHTMLEdit),
				    setup_paragraph_style_option_menu (GTK_HTML(lookup_widget(mainwindow,html))),
				    NULL, NULL);
	gtk_toolbar_prepend_widget (GTK_TOOLBAR (tbHTMLEdit),
				    setup_font_size_option_menu (GTK_HTML(lookup_widget(mainwindow,html))),
				    NULL, NULL);
  tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_TEXT_BOLD);
  btnBold = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton1"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnBold);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnBold", btnBold,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBold);
  tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_TEXT_ITALIC);
  btnItalic = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton2"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnItalic);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnItalic", btnItalic,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnItalic);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_TEXT_UNDERLINE);
  btnUnderline = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton3"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnUnderline);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnUnderline", btnUnderline,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnUnderline);
  tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_TEXT_STRIKEOUT);
  btnStrikeout = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton4"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnStrikeout);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnStrikeout", btnStrikeout,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnStrikeout);

  vseparator14 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator14);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "vseparator14", vseparator14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator14);
  gtk_toolbar_append_widget (GTK_TOOLBAR (tbHTMLEdit), vseparator14, NULL, NULL);
  gtk_widget_set_usize (vseparator14, 9, 14);
  tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_ALIGN_LEFT);
  btnAlignLeft = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton5"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnAlignLeft);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnAlignLeft", btnAlignLeft,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnAlignLeft);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_ALIGN_CENTER);
  btnAlignCenter = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton6"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnAlignCenter);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnAlignCenter", btnAlignCenter,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnAlignCenter);

 tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_ALIGN_RIGHT);
  btnAlignRight = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton7"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnAlignRight);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnAlignRight", btnAlignRight,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnAlignRight);

  vseparator15 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator15);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "vseparator15", vseparator15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator15);
  gtk_toolbar_append_widget (GTK_TOOLBAR (tbHTMLEdit), vseparator15, NULL, NULL);
  gtk_widget_set_usize (vseparator15, 9, 14);
  
  tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_TEXT_INDENT);
  btnIndent = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button14"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnIndent);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnIndent", btnIndent,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnIndent);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (mainwindow, GNOME_STOCK_PIXMAP_TEXT_UNINDENT);
  btnUnIndent = gtk_toolbar_append_element (GTK_TOOLBAR (tbHTMLEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button15"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnUnIndent);
  gtk_object_set_data_full (GTK_OBJECT (mainwindow), "btnUnIndent", btnUnIndent,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnUnIndent);
  /* add color combo */
  gtk_toolbar_append_widget (GTK_TOOLBAR (tbHTMLEdit),
				   setup_color_combo (GTK_HTML(lookup_widget(mainwindow,html))),
				   NULL, NULL);
 
  gtk_signal_connect (GTK_OBJECT (btnBold), "toggled",
                      GTK_SIGNAL_FUNC (on_btnBold_toggled),
                      html);
  gtk_signal_connect (GTK_OBJECT (btnItalic), "toggled",
                      GTK_SIGNAL_FUNC (on_btnItalic_toggled),
                      html);		      
  gtk_signal_connect (GTK_OBJECT (btnUnderline), "toggled",
                      GTK_SIGNAL_FUNC (on_btnUnderline_toggled),
                      html);
  gtk_signal_connect (GTK_OBJECT (btnStrikeout), "toggled",
                      GTK_SIGNAL_FUNC (on_btnStrikeout_toggled),
                      html);
  gtk_signal_connect (GTK_OBJECT (btnAlignLeft), "toggled",
                      GTK_SIGNAL_FUNC (on_btnAlignLeft_toggled),
                      html);
  gtk_signal_connect (GTK_OBJECT (btnAlignCenter), "toggled",
                      GTK_SIGNAL_FUNC (on_btnAlignCenter_toggled),
                      html);
  gtk_signal_connect (GTK_OBJECT (btnAlignRight), "toggled",
                      GTK_SIGNAL_FUNC (on_btnAlignRight_toggled),
                      html);
  gtk_signal_connect (GTK_OBJECT (btnIndent), "clicked",
                      GTK_SIGNAL_FUNC (on_btnIndent_clicked),
                      html);
  gtk_signal_connect (GTK_OBJECT (btnUnIndent), "clicked",
                      GTK_SIGNAL_FUNC (on_btnUnIndent_clicked),
                      html);  
  gtk_signal_connect (GTK_OBJECT (lookup_widget(mainwindow,html)), "insertion_font_style_changed",
			GTK_SIGNAL_FUNC (insertion_font_style_changed_cb), 
			NULL);
  gtk_signal_connect (GTK_OBJECT (lookup_widget(mainwindow,html)), "current_paragraph_alignment_changed",
			GTK_SIGNAL_FUNC (paragraph_alignment_changed_cb), 
			NULL);

  return tbHTMLEdit;
}

