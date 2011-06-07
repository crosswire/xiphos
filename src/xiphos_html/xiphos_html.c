#include "xiphos_html.h"

XiphosHtml *xiphos_html_new(DIALOG_DATA * dialog, gboolean is_dialog, gint pane)
{
	XiphosHtml *html;
	html = XIPHOS_HTML(g_object_new(XIPHOS_TYPE_HTML, NULL));
	XiphosHtmlPriv *priv = XIPHOS_HTML_GET_PRIVATE(html);

#if !defined(USE_WEBKIT) && defined(USE_GTKMOZEMBED)
	/* gecko, which has weird "structure privacy" needs */
	gecko_html_set_parms(html, priv, dialog, is_dialog, pane);
#else
	/* webkit or gtkhtml */
	priv->pane = pane;
	priv->is_dialog = is_dialog;
	priv->dialog = dialog;
	

# ifdef USE_GTKMOZEMBED
	//make scrollbar appear on the left for RTL locales
	GtkTextDirection dir = gtk_widget_get_direction(GTK_WIDGET (html));
	if (dir == GTK_TEXT_DIR_RTL)
	    priv->yelper->SetRTL();
# endif /* USE_GTKMOZEMBED */
#endif /* USE_WEBKIT || !USE_GTKMOZEMBED */
	return html;
}
