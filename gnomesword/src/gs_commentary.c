/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
     * GnomeSword Bible Study Tool
     * gs_commentary.c (commentary support)
     * -------------------
     * Fri Apr 12 11:48:29 2002
     * copyright (C) 2002 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

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
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>


#include "gs_gnomesword.h"
#include "gs_html.h"
#include "sw_sword.h"
#include "gs_commentary.h"
#include "sw_commentary.h"
#include "sw_shortcutbar.h"
//#include <gtkhtml/gtkhtml.h>

extern SETTINGS *settings;

GList *comm_list;

static
void on_notebookCOMM_switch_page(GtkNotebook * notebook,
				GtkNotebookPage * page,
				gint page_num, GList * comm_list)
{
    COMM_DATA * c, *c_old;
   // c_old = (COMM_DATA *) g_list_nth_data(comm_list, settings->gbsLastPage);
    c = (COMM_DATA *) g_list_nth_data(comm_list, page_num);
    //-- change tab label to current book name
  /*  gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(settings->workbook),
				    gtk_notebook_get_nth_page(GTK_NOTEBOOK
							      (settings->
							       workbook),
							      0),
				    g->bookName);
    gtk_notebook_set_menu_label_text(GTK_NOTEBOOK
				     (settings->workbook),
				     gtk_notebook_get_nth_page(GTK_NOTEBOOK
							       (settings->
								workbook),
							       0),
				     c->modName);*/

    sprintf(settings->CommWindowModule, "%s", c->modName);
    //backend_displayinCOMM(settings->CommWindowModule, settings->currentverse);
/*
    if (settings->finddialog) {
	gnome_dialog_close(g_old->find_dialog->dialog);
	searchGS_FIND_DLG(g, FALSE, settings->findText);
    }
    GTK_CHECK_MENU_ITEM(g->showtabs)->active = show_tabs_gbs;
    settings->commLastPage = page_num;
    */
    settings->commLastPage = page_num;
}


static void setPageCOMM(gchar * modname, GList * comm_list)
{
    gint page = 0;
    COMM_DATA * c = NULL;

    comm_list = g_list_first(comm_list);
    while (comm_list != NULL) {
	c = (COMM_DATA *) comm_list->data;
	if (!strcmp(c->modName, modname))
	    break;
	++page;
	comm_list = g_list_next(comm_list);
    }

    gtk_notebook_set_page(GTK_NOTEBOOK(settings->notebookCOMM), page);

    settings->commLastPage = page;
}


static gboolean
on_button_release_event(GtkWidget * widget,
			GdkEventButton * event, COMM_DATA * c)
{
    gchar *key;
    switch (event->button) {
	case 1:
	    //if (!in_url) {
		key = buttonpresslookupGS_HTML(c->html);
		if (key) {
		    displaydictlexSBSW(settings->DictWindowModule, key,
				       settings);
		    g_free(key);
		}
	  //  }
	    return TRUE;
	    break;
	case 2:
	    return TRUE;
	    break;
	case 3:
	    return TRUE;
	    break;
    }
    return FALSE;
}


static GtkWidget *createPaneCOMM(SETTINGS * s, GList * mods, COMM_DATA * c, gint count)
{
    GtkWidget *frameCOMM;
    GtkWidget *vbox57;
    GtkWidget *toolbarCOMM;
    GtkWidget *tmp_toolbar_icon;
    GtkWidget *vseparator19;
    GtkWidget *scrolledwindowCOMMhtml;
    GtkWidget *label;


    frameCOMM = gtk_frame_new(NULL);
    gtk_widget_ref(frameCOMM);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "frameCOMM",
			     frameCOMM,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(frameCOMM);
    gtk_container_add(GTK_CONTAINER(s->notebookCOMM), frameCOMM);

    vbox57 = gtk_vbox_new(FALSE, 0);
    gtk_widget_ref(vbox57);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "vbox57", vbox57,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(vbox57);
    gtk_container_add(GTK_CONTAINER(frameCOMM), vbox57);

    toolbarCOMM =
	gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
    gtk_widget_ref(toolbarCOMM);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "toolbarCOMM",
			     toolbarCOMM,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(toolbarCOMM);
    gtk_box_pack_start(GTK_BOX(vbox57), toolbarCOMM, FALSE, TRUE, 0);
    gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarCOMM),
				  GTK_RELIEF_NONE);

    tmp_toolbar_icon =
	gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_REFRESH);
    c->btnCOMMSync =
	gtk_toolbar_append_element(GTK_TOOLBAR(toolbarCOMM),
				   GTK_TOOLBAR_CHILD_BUTTON, NULL,
				   _("Sync"), _("Sync with Bible Text"),
				   NULL, tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref(c->btnCOMMSync);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "c->btnCOMMSync",
			     c->btnCOMMSync,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(c->btnCOMMSync);

    tmp_toolbar_icon =
	gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_BACK);
    c->btnCOMMBack =
	gtk_toolbar_append_element(GTK_TOOLBAR(toolbarCOMM),
				   GTK_TOOLBAR_CHILD_BUTTON, NULL,
				   _("Back"), _("Go to previous comment"),
				   NULL, tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref(c->btnCOMMBack);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "c->btnCOMMBack",
			     c->btnCOMMBack,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(c->btnCOMMBack);

    tmp_toolbar_icon =
	gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_FORWARD);
    c->btnCOMMForward =
	gtk_toolbar_append_element(GTK_TOOLBAR(toolbarCOMM),
				   GTK_TOOLBAR_CHILD_BUTTON, NULL,
				   _("Forward"), _("Go to next comment"),
				   NULL, tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref(c->btnCOMMForward);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "c->btnCOMMForward",
			     c->btnCOMMForward,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(c->btnCOMMForward);

    vseparator19 = gtk_vseparator_new();
    gtk_widget_ref(vseparator19);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "vseparator19",
			     vseparator19,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(vseparator19);
    gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarCOMM), vseparator19, NULL,
			      NULL);
    gtk_widget_set_usize(vseparator19, 5, 7);

    tmp_toolbar_icon =
	gnome_stock_pixmap_widget(s->app, GNOME_STOCK_PIXMAP_PRINT);
    c->btnCOMMPrint =
	gtk_toolbar_append_element(GTK_TOOLBAR(toolbarCOMM),
				   GTK_TOOLBAR_CHILD_BUTTON, NULL,
				   _("Print"), _("Print Comment"), NULL,
				   tmp_toolbar_icon, NULL, NULL);
    gtk_widget_ref(c->btnCOMMPrint);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "c->btnCOMMPrint",
			     c->btnCOMMPrint,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(c->btnCOMMPrint);

    scrolledwindowCOMMhtml = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_ref(scrolledwindowCOMMhtml);
    gtk_object_set_data_full(GTK_OBJECT(s->app),
			     "scrolledwindowCOMMhtml",
			     scrolledwindowCOMMhtml,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(scrolledwindowCOMMhtml);
    gtk_box_pack_start(GTK_BOX(vbox57), scrolledwindowCOMMhtml, TRUE, TRUE,
		       0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				   (scrolledwindowCOMMhtml),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);


    c->html = gtk_html_new();
    gtk_widget_ref(c->html);
    gtk_object_set_data_full(GTK_OBJECT(s->app),
			     "c->html", c->html,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(c->html);
    gtk_container_add(GTK_CONTAINER(scrolledwindowCOMMhtml), c->html);
    gtk_html_load_empty(GTK_HTML(c->html));

    label = gtk_label_new(c->modName);
    gtk_widget_ref(label);
    gtk_object_set_data_full(GTK_OBJECT(s->app), "label", label,
			     (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(label);
    gtk_notebook_set_tab_label(GTK_NOTEBOOK(s->notebookCOMM),
			       gtk_notebook_get_nth_page(GTK_NOTEBOOK
							 (s->
							  notebookCOMM),
							 count), label);
    gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(s->notebookCOMM),
				     gtk_notebook_get_nth_page
				     (GTK_NOTEBOOK(s->notebookCOMM),
				      count), (gchar *) c->modName);



    gtk_signal_connect(GTK_OBJECT(c->html), "link_clicked",
		       GTK_SIGNAL_FUNC(on_link_clicked), NULL);
    gtk_signal_connect(GTK_OBJECT(c->html), "on_url",
		       GTK_SIGNAL_FUNC(on_url), (gpointer) s->app);
    gtk_signal_connect(GTK_OBJECT(c->html), "button_release_event",
		       GTK_SIGNAL_FUNC(on_button_release_event), c);

/*
    gtk_signal_connect(GTK_OBJECT(c->btnCOMMSync), "clicked",
		       GTK_SIGNAL_FUNC(on_btnCOMMSync_clicked), NULL);
    gtk_signal_connect(GTK_OBJECT(c->btnCOMMBack), "clicked",
		       GTK_SIGNAL_FUNC(on_btnCOMMBack_clicked), NULL);
    gtk_signal_connect(GTK_OBJECT(c->btnCOMMForward), "clicked",
		       GTK_SIGNAL_FUNC(on_btnCOMMForward_clicked), NULL);
    gtk_signal_connect(GTK_OBJECT(c->btnCOMMPrint), "clicked",
		       GTK_SIGNAL_FUNC(on_btnCOMMPrint_clicked), NULL);
*/
    return frameCOMM;
}

void gui_displayCOMM(gchar *key)
{
    backend_displayinCOMM(settings->commLastPage, key);
    
    /*GList *tmp = NULL;
    gint index = 0;
    
    tmp = comm_list;
    tmp = g_list_first(tmp);
    while (tmp != NULL) {
	
    }*/
    
}

void gui_setupCOMM(SETTINGS * s)
{
    GList *tmp = NULL;
    GList *mods = NULL;
    gchar *modname;
    gchar *modbuf;
    gchar *keybuf;
    COMM_DATA *c;
    gint count = 0;

    comm_list = NULL;

    mods = backend_getModListOfTypeSWORD(COMM_MODS);
    tmp = mods;
    tmp = g_list_first(tmp);
    while (tmp != NULL) {
	modname = (gchar *) tmp->data;
	c = g_new(COMM_DATA, 1);
	c->modName = modname;
	c->modDescription = backend_getmodDescriptionSWORD(modname);
	c->searchstring = NULL;
	createPaneCOMM(s, mods, c, count);
	backend_newDisplayCOMM(c->html, c->modName, s);
	comm_list = g_list_append(comm_list, (COMM_DATA*) c);
	++count;
	tmp = g_list_next(tmp);
    }

    gtk_signal_connect(GTK_OBJECT(s->notebookCOMM), "switch_page",
		       GTK_SIGNAL_FUNC(on_notebookCOMM_switch_page),
		       comm_list);

    modbuf = g_strdup(s->CommWindowModule);
    keybuf = g_strdup(s->currentverse);

    setPageCOMM(modbuf, comm_list);

    g_free(modbuf);
    g_free(keybuf);
    g_list_free(tmp);
    g_list_free(mods);
    //s->dlLastPage = 0;
}

void gui_shutdownCOMM(void)
{
    comm_list = g_list_first(comm_list);
    while (comm_list != NULL) {
	COMM_DATA *c = (COMM_DATA *) comm_list->data;
	if (c->find_dialog)
	    g_free(c->find_dialog);	/* free any search dialogs created */
	g_free((COMM_DATA *) comm_list->data);
	comm_list = g_list_next(comm_list);
    }
    g_list_free(comm_list);
}

/******  end of file  ******/
