/*
 *  Copyright (C) 2000-2004 Marco Pesenti Gritti
 *  Copyright (C) 2003-2006 Christian Persch
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  $Id: Yelper.cpp,v 1.7 2006/09/17 17:24:32 chpe Exp $
 */

#include <mozilla-config.h>
#include <config.h>

#include <stdlib.h>
#include <unistd.h>

#define MOZILLA_INTERNAL_API
#include <nsString.h>

#include <gtkmozembed.h>
#include <gtkmozembed_internal.h>
#include <nsComponentManagerUtils.h>
#include <nsICommandManager.h>
#include <nsIDocShell.h>
#include <nsIDOMDocument.h>
#include <nsIDOMEventTarget.h>
#include <nsIDOMHTMLAnchorElement.h>
#include <nsIDOMMouseEvent.h>
#include <nsIDOMNode.h>
#include <nsIDOMNSEvent.h>
#include <nsIDOMWindow.h>
#include <nsIDOMKeyEvent.h>
#include <nsIInterfaceRequestorUtils.h>
#include <nsIPrefService.h>
#include <nsIPrintSettings.h>
#include <nsISelectionController.h>
#include <nsITypeAheadFind.h>
#include <nsIWebBrowser.h>
#include <nsIWebBrowserPrint.h>
#include <nsIWebBrowserSetup.h>
#include <nsServiceManagerUtils.h>
#include <nsIPresShell.h> 
#include <nsIDOMNamedNodeMap.h>
//#include <nsIFrame.h>

#ifndef HAVE_GECKO_1_9
#include <nsIDocShell.h>
#include <nsIDocShellTreeItem.h>
#include <nsISelectionDisplay.h>
#include <nsISimpleEnumerator.h>
#include <nsITypeAheadFind.h>
#endif /* !HAVE_GECKO_1_9 */

#include "gecko/gecko-services.h"

#include "gecko/Yelper.h"

#include "gui/widgets.h"

#include "main/module_dialogs.h"
#include "main/previewer.h"
#include "main/sword.h"
#include "main/url.hh"


extern gboolean shift_key_presed;

#define NS_TYPEAHEADFIND_CONTRACTID "@mozilla.org/typeaheadfind;1"

Yelper::Yelper (GtkMozEmbed *aEmbed)
: mInitialised(PR_FALSE)
, mSelectionAttention(PR_FALSE)
, mHasFocus(PR_FALSE)
, mEmbed(aEmbed)
{
	//debug_print (DB_DEBUG, "Yelper ctor [%p]\n", this);
}

Yelper::~Yelper ()
{
	//debug_print (DB_DEBUG, "Yelper dtor [%p]\n", this);
}

nsresult
Yelper::Init ()
{
	if (mInitialised) return NS_OK;

	nsresult rv = NS_ERROR_FAILURE;
	gtk_moz_embed_get_nsIWebBrowser (mEmbed,
					 getter_AddRefs (mWebBrowser));
	NS_ENSURE_TRUE (mWebBrowser, rv);

	nsCOMPtr<nsIWebBrowserSetup> setup (do_QueryInterface (mWebBrowser, &rv));
	NS_ENSURE_SUCCESS (rv, rv);

	setup->SetProperty (nsIWebBrowserSetup::SETUP_USE_GLOBAL_HISTORY, PR_FALSE);

	rv = mWebBrowser->GetContentDOMWindow (getter_AddRefs (mDOMWindow));
	NS_ENSURE_SUCCESS (rv, rv);

	/* This will instantiate an about:blank doc if necessary */
	nsCOMPtr<nsIDOMDocument> domDocument;
	rv = mDOMWindow->GetDocument (getter_AddRefs (domDocument));
	NS_ENSURE_SUCCESS (rv, rv);

	mFinder = do_CreateInstance (NS_TYPEAHEADFIND_CONTRACTID, &rv);
	NS_ENSURE_SUCCESS (rv, rv);

	nsCOMPtr<nsIDocShell> docShell (do_GetInterface (mWebBrowser, &rv));
	NS_ENSURE_SUCCESS (rv, rv);

	rv = mFinder->Init (docShell);
	NS_ENSURE_SUCCESS (rv, rv);

#ifdef HAVE_GECKO_1_9
//	mFinder->SetSelectionModeAndRepaint (nsISelectionController::SELECTION_ON);
#else
	mFinder->SetFocusLinks (PR_TRUE);
#endif

	mInitialised = PR_TRUE;

	return NS_OK;
}

void
Yelper::Destroy ()
{
	mEmbed = nsnull;
	mWebBrowser = nsnull;
	mDOMWindow = nsnull;
	mFinder = nsnull;

	mInitialised = PR_FALSE;
}

void
Yelper::DoCommand (const char *aCommand)
{
	if (!mInitialised) return;

	nsCOMPtr<nsICommandManager> cmdManager (do_GetInterface (mWebBrowser));
	if (!cmdManager) return;

	cmdManager->DoCommand (aCommand, nsnull, nsnull);
}

void Yelper::JumpToAnchor(const char *anchor)
{
	nsAutoString aAnchorName;
	nsresult rv, result;
	
	aAnchorName.AssignWithConversion (anchor);
	
	nsCOMPtr<nsIDocShell> docShell (do_GetInterface (mWebBrowser, &rv));

	if ( !docShell  ) {
		return;
	}

	/* get nsIPresShell */

	nsCOMPtr<nsIPresShell> presShell;
	result = docShell->GetPresShell(getter_AddRefs(presShell));
	if (!NS_SUCCEEDED(result) || (!presShell)) 
		return;
		
	presShell->GoToAnchor(aAnchorName, 1);
}
void
Yelper::SetFindProperties (const char *aSearchString,
			   PRBool aCaseSensitive,
			   PRBool aWrap)
{
	if (!mInitialised) return;

	mFinder->SetCaseSensitive (aCaseSensitive);
	/* aWrap not supported for typeaheadfind
	 * search string is set in ::Find for typeaheadfind
	 */
}

PRBool
Yelper::Find (const char *aSearchString)
{
	NS_ENSURE_TRUE (aSearchString, PR_FALSE);

	if (!mInitialised) return PR_FALSE;

	SetSelectionAttention (PR_TRUE);

	nsresult rv;
	PRUint16 found = nsITypeAheadFind::FIND_NOTFOUND;
#ifdef HAVE_GECKO_1_9
	rv = mFinder->Find (NS_ConvertUTF8toUTF16 (aSearchString),
			    PR_FALSE /* links only? */,
			    mHasFocus,
			    &found);
#else
	rv = mFinder->Find (NS_ConvertUTF8toUTF16 (aSearchString),
			    PR_FALSE /* links only? */,
			    &found);
#endif

	return NS_SUCCEEDED (rv) && (found == nsITypeAheadFind::FIND_FOUND || found == nsITypeAheadFind::FIND_WRAPPED);
}

PRBool
Yelper::FindAgain (PRBool aForward)
{
	if (!mInitialised) return PR_FALSE;

	SetSelectionAttention (PR_TRUE);

	nsresult rv;
	PRUint16 found = nsITypeAheadFind::FIND_NOTFOUND;
#ifdef HAVE_GECKO_1_9
	rv = mFinder->FindAgain (!aForward, mHasFocus, &found);
#else
	if (aForward) {
		rv = mFinder->FindNext (&found);
	}
	else {
		rv = mFinder->FindPrevious (&found);
	}
#endif /* HAVE_GECKO_1_9 */

	return NS_SUCCEEDED (rv) && (found == nsITypeAheadFind::FIND_FOUND || found == nsITypeAheadFind::FIND_WRAPPED);
}

void
Yelper::SetSelectionAttention (PRBool aAttention)
{
#if 0
	if (aAttention == mSelectionAttention) return;
	
	mSelectionAttention = aAttention;
	
	NS_ENSURE_TRUE (mFinder, );

	PRInt16 display;
	if (aAttention) {
		display = nsISelectionController::SELECTION_ATTENTION;
	}
	else {
		display = nsISelectionController::SELECTION_ON;
	}

#ifdef HAVE_GECKO_1_9
	mFinder->SetSelectionModeAndRepaint (display);
#else
	nsresult rv;
	nsCOMPtr<nsIDocShell> shell (do_GetInterface (mWebBrowser, &rv));
	/* It's okay for this to fail, if the tab is closing, or if
	* we weren't attached to any tab yet
	*/
	if (NS_FAILED (rv) || !shell) return;
	
	nsCOMPtr<nsISimpleEnumerator> enumerator;
	rv = shell->GetDocShellEnumerator (nsIDocShellTreeItem::typeContent,
					   nsIDocShell::ENUMERATE_FORWARDS,
					   getter_AddRefs (enumerator));
	NS_ENSURE_SUCCESS (rv, );

	PRBool hasMore = PR_FALSE;
	while (NS_SUCCEEDED (enumerator->HasMoreElements (&hasMore)) && hasMore) {
		nsCOMPtr<nsISupports> element;
		enumerator->GetNext (getter_AddRefs (element));
		if (!element) continue;
	
		nsCOMPtr<nsISelectionDisplay> sd (do_GetInterface (element));
		if (!sd) continue;
	
		nsCOMPtr<nsISelectionController> controller (do_QueryInterface (sd));
		if (!controller) continue;
	
		controller->SetDisplaySelection (display);
	}
#endif /* HAVE_GECKO_1_9 */
#endif /* 0 */
}

// Nautilus CREDITS here
 
gchar *Yelper::GetAttribute (nsIDOMNode *node, gchar *attribute)
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

gint
Yelper::ProcessMouseOver (void* aEvent, int pane, 
			  gboolean is_dialog, DIALOG_DATA * dialog)
{
	nsresult result;
	PRBool aShiftKey;
	nsIDOMEventTarget *aCurrentTarget;
	GS_message(("mouse over pane: %d",pane));
	nsIDOMMouseEvent *event = (nsIDOMMouseEvent*) aEvent;	
	//DIALOG_DATA *dialog = (DIALOG_DATA *)data;
	event->GetShiftKey(&aShiftKey);
	if(aShiftKey)  {    
	       return 1;
	}
	if(shift_key_presed)
		return FALSE;
	if(pane == VIEWER_TYPE)
		return FALSE;
	main_clear_viewer();
	/*
	nsCOMPtr<nsIDOMNSEvent> nsEvent = do_QueryInterface(event, &result);
	if (NS_FAILED(result) || !nsEvent) {
		
		GS_message(("nsEvent failed %d",NS_ERROR_FAILURE));
		return NS_ERROR_FAILURE;
	}

	nsCOMPtr<nsIDOMEventTarget> OriginalTarget;
	result = nsEvent->GetOriginalTarget(getter_AddRefs(OriginalTarget));
	if (NS_FAILED(result) || !OriginalTarget) {
		
		GS_message(("OriginalTarget failed %d",NS_ERROR_FAILURE));
		return NS_ERROR_FAILURE;
	}

	nsCOMPtr<nsIDOMNode> OriginalNode = do_QueryInterface(OriginalTarget);
	if (!OriginalNode) {
		
		GS_message(("OriginalNode failed %d",NS_ERROR_FAILURE));
		return NS_ERROR_FAILURE;
	}
	
	nsAutoString node_name;
	OriginalNode->GetNodeName(node_name);
	//gchar mybuf[80];
	//node_name.ToCString( mybuf, 12);
	//g_message("node name: %s", mybuf);
	if(node_name.EqualsIgnoreCase("a")) {
		PRBool _retval;
		OriginalNode->HasAttributes(&_retval);
		if(_retval) {
			gchar *tmp = GetAttribute(OriginalNode,"href");
			if (tmp && strlen(tmp)) {
				GString *url_clean = g_string_new(NULL);
				const gchar *url_chase;
				int i = 0;
				for (url_chase = tmp; *url_chase; ++url_chase) {
					switch (*url_chase) {
					case '/':
						if(i > 2)
							g_string_append(url_clean, "%2F");
						else
							g_string_append_c(url_clean, *url_chase);
						++i;
						break;
					default:
							g_string_append_c(url_clean, *url_chase);
						break;
					}
				}
				if(is_dialog) 
					main_dialogs_url_handler(dialog, url_clean->str, FALSE);
				else 
					main_url_handler(url_clean->str, FALSE);
				g_string_free(url_clean, TRUE);	
				g_free(tmp);
			}
		}
	}*/
	return FALSE;	
}

gint Yelper::ProcessMouseDblClickEvent (void* aEvent)
{
	nsAutoString aType;
	nsresult rv;
	PRUnichar **selText;
	g_return_val_if_fail(aEvent != NULL,0);
	nsCOMPtr<nsISelection> oSelection;
	nsIDOMEvent *domEvent = static_cast<nsIDOMEvent*>(aEvent);
	nsCOMPtr<nsIDOMMouseEvent> event (do_QueryInterface (domEvent));
	if (!event) return 0;
		
#ifdef DEBUG
/*	domEvent->GetType(aType);
	gchar mybuf[80];
	aType.ToCString( mybuf, 79);
	g_warning("domEvent->GetType: %s",mybuf);*/
#endif
	gtk_editable_delete_text((GtkEditable *)widgets.entry_dict,0,-1);
	
	DoCommand("cmd_copy");
	//gtk_editable_select_region((GtkEditable *)widgets.entry_dict,0,-1);
	//gtk_editable_delete_selection((GtkEditable *)widgets.entry_dict);
	gtk_editable_paste_clipboard((GtkEditable *)widgets.entry_dict);
	gtk_widget_activate(widgets.entry_dict);
/*	rv = mDOMWindow->GetSelection(getter_AddRefs(oSelection));
	rv = oSelection->ToString(selText);
	g_message("selText: %s", selText);*/
	return 1;
}

gint
Yelper::ProcessMouseUpEvent (void* aEvent)
{	
	g_return_val_if_fail(aEvent != NULL,0);
	
	nsIDOMEvent *domEvent = static_cast<nsIDOMEvent*>(aEvent);
	nsCOMPtr<nsIDOMMouseEvent> event (do_QueryInterface (domEvent));
	if (!event) return 0;
		
#ifdef DEBUG
	nsAutoString aType;
	domEvent->GetType(aType);
	gchar mybuf[80];
	aType.ToCString( mybuf, 79);
	GS_message(("domEvent->GetType: %s",mybuf));
#endif
	
	PRUint16 button = 2;
	event->GetButton (&button);

	//GS_message(("mouse button up: %d",button));
	if(button == 1)	       
	        shift_key_presed = FALSE; 
	
	
	return 0;	
}

gint
Yelper::ProcessMouseEvent (void* aEvent)
{	
	g_return_val_if_fail(aEvent != NULL,0);
	
	nsIDOMEvent *domEvent = static_cast<nsIDOMEvent*>(aEvent);
	nsCOMPtr<nsIDOMMouseEvent> event (do_QueryInterface (domEvent));
	if (!event) return 0;
	
	
#ifndef DEBUG
	nsAutoString aType;	
	domEvent->GetType(aType);
	gchar mybuf[80];
	aType.ToCString( mybuf, 79);
	GS_message(("domEvent->GetType: %s",mybuf));
#endif	
	
	PRUint16 button = 2;
	event->GetButton (&button);
	
	GS_message(("mouse button: %d",button));	
	
	if(button == 1) shift_key_presed = TRUE; 
	
	/* Mozilla uses 2 as its right mouse button code */
	if (button != 2) return 0;	

	GS_message(("g_signal_emit_by_name"));
	if(mEmbed)
		g_signal_emit_by_name (mEmbed, "popupmenu_requested", NULL);  //,
			        // NS_ConvertUTF16toUTF8 (href).get());
	return 1;	
}

gint Yelper::ProcessKeyDownEvent(GtkMozEmbed *embed, gpointer dom_event)
{
	nsIDOMKeyEvent *event = (nsIDOMKeyEvent*) dom_event;
	if (!event) return 0;
	nsresult rv;
	PRUint32 keyCode;
	rv = event->GetKeyCode(&keyCode);
	if (NS_FAILED(rv)) return rv;
	//g_message("keycode: %d",keyCode);
	
	return NS_OK;
}	

gint Yelper::ProcessKeyReleaseEvent(GtkMozEmbed *embed, gpointer dom_event)
{
	shift_key_presed = FALSE;
	return NS_OK;	
}

gint Yelper::redraw(GtkMozEmbed *embed)
{
/*	nsresult rv = NS_ERROR_FAILURE;
	nsCOMPtr<nsIDocShell> docShell (do_GetInterface (mWebBrowser, &rv));
	NS_ENSURE_SUCCESS (rv, rv);
	nsCOMPtr<nsIPresShell> presShell;
	rv = docShell->GetPresShell(getter_AddRefs(presShell));
        if (NS_SUCCEEDED(rv) && presShell) {
		nsIFrame *rootFrame = presShell->GetRootFrame();
 
		if (rootFrame) {
			nsRect r(nsPoint(0, 0), rootFrame->GetSize());
			rootFrame->Invalidate(r, 1);
			return 1;
	        }
	}
	return 0;
	*/
	
	//mDOMWindow->Redraw();	
}

#ifdef USE_GTKUPRINT
nsresult 
Yelper::Print (GeckoPrintInfo *print_info, PRBool preview, int *prev_pages)
{
  nsresult rv;
	
  nsCOMPtr<nsIWebBrowserPrint> print(do_GetInterface (mWebBrowser, &rv));
  NS_ENSURE_SUCCESS (rv, rv);

  nsCOMPtr<nsIPrintSettings> settings;

  rv = print->GetGlobalPrintSettings (getter_AddRefs (settings));
  NS_ENSURE_SUCCESS (rv, rv);

  rv = PrintListener::SetPrintSettings (print_info, preview, settings);

  NS_ENSURE_SUCCESS (rv, rv);

  nsCOMPtr<PrintListener> listener = new PrintListener (print_info, print);

  if (!preview){
	GS_message(("Yelper::Print4"));
	rv = print->Print (settings, listener);
	GS_message(("Yelper::Print5"));
  }
  else {
    rv = print->PrintPreview (settings, mDOMWindow, nsnull);
    rv |= print->GetPrintPreviewNumPages (prev_pages);
  }
  GS_message(("Yelper::Print6"));
  return rv;

}
#endif

nsresult
Yelper::PrintPreviewNavigate (int page_no)
{
  nsresult rv;
  nsCOMPtr<nsIWebBrowserPrint> print(do_GetInterface (mWebBrowser, &rv));
  NS_ENSURE_SUCCESS (rv, rv);

  return print->PrintPreviewNavigate (0, page_no);
}

nsresult 
Yelper::PrintPreviewEnd ()
{
  nsresult rv;
  nsCOMPtr<nsIWebBrowserPrint> print(do_GetInterface (mWebBrowser, &rv));
  NS_ENSURE_SUCCESS (rv, rv);

  return print->ExitPrintPreview ();

}
