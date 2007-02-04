/*
 *  Copyright (C) 2000-2004 Marco Pesenti Gritti
 *  Copyright (C) 2003-2005 Christian Persch
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
 *  $Id: Yelper.h,v 1.4 2006/09/17 17:24:32 chpe Exp $
 */

#ifndef __YELPER_H__
#define __YELPER_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gtkmozembed.h>
#include <nsCOMPtr.h>
#include <nsIDOMNode.h>

#ifdef USE_GTKUPRINT
#include "gecko/gecko-print.h"
#endif

class nsIDOMWindow;
class nsITypeAheadFind;
class nsIWebBrowser;

class Yelper
{
public:
	Yelper (GtkMozEmbed *aEmbed);
	~Yelper ();

	nsresult Init ();
	void Destroy ();

	void DoCommand (const gchar *aCommand);
        void JumpToAnchor(const gchar *anchor);
	void SetFindProperties (const gchar *aSearchString,
				PRBool aCaseSensitive,
				PRBool aWrap);
	PRBool Find (const gchar *aSearchString);
	PRBool FindAgain (PRBool aForward);
	void SetSelectionAttention (PRBool aSelectionAttention);
        gchar *GetAttribute (nsIDOMNode *node, gchar *attribute);
        gint ProcessMouseOver (void* aEvent, gint pane);
        gint ProcessMouseDblClickEvent (void* aEvent);
	gint ProcessMouseEvent (void *aEvent);
        gint ProcessKeyDownEvent(GtkMozEmbed *embed, gpointer dom_event);
        gint ProcessKeyReleaseEvent(GtkMozEmbed *embed, gpointer dom_event);

#ifdef USE_GTKUPRINT
	nsresult Print (GeckoPrintInfo *print_info, PRBool preview,
			gint *prev_pages);
#endif
	nsresult PrintPreviewNavigate (gint page_no);
	nsresult PrintPreviewEnd ();

private:
	PRPackedBool mInitialised;
	PRPackedBool mSelectionAttention;
	PRPackedBool mHasFocus;
	GtkMozEmbed *mEmbed;
	nsCOMPtr<nsIWebBrowser> mWebBrowser;
	nsCOMPtr<nsIDOMWindow> mDOMWindow;
	nsCOMPtr<nsITypeAheadFind> mFinder;
};

#endif /* !__YELPER_H__ */
