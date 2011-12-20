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

    miscgroup.add_option('--disable-dbus',
                   action = 'store_true',
                   default = False,
                   help = "Disable the Xiphos dbus API [default: enabled]",
                   dest = 'without_dbus')

def configure(ctx):
    ctx.env.common_libs = ['sword', 'gconf-2.0','gmodule-2.0','glib-2.0','libgsf-1','libxml-2.0']

    conf.check_cfg(atleast_pkgconfig_version='0.9.0')

    ctx.load('compiler_c compiler_cxx')
    ctx.load('xiphos_backends', tooldir=_tooldir)
    ctx.load('intltool')

    common_libs.extend(['--cflags','--libs'])

    # strip xiphos binary

    if not opt.without_dbus:
        conf.check_pkg('dbus-glib-1', '0.60', True, var='DBUS')
        # we need a modified version of dbus.py for running on windows
        #conf.check_tool('dbus', tooldir=_tooldir)
    
    conf.check_tool('glib2')

    ### App info, paths
    define = conf.define
    sub = Utils.subst_vars

    env['VERSION'] = VERSION
    env['APPNAME'] = APPNAME
    env['PACKAGE'] = PACKAGE
#    env['HELPDIR'] = sub(opt.helpdir, env)

    define('PACKAGE_VERSION', VERSION)
    define('GETTEXT_PACKAGE', PACKAGE)
    define('PACKAGE', PACKAGE)
    define('PACKAGE_NAME', APPNAME)
    define('PACKAGE_STRING', '%s %s' % (APPNAME, VERSION))
    define('PACKAGE_TARNAME', PACKAGE)

    define('INSTALL_PREFIX', conf.escpath(sub('${PREFIX}/', env)))
    define('PACKAGE_BUGREPORT','http://sourceforge.net/tracker/?group_id=5528&atid=105528' )
    define('PACKAGE_DATA_DIR', conf.escpath(sub('${DATAROOTDIR}/${PACKAGE}', env)))
#    define('PACKAGE_DOC_DIR', conf.escpath(env['DOCDIR']))
    define('PACKAGE_HELP_DIR', conf.escpath(sub('${DATAROOTDIR}/gnome/help/${PACKAGE}', env)))
#    define('PACKAGE_LOCALE_DIR', conf.escpath(env['LOCALEDIR']))
    define('PACKAGE_MENU_DIR', conf.escpath(sub('${DATAROOTDIR}/applications', env)))
    #define('PACKAGE_PIXMAPS_DIR', conf.escpath(sub('${DATAROOTDIR}/pixmaps/${PACKAGE}', env)))
    define('PACKAGE_PIXMAPS_DIR', conf.escpath(sub('${DATAROOTDIR}/${PACKAGE}', env)))
    define('PACKAGE_SOURCE_DIR', conf.escpath(os.path.abspath(top))) # folder where was wscript executed


    conf.check_cfg(msg="Checking for GNOME related libs",
                   package='',
                   args=common_libs,
                   uselib_store='GNOME',
                   mandatory=True)

    env.append_value('ALL_LIBS', 'GNOME')


    # Check for header files
    for h in _headers:
        conf.check(header_name=h)

    # Define to 1 if you have the `strcasestr' function.
    # this function is part of some glibc, string.h
    # could be missing in win32
    conf.check_cc(msg='Checking for function strcasestr', define_name="HAVE_STRCASESTR",
            fragment='int main() {strcasestr("hello","he");}\n')


    conf.write_config_header('config.h')


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
