#include "xiphos_html.h"

XiphosHtml *xiphos_html_new(DIALOG_DATA * dialog, gboolean is_dialog, gint pane)
{
	XiphosHtml *html;
	html = XIPHOS_HTML(g_object_new(XIPHOS_TYPE_HTML, NULL));
	XiphosHtmlPriv *priv = XIPHOS_HTML_GET_PRIVATE(html);
	priv->pane = pane;
	priv->is_dialog = is_dialog;
	priv->dialog = dialog;

#ifdef USE_GTKMOZEMBED
	//make scrollbar appear on the left for RTL locales
	GtkTextDirection dir = gtk_widget_get_direction(GTK_WIDGET (html));
	if (dir == GTK_TEXT_DIR_RTL)
	    priv->yelper->SetRTL();
#endif
	return html;
}
