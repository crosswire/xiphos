/*
 * Miro - an RSS based video player application
 * Copyright (C) 2005-2009 Participatory Culture Foundation
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the OpenSSL
 * library.
 *
 * You must obey the GNU General Public License in all respects for all of
 * the code used other than OpenSSL. If you modify file(s) with this
 * exception, you may extend this exception to your version of the file(s),
 * but you are not obligated to do so. If you do not wish to do so, delete
 * this exception statement from your version. If you delete this exception
 * statement from all source files in the program, then also delete it here.
**/

#include "windows.h"
#include "gtk/gtk.h"
#include "gdk/gdk.h"

#include <stdio.h>

/* FixFocus.cpp
 *
 * Fix focus problems between GTK and XULRunner.  GTK likes to keep it's
 * top-level window focused and handle which widget is focused internally.
 * XULRunner just let's child windows get focus.
 *
 */


/*
 * Override the Window proc for top-level GTK windows that contain an embedded
 * XULRunnerBrowser.  This is needed because GTK's focus model is much
 * different than XULRunner's.
 */
LRESULT CALLBACK ToplevelFocusHackWndProc(HWND hwnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam)
{
    GdkWindow* window = gdk_window_lookup((GdkNativeWindow)hwnd);
    WNDPROC old_window_proc = (WNDPROC)GetProp(hwnd,
            "ToplevelFocusHackOldProc");
    if (!window) {
        if (!old_window_proc) {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        } else {
            return CallWindowProc(old_window_proc, hwnd, uMsg, wParam, lParam);
        }
    }

    switch (uMsg) {
        case WM_MOUSEACTIVATE:
            // Mouse click on a non-browser widget.  Ensure that the top-level
            // window has the keyboard focus.
            // (We know it can't be a browser widget, because we handle
            // WM_MOUSEACTIVATE in BrowserFocusHackWndProc()).
            gdk_window_focus(window, 0);
            return MA_NOACTIVATE;

        case WM_KILLFOCUS:
            // GTK's toplevel window is losing focus to a child window.
            // This is probably a XULRunner window.  Handle the event so that
            // the window doesn't think it's lost focus.
            if (wParam && IsChild(hwnd, (HWND)wParam)) {
                return 0;
            }
            break;

        case WM_DESTROY:
        case WM_NCDESTROY:
            // The Window is about to be destroyed -- Cleanup
            SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)old_window_proc);
            RemoveProp(hwnd, "ToplevelFocusHackOldProc");
            break;
    }
    return CallWindowProc(old_window_proc, hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK BrowserFocusHackWndProc(HWND hwnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam)
{
    HWND parent;
    WNDPROC old_window_proc  = (WNDPROC)GetProp(hwnd,
            "BrowserFocusHackOldProc");
    if (!old_window_proc) {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    switch (uMsg) {
        case WM_MOUSEACTIVATE:
            // The user clicked on a xulrunner browser.  Have the GTK widget
            // grab focus.

            // NOTE: the window that we are handling messages for is the child
            // ofthe actual GTK widget.  See browser.py for info on why this
            // is.
            parent = GetParent(hwnd);
            GdkWindow* window;
            window = gdk_window_lookup((GdkNativeWindow)parent);
            if (window) {
                GtkWidget* browser_widget;
                gdk_window_get_user_data(window, (gpointer*)&browser_widget);
                if (browser_widget) {
                    gtk_widget_grab_focus(browser_widget);
                }
            }
            return MA_ACTIVATE;
            break;
        case WM_NCDESTROY:
            // The Window is about to be destroyed -- Cleanup
            SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)old_window_proc);
            RemoveProp(hwnd, "BrowserFocusHackOldProc");
            break;
    }
    return CallWindowProc(old_window_proc, hwnd, uMsg, wParam, lParam);
}

static void install_toplevel_focus_fix(HWND hwnd)
{
    HWND root_window = GetAncestor(hwnd, GA_ROOT);
    if (GetProp(root_window, "ToplevelFocusHackOldProc") != NULL) {
        /* We already installed the fix, maybe there are 2 browser's
         * embedded? */
        return;
    }
    WNDPROC old_proc = (WNDPROC) SetWindowLongPtr(root_window,
                GWL_WNDPROC, (LONG_PTR)ToplevelFocusHackWndProc);
    SetProp(root_window,"ToplevelFocusHackOldProc", (void *)old_proc);
}


static void install_browser_focus_fix(HWND hwnd)
{

    WNDPROC old_proc = (WNDPROC) SetWindowLongPtr(hwnd,
                GWL_WNDPROC, (LONG_PTR)BrowserFocusHackWndProc);
    SetProp(hwnd, "BrowserFocusHackOldProc", (HANDLE)old_proc);
}

void install_focus_fixes(HWND hwnd)
{
    install_toplevel_focus_fix(hwnd);
    install_browser_focus_fix(hwnd);
}
