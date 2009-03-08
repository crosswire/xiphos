#! /usr/bin/env python
# encoding: utf-8

# defines not needed anymore

#  SUSE_10_2
#  USE_GTKHTML38_3_13 

import sys
if sys.version_info < (2,3):
    raise RuntimeError("Python 2.3 or newer is required")

import os, os.path
import intltool, gnome

from os.path import join, dirname, abspath

from waffles.gecko import Gecko
from waffles.gtkhtml import Gtkhtml
from waffles.misc import okmsg

# the following two variables are used by the target "waf dist"
VERSION='3.0.1'
APPNAME='xiphos'


# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = 'build'


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



    ### editor.py
    conf.check_cfg(package='ORBit-2.0', uselib_store='ORBIT',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='libbonobo-2.0', uselib_store='BONOBO',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')
    conf.check_cfg(package='bonobo-activation-2.0', uselib_store='BONOBOACTIV',
            atleast_version='2.0.0', mandatory=True, args='--cflags --libs')
    ### END editor.py



    # TODO: use gtkhtml.py



    ### editor.py
    # IDL for editor

    # ORBIT_IDL
    env['ORBIT_IDL'] = conf.check_cfg(package='ORBit-2.0', okmsg=okmsg,
            msg='Checking for program ORBit idl', args='--variable=orbit_idl').strip()


    # BONOBO_IDL_INCLUDES
    idl1 = conf.check_cfg(package='libbonobo-2.0', okmsg=okmsg,
            msg='Checking for bonobo idl dir', args='--variable=idldir').strip()

    idl2 = conf.check_cfg(package='bonobo-activation-2.0', okmsg=okmsg,
            msg='Checking for bonobo-activation idl dir', args='--variable=idldir').strip()

    env['BONOBO_IDL_INCLUDES'] = '-I%s -I%s' % (idl1, idl2)
    ### END editor.py




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

    ### gtkhtml
    if not Gtkhtml(conf).detect():
        print 'Error: GTKHTML not found'
        exit(1)
     

    ######################
    ### gecko (xulrunner) for html rendering
    # gtkhtml only for editor
    if not opt.gtkhtml and Gecko(conf).detect():
        dfn('USE_GTKMOZEMBED', 1)
        env.append_value('CCFLAGS', env['GECKO_CCFLAGS'])
        env.append_value('CXXFLAGS', env['GECKO_CCFLAGS'])
    # gtkhtml for html rendering and editor
    else:
        print "Using 'GTKHTML instead of Gecko' isn't yet implemented"
        dfn('GTKHTML', 1)

    ######################



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
    dfn('PREFIX', sub('${PREFIX}', env))
    dfn('SYSCONFDIR', sub('${PREFIX}/etc', env))
    dfn('DATADIR', sub('${PREFIX}/share', env))
    dfn('LIBDIR', sub('${PREFIX}/lib', env))
    dfn('SHARE_DIR', sub('${PREFIX}/share/xiphos', env))

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
    conf.env.append_value('CCFLAGS', '-DHAVE_CONFIG_H')
    conf.env.append_value('CXXFLAGS', '-DHAVE_CONFIG_H')

    conf.write_config_header('config.h')

    # process configure for subfolders
    conf.sub_config('src/editor') # generate Editor source from idl


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

    ### editor.py
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
    ### END editor.py

