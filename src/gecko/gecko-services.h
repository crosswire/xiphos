/*
 *  Copyright (C) 2000 Marco Pesenti Gritti
 *  Copyright (C) 2005 Don Scorgie <DonScorgie@Blueyonder.co.uk>
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
 */

#ifndef __GECKO_SERVICES_H
#define __GECKO_SERVICES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef USE_GTKUPRINT
#include <nsIPrintingPromptService.h>
#include <nsIPrintProgressParams.h>
#include <nsIWebProgressListener.h>

#include "gecko/gecko-print.h"

#define G_PRINTINGPROMPTSERVICE_CID \
  { /* dbf438d3-5f62-4978-a700-6fc39447477c */			\
    0xdbf438d3, 0x5f62, 0x4978,					\
      { 0xa7, 0x00, 0x6f, 0xc3, 0x94, 0x47, 0x47, 0x7c } }

#define G_PRINTINGPROMPTSERVICE_CLASSNAME "Geckos Printing Prompt Service"
#define G_PRINTINGPROMPTSERVICE_CONTRACTID "@mozilla.org/embedcomp/printingprompt-service;1"

void gecko_register_printing ();

/* Overwriting PrintPromptService makes the default mozilla print dialog
 * not appear.  We already have our print dialog shown
 */
class GPrintingPromptService : public nsIPrintingPromptService, nsIWebProgressListener, nsIPrintProgressParams
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPRINTINGPROMPTSERVICE
  NS_DECL_NSIWEBPROGRESSLISTENER
  NS_DECL_NSIPRINTPROGRESSPARAMS

  GPrintingPromptService();
  virtual ~GPrintingPromptService();

protected:
  GeckoPrintInfo *mPrintInfo;
};

/* The PrintListener allows us to update the progress bars.
 * Its not much help, but better than nothing
 */
class PrintListener : public nsIWebProgressListener
{
public:
  PrintListener (GeckoPrintInfo *in, nsIWebBrowserPrint *p);
  virtual ~PrintListener ();

  NS_DECL_ISUPPORTS
  NS_DECL_NSIWEBPROGRESSLISTENER
    
    static nsresult SetPrintSettings (GeckoPrintInfo *settings, PRBool preview, 
				      nsIPrintSettings *target);

protected:
  GeckoPrintInfo *info;
  nsIWebBrowserPrint *print;
  gboolean cancel_happened;
  gboolean called_finish;
};

#endif
#endif
