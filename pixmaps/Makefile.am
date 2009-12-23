pkgicondir=@datadir@/icons/hicolor/scalable/apps
pkgicon_DATA=xiphos.svg

pixmapdir=@datadir@/pixmaps/xiphos
pixmap_DATA= \
	about.png \
	book_closed.png \
	book_closed_rtol.png \
	book_open.png \
	book_open_rtol.png \
	chat_icon-16.png \
	dlg-un-16.png \
	dlg-un.png \
	epiphany-bookmark-page.png \
	epiphany-bookmarks.png \
	epiphany-secure.png \
	epiphany-unsecure.png \
	gnome-session.png \
	gnome-window-manager.png \
	gnome-windows.png \
	gs2-48x48.ico \
	gs2-48x48-old.ico \
	gs2-48x48-old.png \
	gs2-48x48.png \
	gs2.xpm \
	indexed-16.png \
	indexed.png \
	init-settings.png \
	logo.png \
	mailing_list_icon-16.png \
	mozilla-icon.png \
	new_tab_button.png \
	silk-edit-bookmarks.png \
	splash.png \
	sword3.png \
	sword_icon-16.png \
	sword.png \
	translation_icon-16.png \
	tt.png \
	window-close.png \
	xiphos-x-16.png \
	xiphos.xpm

EXTRA_DIST=$(pkgicon_DATA) $(pixmap_DATA)

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache

update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi
