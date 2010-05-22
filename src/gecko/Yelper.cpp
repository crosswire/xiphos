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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  $Id: Yelper.cpp,v 1.7 2006/09/17 17:24:32 chpe Exp $
 */

#include <mozilla-config.h>
#include <config.h>

#include <stdlib.h>
#include <unistd.h>

#include <nsStringAPI.h>

#ifdef NS_HIDDEN
# undef NS_HIDDEN
#endif
#ifdef NS_IMPORT_
# undef NS_IMPORT_
#endif
#ifdef NS_EXPORT_
# undef NS_EXPORT_
#endif

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
#include <nsIPrefBranch.h>
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

#include "gecko/gecko-services.h"

#include "gecko/Yelper.h"

#include "gui/widgets.h"
#include "gui/dictlex.h"

#include "main/module_dialogs.h"
#include "main/previewer.h"
#include "main/sword.h"
#include "main/url.hh"


extern gboolean shift_key_pressed;

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

	mInitialised = PR_TRUE;

	nsCOMPtr<nsIPrefService> prefService = do_GetService (NS_PREFSERVICE_CONTRACTID);
	nsCOMPtr<nsIPrefBranch> pref;
	prefService->GetBranch("", getter_AddRefs(pref));
	rv = pref->SetBoolPref("layout.word_select.stop_at_punctuation", PR_TRUE);
	NS_ENSURE_SUCCESS (rv, rv);
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

	aAnchorName.Assign(NS_ConvertUTF8toUTF16(anchor));

	nsCOMPtr<nsIDocShell> docShell (do_GetInterface (mWebBrowser, &rv));

	if ( !docShell  ) {
		return;
	}

	// * get nsIPresShell *

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

	rv = mFinder->Find (NS_ConvertUTF8toUTF16 (aSearchString),
			    PR_FALSE ,//  links only? *
			    //mHasFocus,
			    &found);

	return NS_SUCCEEDED (rv) && (found == nsITypeAheadFind::FIND_FOUND || found == nsITypeAheadFind::FIND_WRAPPED);
}

PRBool
Yelper::FindAgain (PRBool aForward)
{
	if (!mInitialised) return PR_FALSE;

	SetSelectionAttention (PR_TRUE);

	nsresult rv;
	PRUint16 found = nsITypeAheadFind::FIND_NOTFOUND;

	rv = mFinder->FindAgain (!aForward, mHasFocus, &found);

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

	mFinder->SetSelectionModeAndRepaint (display);
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
#endif /* 0 */
}

// Nautilus CREDITS here

gint
Yelper::ProcessMouseOver (void* aEvent, int pane,
			  gboolean is_dialog, DIALOG_DATA * dialog)
{
	extern gboolean in_url;
	PRBool aShiftKey;

	//GS_message(("mouse over pane: %d",pane));
	nsIDOMMouseEvent *event = (nsIDOMMouseEvent*) aEvent;
	//DIALOG_DATA *dialog = (DIALOG_DATA *)data;
	event->GetShiftKey(&aShiftKey);
	if (aShiftKey)  {
	       return 1;
	}
	if (shift_key_pressed)
		return FALSE;
	if (pane == VIEWER_TYPE)
		return FALSE;
	if (in_url) {
		in_url = FALSE;
		return FALSE;
	}
	main_clear_viewer();
	return FALSE;
}

gint Yelper::ProcessMouseDblClickEvent (void* aEvent)
{
	nsAutoString aType;

	g_return_val_if_fail(aEvent != NULL,0);
	nsCOMPtr<nsISelection> oSelection;
	nsIDOMEvent *domEvent = static_cast<nsIDOMEvent*>(aEvent);
	nsCOMPtr<nsIDOMMouseEvent> event (do_QueryInterface (domEvent));
	if (!event) return 0;

  /**
     * Returns the whole selection into a plain text string.
     */
  /* wstring toString (); */
 // NS_SCRIPTABLE NS_IMETHOD ToString(PRUnichar **_retval) = 0;

	gtk_editable_delete_text((GtkEditable *)widgets.entry_dict,0,-1);

	DoCommand("cmd_copy");

	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

	gtk_clipboard_request_text (clipboard,
				    gui_get_clipboard_text_for_lookup,
				    NULL);
	return 1;
}

gint
Yelper::ProcessMouseUpEvent (void* aEvent)
{
	g_return_val_if_fail(aEvent != NULL,0);

	nsIDOMEvent *domEvent = static_cast<nsIDOMEvent*>(aEvent);
	nsCOMPtr<nsIDOMMouseEvent> event (do_QueryInterface (domEvent));
	if (!event) return 0;

	PRUint16 button = 2;
	event->GetButton (&button);

	if (button == 1)
	        shift_key_pressed = FALSE;


	return 0;
}

gint
Yelper::ProcessMouseEvent (void* aEvent)
{
	g_return_val_if_fail(aEvent != NULL,0);

	nsIDOMEvent *domEvent = static_cast<nsIDOMEvent*>(aEvent);
	nsCOMPtr<nsIDOMMouseEvent> event (do_QueryInterface (domEvent));
	if (!event) return 0;

	PRUint16 button = 2;
	event->GetButton (&button);

	GS_message(("mouse button: %d",button));

	if (button == 1) shift_key_pressed = TRUE;

	/* Mozilla uses 2 as its right mouse button code */
	if (button != 2) return 0;

	GS_message(("g_signal_emit_by_name"));
	if (mEmbed)
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
	GS_message(("Yelper::ProcessKeyReleaseEvent"));
	shift_key_pressed = FALSE;
	return NS_OK;
}

#ifdef HAVE_GTKUPRINT
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

//make all xulrunner widgets appear RTL
void
Yelper::SetRTL()
{
	nsresult rv;
	nsCOMPtr<nsIPrefService> prefService =
		do_GetService (NS_PREFSERVICE_CONTRACTID);
	nsCOMPtr<nsIPrefBranch> pref;
	prefService->GetBranch("", getter_AddRefs(pref));
	rv = pref->SetIntPref("bidi.direction", 2);
}
