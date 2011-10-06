Name:		xiphos 
Version:	3.1.4
Release:	1
Summary:	Bible study and research tool

Group:		Applications/Text
License:	GPLv2
URL:		http://xiphos.sourceforge.net/
Source0:	http://downloads.sourceforge.net/gnomesword/xiphos-%{version}.tar.gz
Source1:        %name-%version.tar.gz
#Patch0:         path-nspr.patch
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

BuildRequires:	sword-devel >= 1.5.11
BuildRequires:	gettext
BuildRequires:	desktop-file-utils
BuildRequires:	gnome-doc-utils
BuildRequires:	libgsf-devel
BuildRequires:	perl(XML::Parser) intltool
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
AutoReqProv:    on
Requires:	yelp
Obsoletes:	gnomesword < 2.4.2
Provides:	gnomesword = %{version}-%{release}
BuildRequires:    scrollkeeper

%if 0%{?rhel_version} || 0%{?centos_version}
BuildRequires:  libgnomeprintui22-devel
%if 0%{?rhel_version} != 406
BuildRequires:	dbus-glib-devel
%endif
%endif

%if 0%{?fedora} 
BuildRequires:  libgnomeprintui22-devel
BuildRequires:	  rarian-compat
%if 0%{?fedora version} >= 15
BuildRequires:  gtk3-devel
BuildRequires:	gtkhtml3-devel
Requires:  webkitgtk >= 1.3
BuildRequires:  webkitgtk-devel >= 1.3
%else
BuildRequires:  gtk2-devel
BuildRequires:	gtkhtml2-devel
Requires:  webkitgtk >= 1.2
BuildRequires:  webkitgtk-devel >= 1.2
%endif
%endif

# Breaking dependency chain on Buildservice
# It's not actually buildrequires
# You can set "Prefer: package" using
# osc meta prjconf PROJECT instead of these
# lines.
%if 0%{?fedora} == 11
BuildRequires: pulseaudio-esound-compat
%endif
%if 0%{?mandriva_version} == 201000
Prefer: PolicyKit-gnome
%endif
# End break dependencies

%if 0%{?mandriva_version}
BuildRequires:  libdbus-glib-devel
Requires:  webkit
%ifarch x86_64
BuildRequires:	lib64gtkhtml-3.14-devel
BuildRequires:	lib64gtkhtml-3.14-devel
BuildRequires:  lib64webkitgtk1.0-devel
%else
BuildRequires:	libgtkhtml-3.14-devel
BuildRequires:	libgtkhtml-3.14-devel
BuildRequires:  libwebkitgtk1.0-devel
%endif
%endif

# If compiling for SuSE Development version (will be 12.1), this next line should be changed to
# >= 1140 because the development version still returns 1140 for %{?suse_version}
%if 0%{?suse_version} > 1140
BuildRequires:  gtkhtml-devel
BuildRequires:  gtk3-devel
%else
BuildRequires:  gtkhtml2-devel
BuildRequires:  gtk2-devel >= 2.14
%endif

%if 0%{?suse_version} >= 1140
Requires:  libwebkitgtk-1_0-0
BuildRequires:  libwebkitgtk-devel
%else
Requires:  libwebkit-1_0-2
BuildRequires:  libwebkit-devel
%endif


%if 0%{?suse_version}
BuildRequires:  libgnomeprintui-devel
BuildRequires:  update-desktop-files
BuildRequires:  libxslt-devel
Requires:       gnome-doc-utils
BuildRequires:  rarian-devel
BuildRequires:  gnome-doc-utils-devel
BuildRequires:  dbus-1-glib-devel
%endif

%description
Xiphos (formerly known as GnomeSword) is a Bible study tool written for Linux,
UNIX, and Windows under the GNOME toolkit, offering a rich and featureful
environment for reading, study, and research using modules from The SWORD
Project and elsewhere.

%lang_package
%prep
%setup -q
#%patch0 -p1
%build
export CFLAGS=$RPM_OPT_FLAGS
./waf configure %{?jobs:-j%jobs} --prefix=/usr 
./waf build %{?jobs:-j%jobs}

%install
./waf install %{?jobs:-j%jobs} --destdir=$RPM_BUILD_ROOT
rm -rf %buildroot%_datadir/doc/%{name}

%if 0%{?fedora version} >= 13
desktop-file-install --delete-original \
	--add-category=X-Bible		\
	--add-category=X-Religion	\
	--dir=%buildroot%_datadir/applications \
	--copy-name-to-generic-name \
	%buildroot%_datadir/applications/xiphos.desktop
%endif

%if 0%{?suse_version}
%suse_update_desktop_file -n -D %{_datadir}/gnome/help/%{name} -G "Bible Study Tool" xiphos Teaching
gtk-update-icon-cache -q -f -t %{_datadir}/icons/hicolor
%endif
%find_lang %{name}


%clean
rm -rf $RPM_BUILD_ROOT

%post
if which scrollkeeper-update>/dev/null 2>&1; then scrollkeeper-update -q -o %{_datadir}/omf/%{name}; fi

%postun
if which scrollkeeper-update>/dev/null 2>&1; then scrollkeeper-update -q; fi

%files
%defattr (-, root, root)
%doc AUTHORS COPYING ChangeLog NEWS README RELEASE-NOTES TODO Xiphos.ogg
%{_bindir}/*
%{_datadir}/applications/*
%{_datadir}/icons/hicolor/scalable/apps/xiphos.svg
%{_datadir}/xiphos
# you might need to comment the next few lines on some distros
#%{_datadir}/pixmaps/xiphos
#/usr/var/scrollkeeper/*
%{_datadir}/gnome/help/xiphos
%{_datadir}/locale/*/LC_MESSAGES/xiphos.mo
%{_datadir}/omf/xiphos


%changelog
%changelog
* Tue Sep 20 2011 Brian Dumont <brian.j.dumont@gmail.com>
- update to fix for OpenSuSE Development Version (will be 12.1)
* Tue Mar 30 2010 Dmitrijs Ledkovs <dmitrij.ledkov@ubuntu.com>
- reworked to use waf
- works on f11; f12; Mandriva2010; SLE11,
- openSUSE 11.1, 11.2 & Factory.
- Verified using openSUSE Buildservice. Hoping to do daily builds.
* Sun Dec 17 2006 Karl Kleinpaste <karl@charcoal.com>
- reworked a lot of cruft for FC 5 & 6.
* Wed Jan 14 2004 Andy Piper <andyp@users.sf.net>
- cleaned build statements thanks to Hugo (will now build as user)
- changed package name
* Wed Dec 31 2003 Andy Piper <andyp@users.sf.net>
- Better checking for SWORD version
- tidied up macros
* Wed Dec 10 2003 Andy Piper <andyp@users.sf.net>
- Updated dependencies
* Mon Dec 08 2003 Andy Piper <andyp@users.sf.net>
- Updated dependencies
* Fri Dec 05 2003 Andy Piper <andyp@users.sf.net>
- Added to Xiphos CVS
* Thu Nov 27 2003 Hugo van der Kooij <hugo@vanderkooij.org>
- Fixed loose ends in the config
* Tue Nov 25 2003 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 2.0-0.9dev15
- Changed bits to follow new name convention
* Sat Nov 15 2003 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 2.0-0.9dev11
- Build and tested on Fedora Core 1
* Tue Oct 28 2003 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 2.0-0.9dev9
* Sat Oct 25 2003 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 2.0-0.9dev8
* Fri Aug 29 2003  Hugo van der Kooij <hugo@vanderkooij.org>
- Version 1.0-0.9dev5
- Build against Sword 1.5.6rc1
- Build and tested on Red Hat Linux 9
* Mon Mar 25 2003 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 0.7.9
- Build against Sword 1.5.5
- Build and tested on Red Hat Linux 8.0
* Sat Mar 15 2003 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 0.7.8
- Build against Sword 1.5.5
- Build and tested on Red Hat Linux 8.0
* Fri Dec 20 2002 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 0.7.7
- Build against Sword 1.5.5
- Build and tested on Red Hat Linux 8.0
* Sat Dec 14 2002 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 0.7.6
- Build against Sword 1.5.5
- Build and tested on Red Hat Linux 8.0
* Sat Sep 28 2002 Hugo van der Kooij <hugo@vanderkooij.org>
- Build against Sword 1.5.4
* Mon Sep 02 2002 Hugo van der Kooij <hugo@vanderkooij.org>
- Fixed the docs with small patch
* Tue Aug 21 2002 Hugo van der Kooij <hugo@vanderkooij.org>
- Version 0.7.5
- Install in the regular tree (not /usr/local)
- Needs fix on documentation directory (usr/doc should be /usr/share/doc)
* Thu Dec 13 2001 Stuart Gathman <stuart@bmsi.com>
- version 0.5.2c
- move applnk to /usr/share/gnome
* Mon Dec 25 2000 Stuart Gathman <stuart@bmsi.com>
- version 0.3.7a
* Thu Nov 30 2000 Stuart Gathman <stuart@bmsi.com>
- version 0.3.2
* Fri Nov 17 2000 Stuart Gathman <stuart@bmsi.com>
- add applnk entry
* Wed Nov 08 2000 Stuart Gathman <stuart@bmsi.com>
- build against sword-1.5.1a
- SWModule::Delete renamed to SWModule::deleteEntry
* Fri Oct 28 2000 Stuart Gathman <stuart@bmsi.com>
- initial release
