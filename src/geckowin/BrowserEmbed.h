//copyright xiphos; license gpl2

#ifndef __PCF_MIRO_BROWSER_EMBED_H__
#define __PCF_MIRO_BROWSER_EMBED_H__

#include "prtypes.h"
#include "nsCOMPtr.h"
#include "nsIWebBrowserChrome.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIWebBrowser.h"
#include "nsIInterfaceRequestor.h"
#include "nsIWebBrowserChromeFocus.h"
#include "docshell/nsIWebNavigation.h"
#include "widget/nsIBaseWindow.h"
#include "uriloader/nsIURIContentListener.h"
#include "uriloader/nsIWebProgressListener.h"
#include "xpcom/nsWeakReference.h"
#include "shistory/nsIHistoryEntry.h"
#include "shistory/nsISHistory.h"
#include "EmbedEventListener.h"
#include "gecko-html.h"
#include "content/nsPIDOMEventTarget.h"
#include "nsEmbedString.h"
#include "fastfind/nsITypeAheadFind.h"
#include "dom/nsPIDOMWindow.h"

#include "main/module_dialogs.h"

typedef void(*focusCallback)(PRBool forward, void* data);
typedef int(*uriCallback)(char* uri, void* data);
typedef void(*networkCallback)(PRBool is_start, void* data);

class EmbedEventListener;

class BrowserEmbed   : public nsIWebBrowserChrome,
                           public nsIWebBrowserChromeFocus,
                           public nsIEmbeddingSiteWindow,
                           public nsIInterfaceRequestor,
                           public nsIURIContentListener,
                           public nsIWebProgressListener,
                           public nsSupportsWeakReference

{
public:
    BrowserEmbed();
    virtual ~BrowserEmbed();

    /*
     * Methods from the XPCOM interfaces we implement.  These are proiveded
     * for XULRunner.
     */

    NS_DECL_ISUPPORTS
    NS_DECL_NSIWEBBROWSERCHROME
    NS_DECL_NSIEMBEDDINGSITEWINDOW
    NS_DECL_NSIINTERFACEREQUESTOR
    NS_DECL_NSIWEBBROWSERCHROMEFOCUS
    NS_DECL_NSIURICONTENTLISTENER
    NS_DECL_NSIWEBPROGRESSLISTENER

    /*
     * Methods to interact with the MiroBrowserEmbed from Cython.  These are
     * called by Miro.
     */

    // Create a WebBrowser object and place it inside parentWindow.  This must
    // be called before any other methods.  
    nsresult init(unsigned long parentWindow, int x, int y, int width, 
		  int height, GeckoHtml *owner);
    // Stop the browser from painting to the screen or handling input
    nsresult disable();
    // Startup the browser again after a call to disable()
    nsresult enable();
    // Load a URI into the browser
    nsresult loadURI(const char* uri);
    // Gets the current uri from mWebNavigator
    nsresult getCurrentURI(char ** uri);
    // Gets the current title from a long chain of things
    nsresult getCurrentTitle(wchar_t ** aTitle);
    // Call when the parent window changes size
    nsresult resize(int x, int y, int width, int height);
    // Give the browser keyboard focus
    nsresult focus();
    // Browser Navigation buttons.  Their functionality corresponds to the
    // nsIWebNavigation interface
    int canGoBack();
    int canGoForward();
    void goBack();
    void goForward();
    void stop();
    void reload();
    // Set the focus callback.  This will be called when the user tabs through
    // all the elements in the browser and the next Widget should be given
    // focus.
    void SetFocusCallback(focusCallback callback, void* data);
    // Set the URI callback.  This well be called when we are about to load a
    // new URI.  It should return 0 if the URI shouldn't be loaded.
    void SetURICallback(uriCallback callback, void* data);
    // Set the Network callback.  This is called when we start loading a
    // document and when all network activity for a document is finished
    // new URI.  It should return 0 if the URI shouldn't be loaded.
    void SetNetworkCallback(networkCallback callback, void* data);
    // Destroy the broswer
    void destroy();

    char * GetLinkMessage();
    gint ProcessMouseDblClickEvent(void *aEvent);
    gint ProcessMouseEvent(void *aEvent);
    gint ProcessMouseUpEvent(void *aEvent);
    gint ProcessMouseOver(void *aEvent, int pane,
				gboolean is_dialog, DIALOG_DATA *dialog);

    gint ProcessKeyDownEvent(void *aEvent);
    gint ProcessKeyReleaseEvent(void *aEvent);

    void DoCommand (const char *aAcommand);

    nsresult OpenStream (const char *aBaseURI, const char *aContentType);
    nsresult AppendToStream (const PRUint8 *aData, PRUint32 aLen);
    nsresult CloseStream (void);
    void ContentStateChange(void);
    void SetRTL();
    PRBool Find(const char* aSearchString);
    PRBool FindAgain(PRBool aForward);
    void SetSelectionAttention (PRBool aAttention);
    void JumpToAnchor (const char *anchor);
    void GetListener (void);
    void SetFindProperties (PRBool aCaseSensitive);

    void ChildFocusIn(void);
    void ChildFocusOut(void);

    EmbedEventListener *mEventListener;
    nsCOMPtr<nsISupports> mEventListenerGuard;
    PRBool mListenersAttached;
    nsCOMPtr<nsPIDOMEventTarget> mEventTarget;

    nsString mLinkMessage;
    GeckoHtml *mOwner;

protected:
    nativeWindow mWindow;
    PRUint32     mChromeFlags;
    PRBool       mContinueModalLoop;
    focusCallback mFocusCallback;
    uriCallback mURICallback;
    networkCallback mNetworkCallback;
    void* mFocusCallbackData;
    void* mURICallbackData;
    void* mNetworkCallbackData;
    PRPackedBool mInitialised;
    PRPackedBool mSelectionAttention;
    nsCOMPtr<nsITypeAheadFind> mFinder;

    nsCOMPtr<nsIWebBrowser> mWebBrowser;
    nsCOMPtr<nsIWebNavigation> mWebNavigation;
    nsCOMPtr<nsIURIContentListener> mParentContentListener;
    void log(int level, char* string);

 private:
    void AttachListeners (void);
    void DetachListeners (void);
    nsresult GetPIDOMWindow(nsPIDOMWindow **aPIWin);

};

/* Couple of utility functions, since the XPCOM Macros don't seem to work from
 * Cython.
 */
void addref(BrowserEmbed* browser);
void release(BrowserEmbed* browser);

#endif /* __PCF_MIRO_BROWSER_EMBED_H__ */
