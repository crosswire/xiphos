//copyright xiphos; license gpl2

/*
 * Init.cpp
 *
 * Initialize and Shutdown XPCom
 */

#include <stdlib.h>

#include "mozilla-config.h"
//#define XPCOM_GLUE
#include "nsCOMPtr.h"
#include "nsEmbedString.h"
#include "nsILocalFile.h"
//#include "nsXPCOMGlue.h" 
#include "pref/nsIPref.h"
#include "xulapp/nsXULAppAPI.h"
#include "xpcom/nsServiceManagerUtils.h"
#include "nsXPCOM.h"
#include "nsEmbedAPI.h"

#include "Init.h"

//XRE_InitEmbeddingType XRE_InitEmbedding;
//XRE_TermEmbeddingType XRE_TermEmbedding; 

bool init_xulrunner(const char* xul_dir, const char* app_dir)
{
    nsresult rv;

    char xpcom_dll[_MAX_PATH ];
    if(strlen(xul_dir) >= _MAX_PATH) {
      return false;
    }
    strcpy(xpcom_dll, xul_dir);
    strncat(xpcom_dll, "\\xpcom.dll", _MAX_PATH - strlen(xpcom_dll));

    //rv = XPCOMGlueStartup(xpcom_dll);
    if (NS_FAILED(rv)) {
        printf("GLUE STARTUP FAILED\n");
        return false;
    }

//      const nsDynamicFunctionLoad dynamicSymbols[] = {
//         { "XRE_InitEmbedding", (NSFuncPtr*) &XRE_InitEmbedding },
//         { "XRE_TermEmbedding", (NSFuncPtr*) &XRE_TermEmbedding },
//         { nsnull, nsnull }
//     }; 
   
//     XPCOMGlueLoadXULFunctions(dynamicSymbols);

    nsCOMPtr<nsILocalFile> xul_dir_file;
    rv = NS_NewNativeLocalFile(nsEmbedCString(xul_dir), PR_FALSE,
            getter_AddRefs(xul_dir_file));
    NS_ENSURE_SUCCESS(rv, rv);

    nsCOMPtr<nsILocalFile> app_dir_file;
    rv = NS_NewNativeLocalFile(nsEmbedCString(app_dir), PR_FALSE,
                getter_AddRefs(app_dir_file));
    NS_ENSURE_SUCCESS(rv, rv);

    //rv = NS_InitEmbedding(xul_dir_file, nsnull);
    //nsCOMPtr<nsIComponentRegistrar> registrar;
    //rv = NS_GetComponentRegistrar(getter_AddRefs(registrar));
    //if (NS_FAILURE(rv)) return false;
    //rv = registrar->AutoRegister(nsnull);
    //rv = NS_InitXPCOM3(nsnull, xul_dir_file, nsnull, nsnull, 0);
    //if (rv != NS_OK)
    //  printf("init failed\n");
    rv = XRE_InitEmbedding(xul_dir_file, app_dir_file, 0, 0, 0);
    NS_ENSURE_SUCCESS(rv, rv);
    return true;
    //return NS_OK;
}


/*nsresult setup_user_agent(const char* vendor, const char* vendor_sub, 
        const char* comment)
{
    nsresult rv;
    nsCOMPtr<nsIPref> prefs(do_GetService(NS_PREF_CONTRACTID, &rv));
    NS_ENSURE_SUCCESS(rv, rv);
    rv = prefs->SetCharPref("general.useragent.vendor", vendor);
    NS_ENSURE_SUCCESS(rv, rv);
    rv = prefs->SetCharPref("general.useragent.vendorSub", vendor_sub);
    NS_ENSURE_SUCCESS(rv, rv);
    rv = prefs->SetCharPref("general.useragent.vendorComment", comment);
    NS_ENSURE_SUCCESS(rv, rv);
    return NS_OK;
}
*/

void shutdown_xulrunner()
{
  //XRE_TermEmbedding();
    //XPCOMGlueShutdown();
}
