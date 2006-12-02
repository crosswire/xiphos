/***************************************************************************
 * GnomeSword Bible Study Tool
 * embed.cc - gtkmozembed stuff 
 *
 * Copyright (C) 2004 GnomeSword Developer Team
 *
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
#include <config.h>
#endif

#include <gnome.h>

#ifdef USE_GTKMOZEMBED
#define MOZILLA_INTERNAL_API
#include <gtkmozembed.h>
#include <gtkmozembed_internal.h>
#include <nsIDOMMouseEvent.h>
#include <nsIDOMKeyEvent.h>
#include <nsCOMPtr.h>
#include <nsIDOMDocument.h>
#include <nsIDOMHTMLAnchorElement.h>
#include <nsIMarkupDocumentViewer.h>
#include <nsIDocShell.h>
#include <nsIDocShellTreeItem.h>
#include <nsIDocShellTreeOwner.h>
#include <nsIDocument.h>
#include <nsIPresShell.h>
#include <nsIDocumentViewer.h>
#include <nsIContent.h>
#include <nsIWebBrowser.h>
#include <nsIDocument.h>
#include <nsIDOMMouseEvent.h>
#include <nsIDOMKeyEvent.h>
#include <nsIDOMEventTarget.h>
#include <nsIDOMNSHTMLElement.h>  
#include <nsIDOMHTMLElement.h>  
#include <nsIDOMHTMLTextAreaElement.h>  
#include <nsIDOMNamedNodeMap.h>
#include <nsIWebBrowserPersist.h>
#include <nsNetUtil.h>
#include <nsIWebBrowserFind.h>
#include <nsIDOMNSDocument.h>
#include <nsIDOMNSEvent.h>
#include <nsIDOMNodeList.h>
#include <nsIDOMWindow.h>
#include <nsISelection.h>
#include <nsIDOMRange.h>
#include <nsIWebBrowserFind.h>
#include <nsNetUtil.h>
#include <nsICharsetConverterManager.h>
#include <nsIDOMWindow.h>
#include <nsISelection.h>
#include <nsISHistory.h>
#include <nsIHistoryEntry.h>
#include <nsIWebNavigation.h>
#include <widget/nsIBaseWindow.h>
#include <nsIWebPageDescriptor.h>
//#include <nsIPresContext.h>
#include <nsIEventStateManager.h>
#include <nsIClipboardCommands.h>
#include <nsIParserNode.h>
#include "nsIWebBrowserPrint.h"

#endif


#include "main/embed-dialogs.h"
#include "main/module_dialogs.h"
#include "main/previewer.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/url.hh"

#ifdef __cplusplus
extern "C" {
#include "gui/bibletext.h"
#include "gui/commentary.h"
#include "gui/dictlex.h"
#include "gui/gbs.h"
#include "gui/parallel_view.h"
}
#endif

#include "gui/bibletext_dialog.h"
#include "gui/commentary_dialog.h"
#include "gui/html.h"
#include "gui/widgets.h"

extern gboolean shift_key_presed;


#ifdef USE_GTKMOZEMBED

static
nsresult embed_get_print_settings (nsIWebBrowser *mWebBrowser, nsIPrintSettings **options)
{
	g_return_val_if_fail (mWebBrowser, NS_ERROR_FAILURE);

	nsCOMPtr<nsIWebBrowserPrint> print(do_GetInterface(mWebBrowser));
	NS_ENSURE_TRUE (print, NS_ERROR_FAILURE);

	return print->GetGlobalPrintSettings(options);
}
static
nsresult embed_dialogs_print_real(nsIPrintSettings *options, PRBool preview, 
			nsIWebBrowser *wb, GtkWindow *parent)
{
	nsresult rv;

	g_return_val_if_fail (wb, NS_ERROR_FAILURE);

	g_message("not a NS_ERROR_FAILURE 1");
	
	nsCOMPtr<nsIWebBrowserPrint> print(do_GetInterface(wb, &rv));
	if (NS_FAILED(rv) || !print) return NS_ERROR_FAILURE;

	g_message("not a NS_ERROR_FAILURE 2");
	if (!preview)
	{
		//GPrintListener *listener = new GPrintListener(print, options, parent);
		//rv = print->Print (options, listener);
	}
	else
	{
		rv = print->PrintPreview(options, nsnull, nsnull);//options, nsnull, nsnull);
	}

	return rv;
}

void embed_dialogs_print(gboolean preview, GtkMozEmbed *embed)
{
	nsIPrintSettings *options;
	nsIWebBrowser *wb;
	
	gtk_moz_embed_get_nsIWebBrowser (embed, &wb);
	embed_get_print_settings (wb, &options);

	
	embed_dialogs_print_real(options, preview, wb, NULL);
}


static 
gint embed_dialogs_dom_key_down(GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
	PRBool shift = FALSE;
	nsIDOMKeyEvent *event = (nsIDOMKeyEvent*) dom_event;
	
	//g_message("main_dom_key_down");
	shift_key_presed = TRUE;
}

static 
gint embed_dialogs_dom_key_up(GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
	//g_message("main_dom_key_up");
	shift_key_presed = FALSE;
}

static 
gint embed_dialogs_dom_key_press(GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
	//g_message("main_dom_key_press");
}


// Nautilus CREDITS here
static 
gchar *embed_dialogs_get_attribute (nsIDOMNode *node, gchar *attribute)
{
     	nsresult result;

	nsCOMPtr<nsIDOMNamedNodeMap> attributes;
	result = node->GetAttributes(getter_AddRefs(attributes));
	if (!NS_SUCCEEDED (result) || !attributes) return NULL;

	nsAutoString attr; 

	attr.AssignWithConversion(attribute);

	nsCOMPtr<nsIDOMNode> attrNode;
	result = attributes->GetNamedItem (attr, getter_AddRefs(attrNode));
	if (!NS_SUCCEEDED(result) || !attrNode)  return NULL;

	nsAutoString nodeValue;

	result = attrNode->GetNodeValue(nodeValue);
	if (!NS_SUCCEEDED(result))  return NULL;

	return ToNewCString(nodeValue);
}

static 
gint embed_dialogs_dom_mouse_over(GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
	nsresult result;
	nsIDOMEventTarget *aCurrentTarget;
	nsIDOMMouseEvent *event = (nsIDOMMouseEvent*) dom_event;
	DIALOG_DATA *d = (DIALOG_DATA *) data;
	
	if(shift_key_presed)
		return FALSE;
	if(d->mod_type == BOOK_TYPE)
		return FALSE;
	if(d->mod_type == COMMENTARY_TYPE)
		return FALSE;
	if(d->mod_type == DICTIONARY_TYPE)
		return FALSE;
	
	main_dialogs_clear_viewer(d);
	nsCOMPtr<nsIDOMNSEvent> nsEvent = do_QueryInterface(event, &result);
	if (NS_FAILED(result) || !nsEvent) {
		
		g_message("nsEvent failed %d",NS_ERROR_FAILURE);
		return NS_ERROR_FAILURE;
	}

	nsCOMPtr<nsIDOMEventTarget> OriginalTarget;
	result = nsEvent->GetOriginalTarget(getter_AddRefs(OriginalTarget));
	if (NS_FAILED(result) || !OriginalTarget) {
		
		g_message("OriginalTarget failed %d",NS_ERROR_FAILURE);
		return NS_ERROR_FAILURE;
	}

	nsCOMPtr<nsIDOMNode> OriginalNode = do_QueryInterface(OriginalTarget);
	if (!OriginalNode) {
		
		g_message("OriginalNode failed %d",NS_ERROR_FAILURE);
		return NS_ERROR_FAILURE;
	}
	
	nsAutoString node_name;
	OriginalNode->GetNodeName(node_name);
	if(node_name.EqualsIgnoreCase("span")) {
		PRBool _retval;
		OriginalNode->HasAttributes(&_retval);
		if(_retval) {
			gchar *tmp = embed_dialogs_get_attribute(OriginalNode,"sword_url");
			if (tmp && strlen(tmp)) {
				main_dialogs_url_handler(d, tmp, FALSE);
				g_free(tmp);
			}
		}
	}
	return FALSE;	
}

static 
gint embed_dialogs_dom_mouse_down_cb(GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
	PRUint16 button;
	PRBool aShiftKey;
	//gint pane = GPOINTER_TO_INT(data);
	//g_message("window=%d",GPOINTER_TO_INT(data));
	nsIDOMMouseEvent *aMouseEvent = (nsIDOMMouseEvent*)dom_event;
	aMouseEvent->GetButton(&button);
	aMouseEvent->GetShiftKey(&aShiftKey);	
	DIALOG_DATA *d = (DIALOG_DATA*)data;
	
	if(d->mod_type == TEXT_TYPE) {
		switch(button) {
			case 0:	
				if(aShiftKey) {
					shift_key_presed = TRUE;
					return TRUE;
				}
				else
					shift_key_presed = FALSE;
				break;
			case 1:
				shift_key_presed = TRUE;
				break;
			case 2:
				gui_text_dialog_create_menu(d);
				break;
		}
	} else if(d->mod_type == COMMENTARY_TYPE) {
		switch(button) {
			case 0:
				break;
			case 1:
				break;
			case 2:
				gui_commentary_dialog_create_menu(d);
				break;
		}
	} else if(d->mod_type == BOOK_TYPE) {
		switch(button) {
			case 0:	
				break;
			case 1:
				break;
			case 2:
				//gui_popup_menu_gbs();
				break;
		}
	} else if(d->mod_type == DICTIONARY_TYPE) {
		switch(button) {
			case 0:
				break;
			case 1:
				break;
			case 2:
				//gui_create_pm_dictionary();
				break;
		}
	} /*else if(pane == PARALLEL_TYPE) {
		switch(button) {
			case 0:
				break;
			case 1:
				break;
			case 2:
				gui_popup_menu_parallel();
				break;
		}
	}*/
	
	return FALSE;
}


static 
gint embed_dialogs_dom_mouse_up_cb(GtkMozEmbed *embed, gpointer dom_event, gpointer data)
{
	//PRUint16 button;
	shift_key_presed = FALSE;
	/*
	
	nsIDOMMouseEvent *aMouseEvent = (nsIDOMMouseEvent*)dom_event;
	aMouseEvent->GetButton(&button);
	switch(button) {
		case 0:
			break;
		case 1:
			shift_key_presed = FALSE;
			break;
		case 2:
			break;
	}*/
	return FALSE;	
	
}


static 
gint embed_dialogs_open_uri_cb (GtkMozEmbed *embed, const char *uri, gpointer data)
{
	main_url_handler(uri, TRUE);
	return TRUE;
}

static 
void embed_dialogs_link_message_cb (GtkMozEmbed *embed, gpointer data)
{	
	gchar buf[500];
	extern gboolean shift_key_presed;
	gchar *url = gtk_moz_embed_get_link_message (embed);
#ifdef DEBUG
	g_message(url);
#endif
	if(shift_key_presed)
		return;
	
	if (!strlen(url)) { /* moved out of url - clear appbar - info viewer*/
		//gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), "");
		in_url = FALSE;
		//if(GPOINTER_TO_INT(data) == TEXT_TYPE)
			main_clear_viewer();
	} else {
		in_url = TRUE;	/* we need this for html_button_released */
		/*if(main_url_handler(url, FALSE))
			return;
		
		if (*url == 'I') {
			return;
		} else if (*url == 'U') {
			++url;
			sprintf(buf, "%s %s", _("Unlock "), url);
		} else 
			sprintf(buf, "%s", "");
			
		gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar),
					buf);*/
	}
}


/******************************************************************************
 * Name
 *   get_primary_docshell
 *
 * Synopsis
 *   include "main/embed.h"
 *
 *   nsIDocShell *get_primary_docshell (GtkMozEmbed *b)
 *
 * Description
 *   gets and returns mozilla docshell (copied from Nautilus)
 *
 * Return value
 *   nsIDocShell*
 */

static 
nsIDocShell *get_primary_docshell (GtkMozEmbed *b)
{
	nsresult result;
	nsIWebBrowser *wb;
	gtk_moz_embed_get_nsIWebBrowser (b, &wb);

	nsCOMPtr<nsIDocShell> ds;

        nsCOMPtr<nsIDocShellTreeItem> browserAsItem = do_QueryInterface(wb);
	if (!browserAsItem) return NULL;

	// get the tree owner for that item
	nsCOMPtr<nsIDocShellTreeOwner> treeOwner;
	result = browserAsItem->GetTreeOwner(getter_AddRefs(treeOwner));
	if (!NS_SUCCEEDED (result) || ! treeOwner) return NULL;

	// get the primary content shell as an item
	nsCOMPtr<nsIDocShellTreeItem> contentItem;
	result = treeOwner->GetPrimaryContentShell(getter_AddRefs(contentItem));
	if (!NS_SUCCEEDED (result) || ! contentItem) return NULL;

	// QI that back to a docshell
	ds = do_QueryInterface(contentItem);

	return ds;
}


void embed_dialogs_go_to_anchor(GtkMozEmbed *mozilla_embed, const char *anchor)
{
	nsAutoString aAnchorName;
	nsCOMPtr<nsIDocShell> ds;
	nsresult rv, result;
	
	aAnchorName.AssignWithConversion (anchor);
	
	ds = get_primary_docshell (mozilla_embed);

	if ( ! ds ) {
		return;
	}

	/* get nsIPresShell */

	nsCOMPtr<nsIPresShell> presShell;
	result = ds->GetPresShell(getter_AddRefs(presShell));
	if (!NS_SUCCEEDED(result) || (!presShell)) 
		return;

	/* get nsIDocument */

/*	nsCOMPtr<nsIDocument> document;
	result = presShell->GetDocument(getter_AddRefs(document));
	if (!NS_SUCCEEDED(result) || (!document)) 
		return;
		
	presShell->GoToAnchor(aAnchorName, 1);*/
}
/*
void embed_dialogs_copy_selection(GtkMozEmbed *mozilla_embed)
{
	nsIWebBrowser *wb;
	gtk_moz_embed_get_nsIWebBrowser (mozilla_embed, &wb);
	
	nsCOMPtr<nsIClipboardCommands> clipboard (do_GetInterface(wb));
	clipboard->CopySelection();
}
*/
GtkWidget *embed_dialogs_new(gpointer dialog)
{
	GtkMozEmbed *new_browser;
	DIALOG_DATA *d = (DIALOG_DATA*)dialog;
	
	new_browser = GTK_MOZ_EMBED(gtk_moz_embed_new());
	
	g_signal_connect(GTK_OBJECT(new_browser), "dom_key_down",
				   G_CALLBACK(embed_dialogs_dom_key_down),
				   (DIALOG_DATA*) d);
	g_signal_connect(GTK_OBJECT(new_browser), "dom_key_up",
				   G_CALLBACK(embed_dialogs_dom_key_up),
				   (DIALOG_DATA*) d);
	g_signal_connect(GTK_OBJECT(new_browser), "dom_key_press",
				   G_CALLBACK(embed_dialogs_dom_key_press),
				   (DIALOG_DATA*) d);
	g_signal_connect(GTK_OBJECT(new_browser), "dom_mouse_over",//dom_mouse_out",
				   G_CALLBACK(embed_dialogs_dom_mouse_over),
				   (DIALOG_DATA*) d);
	g_signal_connect(GTK_OBJECT(new_browser), "dom_mouse_down",
				   G_CALLBACK(embed_dialogs_dom_mouse_down_cb),
				   (DIALOG_DATA*) d);
	g_signal_connect(GTK_OBJECT(new_browser), "dom_mouse_up",
				   G_CALLBACK(embed_dialogs_dom_mouse_up_cb),
				   (DIALOG_DATA*) d);
	g_signal_connect(GTK_OBJECT(new_browser), "open_uri",
				   G_CALLBACK(embed_dialogs_open_uri_cb),
				   (DIALOG_DATA*) d);
	g_signal_connect(GTK_OBJECT(new_browser), "link_message",
				   G_CALLBACK(embed_dialogs_link_message_cb),
				   (DIALOG_DATA*) d);
				   
	return GTK_WIDGET(new_browser);
}

#endif
