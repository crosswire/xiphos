/*
 *  Copyright (C) 2002 Philip Langdale
 *  Copyright (C) 2003-2004 Christian Persch
 *  Copyright (C) 2005 Juerg Billeter
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

#include <mozilla-config.h>
#include "config.h"

#ifdef USE_GTKUPRINT
#include <stdlib.h>
#include <unistd.h>

#include <nsStringAPI.h>

#include <nsCOMPtr.h>
#include <nsIComponentManager.h>
#include <nsIComponentRegistrar.h>
#include <nsIGenericFactory.h>
#include <nsILocalFile.h>
#include <nsIPrintSettings.h>
#include <nsServiceManagerUtils.h>
#include <nsXPCOM.h>

#include "gecko/gecko-services.h"
#include "gecko/gecko-print.h"

#include "main/sword.h"

/* Implementation file */
NS_IMPL_ISUPPORTS3(GPrintingPromptService, nsIPrintingPromptService, nsIWebProgressListener, nsIPrintProgressParams)

  GPrintingPromptService::GPrintingPromptService()
{
  mPrintInfo = NULL;
}

GPrintingPromptService::~GPrintingPromptService()
{
  if (mPrintInfo != NULL)
    {
      gecko_print_info_free (mPrintInfo);
    }
}

/* void showPrintDialog (in nsIDOMWindow parent, in nsIWebBrowserPrint webBrowserPrint, in nsIPrintSettings printSettings); */
NS_IMETHODIMP GPrintingPromptService::ShowPrintDialog(nsIDOMWindow *parent, nsIWebBrowserPrint *webBrowserPrint, nsIPrintSettings *printSettings)
{
  return NS_OK;
  
}

/* void showProgress (in nsIDOMWindow parent, in nsIWebBrowserPrint webBrowserPrint, in nsIPrintSettings printSettings, in nsIObserver openDialogObserver, in boolean isForPrinting, out nsIWebProgressListener webProgressListener, out nsIPrintProgressParams printProgressParams, out boolean notifyOnOpen); */
NS_IMETHODIMP GPrintingPromptService::ShowProgress(nsIDOMWindow *parent, nsIWebBrowserPrint *webBrowserPrint, nsIPrintSettings *printSettings, nsIObserver *openDialogObserver, PRBool isForPrinting, nsIWebProgressListener **webProgressListener, nsIPrintProgressParams **printProgressParams, PRBool *notifyOnOpen)
{
  return NS_OK;
}

/* void showPageSetup (in nsIDOMWindow parent, in nsIPrintSettings printSettings, in nsIObserver printObserver); */
NS_IMETHODIMP GPrintingPromptService::ShowPageSetup(nsIDOMWindow *parent, nsIPrintSettings *printSettings, 
						    nsIObserver *printObserver)
{
  return NS_OK;
}

/* void showPrinterProperties (in nsIDOMWindow parent, in wstring printerName, in nsIPrintSettings printSettings); */
NS_IMETHODIMP GPrintingPromptService::ShowPrinterProperties(nsIDOMWindow *parent, const PRUnichar *printerName, nsIPrintSettings *printSettings)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}


/* void onStateChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long aStateFlags, in nsresult aStatus); */
NS_IMETHODIMP GPrintingPromptService::OnStateChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 aStateFlags, nsresult aStatus)
{
  return NS_OK;
}

/* void onProgressChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in long aCurSelfProgress, in long aMaxSelfProgress, in long aCurTotalProgress, in long aMaxTotalProgress); */
NS_IMETHODIMP GPrintingPromptService::OnProgressChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRInt32 aCurSelfProgress, PRInt32 aMaxSelfProgress, PRInt32 aCurTotalProgress, PRInt32 aMaxTotalProgress)
{
  return NS_OK;
}

/* void onLocationChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsIURI location); */
NS_IMETHODIMP GPrintingPromptService::OnLocationChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsIURI *location)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onStatusChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsresult aStatus, in wstring aMessage); */
NS_IMETHODIMP GPrintingPromptService::OnStatusChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsresult aStatus, const PRUnichar *aMessage)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onSecurityChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long state); */
NS_IMETHODIMP GPrintingPromptService::OnSecurityChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 state)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring docTitle; */
NS_IMETHODIMP GPrintingPromptService::GetDocTitle(PRUnichar * *aDocTitle)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP GPrintingPromptService::SetDocTitle(const PRUnichar * aDocTitle)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute wstring docURL; */
NS_IMETHODIMP GPrintingPromptService::GetDocURL(PRUnichar * *aDocURL)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP GPrintingPromptService::SetDocURL(const PRUnichar * aDocURL)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMPL_ISUPPORTS1(PrintListener, nsIWebProgressListener)

  PrintListener::PrintListener (GeckoPrintInfo *in, nsIWebBrowserPrint *p)
{
  info = in;
  print = p;
  cancel_happened = FALSE;
  /*NULL*/

}

PrintListener::~PrintListener ()
{
  /*NULL*/
}

/* void onStateChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long aStateFlags, in nsresult aStatus); */
NS_IMETHODIMP PrintListener::OnStateChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 aStateFlags, nsresult aStatus)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onProgressChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in long aCurSelfProgress, in long aMaxSelfProgress, in long aCurTotalProgress, in long aMaxTotalProgress); */
NS_IMETHODIMP PrintListener::OnProgressChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRInt32 aCurSelfProgress, PRInt32 aMaxSelfProgress, PRInt32 aCurTotalProgress, PRInt32 aMaxTotalProgress)
{
  /*gs_print_update_progress (info, 
			      (1.0 * aCurTotalProgress) / (aMaxTotalProgress * 1.0));*/

  if (info->cancelled && !cancel_happened) {
    /* This doesn't seem to actually cancel anything.
     * therefore, the best course of action is to ignore it
     * until we've finished printing to the file
     * and then free it - Mozilla bug #253926
     */
    print->Cancel();
    cancel_happened = TRUE;
  }
  if (aCurTotalProgress == 100 && aMaxTotalProgress == 100) /* 100% finished */
    gecko_print_moz_finished (info);
  return NS_OK;
}

/* void onLocationChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsIURI location); */
NS_IMETHODIMP PrintListener::OnLocationChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsIURI *location)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onStatusChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in nsresult aStatus, in wstring aMessage); */
NS_IMETHODIMP PrintListener::OnStatusChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, nsresult aStatus, const PRUnichar *aMessage)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onSecurityChange (in nsIWebProgress aWebProgress, in nsIRequest aRequest, in unsigned long state); */
NS_IMETHODIMP PrintListener::OnSecurityChange(nsIWebProgress *aWebProgress, nsIRequest *aRequest, PRUint32 state)
{
  return NS_ERROR_NOT_IMPLEMENTED;
}

/* static functions */

/* static */nsresult
PrintListener::SetPrintSettings (GeckoPrintInfo *settings,
				 PRBool preview,
				 nsIPrintSettings * target)
{
  nsString tmp;

  /* This is a bastard mix of old Yelp stuff, old epiphany stuff
   * and new epiphany stuff.  It does work though.
   */
  /* Initialisation */
  target->SetIsInitializedFromPrinter (PR_FALSE);
  target->SetIsInitializedFromPrefs (PR_FALSE);
  target->SetPrintSilent (PR_FALSE);
  target->SetShowPrintProgress (PR_TRUE);
  target->SetNumCopies (1);

  /* We always print PS to a file and then hand that off to gtk-print */
  target->SetPrinterName (NS_LITERAL_STRING ("PostScript/default").get());
  target->SetPrintToFile (PR_FALSE);

  /* This is the time between printing each page, in ms.
   * It 'gives the user more time to press cancel' !
   * We don't want any of this nonsense, so set this to a low value,
   * just enough to update the print dialogue.
   */
  target->SetPrintPageDelay (50);

  if (!preview) {
    gchar *base;
    const gchar *temp_dir;
    gint fd;
    GtkPageSet pageSet;
    GtkPrintPages printPages;

    target->SetPrintToFile (PR_TRUE);

    temp_dir = g_get_tmp_dir ();
    base = g_build_filename (temp_dir, "printXXXXXX", NULL);
    fd = g_mkstemp (base);
    close(fd);
    settings->tempfile = g_strdup (base);
    
    g_free (base);
    
    
    NS_CStringToUTF16 (nsDependentCString(settings->tempfile),
		       NS_CSTRING_ENCODING_UTF8, tmp);
    target->SetPrintToFile (PR_TRUE);
    target->SetToFileName (tmp.get());

    pageSet = gtk_print_settings_get_page_set (settings->config);
    target->SetPrintOptions (nsIPrintSettings::kPrintEvenPages,
			     pageSet != GTK_PAGE_SET_ODD);
    target->SetPrintOptions (nsIPrintSettings::kPrintEvenPages,
			     pageSet != GTK_PAGE_SET_EVEN);

    target->SetPrintReversed (gtk_print_settings_get_reverse (settings->config));

    printPages = gtk_print_settings_get_print_pages (settings->config);
    switch (printPages) {
    case GTK_PRINT_PAGES_RANGES: {
      int numRanges = 0;
      GtkPageRange *pageRanges = gtk_print_settings_get_page_ranges (settings->config, &numRanges);
      if (numRanges > 0) {
	/* FIXME: We can only support one range, 
	 * For now, ignore more ranges */
	target->SetPrintRange (nsIPrintSettings::kRangeSpecifiedPageRange);
	target->SetStartPageRange (pageRanges[0].start+1);
	target->SetEndPageRange (pageRanges[0].end+1);

	g_free (pageRanges);
      }
      break;
    }
    case GTK_PRINT_PAGES_CURRENT:
      /* not supported, fall through */
    case GTK_PRINT_PAGES_ALL:
      target->SetPrintRange (nsIPrintSettings::kRangeAllPages);
      break;
      /* FIXME: we need some custom ranges here, "Selection" and 
       * "Focused Frame" */
    }
  } else {
    target->SetPrintOptions (nsIPrintSettings::kPrintEvenPages, PR_TRUE);
    target->SetPrintOptions (nsIPrintSettings::kPrintEvenPages, PR_TRUE);
    target->SetPrintReversed (PR_FALSE);
    target->SetPrintRange (nsIPrintSettings::kRangeAllPages);
  }

  switch (gtk_print_settings_get_orientation (settings->config)) {
  case GTK_PAGE_ORIENTATION_PORTRAIT:
  case GTK_PAGE_ORIENTATION_REVERSE_PORTRAIT: /* not supported */
    target->SetOrientation (nsIPrintSettings::kPortraitOrientation);
    break;
  case GTK_PAGE_ORIENTATION_LANDSCAPE:
  case GTK_PAGE_ORIENTATION_REVERSE_LANDSCAPE: /* not supported */
    target->SetOrientation (nsIPrintSettings::kLandscapeOrientation);
    break;
  }

  target->SetPrintInColor (gtk_print_settings_get_use_color (settings->config));

  target->SetPaperSizeUnit(nsIPrintSettings::kPaperSizeMillimeters);

#ifndef HAVE_GECKO_1_9	
  target->SetPaperSize (nsIPrintSettings::kPaperSizeDefined);
#endif
	
  GtkPaperSize *paperSize = gtk_page_setup_get_paper_size (settings->setup);
  if (!paperSize) {
    GS_warning(("Paper size not set.  Aborting!\n"));
    return NS_ERROR_FAILURE;
  }

  target->SetPaperSizeType (nsIPrintSettings::kPaperSizeDefined);
  target->SetPaperWidth (gtk_paper_size_get_width (paperSize, GTK_UNIT_MM));
  target->SetPaperHeight (gtk_paper_size_get_height (paperSize, GTK_UNIT_MM));

#ifdef HAVE_GECKO_1_9
  target->SetPaperName (NS_ConvertUTF8toUTF16 (gtk_paper_size_get_name (paperSize)).get ());
#else
  {
    /* Mozilla bug https://bugzilla.mozilla.org/show_bug.cgi?id=307404
     * means that we cannot actually use any paper sizes except mozilla's
     * builtin list, and we must refer to them *by name*!
     */
    static const struct {
      const char gtkPaperName[13];
      const char mozPaperName[10];
    } paperTable [] = {
      { GTK_PAPER_NAME_A5, "A5" },
      { GTK_PAPER_NAME_A4, "A4" },
      { GTK_PAPER_NAME_A3, "A3" },
      { GTK_PAPER_NAME_LETTER, "Letter" },
      { GTK_PAPER_NAME_LEGAL, "Legal" },
      { GTK_PAPER_NAME_EXECUTIVE, "Executive" },
    };
  
    const char *paperName = gtk_paper_size_get_name (paperSize);
  
    PRUint32 i;
    for (i = 0; i < G_N_ELEMENTS (paperTable); i++) {
      if (g_ascii_strcasecmp (paperTable[i].gtkPaperName, paperName) == 0) {
	paperName = paperTable[i].mozPaperName;
	break;
      }
    }
    if (i == G_N_ELEMENTS (paperTable)) {
      /* Not in table, fall back to A4 */
      GS_warning(("Unknown paper name '%s', falling back to A4", 
		  gtk_paper_size_get_name (paperSize)));
      paperName = paperTable[1].mozPaperName;
    }
  
    target->SetPaperName (NS_ConvertUTF8toUTF16 (paperName).get ());
  }
#endif /* !HAVE_GECKO_1_9 */
 
  /* Sucky mozilla wants margins in inch! */
  target->SetMarginTop (gtk_page_setup_get_top_margin (settings->setup, GTK_UNIT_INCH));
  target->SetMarginBottom (gtk_page_setup_get_bottom_margin (settings->setup, GTK_UNIT_INCH));
  target->SetMarginLeft (gtk_page_setup_get_left_margin (settings->setup, GTK_UNIT_INCH));
  target->SetMarginRight (gtk_page_setup_get_right_margin (settings->setup, GTK_UNIT_INCH));

  
  NS_CStringToUTF16 (nsDependentCString(settings->header_left_string),
		     NS_CSTRING_ENCODING_UTF8, tmp);
  target->SetHeaderStrLeft (tmp.get());
    
  NS_CStringToUTF16 (nsDependentCString(settings->header_center_string),
		     NS_CSTRING_ENCODING_UTF8, tmp);
  target->SetHeaderStrCenter (tmp.get());
    
  NS_CStringToUTF16 (nsDependentCString(settings->header_right_string),
		     NS_CSTRING_ENCODING_UTF8, tmp);
  target->SetHeaderStrRight (tmp.get());
    
  NS_CStringToUTF16 (nsDependentCString(settings->footer_left_string),
		     NS_CSTRING_ENCODING_UTF8, tmp); 
  target->SetFooterStrLeft (tmp.get());
    
  NS_CStringToUTF16 (nsDependentCString(settings->footer_center_string),
		     NS_CSTRING_ENCODING_UTF8, tmp);
  target->SetFooterStrCenter(tmp.get());
    
  NS_CStringToUTF16 (nsDependentCString(settings->footer_right_string),
		     NS_CSTRING_ENCODING_UTF8, tmp);
  target->SetFooterStrRight(tmp.get());

  /* FIXME I think this is the right default, but this prevents the user
   * from cancelling the print immediately, see the stupid comment in 
   * nsPrintEngine:
   *  "DO NOT allow the print job to be cancelled if it is Print FrameAsIs
   *   because it is only printing one page."
   * We work around this by just not sending the job to the printer then.
   */
  target->SetPrintFrameType(nsIPrintSettings::kFramesAsIs); /* FIXME setting */
  target->SetPrintFrameTypeUsage (nsIPrintSettings::kUseSettingWhenPossible);

  target->SetScaling (gtk_print_settings_get_scale (settings->config) / 100.0);

  /* FIXME: What do these do?  Need to learn to fix them properly
   * For now, leave at Epiphany type defaults 
   */

  target->SetShrinkToFit (PR_FALSE); /* FIXME setting */
    
  target->SetPrintBGColors (PR_FALSE); /* FIXME setting */
  target->SetPrintBGImages (PR_FALSE); /* FIXME setting */

  /* target->SetPlexName (LITERAL ("default")); */
  /* target->SetColorspace (LITERAL ("default")); */
  /* target->SetResolutionName (LITERAL ("default")); */
  /* target->SetDownloadFonts (PR_TRUE); */

  return NS_OK;

};

/* component registration */

NS_GENERIC_FACTORY_CONSTRUCTOR(GPrintingPromptService)

static const nsModuleComponentInfo sAppComps[] = {
    {
        G_PRINTINGPROMPTSERVICE_CLASSNAME,
        G_PRINTINGPROMPTSERVICE_CID,
        G_PRINTINGPROMPTSERVICE_CONTRACTID,
        GPrintingPromptServiceConstructor
    },
};

void
gecko_register_printing ()
{
  nsresult rv;
  nsCOMPtr<nsIComponentRegistrar> cr;
  rv = NS_GetComponentRegistrar(getter_AddRefs(cr));
  NS_ENSURE_SUCCESS (rv, );

  nsCOMPtr<nsIComponentManager> cm;
  rv = NS_GetComponentManager (getter_AddRefs (cm));
  NS_ENSURE_SUCCESS (rv, );

  nsCOMPtr<nsIGenericFactory> componentFactory;
  rv = NS_NewGenericFactory(getter_AddRefs(componentFactory),
			    &(sAppComps[0]));
    
  if (NS_FAILED(rv) || !componentFactory)
    {
      GS_warning(("Failed to make a factory for %s\n", sAppComps[0].mDescription));
      return;
    }
   
  rv = cr->RegisterFactory(sAppComps[0].mCID,
			   sAppComps[0].mDescription,
			   sAppComps[0].mContractID,
			   componentFactory);
  if (NS_FAILED(rv))
    {
      GS_warning(("Failed to register %s\n", sAppComps[0].mDescription));
    }
    
}
#endif
