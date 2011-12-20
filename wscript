#! /usr/bin/env python
# encoding: utf-8

import os, os.path
import string

from waflib import Logs,Utils,Options

from waflib.Tools import (
     c_preproc,
     ccroot,
     intltool,
)

c_preproc.go_absolute=1
c_preproc.strict_quotes=0

import waffles.misc
#import waffles.gnome as gnome

bzrrevno = '$CurRevno$'
bzrrevid = '$CurRevid$'

VERSION='3.1.4+webkit'

if VERSION.endswith('webkit'):
    import commands
    from string import ascii_letters
    VERSION+=commands.getoutput('svnversion')[:4]


APPNAME='xiphos'
PACKAGE='xiphos'
top = '.'
out = 'build'

_tooldir = './waffles/'
ROOTDIR_WIN32 = 'C:\msys'
_headers = '''
locale.h
stdint.h
stdlib.h
string.h
sys/stat.h
sys/types.h
unistd.h
sys/socket.h
winsock.h
'''.split()
def options(ctx):
    ctx.load('compiler_c compiler_cxx')
    ctx.load('xiphos_backends', tooldir=_tooldir)

    miscgroup = ctx.add_option_group('Miscellaneous Options')

    miscgroup.add_option('-d', '--debug-level',
                action = 'store',
		default = 'ultradebug',
		help = "Specify the debugging level ['ultradebug', 'debug', 'release', 'optimized']",
		choices = ['ultradebug', 'debug', 'release', 'optimized'],
		dest = 'debug_level')

    miscgroup.add_option('--strip', action='store_true', default=False,
                    help='Strip resulting binary')

    miscgroup.add_option('--enable-delint', action='store_true', default=False,
            dest='delint',
            help='Use -Wall -Werror [default: disabled]')

    miscgroup.add_option('--disable-dbus',
                   action = 'store_true',
                   default = False,
                   help = "Disable the Xiphos dbus API [default: enabled]",
                   dest = 'without_dbus')

def configure(ctx):
    ctx.load('compiler_c compiler_cxx')
    ctx.load('xiphos_backends', tooldir=_tooldir)
    ctx.load('intltool')
    ctx.load('misc', tooldir=_tooldir)
#    ctx.load('gnome', tooldir=_tooldir)

    # disable console window in win32
    if opt.no_console and env['IS_WIN32']:
        env.append_value('LINKFLAGS', '-mwindows')

    # strip xiphos binary
    if env['IS_CROSS_WIN32']:
        env['STRIP'] = conf.find_program('i686-w64-mingw32-strip', mandatory=True)
    else:
        env['STRIP'] = conf.find_program('strip', mandatory=True)

    if (not env['IS_LINUX']) and (opt.debug_level in ['release','optimized']):
        env['TO_STRIP'] = True
    else:
        env['TO_STRIP'] = opt.strip

    if not opt.without_dbus:
        conf.check_pkg('dbus-glib-1', '0.60', True, var='DBUS')
        # we need a modified version of dbus.py for running on windows
        conf.check_tool('dbus', tooldir=_tooldir)
    
    conf.check_tool('glib2')

    ### App info, paths
    define = conf.define
    sub = Utils.subst_vars

    env['VERSION'] = VERSION
    env['APPNAME'] = APPNAME
    env['PACKAGE'] = PACKAGE
    env['HELPDIR'] = sub(opt.helpdir, env)

    define('PACKAGE_VERSION', VERSION)
    define('GETTEXT_PACKAGE', PACKAGE)
    define('PACKAGE', PACKAGE)
    define('PACKAGE_NAME', APPNAME)
    define('PACKAGE_STRING', '%s %s' % (APPNAME, VERSION))
    define('PACKAGE_TARNAME', PACKAGE)

    define('INSTALL_PREFIX', conf.escpath(sub('${PREFIX}/', env)))
    define('PACKAGE_BUGREPORT','http://sourceforge.net/tracker/?group_id=5528&atid=105528' )
    define('PACKAGE_DATA_DIR', conf.escpath(sub('${DATAROOTDIR}/${PACKAGE}', env)))
    define('PACKAGE_DOC_DIR', conf.escpath(env['DOCDIR']))
    define('PACKAGE_HELP_DIR', conf.escpath(sub('${DATAROOTDIR}/gnome/help/${PACKAGE}', env)))
    define('PACKAGE_LOCALE_DIR', conf.escpath(env['LOCALEDIR']))
    define('PACKAGE_MENU_DIR', conf.escpath(sub('${DATAROOTDIR}/applications', env)))
    #define('PACKAGE_PIXMAPS_DIR', conf.escpath(sub('${DATAROOTDIR}/pixmaps/${PACKAGE}', env)))
    define('PACKAGE_PIXMAPS_DIR', conf.escpath(sub('${DATAROOTDIR}/${PACKAGE}', env)))
    define('PACKAGE_SOURCE_DIR', conf.escpath(os.path.abspath(srcdir))) # folder where was wscript executed

    common_libs = string.join(
    '''
    "gconf-2.0"
    "gmodule-2.0"
    "glib-2.0"
    "libgsf-1 >= 1.14"
    "libxml-2.0"
    --cflags --libs'''
    .split()," ")

    if env['ENABLE_GTK2']:
        env.append_value('ALL_LIBS', 'LIBGLADE')
        # We need to know specific versions of GTK+-2.0
        conf.check_cfg(package="gtk+-2.0",
                       atleast_version = "2.16",
                       uselib_store="GTK_216")
        conf.check_cfg(package="gtk+-2.0",
                       atleast_version = "2.18",
                       uselib_store="GTK_218")
        conf.check_cfg(package="gtk+-2.0",
                       atleast_version = "2.20",
                       uselib_store="GTK_220")
        common_libs += ' libglade-2.0'
        common_libs += ' "gtk+-2.0 >= 2.14" '
        common_libs += ' "libgtkhtml-3.14 >= 3.23" '
        if conf.check_cfg(modversion='gtkhtml-editor-3.14',
                          msg='Checking for GNOME3 gtkhtml-editor',
                          okmsg='Definitely',
                          errmsg='Probably not'
                          ):
            common_libs += ' "gtkhtml-editor-3.14" '
        else:
            common_libs += ' "gtkhtml-editor" '
    else:
        # So far, we only care about GTK+3, not any of its subversions
        common_libs += ' "gtk+-3.0" '
        conf.define('USE_GTK_3', 1)
	conf.define('USE_GTKBUILDER', 1)
        # FC15 and Oneiric have this, Natty does not
        if conf.check_cfg(modversion='libgtkhtml-4.0', msg='Checking for libgtkhtml4', okmsg='ok', errmsg='fail'):
            common_libs += ' "libgtkhtml-4.0" '
        # FC15 and Oneiric have this, Natty does not
        if conf.check_cfg(modversion="gtkhtml-editor-4.0", msg="Checking for GtkHTML Editor 4.0", okmsg='ok', errmsg='fail', mandatory=True):
            common_libs += ' "gtkhtml-editor-4.0" '
    
    conf.check_cfg(atleast_pkgconfig_version='0.9.0')
    conf.check_cfg(msg="Checking for GNOME related libs",
                   package='',
                   args=common_libs,
                   uselib_store='GNOME',
                   mandatory=True)

    env.append_value('ALL_LIBS', 'GNOME')

    conf.check_cfg(package="gtk+-unix-print-2.0",
                   uselib_stor='GTKUPRINT')

    env.append_value('ALL_LIBS', 'GTKUPRINT')

    conf.check_cfg(package='sword',
                   args='"sword >= 1.6.1" --cflags --libs',
                   uselib_store='SWORD',
                   mandatory=True)
    env.append_value('ALL_LIBS', 'SWORD')

    ######################
    ### gtk-webkit for html rendering
    #
    if env['ENABLE_WEBKIT'] and env['USE_GTK_3']:
        conf.check_cfg(package='webkitgtk-3.0',
                        uselib_store='WEBKIT',
                        args='--libs --cflags',
                        msg='Checking for webkit',
                        mandatory=True)
        conf.define('USE_WEBKIT', 1)
        env.append_value('ALL_LIBS', 'WEBKIT')
    elif env['ENABLE_WEBKIT']:
        conf.check_cfg(package='webkit-1.0',
			uselib_store='WEBKIT',
                        args='--libs --cflags',
			msg='Checking for webkit',
                        mandatory=True)
	env.append_value('ALL_LIBS', 'WEBKIT')
        conf.define('USE_WEBKIT', 1)
    ######################
    ### gecko (xulrunner) for html rendering
    # gtkhtml only for editor
    elif not env['ENABLE_GTKHTML']:
        if not env["IS_WIN32"]:

            conf.check_cfg (package='nspr', uselib_store='NSPR')    
            conf.check_cfg (package='',
                            uselib_store='GECKO',
                            args='"libxul-embedding >= 1.9.0" --define-variable=includetype=unstable "nspr" --cflags --libs',
                            msg='checking for libxul-embedding')

            conf.define('GECKO_HOME', conf.check_cfg(package='libxul-embedding',
                                                     args='--variable=sdkdir',
                                                     okmsg=waffles.misc.myokmsg,
                                                     msg="Checking for libxul sdkdir").strip())
            conf.define('GECKO_VER', conf.check_cfg(package='libxul-embedding',
                                                    args='--modversion',
                                                     okmsg=waffles.misc.myokmsg,
                                                    msg="Checking for Gecko GREVersion").strip())
            gecko = env['GECKO_VER']

            def geckoversion(i):
                    if i.startswith('1.'):
                            if len(i) >= 4 and not i[3].isalpha():
                                    return (i[0:5]+'.0', i[0:5]+'.99')
                            else:
                                    return (i[0:4]+'0', i[0:3]+'.0.99')
                    else:
                            return ('2.0b', '2.0.99')
            geckomin, geckomax = geckoversion(env['GECKO_VER'])
            conf.check_message("Gecko", "GREVersionMin", 1, geckomin)
            conf.check_message("Gecko", "GREVersionMax", 1, geckomax)
            conf.define('GECKO_MIN', geckomin)
            conf.define('GECKO_MAX', geckomax)

        else:
                    d = env['MOZILLA_DISTDIR']
                    conf.define['CPPPATH_GECKO'] = ['%s/sdk/include' % d,
                                                '%s/include' % d,
                                                '%s/include/widget' % d,
                                                '%s/include/xpcom' % d,
                                                '%s/include/dom' % d,
                                                '%s/include/content' % d,
                                                '%s/include/layout' % d,
                                                '%s/include/gfx' % d]
                    conf.define['LIBPATH_GECKO'] = ['%s/sdk/lib' % d]
                    conf.define['LIB_GECKO'] = ['xpcomglue_s', 'xpcom', 'xul', 'nspr4']
                    # FIXME: how to detect Gecko-ver on Win similar to pkg-config on unix?
                    conf.define('GECKO_MIN', '1.9.0.0')
                    conf.define('GECKO_MAX', '2.0.0.*')

        env.append_value('ALL_LIBS', 'NSPR')
        env.append_value('ALL_LIBS', 'GECKO')
        conf.define('USE_GTKMOZEMBED', 1)

    # Check for header files
    for h in _headers:
        conf.check(header_name=h)

    # Define to 1 if you have the `strcasestr' function.
    # this function is part of some glibc, string.h
    # could be missing in win32
    conf.check_cc(msg='Checking for function strcasestr', define_name="HAVE_STRCASESTR",
            fragment='int main() {strcasestr("hello","he");}\n')

    # appropriate cflags
    env.append_value('CXXFLAGS', env['CXXFLAGS_%s' % opt.debug_level.upper()])
    env.append_value('CCFLAGS', env['CCFLAGS_%s' % opt.debug_level.upper()])

    if opt.delint:
        env.append_value('CXXFLAGS', env['CXXFLAGS_DELINT'])
        env.append_value('CCFLAGS', env['CCFLAGS_DELINT'])

    # portable adding define, should work gcc and msvc
    env.append_value('CXXFLAGS', env['CXXDEFINES_ST'] % 'HAVE_CONFIG_H')
    env.append_value('CCFLAGS', env['CCDEFINES_ST'] % 'HAVE_CONFIG_H')

    conf.write_config_header('config.h')

    env.append_value('CCFLAGS', env['CCFLAGS_SAFE'])
    env.append_value('CXXFLAGS', env['CXXFLAGS_SAFE'])

def build(bld):
    from waffles.hashing import GetHashofDirs

    bld.env.CXXDEPS_SWORD = GetHashofDirs(bld.env.CPPPATH_SWORD)

    env = bld.env
    opt = Options.options

    # process subfolders
    bld.add_subdirs("""
        src/backend
        src/editor
        src/main
        src/gnome2
        src/xiphos_html
        ui
    """)
    # use GECKO
    if env['ENABLE_WEBKIT']:
	bld.add_subdirs('src/webkit')
    elif not env['ENABLE_GTKHTML']:
        if env["IS_WIN32"]:
            bld.add_subdirs('src/geckowin')
        else:
            bld.add_subdirs('src/gecko')
            
    if env['IS-WIN32']:
      bld.add_subdirs('win32/include')

    if env['HAVE_DBUS']:
        import shutil
        bld.add_subdirs('src/examples')
        # this seems bad, but I don't know how to do it otherwise
        shutil.copy('src/gnome2/ipc-interface.xml', 'src/examples')
        shutil.copy('src/gnome2/marshal.list', 'src/examples')


    bld.install_files('${PACKAGE_DOC_DIR}', """
        README
	RELEASE-NOTES
	COPYING
	AUTHORS
	ChangeLog
	INSTALL
	NEWS
	TODO
	Xiphos.ogg
    """)

    bld.install_files('${PACKAGE_PIXMAPS_DIR}',bld.path.ant_glob('pixmaps/*.png'))
    bld.install_files('${PACKAGE_PIXMAPS_DIR}',bld.path.ant_glob('pixmaps/*.ico'))
    bld.install_files('${PACKAGE_PIXMAPS_DIR}',bld.path.ant_glob('pixmaps/*.xpm'))    
    bld.install_files('${DATAROOTDIR}/icons/hicolor/scalable/apps','pixmaps/xiphos.svg')
    # handle .desktop creation and installation
    if not env["IS_WIN32"]:
        bld.new_task_gen(
                features='subst',
                source='xiphos.desktop.in.in',
                target='xiphos.desktop.in',
                dict={'xiphos_exec': 'xiphos',
                      'PACKAGE_PIXMAPS_DIR': env['PACKAGE_PIXMAPS_DIR'],
                      'INSTALL_PREFIX': env['INSTALL_PREFIX']},
        )
        bld.new_task_gen(
                features='intltool_in',
                source='xiphos.desktop.in',
                target='xiphos.desktop',
                install_path='${PACKAGE_MENU_DIR}',
                podir='po',
                flags=['-d', '-q', '-u', '-c']
        )


    # Type of help format depends on OS.
    # WIN32: chm
    # Other OS: just xml
    # FIXME create and install help doesnt work yet on windows
    if not bld.env['DISABLE_HELP']:
        bld.add_subdirs('help')

    if bld.env['INTLTOOL']:
        bld.add_subdirs('po')

    def post(ctx):
        if bld.env['POST_INSTALL']:
            gnome.postinstall_scrollkeeper('xiphos') # Installing the user docs
            gnome.postinstall_icons() # Updating the icon cache

    bld.add_post_fun(post)

def dist_hook():
    import shutil
    shutil.rmtree('win32')

def run(ctx):
    '''Execute xiphos binary from build directory'''
    import subprocess
    subprocess.call(os.path.join(blddir,'default/src/gnome2/xiphos'))


def test(ctx):
    '''Run unit tests'''
    import subprocess
    subprocess.call(os.path.join(srcdir,'tests/build.py'))

def dist(ctx):
    import Scripting
    Scripting.dist()
    Scripting.g_gz = 'gz'
    Scripting.dist()
