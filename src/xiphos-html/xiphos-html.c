/* vi: set sw=4 ts=4 wrap ai expandtab : */
/*
 * xiphos-html.c: This file is part of xiphos.
 *
 * Copyright (C) 2019 Wu Xiaotian <yetist@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * */

#include "xiphos-html.h"
#include "xiphos-html-marshal.h"

#include "main/url.hh"
#include "main/module_dialogs.h"
#include "gui/dictlex.h"

/* TODO: Decoupling code, using signals? */
extern gboolean in_url;

static gchar *x_uri = NULL;
static gboolean db_click = FALSE;

enum {
    URI_SELECTED,
    FRAME_SELECTED,
    TITLE_CHANGED,
    POPUPMENU_REQUESTED,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

typedef struct {
    WebKitWebView  webview;
    gchar         *content;
    gchar         *mime;
    gchar         *find_string;
    gboolean       initialised;
    gchar         *base_uri;
    gchar         *anchor;
    gboolean       frames_enabled;
    guint          timeout;
    gint           pane;
    gboolean       is_dialog;
    DIALOG_DATA   *dialog;
} XiphosHtmlPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (XiphosHtml, xiphos_html, WEBKIT_TYPE_WEB_VIEW);

static gboolean button_release_handler(GtkWidget *widget, GdkEventButton *event)
{
    if (event->type == GDK_BUTTON_RELEASE && db_click) {
        g_print (" button 1 = double click!\n");

        webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW(widget),
                                                 WEBKIT_EDITING_COMMAND_COPY);
        GtkClipboard *clipboard = gtk_widget_get_clipboard(widget, GDK_SELECTION_CLIPBOARD);
        /* TODO: Decoupling code, using signals? */
        gtk_clipboard_request_text(clipboard, gui_get_clipboard_text_for_lookup, NULL);
    }
    return FALSE;
}

static gboolean button_press_handler(GtkWidget *widget, GdkEventButton *event)
{
    XiphosHtmlPrivate *priv;
    priv = xiphos_html_get_instance_private (XIPHOS_HTML(widget));

    if (event->type == GDK_2BUTTON_PRESS && !x_uri) {
        db_click = TRUE;
        return FALSE;
    }

    db_click = FALSE;

    /* this is a left click */
    if (event->button == 1) {
        /* go to the correct url otherwise webkit will do its own thing */
        if (x_uri) {
            if (priv->is_dialog) {
                /* TODO: Decoupling code, using signals? */
                g_debug ("dialog pressed");
                main_dialogs_url_handler(priv->dialog, x_uri, TRUE);
            } else {
                /* TODO: Decoupling code, using signals? */
                g_debug ("main url pressed");
                main_url_handler (x_uri, TRUE);
            }
            return TRUE;
        }
        return FALSE;
    }

    /* this is a right click */
    if (event->button == 3) {
        /* display our own popup menu instead of webkit's one */
        g_signal_emit(widget, signals[POPUPMENU_REQUESTED], 0, priv->dialog, FALSE);
        return TRUE;
    }

    if (event->button == 2) {
        /* ignore middle button */
        return TRUE;
    }

    return FALSE;
}

/* this is the link handler for when a link is clicked */
static void target_changed (GtkWidget *widget,
                            WebKitHitTestResult *hit_test_result,
                            guint modifiers,
                            gpointer user_data)
{
    const char *uri;
    XiphosHtmlPrivate *priv;

    uri = webkit_hit_test_result_get_link_uri (hit_test_result);

    priv = xiphos_html_get_instance_private (XIPHOS_HTML(widget));

    g_debug ("html_link_message: uri = %s", (uri ? uri : "-none-"));

    if (x_uri != NULL) {
        g_free(x_uri);
        x_uri = NULL;
    }

    if (uri) {
        x_uri = g_strdup(uri);
        /* TODO: Decoupling code, using signals? */
        in_url = 1;
    } else {
        /* TODO: Decoupling code, using signals? */
        in_url = 0;
    }

    g_signal_emit(widget, signals[URI_SELECTED], 0, uri, FALSE);

    if (uri) {
        if (priv->is_dialog) {
            g_debug ("call dialog link pressed");
            /* TODO: Decoupling code, using signals? */
            main_dialogs_url_handler(priv->dialog, uri, FALSE);
        } else {
            g_debug ("main url handler");
            /* TODO: Decoupling code, using signals? */
            main_url_handler(uri, FALSE);
        }
    }
}

static void xiphos_html_finalize(GObject *object)
{
    XiphosHtml *html;
    XiphosHtmlPrivate *priv;

    html = XIPHOS_HTML (object);
    priv = xiphos_html_get_instance_private (html);

    if (priv->timeout)
        g_source_remove (priv->timeout);
    if (priv->base_uri != NULL)
        g_free(priv->base_uri);
    if (priv->anchor != NULL)
        g_free(priv->anchor);

    G_OBJECT_CLASS (xiphos_html_parent_class)->finalize(object);
}

static void xiphos_html_dispose (GObject *object)
{
    G_OBJECT_CLASS (xiphos_html_parent_class)->dispose (object);
}

static void xiphos_html_realize (GtkWidget *widget)
{
    GTK_WIDGET_CLASS(xiphos_html_parent_class)->realize(widget);

    g_signal_connect(G_OBJECT(widget), "button-press-event", G_CALLBACK(button_press_handler), NULL);
    g_signal_connect(G_OBJECT(widget), "button-release-event", G_CALLBACK(button_release_handler), NULL);
    g_signal_connect(G_OBJECT(widget), "mouse-target-changed", G_CALLBACK(target_changed), NULL);
}

static void xiphos_html_class_init (XiphosHtmlClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gobject_class->finalize = xiphos_html_finalize;
    gobject_class->dispose = xiphos_html_dispose;

    widget_class->realize = xiphos_html_realize;

    signals[URI_SELECTED] = g_signal_new("uri_selected",
                                         G_TYPE_FROM_CLASS(klass),
                                         G_SIGNAL_RUN_LAST,
                                         G_STRUCT_OFFSET(XiphosHtmlClass, uri_selected),
                                         NULL,
                                         NULL,
                                         _xiphos_html_marshal_VOID__POINTER_BOOLEAN,
                                         G_TYPE_NONE, 2, G_TYPE_POINTER, G_TYPE_BOOLEAN);

    signals[FRAME_SELECTED] = g_signal_new("frame_selected",
                                           G_TYPE_FROM_CLASS(klass),
                                           G_SIGNAL_RUN_LAST,
                                           G_STRUCT_OFFSET(XiphosHtmlClass, frame_selected),
                                           g_signal_accumulator_true_handled, NULL,
                                           _xiphos_html_marshal_BOOLEAN__POINTER_BOOLEAN,
                                           G_TYPE_BOOLEAN,
                                           2, G_TYPE_POINTER, G_TYPE_BOOLEAN);

    signals[POPUPMENU_REQUESTED] = g_signal_new("popupmenu_requested",
                                                G_TYPE_FROM_CLASS(klass),
                                                G_SIGNAL_RUN_LAST,
                                                G_STRUCT_OFFSET(XiphosHtmlClass, popupmenu_requested),
                                                NULL,
                                                NULL,
                                                g_cclosure_marshal_VOID__POINTER,
                                                G_TYPE_NONE, 1, G_TYPE_POINTER);
}

static void xiphos_html_init (XiphosHtml *html)
{
    XiphosHtmlPrivate *priv;

    priv = xiphos_html_get_instance_private (html);

    priv->base_uri = NULL;
    priv->anchor = NULL;
    priv->timeout = 0;
    priv->content = NULL;
    priv->mime = NULL;
    priv->initialised = FALSE;
}

GtkWidget* xiphos_html_new (DIALOG_DATA *dialog, gboolean is_dialog, gint pane)
{
    XiphosHtml *html;
    XiphosHtmlPrivate *priv;

    html = g_object_new (XIPHOS_TYPE_HTML, NULL);

    priv = xiphos_html_get_instance_private (html);
    priv->pane = pane;
    priv->is_dialog = is_dialog;
    priv->dialog = dialog;

    return GTK_WIDGET (html);
}

void xiphos_html_set_base_uri (XiphosHtml *html, const gchar *uri)
{
    XiphosHtmlPrivate *priv;

    g_return_if_fail (XIPHOS_IS_HTML(html));
    priv = xiphos_html_get_instance_private (html);

    if (priv->base_uri != NULL) {
        g_free(priv->base_uri);
    }

    if (uri != NULL) {
        priv->base_uri = g_strdup (uri);
    } else {
        priv->base_uri = g_strdup ("file://");
    }
}

void xiphos_html_open_stream (XiphosHtml *html, const gchar *mime)
{
    XiphosHtmlPrivate *priv;

    g_return_if_fail (XIPHOS_IS_HTML (html));

    priv = xiphos_html_get_instance_private (html);

    xiphos_html_set_base_uri (html, NULL);

    priv->frames_enabled = FALSE;
    if (priv->content != NULL) {
        g_free(priv->content);
        priv->content = NULL;
    }

    if (priv->mime != NULL)
        g_free(priv->mime);
    priv->mime = g_strdup (mime);
}

void xiphos_html_write (XiphosHtml *html, const gchar *data)
{
    XiphosHtmlPrivate *priv;
    gchar *tmp = NULL;

    g_return_if_fail(XIPHOS_IS_HTML(html));

    priv = xiphos_html_get_instance_private (html);

    if (priv->content != NULL) {
        tmp = g_strjoin(NULL, priv->content, data, NULL);
        g_free(priv->content);
        priv->content = tmp;
    } else {
        priv->content = g_strdup(data);
    }
}

void xiphos_html_printf (XiphosHtml *html, gchar *format, ...)
{
    va_list args;
    gchar *string;

    g_return_if_fail(format != NULL);

    va_start(args, format);
    string = g_strdup_vprintf(format, args);
    va_end(args);

    xiphos_html_write(html, string);

    g_free(string);
}

void xiphos_html_close (XiphosHtml *html)
{
    XiphosHtmlPrivate *priv;
    GBytes *html_bytes;

    priv = xiphos_html_get_instance_private (html);
    if (!priv->initialised) {
        priv->initialised = TRUE;
    }

    html_bytes = g_bytes_new(priv->content, strlen(priv->content));

    webkit_web_view_load_bytes(WEBKIT_WEB_VIEW(html),
                               html_bytes,
                               priv->mime,
                               NULL,
                               priv->base_uri);
    g_bytes_unref (html_bytes);

    if (priv->content != NULL) {
        g_free(priv->content);
        priv->content = NULL;
    }

    if (priv->mime != NULL) {
        g_free(priv->mime);
        priv->mime = NULL;
    }
}

void xiphos_html_render_data (XiphosHtml *html, const char *data)
{
    g_debug ("in render_data, data is %s", data);
    xiphos_html_open_stream(html, "text/html");
    xiphos_html_write(html, data);
    xiphos_html_close(html);
}

void xiphos_html_frames (XiphosHtml *html, gboolean enable)
{
    XiphosHtmlPrivate *priv;
    priv = xiphos_html_get_instance_private (html);

    priv->frames_enabled = enable;
}

gboolean xiphos_html_find (XiphosHtml *html, const gchar *find_string)
{
    XiphosHtmlPrivate *priv;
    priv = xiphos_html_get_instance_private (html);

    if (priv->find_string)
        g_free(priv->find_string);
    priv->find_string = g_strdup(find_string);

    WebKitFindController *find_controller;

    find_controller = webkit_web_view_get_find_controller(WEBKIT_WEB_VIEW(html));
    webkit_find_controller_search(find_controller,
                                  find_string,
                                  WEBKIT_FIND_OPTIONS_CASE_INSENSITIVE,
                                  G_MAXUINT);
    return TRUE;
}

gboolean xiphos_html_find_again (XiphosHtml *html, gboolean forward)
{
    XiphosHtmlPrivate *priv;
    WebKitFindController *find_controller;

    priv = xiphos_html_get_instance_private (html);

    find_controller = webkit_web_view_get_find_controller(WEBKIT_WEB_VIEW(html));
    webkit_find_controller_search(find_controller,
                                  priv->find_string,
                                  WEBKIT_FIND_OPTIONS_NONE,
                                  G_MAXUINT);
    return TRUE;
}

void xiphos_html_jump_to_anchor (XiphosHtml *html, gchar *anchor)
{
    XiphosHtmlPrivate *priv;

    g_return_if_fail(html != NULL);

    priv = xiphos_html_get_instance_private (html);

    g_free(priv->anchor);
    priv->anchor = g_strdup(anchor);
}

void xiphos_html_copy_selection (XiphosHtml *html)
{
    webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW(html),
                                             WEBKIT_EDITING_COMMAND_COPY);
}

void xiphos_html_select_all (XiphosHtml *html)
{
    webkit_web_view_execute_editing_command (WEBKIT_WEB_VIEW(html),
                                             WEBKIT_EDITING_COMMAND_SELECT_ALL);
}

void xiphos_html_print (XiphosHtml *html)
{
    WebKitPrintOperation *print_operation;
    print_operation = webkit_print_operation_new (WEBKIT_WEB_VIEW(html));
    webkit_print_operation_run_dialog (print_operation, NULL);
}
