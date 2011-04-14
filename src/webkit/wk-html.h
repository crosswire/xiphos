#ifndef __WK_HTML_H__
#define __WK_HTML_H__

#include <webkit/webkit.h>  
#include "main/module_dialogs.h"

#include "../main/module_dialogs.h"

G_BEGIN_DECLS

#define WK_TYPE_HTML         (wk_html_get_type ())
#define WK_HTML(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), WK_TYPE_HTML, WkHtml))
#define WK_HTML_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), WK_TYPE_HTML, WkHtmlClass))
#define WK_HTML_IS_HTML(o)   (G_TYPE_CHECK_INSTANCE_TYPE ((o), WK_TYPE_HTML))
#define WK_HTML_IS_HTML_CLASS(k) (G_TYPE_CHECK_INSTANCE_GET_CLASS ((o), WK_TYPE_HTML, WkHtmlClass))

typedef struct _WkHtml       WkHtml;
typedef struct _WkHtmlClass  WkHtmlClass;
typedef struct _WkHtmlPriv   WkHtmlPriv;

struct _WkHtml {
	WebKitWebView parent;
	WkHtmlPriv *priv;
};
struct _WkHtmlPriv {
	gchar *content;
	gchar *mime;
	gchar *find_string;
	gboolean initialised;
	gchar *base_uri;
	gchar *anchor;
	gboolean frames_enabled;
	guint timeout;
	gint pane;
	gboolean is_dialog;
	DIALOG_DATA * dialog;
};
struct _WkHtmlClass {
	WebKitWebViewClass parent;

        /* Signals */
	void (*uri_selected) (WkHtml         *view,
			      gchar          *uri,
			      gboolean       handled);
	gboolean (*frame_selected) (WkHtml   *view,
				    gchar    *uri,
				    gboolean handled);
	void (*title_changed) (WkHtml        *view,
			       const gchar   *new_title);
	void (*popupmenu_requested) (WkHtml  *view,
				     const gchar *link);
};

GType       wk_html_get_type           (void);
WkHtml *    wk_html_new                (DIALOG_DATA * dialog, gboolean is_dialog, gint pane);
void        wk_html_set_base_uri       (WkHtml      *html,
	                                const gchar *uri);
void        wk_html_open_stream        (WkHtml      *html,
					const gchar *mime);
void        wk_html_write              (WkHtml      *html,
					const gchar *data,
					gint         len);
void        wk_html_printf             (WkHtml      *html,
					gchar *format,
					...) G_GNUC_PRINTF (2,3);
void        wk_html_close              (WkHtml       *html);

void        wk_html_render_data        (WkHtml       *html,
					const char   *data,
					guint32      len);

void        wk_html_frames             (WkHtml       *html,
					gboolean      enable);
gboolean    wk_html_find               (WkHtml       *html,
					const gchar  *find_string);
gboolean    wk_html_find_again         (WkHtml       *html,
					gboolean     forward);
void        wk_html_set_find_props     (WkHtml       *html,
					const char   *str,
					gboolean     match_case,
					gboolean     wrap);
void        wk_html_jump_to_anchor     (WkHtml       *html,
					gchar        *anchor);
void        wk_html_copy_selection     (WkHtml       *html);

void        wk_html_select_all         (WkHtml       *html);

void        wk_html_print              (WkHtml       *html);

gboolean    wk_html_initialize         (void);
void        wk_html_shutdown           (void);

G_END_DECLS

#endif /* __WK_HTML_H__ */
				    
