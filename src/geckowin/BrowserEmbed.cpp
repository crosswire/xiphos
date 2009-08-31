//copyright xiphos 2009. license gpl2

#include <windows.h>
#include "nsCOMPtr.h"
#include "content/nsPIDOMEventTarget.h"
#include "nsComponentManagerUtils.h"
#include "nsEmbedCID.h"
#include "nsEmbedString.h"
#include "nsIDOMWindow.h"
#include "nsIInterfaceRequestorUtils.h"
#include "nsISupportsImpl.h"
#include "nsIWebBrowser.h"
#include "nsIWebBrowserFocus.h"
#include "docshell/nsIDocShell.h"
#include "docshell/nsDocShellCID.h"
#include "docshell/nsIDocShellTreeItem.h"
#include "docshell/nsIWebNavigationInfo.h"
#include "layout/nsIPresShell.h" 
#include "necko/nsIURI.h"
#include "xpcom/nsServiceManagerUtils.h"
#include "dom/nsIDOMMouseEvent.h"
#include "dom/nsIDOMUIEvent.h"
#include "dom/nsIDOMKeyEvent.h"
#include "nsVoidArray.h"
#include "nsIWebBrowserChrome.h"
#include "nsIWebBrowserStream.h"
#include "necko/nsNetUtil.h"
#include "nsIPrefService.h"
#include "nsIPrefBranch.h"
#include "dom/nsPIDOMWindow.h"
#include "fastfind/nsITypeAheadFind.h"
#include "nsISelectionController.h"
#include "commandhandler/nsICommandManager.h"
#include "nsIFocusController.h"
#include "nsIDOMBarProp.h"
#include "nsIDOMEvent.h"
#include "nsIDOMEventTarget.h"
#include "nsIDOMEventGroup.h"


#include "BrowserEmbed.h"
#include "FixFocus.h"
#include "gecko-html.h"

#include "gui/widgets.h"
#include "gui/dictlex.h"
#include "main/module_dialogs.h"
#include "main/previewer.h"
#include "main/sword.h"
#include "main/url.hh"

#include <stdio.h>

extern gboolean shift_key_pressed;

#define NS_TYPEAHEADFIND_CONTRACTID "@mozilla.org/typeaheadfind;1"

BrowserEmbed::BrowserEmbed()
{
    mWindow = nsnull;
    mFocusCallback = nsnull;
    mInitialised = PR_FALSE;
    mEventTarget = nsnull;
}

BrowserEmbed::~BrowserEmbed()
{
}

nsresult BrowserEmbed::init(unsigned long parentWindow, int x, 
			    int y, int width, int height, GeckoHtml *owner)
{
	if (mInitialised) return NS_OK;

	nsresult rv;
//	rv = NS_InitEmbedding(nsnull, nsnull);
	mOwner = owner;

	GS_message(("line 106"));
    mWebBrowser = do_CreateInstance(NS_WEBBROWSER_CONTRACTID, &rv);
    if (!mWebBrowser)
	    return NS_ERROR_FAILURE;
    
    if (NS_FAILED(rv)) return rv;
    mWindow = (nativeWindow)parentWindow;
    //mWindow = reinterpret_cast<nativeWindow>(parentWindow);
    
    this->AddRef();
    mWebBrowser->SetContainerWindow((nsIWebBrowserChrome*)this);
    //mWebBrowser->SetContainerWindow(static_cast<nsIWebBrowserChrome*>(this));

    nsCOMPtr<nsIDocShellTreeItem> dsti = do_QueryInterface(mWebBrowser);
    dsti->SetItemType(nsIDocShellTreeItem::typeAll);

    nsCOMPtr<nsIBaseWindow> browserBaseWindow(do_QueryInterface(mWebBrowser));
    browserBaseWindow->InitWindow(mWindow, nsnull, 0, 0, width, height);
    browserBaseWindow->Create();
    browserBaseWindow->SetVisibility(PR_TRUE);
    browserBaseWindow->SetEnabled(PR_TRUE);
    
    GS_message(("line 128"));
    install_focus_fixes((HWND)mWindow);
    rv = mWebBrowser->GetParentURIContentListener(
            getter_AddRefs(mParentContentListener));
    NS_ENSURE_SUCCESS(rv, rv);
    rv = mWebBrowser->SetParentURIContentListener(
            static_cast<nsIURIContentListener *>(this));
    NS_ENSURE_SUCCESS(rv, rv);

    nsCOMPtr<nsIWeakReference> weakRef;
    rv = GetWeakReference(getter_AddRefs(weakRef));
    NS_ENSURE_SUCCESS(rv, rv);

    rv = mWebBrowser->AddWebBrowserListener(weakRef,
            NS_GET_IID(nsIWebProgressListener));
    NS_ENSURE_SUCCESS(rv, rv);
    mWebNavigation = do_QueryInterface(mWebBrowser);
    
    GS_message(("line 146"));
    //set up keyboard and mouse event listeners
    mEventListener = new EmbedEventListener();
    mEventListener->Init(this);

    mFinder = do_CreateInstance (NS_TYPEAHEADFIND_CONTRACTID, &rv);
    NS_ENSURE_SUCCESS (rv, rv);

    nsCOMPtr<nsIDOMWindow> domWindow;
    mWebBrowser->GetContentDOMWindow(getter_AddRefs(domWindow));

    nsCOMPtr<nsIDOMWindow2> d2 (do_QueryInterface(domWindow));
    d2->GetWindowRoot(getter_AddRefs(mEventTarget));
    
    mEventTarget->AddEventListener(NS_LITERAL_STRING("mouseover"), (nsIDOMEventListener *)mEventListener, PR_FALSE);
    mEventTarget->AddEventListener(NS_LITERAL_STRING("click"), (nsIDOMEventListener *)mEventListener, PR_FALSE);
    mEventTarget->AddEventListener(NS_LITERAL_STRING("dblclick"), (nsIDOMEventListener *)mEventListener, PR_FALSE);
    mEventTarget->AddEventListener(NS_LITERAL_STRING("mouseout"), (nsIDOMEventListener *) mEventListener, PR_FALSE);

    mInitialised = PR_TRUE;
    return NS_OK;
}

nsresult BrowserEmbed::disable()
{
    nsresult rv;
    nsCOMPtr<nsIBaseWindow> browserBaseWindow(do_QueryInterface(mWebBrowser));

    rv = browserBaseWindow->SetVisibility(PR_FALSE);
    NS_ENSURE_SUCCESS(rv, rv);
    rv = browserBaseWindow->SetEnabled(PR_FALSE);
    NS_ENSURE_SUCCESS(rv, rv);
    return NS_OK;
}

nsresult BrowserEmbed::enable()
{
    nsresult rv;
    nsCOMPtr<nsIBaseWindow> browserBaseWindow(do_QueryInterface(mWebBrowser));

    rv = browserBaseWindow->SetVisibility(PR_TRUE);
    NS_ENSURE_SUCCESS(rv, rv);
    rv = browserBaseWindow->SetEnabled(PR_TRUE);
    NS_ENSURE_SUCCESS(rv, rv);
    rv = browserBaseWindow->Repaint(PR_FALSE);
    NS_ENSURE_SUCCESS(rv, rv);
    return NS_OK;
}

void BrowserEmbed::destroy()
{
    nsCOMPtr<nsIBaseWindow> browserBaseWindow(do_QueryInterface(mWebBrowser));
    browserBaseWindow->Destroy();
}

// Load a URI into the browser
nsresult BrowserEmbed::loadURI(const char* uri)
{
    mWebNavigation->LoadURI(NS_ConvertASCIItoUTF16(uri).get(),
            nsIWebNavigation::LOAD_FLAGS_NONE, 0, 0, 0);
    return NS_OK;
}

nsresult BrowserEmbed::getCurrentURI(char ** uri)
{
    nsresult rv;
    nsCAutoString specString;
    nsIURI *aURI;

    rv = mWebNavigation->GetCurrentURI(&aURI);
    NS_ENSURE_SUCCESS(rv, rv);

    rv = aURI->GetSpec(specString);
    NS_ENSURE_SUCCESS(rv, rv);

    *uri = (char *) specString.get();
    return NS_OK;
}

nsresult BrowserEmbed::getCurrentTitle(wchar_t ** aTitle)
{
    nsresult rv;
    nsISHistory *history;
    nsIHistoryEntry *historyentry;
    wchar_t *title;
    PRInt32 index;

    rv = mWebNavigation->GetSessionHistory(&history);
    NS_ENSURE_SUCCESS(rv, rv);

    rv = history->GetIndex(&index);
    NS_ENSURE_SUCCESS(rv, rv);

    rv = history->GetEntryAtIndex(index, PR_FALSE, &historyentry);
    NS_ENSURE_SUCCESS(rv, rv);

    rv = historyentry->GetTitle(&title);
    NS_ENSURE_SUCCESS(rv, rv);

    *aTitle = title;
    return NS_OK;
}

// Called when the parent window changes size
nsresult BrowserEmbed::resize(int x, int y, int width, int height)
{
    nsCOMPtr<nsIBaseWindow>browserBaseWindow(
            do_QueryInterface(mWebBrowser));
    if(!browserBaseWindow) return NS_ERROR_FAILURE;
    return browserBaseWindow->SetPositionAndSize(x, y, width, height,
            PR_TRUE);
}

// Give the browser keyboard focus
nsresult BrowserEmbed::focus()
{
    nsCOMPtr<nsIWebBrowserFocus> browserFocus(
            do_GetInterface(mWebBrowser));
    if(!browserFocus) return NS_ERROR_FAILURE;
    return browserFocus->Activate();
}

// Set the focus callback.  This will be called when the user tabs through all
// the elements in the browser and the next Widget should be given focus.
void BrowserEmbed::SetFocusCallback(focusCallback callback, void* data)
{
    mFocusCallback = callback;
    mFocusCallbackData = data;
}

void BrowserEmbed::SetURICallback(uriCallback callback, void* data)
{
    mURICallback = callback;
    mURICallbackData = data;
}

void BrowserEmbed::SetNetworkCallback(networkCallback callback, void* data)
{
    mNetworkCallback = callback;
    mNetworkCallbackData = data;
}

int BrowserEmbed::canGoBack()
{
    PRBool retval;
    mWebNavigation->GetCanGoBack(&retval);
    return retval;
}

int BrowserEmbed::canGoForward()
{
    PRBool retval;
    mWebNavigation->GetCanGoForward(&retval);
    return retval;
}

void BrowserEmbed::goBack()
{
    mWebNavigation->GoBack();
}

void BrowserEmbed::goForward()
{
    mWebNavigation->GoForward();
}

void BrowserEmbed::stop()
{
    mWebNavigation->Stop(nsIWebNavigation::STOP_ALL);
}

void BrowserEmbed::reload()
{
    mWebNavigation->Reload(nsIWebNavigation::LOAD_FLAGS_NONE);
}

//*****************************************************************************
// BrowserEmbed::nsISupports
//*****************************************************************************   

NS_IMPL_ADDREF(BrowserEmbed)
NS_IMPL_RELEASE(BrowserEmbed)

NS_INTERFACE_MAP_BEGIN(BrowserEmbed)
   NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIWebBrowserChrome)
   NS_INTERFACE_MAP_ENTRY(nsIWebBrowserChrome)
   NS_INTERFACE_MAP_ENTRY(nsIEmbeddingSiteWindow)
   NS_INTERFACE_MAP_ENTRY(nsIInterfaceRequestor)
   NS_INTERFACE_MAP_ENTRY(nsIWebBrowserChromeFocus)
   NS_INTERFACE_MAP_ENTRY(nsIURIContentListener)
   NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
   NS_INTERFACE_MAP_ENTRY(nsISupportsWeakReference)
   NS_INTERFACE_MAP_ENTRY(nsIContextMenuListener)
NS_INTERFACE_MAP_END


//*****************************************************************************
// BrowserEmbed::nsIWebBrowserChrome
//*****************************************************************************   

NS_IMETHODIMP BrowserEmbed::SetStatus(PRUint32 aType, const PRUnichar* aStatus)
{
	switch (aType) {
	case STATUS_LINK:
	{
		mLinkMessage = aStatus;
		gecko_html_emit_link_message(mOwner);
	}
	break;
	}

    return NS_OK;
}

NS_IMETHODIMP BrowserEmbed::GetWebBrowser(nsIWebBrowser** aWebBrowser)
{
    NS_ENSURE_ARG_POINTER(aWebBrowser);
    *aWebBrowser = mWebBrowser;
    NS_IF_ADDREF(*aWebBrowser);
    return NS_OK;
}

NS_IMETHODIMP BrowserEmbed::SetWebBrowser(nsIWebBrowser* aWebBrowser)
{
    mWebBrowser = aWebBrowser;
    return NS_OK;
}

NS_IMETHODIMP BrowserEmbed::GetChromeFlags(PRUint32* aChromeMask)
{
    *aChromeMask = mChromeFlags;
    return NS_OK;
}

NS_IMETHODIMP BrowserEmbed::SetChromeFlags(PRUint32 aChromeMask)
{
	
    mChromeFlags = aChromeMask;

    return NS_OK;
}

NS_IMETHODIMP BrowserEmbed::DestroyBrowserWindow(void)
{
    return NS_OK;
}


// IN: The desired browser client area dimensions.
NS_IMETHODIMP BrowserEmbed::SizeBrowserTo(PRInt32 aWidth, PRInt32 aHeight)
{
  return NS_OK;
}


NS_IMETHODIMP BrowserEmbed::ShowAsModal(void)
{
  mContinueModalLoop = PR_TRUE;
  //AppCallbacks::RunEventLoop(mContinueModalLoop);

  return NS_OK;
}

NS_IMETHODIMP BrowserEmbed::IsWindowModal(PRBool *_retval)
{
    *_retval = PR_FALSE;
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP BrowserEmbed::ExitModalEventLoop(nsresult aStatus)
{
  mContinueModalLoop = PR_FALSE;
  return NS_OK;
}


//*****************************************************************************
// BrowserEmbed::nsIWebBrowserChromeFocus
//*****************************************************************************   
NS_IMETHODIMP BrowserEmbed::FocusNextElement()
{
    if(mFocusCallback) mFocusCallback(PR_TRUE, mFocusCallbackData);
    return NS_OK;
}

NS_IMETHODIMP BrowserEmbed::FocusPrevElement()
{
    if(mFocusCallback) mFocusCallback(PR_FALSE, mFocusCallbackData);
    return NS_OK;
}


//*****************************************************************************
// BrowserEmbed::nsIEmbeddingSiteWindow
//*****************************************************************************   

NS_IMETHODIMP BrowserEmbed::SetDimensions(PRUint32 aFlags, PRInt32 x, PRInt32 y, PRInt32 cx, PRInt32 cy)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP BrowserEmbed::GetDimensions(PRUint32 aFlags, PRInt32 *x, PRInt32 *y, PRInt32 *cx, PRInt32 *cy)
{
    if (aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_POSITION)
    {
        *x = 0;
        *y = 0;
    }
    if (aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_INNER ||
        aFlags & nsIEmbeddingSiteWindow::DIM_FLAGS_SIZE_OUTER)
    {
        *cx = 0;
        *cy = 0;
    }
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void setFocus (); */
NS_IMETHODIMP BrowserEmbed::SetFocus()
{
    return NS_OK;
}

/* attribute wstring title; */
NS_IMETHODIMP BrowserEmbed::GetTitle(PRUnichar * *aTitle)
{
   NS_ENSURE_ARG_POINTER(aTitle);

   *aTitle = nsnull;
   
   return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP BrowserEmbed::SetTitle(const PRUnichar * aTitle)
{
	gecko_html_emit_title_changed(mOwner);
	return NS_OK;
}

/* attribute boolean visibility; */
NS_IMETHODIMP BrowserEmbed::GetVisibility(PRBool * aVisibility)
{
    NS_ENSURE_ARG_POINTER(aVisibility);
    *aVisibility = PR_TRUE;
    return NS_OK;
}
NS_IMETHODIMP BrowserEmbed::SetVisibility(PRBool aVisibility)
{
    return NS_OK;
}

/* attribute nativeSiteWindow siteWindow */
NS_IMETHODIMP BrowserEmbed::GetSiteWindow(void * *aSiteWindow)
{
   NS_ENSURE_ARG_POINTER(aSiteWindow);

   *aSiteWindow = mWindow;
   return NS_OK;
}

//*****************************************************************************
// BrowserEmbed::nsIURIContentListener
//*****************************************************************************   
/* boolean onStartURIOpen (in nsIURI aURI); */
NS_IMETHODIMP BrowserEmbed::OnStartURIOpen(nsIURI *aURI, PRBool *_retval)
{
	nsresult rv;
	
	nsCAutoString specString;
	rv = aURI->GetSpec(specString);

	if (NS_FAILED(rv))
		return rv;
	
	gecko_html_emit_uri_open(mOwner, specString.get());

	return NS_OK;
}

/* boolean doContent (in string aContentType, in boolean aIsContentPreferred, in nsIRequest aRequest, out nsIStreamListener aContentHandler); */
NS_IMETHODIMP BrowserEmbed::DoContent(const char *aContentType, PRBool aIsContentPreferred, nsIRequest *aRequest, nsIStreamListener **aContentHandler, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isPreferred (in string aContentType, out string aDesiredContentType); */
NS_IMETHODIMP BrowserEmbed::IsPreferred(const char *aContentType, char **aDesiredContentType, PRBool *_retval)
{
    // This method was pretty much copied from GtkMozEmbed
    *_retval = PR_FALSE;
    *aDesiredContentType = nsnull;

    if (aContentType) {
        nsCOMPtr<nsIWebNavigationInfo> webNavInfo(
                do_GetService(NS_WEBNAVIGATION_INFO_CONTRACTID));
        if (webNavInfo) {
            PRUint32 canHandle;
            nsresult rv =
                webNavInfo->IsTypeSupported(nsDependentCString(aContentType),
                        mWebNavigation, &canHandle);
            NS_ENSURE_SUCCESS(rv, rv);
            *_retval = (canHandle != nsIWebNavigationInfo::UNSUPPORTED);
        }
    }
    return NS_OK;
}

/* boolean canHandleContent (in string aContentType, in boolean aIsContentPreferred, out string aDesiredContentType); */
NS_IMETHODIMP BrowserEmbed::CanHandleContent(const char *aContentType, PRBool aIsContentPreferred, char **aDesiredContentType, PRBool *_retval)
{
    *_retval = PR_FALSE;
    return NS_OK;
}

/* attribute nsISupports loadCookie; */
NS_IMETHODIMP BrowserEmbed::GetLoadCookie(nsISupports * *aLoadCookie)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP BrowserEmbed::SetLoadCookie(nsISupports * aLoadCookie)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute nsIURIContentListener parentContentListener; */
NS_IMETHODIMP BrowserEmbed::GetParentContentListener(nsIURIContentListener * *aParentContentListener)
{
    *aParentContentListener = mParentContentListener;
    (*aParentContentListener)->AddRef();
    return NS_OK;
}
NS_IMETHODIMP BrowserEmbed::SetParentContentListener(nsIURIContentListener * aParentContentListener)
{
    mParentContentListener = aParentContentListener;
    return NS_OK;
}

//*****************************************************************************
// BrowserEmbed::nsIWebProgressListener
//*****************************************************************************   

/* void onStateChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long aStateFlags, in nsresult aStatus); */
NS_IMETHODIMP BrowserEmbed::OnStateChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 aStateFlags, nsresult aStatus)
{
    if((aStateFlags & nsIWebProgressListener::STATE_IS_NETWORK) &&
            mNetworkCallback) {
        if(aStateFlags & nsIWebProgressListener::STATE_START) {
            mNetworkCallback(PR_TRUE, mNetworkCallbackData);
        } else if(aStateFlags & nsIWebProgressListener::STATE_STOP) {
            mNetworkCallback(PR_FALSE, mNetworkCallbackData);
        }
    }
    return NS_OK;
}

/* void onProgressChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in long aCurSelfProgress, in long aMaxSelfProgress, in long aCurTotalProgress, in long aMaxTotalProgress); */
NS_IMETHODIMP BrowserEmbed::OnProgressChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRInt32 aCurSelfProgress, PRInt32 aMaxSelfProgress, PRInt32 aCurTotalProgress, PRInt32 aMaxTotalProgress)
{
    return NS_OK;
}

/* void onLocationChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsIURI aLocation); */
NS_IMETHODIMP BrowserEmbed::OnLocationChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsIURI *aLocation)
{
    return NS_OK;
}

/* void onStatusChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsresult aStatus, in wstring aMessage); */
NS_IMETHODIMP BrowserEmbed::OnStatusChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsresult aStatus, const PRUnichar *aMessage)
{
    return NS_OK;
}

/* void onSecurityChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long aState); */
NS_IMETHODIMP BrowserEmbed::OnSecurityChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 aState)
{
    return NS_OK;
}


//*****************************************************************************
// BrowserEmbed::nsIInterfaceRequestor
//*****************************************************************************   

NS_IMETHODIMP BrowserEmbed::GetInterface(const nsIID &aIID, void** aInstancePtr)
{
    NS_ENSURE_ARG_POINTER(aInstancePtr);

    *aInstancePtr = 0;
    if (aIID.Equals(NS_GET_IID(nsIDOMWindow)))
    {
        if (mWebBrowser)
        {
            return mWebBrowser->GetContentDOMWindow((nsIDOMWindow **) aInstancePtr);
        }
        return NS_ERROR_NOT_INITIALIZED;
    }
    return QueryInterface(aIID, aInstancePtr);
}

void addref(BrowserEmbed* browser)
{
    NS_ADDREF(browser);
}
void release(BrowserEmbed* browser)
{
    NS_RELEASE(browser);
}

char *
BrowserEmbed::GetLinkMessage()
{
	char *retval = g_strdup(NS_ConvertUTF16toUTF8(mLinkMessage).get());
	return retval;
}

gint 
BrowserEmbed::ProcessMouseDblClickEvent (void* aEvent)
{
	nsAutoString aType;

	g_return_val_if_fail(aEvent != NULL,0);
	nsCOMPtr<nsISelection> oSelection;
	nsIDOMEvent *domEvent = static_cast<nsIDOMEvent*>(aEvent);
	nsCOMPtr<nsIDOMMouseEvent> event (do_QueryInterface (domEvent));
	if (!event) return 0;
	
#ifdef DEBUG
#ifndef HAVE_GECKO_1_9
	domEvent->GetType(aType);
	gchar mybuf[80];
	aType.ToCString( mybuf, 79);
	g_warning("domEvent->GetType: %s",mybuf);
#endif
#endif
	gtk_editable_delete_text((GtkEditable *)widgets.entry_dict,0,-1);
	
	DoCommand("cmd_copy");
	
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
	
	gtk_clipboard_request_text (clipboard,
				    gui_get_clipboard_text_for_lookup, 
				    NULL);
	return 1;
}

gint
BrowserEmbed::ProcessMouseEvent (void* aEvent)
{	
	g_return_val_if_fail(aEvent != NULL,0);
	
	nsIDOMEvent *domEvent = static_cast<nsIDOMEvent*>(aEvent);
	nsCOMPtr<nsIDOMMouseEvent> event (do_QueryInterface (domEvent));
	if (!event) return 0;
	
	PRUint16 button = 2;
	event->GetButton (&button);
	
	GS_message(("mouse button: %d",button));	
	
	if(button == 1) shift_key_pressed = TRUE; 
	
	/* Mozilla uses 2 as its right mouse button code */
	if (button != 2) return 0;	

	GS_message(("g_signal_emit_by_name"));
	if(mWebBrowser)
		g_signal_emit_by_name (mWebBrowser, "popupmenu_requested", NULL);  //,
			        // NS_ConvertUTF16toUTF8 (href).get());
	return 1;	
}

gint
BrowserEmbed::ProcessMouseUpEvent (void* aEvent)
{
	g_return_val_if_fail(aEvent != NULL, 0);

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
BrowserEmbed::ProcessMouseOver (void* aEvent, int pane,
				gboolean is_dialog, DIALOG_DATA *dialog)
{
  GS_message(("in mouse over"));
#ifdef HAVE_GECKO_1_9
	extern gboolean in_url;
#endif
	PRBool aShiftKey;

	nsIDOMMouseEvent *event = (nsIDOMMouseEvent*) aEvent;
	event->GetShiftKey(&aShiftKey);
	if (aShiftKey)
		return 1;
	if (shift_key_pressed)
		return FALSE;
	if (pane == VIEWER_TYPE)
		return FALSE;
#ifdef HAVE_GECKO_1_9
	if (in_url) {
		in_url = FALSE;
		return FALSE;
	}
#endif
	main_clear_viewer();
	return FALSE;
}

gint
BrowserEmbed::ProcessKeyDownEvent (gpointer dom_event)
{
	nsIDOMKeyEvent *event = (nsIDOMKeyEvent*) dom_event;
	if (!event) return 0;
	nsresult rv;
	PRUint32 keyCode;
	rv = event->GetKeyCode(&keyCode);
	if (NS_FAILED(rv)) return rv;
	return NS_OK;
}

gint
BrowserEmbed::ProcessKeyReleaseEvent (gpointer dom_event)
{
	shift_key_pressed = FALSE;
	return NS_OK;
}

void
BrowserEmbed::AttachListeners (void)
{
// 	if (!mEventTarget || mListenersAttached)
// 		return;
	
// 	nsIDOMEventListener *eventListener = 
// 		static_cast<nsIDOMEventListener *>(static_cast<nsIDOMKeyListener *>(mEventListener));

// 	nsresult rv;
// 	rv = mEventTarget->AddEventListenerByIID(eventListener,
// 						 NS_GET_IID(nsIDOMKeyListener));
// 	if (NS_FAILED(rv)) {
// 		NS_WARNING("Failed to add key listener\n");
// 		return;
// 	}
	
// 	rv = mEventTarget->AddEventListenerByIID(eventListener,
// 						 NS_GET_IID(nsIDOMMouseListener));
// 	if (NS_FAILED(rv)) {
// 		NS_WARNING("Failed to add mouse listener\n");
// 		return;
// 	}

// 	rv = mEventTarget->AddEventListenerByIID(eventListener,
// 						 NS_GET_IID(nsIDOMUIListener));
// 	if (NS_FAILED(rv)) {
// 		NS_WARNING("Failed to add UI listener\n");
// 		return;
// 	}
	
// 	mListenersAttached = PR_TRUE;
}

void
BrowserEmbed::DetachListeners(void)
{
// 	if (!mListenersAttached || !mEventTarget)
// 		return;

// 	nsIDOMEventListener *eventListener =
// 		static_cast<nsIDOMEventListener *>(static_cast<nsIDOMKeyListener *>(mEventListener));
	
// 	nsresult rv;
// 	rv = mEventTarget->RemoveEventListenerByIID(eventListener,
// 						    NS_GET_IID(nsIDOMKeyListener));
// 	if (NS_FAILED(rv)) {
// 		NS_WARNING("Failed to remove key listener\n");
// 		return;
// 	}
	
// 	rv =
// 		mEventTarget->RemoveEventListenerByIID(eventListener,
// 						       NS_GET_IID(nsIDOMMouseListener));
// 	if (NS_FAILED(rv)) {
// 		NS_WARNING("Failed to remove mouse listener\n");
// 		return;
// 	}
	
// 	rv = mEventTarget->RemoveEventListenerByIID(eventListener,
// 						    NS_GET_IID(nsIDOMUIListener));
// 	if (NS_FAILED(rv)) {
// 		NS_WARNING("Failed to remove UI listener\n");
// 		return;
// 	}
	
// 	mListenersAttached = PR_FALSE;
}
	
nsresult
BrowserEmbed::OpenStream(const char *aBaseURI, const char *aContentType)
{
	//mEventTarget = nsnull;
	mListenersAttached = nsnull;
//	ContentStateChange();
  
	nsCOMPtr<nsIWebBrowserStream> wbStream = do_QueryInterface(mWebBrowser);
	if (!wbStream) return NS_ERROR_FAILURE;

	nsCOMPtr<nsIURI> uri;
	nsresult rv = NS_NewURI(getter_AddRefs(uri), aBaseURI);
	if (NS_FAILED(rv))
		return rv;

	rv = wbStream->OpenStream(uri, nsDependentCString(aContentType));
	return rv;
}

nsresult
BrowserEmbed::AppendToStream(const PRUint8 *aData, PRUint32 aLen)
{
  GS_message(("in append stream"));
 

	nsCOMPtr<nsIWebBrowserStream> wbStream = do_QueryInterface(mWebBrowser);
	if (!wbStream) return NS_ERROR_FAILURE;

	GS_message(("end append stream"));

	return wbStream->AppendToStream(aData, aLen);
	
}

nsresult
BrowserEmbed::CloseStream(void)
{
	nsCOMPtr<nsIWebBrowserStream> wbStream = do_QueryInterface(mWebBrowser);
	if (!wbStream) return NS_ERROR_FAILURE;

	nsresult rv =  wbStream->CloseStream();

	GS_message(("end close stream"));

	return rv;
}

void
BrowserEmbed::ContentStateChange(void)
{
	if (mListenersAttached)
		return;
	GS_message(("in content state change"));
	GetListener();
	
	if (!mEventTarget)
		return;

	AttachListeners();
	GS_message(("end content state change"));
}

void
BrowserEmbed::GetListener(void)
{
	GS_message(("get listener begin"));
	if (mEventTarget)
		return;
	
	nsCOMPtr<nsPIDOMWindow> piWin;
	GetPIDOMWindow(getter_AddRefs(piWin));

	if(!piWin)
		return;
	GS_message(("in get listener"));
	mEventTarget = do_QueryInterface(piWin->GetChromeEventHandler());
}

void
BrowserEmbed::SetRTL()
{
	nsresult rv;
	nsCOMPtr<nsIPrefService> prefService = 
		do_GetService (NS_PREFSERVICE_CONTRACTID);
	nsCOMPtr<nsIPrefBranch> pref;
	prefService->GetBranch("", getter_AddRefs(pref));
	rv = pref->SetIntPref("bidi.direction", 2);
}

PRBool
BrowserEmbed::Find (const char *aSearchString)
{
	NS_ENSURE_TRUE (aSearchString, PR_FALSE);

	if (!mInitialised) return PR_FALSE;

	SetSelectionAttention (PR_TRUE);

	nsresult rv;
	PRUint16 found = nsITypeAheadFind::FIND_NOTFOUND;

	rv = mFinder->Find (NS_ConvertUTF8toUTF16 (aSearchString),
			    PR_FALSE,
			    &found);

	return NS_SUCCEEDED (rv) && (found == nsITypeAheadFind::FIND_FOUND 
				     || found == nsITypeAheadFind::FIND_WRAPPED);
}

PRBool
BrowserEmbed::FindAgain (PRBool aForward)
{
	if (!mInitialised) return PR_FALSE;

	SetSelectionAttention (PR_TRUE);
	
	nsresult rv;
	PRUint16 found = nsITypeAheadFind::FIND_NOTFOUND;
#ifdef HAVE_GECKO_1_9
	rv = mFinder->FindAgain (!aForward, PR_FALSE, &found);
#else
	if (aForward) {
		rv = mFinder->FindNext (&found);
	}
	else {
		rv = mFinder->FindPrevious (&found);
	}
#endif

	return NS_SUCCEEDED (rv) && (found == nsITypeAheadFind::FIND_FOUND
				     || found == nsITypeAheadFind::FIND_WRAPPED);
}

void
BrowserEmbed::SetSelectionAttention (PRBool aAttention)
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
#endif
	nsresult rv;
	nsCOMPtr<nsIDocShell> shell (do_GetInterface (mWebBrowser, &rv));

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
#endif
}

void
BrowserEmbed::SetFindProperties (PRBool aCaseSensitive)
{
	if (!mInitialised) return;
	mFinder->SetCaseSensitive (aCaseSensitive);
}

void
BrowserEmbed::JumpToAnchor(const char *anchor)
{
	nsAutoString aAnchorName;
	nsresult rv, result;

	aAnchorName.Assign(NS_ConvertUTF8toUTF16(anchor));

	nsCOMPtr<nsIDocShell> docShell (do_GetInterface (mWebBrowser, &rv));

	if (!docShell) 
		return;
	
	nsCOMPtr<nsIPresShell> presShell;
	result = docShell->GetPresShell(getter_AddRefs(presShell));
	if (!NS_SUCCEEDED(result) || (!presShell))
		return;

	presShell->GoToAnchor(aAnchorName, 1);
}

void
BrowserEmbed::DoCommand (const char *aAcommand)
{
	if (!mInitialised) return;

	nsCOMPtr<nsICommandManager> cmdManager (do_GetInterface (mWebBrowser));
	if (!cmdManager) return;

	cmdManager->DoCommand (aAcommand, nsnull, nsnull);
}

nsresult
BrowserEmbed::GetPIDOMWindow(nsPIDOMWindow **aPIWin)
{
	GS_message(("in pidomwindow"));
	*aPIWin = nsnull;

	nsCOMPtr<nsIDOMWindow> domWindow;
	mWebBrowser->GetContentDOMWindow(getter_AddRefs(domWindow));
	if (!domWindow)
		return NS_ERROR_FAILURE;

	GS_message(("after getcontentdomwindow"));

	nsCOMPtr<nsPIDOMWindow> domWindowPrivate = do_QueryInterface(domWindow);
	GS_message(("before get private root"));
	*aPIWin = domWindowPrivate->GetPrivateRoot();

	if (*aPIWin) {
		NS_ADDREF(*aPIWin);
		return NS_OK;
	}
	
	GS_message(("end pidomwindow"));

	return NS_ERROR_FAILURE;

}

void
BrowserEmbed::ChildFocusIn(void)
{
	nsresult rv;
	nsCOMPtr<nsIWebBrowserFocus> webBrowserFocus(do_QueryInterface(mWebBrowser));
	if (!webBrowserFocus)
		return;

	webBrowserFocus->Activate();
}

void
BrowserEmbed::ChildFocusOut(void)
{
	nsresult rv;
	nsCOMPtr<nsIWebBrowserFocus> webBrowserFocus(do_QueryInterface(mWebBrowser));
	if (!webBrowserFocus)
		return;

	webBrowserFocus->Deactivate();
}

NS_IMETHODIMP BrowserEmbed::OnShowContextMenu(
	PRUint32 aContextFlags,
	nsIDOMEvent *aEvent,
	nsIDOMNode *aNode)
{
	g_signal_emit_by_name (mOwner, "popupmenu_requested", NULL);  //,
	//return NS_OK;
}
