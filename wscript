#! /usr/bin/env python
# encoding: utf-8

import sys
if sys.version_info < (2,3):
    raise RuntimeError("Python 2.3 or newer is required")

import os, os.path
import intltool, gnome

from os.path import join, dirname, abspath

# the following two variables are used by the target "waf dist"
VERSION='3.0.1'
APPNAME='xiphos'


# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = 'build'

gtkhtml_api = '3.14'
gecko_name = 'xulrunner'
gecko_home = '/usr/lib/xulrunner'
gecko_subdirs = '''
        commandhandler
	content
	docshell
	browser
	dom
	fastfind
	find
	gtkembedmoz
	gfx
	layout
	necko
	nspr
	pref
	string
	uriloader
	webbrwsr
	webshell
	widget
	xpcom
    '''.split()


def set_options(opt):

    opt.add_option('--enable-debug', action='store_true', default=False,
            dest='debug', help='Build debug version [Default: False]')

    opt.add_option('--enable-paratab', action='store_true', default=True,
            dest='paratab', help='Use paratab [Default: True]')

    opt.add_option('--enable-autoclear', action='store_true', default=False,
            dest='autoclear', help='Use previewer autoclear [Default: False]')

    opt.add_option('--enable-old_navbar', action='store_true', default=False,
            dest='old_navbar', help='Use old_navbar [Default: False]')

    opt.add_option('--enable-gtkhtml', action='store_true', default=False,
            dest='gtkhtml',
            help='Use gtkhtml instead of gtkmozembed [Default: False]')

    opt.add_option('--enable-opensuse102', action='store_true', default=False,
            dest='opensuse102', help='Use opensuse102 [Default: False]')

    opt.add_option('--enable-maintainer-mode', action='store_true',
            default=False, dest='maintainer_mode',
            help='''Enable make rules and dependencies not useful (and sometimes confusing) to the casual installer''')

    opt.tool_options('g++')
    opt.tool_options('gcc')


def configure(conf):

    ## detect OS

    import Utils
    platfm = Utils.detect_platform()
    print 'Platform: %s' % platfm

    if platfm == 'linux':
        pass
    elif platfm == 'win32':
        pass
    elif platfm == 'cygwin':
        pass
    elif platfm == 'darwin': # Mac OSX
        pass
    elif platfm == 'posix': # probably BSD will be in this category
        pass
    else:
        print 'UNKNOWN or UNSUPPORTED platform'
        pass # use default values

    ## cmd line options

    import Options
    opt = Options.options
    dfn = conf.define
    env = conf.env

    if opt.debug:
        dfn('DEBUG', 1)
    if opt.paratab:
        dfn('USE_PARALLEL_TAB', 1)
    if opt.autoclear:
        dfn('USE_PREVIEWER_AUTOCLEAR', 1)
    if opt.old_navbar:
        dfn('OLD_NAVBAR ', 1)
    if opt.gtkhtml:
        dfn('GTKHTML', 1)
    if opt.opensuse102:
        dfn('SUSE_10_2', 1)
    if opt.maintainer_mode:
        dfn('MAINTAINER_MODE', 1)

    # checks of waf tools (modules)
    conf.check_tool('gcc g++ gnome intltool glib2')

    ## CXX flags (compiler arguments)
    conf.check_cxx(cxxflags='-ftemplate-depth-25')
    conf.env.append_value('CCFLAGS', '-g -O2')
    conf.env.append_value('CXXFLAGS', '-g -O2 -ftemplate-depth-128')

    # pkg-config
    conf.check_cfg(atleast_pkgconfig_version='0.9.0')

    ## GTK+/Gnome libs

    # GTK+ x11 implementation
    if conf.check_cfg(package='gtk+-x11-2.0', uselib_store='GTK',
            atleast_version='2.0.0', args='--cflags --libs'):
        dfn('HAVE_LIBGTK_X11_2_0', 1)
    # GTK+ win32 implementation
    elif conf.check_cfg(package='gtk+-win32-2.0', uselib_store='GTK',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs'):
        dfn('HAVE_LIBGTK_WIN32_2_0', 1)

    conf.check_cfg(package='glib-2.0', uselib_store='GLIB',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='libgnomeui-2.0', uselib_store='GNOMEUI',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')

    conf.check_cfg(package='libgnomeprintui-2.2', uselib_store='GNOMEPRINTUI',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='libgnomeprint-2.2', uselib_store='GNOMEPRINT',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='ORBit-2.0', uselib_store='ORBIT',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')

    conf.check_cfg(package='libbonobo-2.0', uselib_store='BONOBO',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='bonobo-activation-2.0', uselib_store='BONOBOACTIV',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')


    # TODO: more precise detection
    # GTKHTML API 3.8
    if conf.check_cfg(package='libgtkhtml-3.8', uselib_store='GTKHTML',
            atleast_version='3.8.0', args='--cflags --libs'):
        # Define if you want to use GtkHtml-3.8 */
        dfn('USE_GTKHTML38', 1)

    # GTKHTML API 3.14
    if conf.check_cfg(package='libgtkhtml-3.14', uselib_store='GTKHTML',
            atleast_version='3.14.0', args='--cflags --libs'):
        # Define if you want to use GtkHtml-3.8 */
        dfn('USE_GTKHTML38', 1)
        # Define if you want to use GtkHtml-3.14 */
        dfn('USE_GTKHTML3_14', 1)


    def okmsg(kw):
        # some customization for displaying pkg-config values on the line with ok msg
        # to be able use this function there is need for file ./wafadmin/Tools/config_c.py
        # from svn in r5753
        return 'ok %s' % kw['success'].strip()


    # GTKHML api version
    # values returned by pkg-config may contain '\n' at the end
    env['GTKHTML_API'] = conf.check_cfg(package='libgtkhtml-%s' % gtkhtml_api, okmsg=okmsg,
            msg='Checking for gtkhtml api version', args='--variable=gtkhtml_apiversion').strip()
    dfn('GTKHTML_API_VERSION', env['GTKHTML_API'])

    # GTKHTML data directory
    env['GTKHTML_DATA'] = conf.check_cfg(package='libgtkhtml-%s' % gtkhtml_api, okmsg=okmsg,
            msg='Checking for gtkhtml datadir', args='--variable=gtkhtml_datadir').strip()
    dfn('GTKHTML_DATA_DIR', env['GTKHTML_DATA'])


    # TODO
    # Define if you want to use GtkHtml-3.8-3.13 */
    #dfn('USE_GTKHTML38_3_13', 1)

    # TODO
    # Define if you want to use GtkHtml-3.14 */
    #dfn('USE_GTKHTML3_14_23', 1)


    ### IDL for editor



    # ORBIT_IDL
    env['ORBIT_IDL'] = conf.check_cfg(package='ORBit-2.0', okmsg=okmsg,
            msg='Checking for program ORBit idl', args='--variable=orbit_idl').strip()


    # BONOBO_IDL_INCLUDES
    idl1 = conf.check_cfg(package='libbonobo-2.0', okmsg=okmsg,
            msg='Checking for bonobo idl dir', args='--variable=idldir').strip()

    idl2 = conf.check_cfg(package='bonobo-activation-2.0', okmsg=okmsg,
            msg='Checking for bonobo-activation idl dir', args='--variable=idldir').strip()

    env['BONOBO_IDL_INCLUDES'] = '-I%s -I%s' % (idl1, idl2)


    # Other
    conf.check_cfg(package='libxml-2.0', uselib_store='XML',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')

    if conf.check_cfg(package='gtk+-unix-print-2.0', uselib_store='UPRINT',
            atleast_version='2.0.0', mandatory=False, args='--cflags --libs'):
        dfn('USE_GTKUPRINT', 1)

    conf.check_cfg(package='ImageMagick++', uselib_store='MAGICK',
            atleast_version='6.0.0', mandatory=True, args='--cflags --libs',
            errmsg='either no ImageMagick++ or ImageMagick++ not recent enough')

    # Sword
    conf.check_cfg(package='sword', uselib_store='SWORD',
            atleast_version='1.5.11', mandatory=True, args='--cflags --libs',
            errmsg='error: either no sword or sword not recent enough')

    # TODO: implement GTKHTML
    # TODO: implement gecko detecting (rewrite gecko.m4 in python)

    ## Gecko

    if conf.check_cfg(package='%s-gtkmozembed' % gecko_name, uselib_store='GTKMOZEMBED',
            atleast_version='1.7', args='--cflags --libs'):
        dfn('HAVE_GECKO_1_7', 1)

    if conf.check_cfg(package='%s-gtkmozembed' % gecko_name, atleast_version='1.8'):
        dfn('HAVE_GECKO_1_8', 1)
    if conf.check_cfg(package='%s-gtkmozembed' % gecko_name, atleast_version='1.8.1'):
        dfn('HAVE_GECKO_1_8_1', 1)
    if conf.check_cfg(package='%s-gtkmozembed' % gecko_name, atleast_version='1.9'):
        dfn('HAVE_GECKO_1_9', 1)

    # other parts of gecko
    conf.check_cfg(package='%s-nss' % gecko_name, uselib_store='GECKONSS',
            args='--cflags --libs')
    conf.check_cfg(package='%s-nspr' % gecko_name, uselib_store='GECKONSPR',
            args='--cflags --libs')

    # define is needed
    dfn('GECKO_HOME', gecko_home)
    
    # put gecko subdirs in string with absolute paths
    gecko_include = ''
    for i in gecko_subdirs:
        gecko_include += join(gecko_home, 'include', i) + ' '
    conf.env['GECKO_INCLUDE'] = gecko_include



    #print 'GECKO headers begin'
    #conf.check(header_name='mozilla-config.h')
    #print 'GECKO headers end'






    # TODO: maybe the following checks should be in a more generic module.

    #always defined to indicate that i18n is enabled */
    dfn('ENABLE_NLS', 1)

    # TODO
    #Define to 1 if you have the `bind_textdomain_codeset' function.
    dfn('HAVE_BIND_TEXTDOMAIN_CODESET', 1)

    # TODO
    #Define to 1 if you have the `dcgettext' function.
    dfn('HAVE_DCGETTEXT', 1)

    #Define to 1 if you have the <dlfcn.h> header file.
    conf.check(header_name='dlfcn.h', define_name='HAVE_DLFCN_H')

    # TODO
    #Define if the GNU gettext() function is already present or preinstalled.
    dfn('HAVE_GETTEXT', 1)

    #Define to 1 if you have the <inttypes.h> header file.
    conf.check(header_name='inttypes.h', define_name='HAVE_INTTYPES_H')

    # TODO FIXME
    #Define if your <locale.h> file defines LC_MESSAGES.
    dfn('HAVE_LC_MESSAGES', 1)

    #Define to 1 if you have the <locale.h> header file.
    # included in gnome checks
    #conf.check(header_name='locale.h', define_name='HAVE_LOCALE_H')

    #Define to 1 if you have the <memory.h> header file.
    conf.check(header_name='memory.h', define_name='HAVE_MEMORY_H')

    #Define to 1 if you have the <stdint.h> header file.
    conf.check(header_name='stdint.h', define_name='HAVE_STDINT_H')

    #Define to 1 if you have the <stdlib.h> header file.
    conf.check(header_name='stdlib.h', define_name='HAVE_STDLIB_H')

    #Define to 1 if you have the <strings.h> header file.
    conf.check(header_name='strings.h', define_name='HAVE_STRINGS_H')

    #Define to 1 if you have the <string.h> header file.
    conf.check(header_name='string.h', define_name='HAVE_STRING_H')

    #Define to 1 if you have the <sys/stat.h> header file.
    conf.check(header_name='sys/stat.h', define_name='HAVE_SYS_STAT_H')

    #Define to 1 if you have the <sys/types.h> header file.
    conf.check(header_name='sys/types.h', define_name='HAVE_SYS_TYPES_H')

    #Define to 1 if you have the <unistd.h> header file.
    conf.check(header_name='unistd.h', define_name='HAVE_UNISTD_H')

    # TODO: check these defines
    # Define to 1 if you have the `strcasestr' function. */
    dfn('HAVE_STRCASESTR', 1)
    # Define to 1 if you have the <sys/select.h> header file. */
    dfn('HAVE_SYS_SELECT_H', 1)
    # Define to 1 if you have the <sys/socket.h> header file. */
    dfn('HAVE_SYS_SOCKET_H', 1)

    # TODO: What's the purpose of STDC? Is xiphos able compile without that?
    # Define to 1 if you have the ANSI C header files. */
    # NOT necessary?
    dfn('STDC_HEADERS', 1)
    # Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
    dfn('TIME_WITH_SYS_TIME', 1)


    dfn('VERSION', VERSION)
    dfn('PACKAGE_VERSION', VERSION)
    dfn('GETTEXT_PACKAGE', APPNAME)
    dfn('PACKAGE', APPNAME)
    dfn('PACKAGE_NAME', APPNAME)
    dfn('PACKAGE_STRING', '%s %s' % (APPNAME, VERSION))
    dfn('PACKAGE_TARNAME', APPNAME)


    ## HARDCODED DEFINES
    import Utils
    sub = Utils.subst_vars
    env = conf.env

    dfn('INSTALL_PREFIX', sub('${PREFIX}/', env))
    #dfn('LT_OBJDIR', '.libs') - what's the purpose?
    dfn('PACKAGE_BUGREPORT','http://sourceforge.net/tracker/?group_id=5528&atid=105528' )
    dfn('PACKAGE_DATA_DIR', sub('${PREFIX}/share/xiphos', env))
    dfn('PACKAGE_DOC_DIR', sub('${PREFIX}/share/doc/xiphos', env))
    dfn('PACKAGE_HELP_DIR', sub('${PREFIX}/share/gnome/help/xiphos', env))
    dfn('PACKAGE_LOCALE_DIR', sub('${PREFIX}/share/locale', env))
    dfn('PACKAGE_MENU_DIR', sub('${PREFIX}/share/applications', env))
    dfn('PACKAGE_PIXMAPS_DIR', sub('${PREFIX}/share/pixmaps/xiphos', env))
    dfn('PACKAGE_SOURCE_DIR', abspath('.')) # foder where was wscript executed

    # some folders for final executable
    dfn('GECKO_HOME', gecko_home)
    dfn('PREFIX', sub('${PREFIX}', env))
    dfn('SYSCONFDIR', sub('${PREFIX}/etc', env))
    dfn('DATADIR', sub('${PREFIX}/share', env))
    dfn('LIBDIR', sub('${PREFIX}/lib', env))
    dfn('SHARE_DIR', sub('${PREFIX}/share/xiphos', env))

    # other HARDCODED
    # Define if mozilla is of the toolkit flavour */
    #dfn('HAVE_MOZILLA_TOOLKIT', 1)

    # Define if you want to use gtkmozembed */
    dfn('USE_GTKMOZEMBED', 1)

    # TODO: how to detect these values? is it possible to detect them?

    # TODO: not necessary SELECT* defines?
    # Define to the type of arg 1 for `select'. */
    #dfn('SELECT_TYPE_ARG1', 'int', quote=0) # dont add quotes around 'int'
    # Define to the type of args 2, 3 and 4 for `select'. */
    #dfn('SELECT_TYPE_ARG234', '(fd_set *)', quote=0)
    # Define to the type of arg 5 for `select'. */
    #dfn('SELECT_TYPE_ARG5', '(struct timeval *)', quote=0)


    #dfn('__cplusplus', 1)

    # let compiler know that we have 'config.h'
    #conf.env.append_value('CFLAGS', '-DHAVE_CONFIG_H')
    conf.env.append_value('CCFLAGS', '-DHAVE_CONFIG_H')
    conf.env.append_value('CXXFLAGS', '-DHAVE_CONFIG_H')
    #conf.env.append_value('CFLAGS', '-DHAVE_CONFIG_H')
    #conf.env.append_value('CCFLAGS', '-D__cplusplus')

    conf.write_config_header('config.h')


def build(bld):
    # process subfolders from here
    bld.add_subdirs('''
        src/backend
        src/gecko
        src/editor
        src/main
        src/gnome2
    ''')

    #mkenums marshal pixmaps')

    #env = bld.env
    #if env['XML2PO'] and env['XSLTPROC2PO']:
            #bld.add_subdirs('help')

    #if env['INTLTOOL']:
            #bld.add_subdirs('po data')

    #if env['HAVE_GTKSHARP'] and env['MCS']:
            #bld.add_subdirs('sharp')

    #if env['SGML2MAN']:
    #	bld.add_subdirs('man')


def shutdown():
    # Postinstall tasks:
    #gnome.postinstall_scrollkeeper('gnome-hello') # Installing the user docs
    #gnome.postinstall_schemas('gnome-test') # Installing GConf schemas
    #gnome.postinstall_icons() # Updating the icon cache
    pass

def distclean():

    lst = ('''
        src/editor/Editor-common.c
        src/editor/Editor.h
        src/editor/Editor-skels.c
        src/editor/Editor-stubs.c
    ''').split()

    print 'Removing files generated for editor:'
    for i in lst:
        if os.path.exists(i):
            print i
            os.remove(i)

