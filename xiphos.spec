Name:		xiphos 
Version:	3.1.3
Release:	1
Summary:	Bible study and research tool

Group:		Applications/Text
License:	GPLv2+
URL:		http://xiphos.sourceforge.net/
Source0:	http://downloads.sourceforge.net/gnomesword/xiphos-%{version}.tar.gz
Source1:        %name-%version.tar.gz
Patch0:         path-nspr.patch
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

BuildRequires:	sword-devel >= 1.5.11
BuildRequires:	gettext
BuildRequires:	desktop-file-utils
BuildRequires:	gnome-doc-utils
BuildRequires:	libglade2-devel 
BuildRequires:	libgsf-devel
BuildRequires:	perl(XML::Parser) intltool
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
AutoReqProv:    on
Requires:	yelp
Requires:       %{name}-lang = %{version}
Obsoletes:	gnomesword < 2.4.2
Provides:	gnomesword = %{version}-%{release}
BuildRequires:    scrollkeeper

%if 0%{?fedora} || 0%{?rhel_version} || 0%{?centos_version}
BuildRequires:  gtk2-devel >= 2.14
BuildRequires:	gtkhtml3-devel
BuildRequires:  libgnomeprintui22-devel
%if 0%{?rhel_version} != 406
BuildRequires:	dbus-glib-devel
%endif
%if 0%{?fedora version} >= 13
BuildRequires:	  xulrunner-devel
BuildRequires:	  rarian-compat
%else
BuildRequires:	xulrunner-devel-unstable
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
%ifarch x86_64
BuildRequires:	lib64gtkhtml-3.14-devel
BuildRequires:	lib64gtkhtml-3.14-devel
%if 0%{?mandriva_version} < 201000
BuildRequires:  lib64xulrunner-unstable-devel
%else
BuildRequires:  lib64xulrunner-devel
%endif
%else
BuildRequires:	libgtkhtml-3.14-devel
BuildRequires:	libgtkhtml-3.14-devel
%if 0%{?mandriva_version} < 201000
BuildRequires:  libxulrunner-unstable-devel
%else
BuildRequires:  libxulrunner-devel
%endif
%endif
%endif





%if 0%{?suse_version} > 1120
%define xulrunner_version 192
%else
%if 0%{?suse_version} > 1110
%define xulrunner_version 191
%else
%if 0%{?suse_version} > 1100
%define xulrunner_version 190
%else
%define xulrunner_version 180
%endif
%endif
%endif

%if 0%{?suse_version}
BuildRequires:  libgnomeprintui-devel
BuildRequires:  gtkhtml2-devel
BuildRequires:  mozilla-xulrunner%{xulrunner_version}-devel
BuildRequires:  gtk2-devel >= 2.14
BuildRequires:  update-desktop-files
BuildRequires:  libxslt-devel
Requires:       gnome-doc-utils
%if 0%{?suse_version} <= 1020
%define _prefix   /opt/gnome
%define _sysconfdir /etc%_prefix
BuildRequires:  scrollkeeper
BuildRequires:  gnome-doc-utils
%else
BuildRequires:  rarian-devel
BuildRequires:  gnome-doc-utils-devel
BuildRequires:  dbus-1-glib-devel
%endif
%endif

%if 0%{?suse_version} <= 1020
%if 0%{?suse_version} > 910
BuildRequires:  mozilla-nspr-devel
%endif
%endif

%if 0%{?centos_version} || 0%{?rhel_version}
BuildRequires:  mozilla-nspr-devel
%endif

%description
Xiphos (formerly known as GnomeSword) is a Bible study tool written for Linux,
UNIX, and Windows under the GNOME toolkit, offering a rich and featureful
environment for reading, study, and research using modules from The SWORD
Project and elsewhere.

%lang_package
%prep
%setup -q
%patch0 -p1
%build
export CFLAGS=$RPM_OPT_FLAGS
./waf configure %{?jobs:-j%jobs} --prefix=/usr 
./waf build %{?jobs:-j%jobs}

%install
./waf install %{?jobs:-j%jobs} --destdir=$RPM_BUILD_ROOT
rm -rf %buildroot%_datadir/doc/%{name}

%if 0%{?suse_version}
%suse_update_desktop_file -n -D %{_datadir}/gnome/help/%{name} -G "Bible Study Tool" xiphos Teaching
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
%{_datadir}/pixmaps/xiphos

%if 0%{?suse_version}
%files lang -f %{name}.lang
%else
%{_datadir}/gnome/help/xiphos
%{_datadir}/locale/*/LC_MESSAGES/xiphos.mo
%{_datadir}/omf/xiphos
%endif

%changelog
%changelog
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
