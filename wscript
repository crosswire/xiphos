#! /usr/bin/env python
# encoding: utf-8

import os, os.path
import intltool
import string
import Utils
import Options
import ccroot
import preproc
preproc.go_absolute=1
preproc.strict_quotes=0

import waffles.misc
import waffles.gnome as gnome

VERSION='4.0.7'

APPNAME='xiphos'
PACKAGE='xiphos'
srcdir = '.'
blddir = 'build'

_tooldir = './waffles/'
ROOTDIR_WIN32 = 'C:\msys'
_headers = '''
dlfcn.h
inttypes.h
locale.h
memory.h
stdint.h
stdlib.h
strings.h
string.h
sys/stat.h
sys/types.h
unistd.h
sys/select.h
sys/socket.h
winsock.h
'''.split()
def set_options(opt):

    opt.tool_options('g++ gcc')

    maingroup = opt.add_option_group('Main Xiphos Options')

    maingroup.add_option('--gtk',
                   action='store', default='auto', dest='gtkver',
                   choices=['auto', '3', '2'],
                   help="Select gtk+ API ['auto', '3', '2']")

    miscgroup = opt.add_option_group('Miscellaneous Options')

    miscgroup.add_option('-d', '--debug-level',
                action = 'store',
		default = ccroot.DEBUG_LEVELS.ULTRADEBUG,
		help = "Specify the debugging level ['ultradebug', 'debug', 'release', 'optimized']",
		choices = ['ultradebug', 'debug', 'release', 'optimized'],
		dest = 'debug_level')

    miscgroup.add_option('--strip', action='store_true', default=False,
                    help='Strip resulting binary')

    miscgroup.add_option('--enable-delint', action='store_true', default=False,
            dest='delint',
            help='Use -Wall -Werror [default: disabled]')

    miscgroup.add_option('--chatty', action='store_true', default=False,
            dest='chatty',
            help='Enable lots of tracing [default: disabled]')

    miscgroup.add_option('--enable-webkit-editor', action='store_true', default=False,
            dest='webkit_editor',
            help='Use webkit editor [default: disabled]')

    miscgroup.add_option('--enable-webkit2', action='store_true', default=False,
            dest='webkit2',
            help='Use webkit2 instead of original webkit [default: disabled]')

    miscgroup.add_option('--disable-dbus',
                   action = 'store_true',
                   default = False,
                   help = "Disable the Xiphos dbus API [default: enabled]",
                   dest = 'without_dbus')

    ### cross compilation from Linux/Unix for win32

    w32 = opt.add_option_group ('Windows and Cross-compile Options', '')

    w32.add_option('--target-platform-win32', action='store_true', default=False,
            dest='cross_win32',
            help='Cross-compile for win32 [default: disabled]')

    w32.add_option('--disable-console',
            action='store_true',
            default=False,
            help='Disable console window in win32 [default: enabled]',
            dest='no_console')

    w32.add_option('--pkgconf-libdir',
		action = 'store',
		default = os.path.join(ROOTDIR_WIN32, 'local', 'lib', 'pkgconfig'),
		help = "Specify dir with *.pc files for cross-compilation",
		dest = 'pkg_conf_libdir')

    w32.add_option('--pkgconf-prefix',
		action = 'store',
		default = os.path.join(ROOTDIR_WIN32, 'local'),
		help = "Specify prefix with folders for headers and libraries for cross-compilation",
		dest = 'pkg_conf_prefix')

    ######### end of windows options.

    group = opt.add_option_group ('Localization and documentation')
    group.add_option('--helpdir',
                     action = 'store',
                     default = '${DATAROOTDIR}/gnome/help/${PACKAGE}',
                     help = "user documentation [default: ${DATAROOTDIR}/gnome/help/${PACKAGE}]",
                     dest = 'helpdir')
    group.add_option('--disable-help',
                     action='store_true',
                    default=False,
                     help='Disable creating help files',
                     dest='disable_help')
    group.add_option('--no-post-install',
                   action = 'store_false',
                   default = True,
                   help = "Disable post-install tasks",
                   dest = 'post_install')

def configure(conf):

    opt = Options.options
    dfn = conf.define
    env = conf.env

    platform = Utils.detect_platform()
    env['IS_WIN32'] = platform == 'win32'
    env['IS_LINUX'] = platform == 'linux'

    env['IS_CROSS_WIN32'] = Options.options.cross_win32

    # cross-compilation for Windows
    # behave mostly like on windows platform
    if env['IS_CROSS_WIN32']:
        env['IS_WIN32'] = True
        env['IS_LINUX'] = False
        Utils.pprint('CYAN', "Cross-compilation")
        env['CROSS'] = os.environ['CROSS']

    # IS_WIN32 means compiling for win32, not necessarily compiling on win32
    if env['IS_WIN32']:
        Utils.pprint('CYAN', "Windows detected")
    elif env['IS_LINUX']:
        Utils.pprint('CYAN', "Linux detected")

    if not (env['IS_LINUX'] or env['IS_WIN32']):
        Utils.pprint('RED', "Assuming %s is UNIX like platform" % platform)
        env['IS_LINUX'] = True

    if env['IS_WIN32']:
        env['ROOTDIR'] = ROOTDIR_WIN32

    conf.check_tool('nothreads', tooldir=_tooldir)
    conf.check_tool('g++ gcc')
    # cross compiler
    if env['IS_CROSS_WIN32']:
        conf.check_tool('cross_linux_win32', tooldir=_tooldir)
  
    conf.check_tool('gnu_dirs misc')
    conf.check_tool('intltool')

    env['DISABLE_HELP'] = opt.disable_help

    if not opt.disable_help:
        if env['IS_LINUX']:
            conf.check_tool('gnome', tooldir=_tooldir)
        else:
            conf.check_tool('documentation', tooldir=_tooldir) # stuff to create help files

    # DATADIR is defined by intltool in config.h - conflict in win32 (mingw)
    conf.undefine('DATADIR')

    env['POST_INSTALL']=opt.post_install

    # delint flags
    env['CXXFLAGS_DELINT'] = ['-Werror', '-Wall']
    env['CCFLAGS_DELINT'] = ['-Werror', '-Wall', '-Wmissing-prototypes',
                             '-Wnested-externs', '-Wpointer-arith', '-Wno-sign-compare']

    # gcc compiler debug levels
    # msvc has levels predefined
    if env['CC_NAME'] == 'gcc':
        env['CCFLAGS_SAFE']       = env['CCFLAGS']
        env['CCFLAGS']            = []
        env['CCFLAGS_OPTIMIZED']  = ['-O2']
        env['CCFLAGS_RELEASE']    = ['-O2']
        env['CCFLAGS_DEBUG']      = ['-g', '-DDEBUG']
        env['CCFLAGS_ULTRADEBUG'] = ['-g3', '-O0', '-DDEBUG']

    if env['CXX_NAME'] == 'gcc':
        env['CXXFLAGS_SAFE']       = env['CXXFLAGS']
        env['CXXFLAGS']            = []
        env['CXXFLAGS_OPTIMIZED']  = ['-O2']
        env['CXXFLAGS_RELEASE']    = ['-O2']
        env['CXXFLAGS_DEBUG']      = ['-g', '-DDEBUG']
        env['CXXFLAGS_ULTRADEBUG'] = ['-g3', '-O0', '-DDEBUG']
    
    if env['IS_WIN32']:
        ## setup for Winsock on Windows (required for read-aloud)
        conf.check(lib='ws2_32', uselib='WSOCK', mandatory=True)
        conf.check(lib='shell32', uselib='SHELLAPI', mandatory=True)
        conf.check(lib='rpcrt4', uselib='RPCRT4', mandatory=True)
        # this isn't supposed to be necessary
        env['LINKFLAGS'] = ['-lws2_32', '-lrpcrt4']
        dfn('WIN32', 1)
        # tool to link icon with executable
        # use tool modified for cross-compilation support
        env['POST_INSTALL']=False
        conf.check_tool('winres', tooldir=_tooldir)
        # the following line does not work because of a problem with waf
        # conf.check_tool('intltool')
        #env['POCOM'] = conf.find_program('msgfmt')
        #env['INTLTOOL'] = '/usr/local/bin/intltool-merge'
    
    if env['IS_CROSS_WIN32']:
        # allows to use linux pkg-config for cross-compilation
        os.environ['PKG_CONFIG_PATH'] = opt.pkg_conf_libdir
        env['PKG_CONFIG_PREFIX'] = opt.pkg_conf_prefix

    # Auto detecting gtk version, if none were specified.
    if opt.gtkver == 'auto':
        if conf.check_cfg(modversion="gtk+-3.0", okmsg='ok',
                          msg='Auto detecting gtk 3'):
            opt.gtkver = '3'
        else:
            conf.check_cfg(modversion="gtk+-2.0", okmsg='ok',
                           msg='Auto detecting gtk 2', mandatory=True)
            opt.gtkver = '2'

    # FIXME: Find out why we still need this!
    dfn('WEBKIT', 1) 
    
    #gtk
    if opt.gtkver == '2':
        env['ENABLE_GTK2'] = True
        dfn('GTK2', 1)

    # disable console window in win32
    if opt.no_console and env['IS_WIN32']:
        env.append_value('LINKFLAGS', '-mwindows')

    # strip xiphos binary
    if env['IS_CROSS_WIN32']:
        env['STRIP'] = conf.find_program(env['CROSS'] + 'strip', mandatory=True)
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

    define('VERSION', VERSION)
    define('PACKAGE_VERSION', VERSION)
    define('GETTEXT_PACKAGE', PACKAGE)
    define('PACKAGE', PACKAGE)
    define('PACKAGE_NAME', APPNAME)
    define('PACKAGE_STRING', '%s %s' % (APPNAME, VERSION))
    define('PACKAGE_TARNAME', PACKAGE)

    define('INSTALL_PREFIX', conf.escpath(sub('${PREFIX}/', env)))
    #dfn('LT_OBJDIR', '.libs') - what's the purpose?
    define('PACKAGE_BUGREPORT','https://github.com/crosswire/xiphos/issues' )
    define('PACKAGE_DATA_DIR', conf.escpath(sub('${DATAROOTDIR}/${PACKAGE}', env)))
    define('PACKAGE_DOC_DIR', conf.escpath(env['DOCDIR']))
    define('PACKAGE_HELP_DIR', conf.escpath(sub('${DATAROOTDIR}/gnome/help/${PACKAGE}', env)))
    define('PACKAGE_LOCALE_DIR', conf.escpath(env['LOCALEDIR']))
    define('PACKAGE_MENU_DIR', conf.escpath(sub('${DATAROOTDIR}/applications', env)))
    #define('PACKAGE_PIXMAPS_DIR', conf.escpath(sub('${DATAROOTDIR}/pixmaps/${PACKAGE}', env)))
    define('PACKAGE_PIXMAPS_DIR', conf.escpath(sub('${DATAROOTDIR}/${PACKAGE}', env)))
    define('PACKAGE_SOURCE_DIR', conf.escpath(os.path.abspath(srcdir))) # folder where was wscript executed
    if not env["IS_WIN32"]:
        # for installing xiphos.appdata.xml
        define('PACKAGE_APPDATA_DIR', conf.escpath(sub('${INSTALL_PREFIX}/share/appdata', env)))

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
        common_libs += ' libglade-2.0'
        common_libs += ' webkit-1.0'
        if opt.webkit_editor:
            conf.define('USE_WEBKIT_EDITOR', 1)
            env['ENABLE_WEBKIT_EDITOR'] = True
            common_libs += ' "gtk+-2.0 >= 2.24" '
        elif conf.check_cfg(modversion='gtkhtml-editor-3.14',
                          msg='Checking for GNOME3 gtkhtml-editor',
                          okmsg='Definitely',
                          errmsg='Probably not',
                          mandatory=True
                          ):
            common_libs += ' "gtk+-2.0 >= 2.14" '
            common_libs += ' "gtkhtml-editor-3.14" '
            common_libs += ' "libgtkhtml-3.14 >= 3.23" '
    else:
        common_libs += ' "gtk+-3.0" '
        conf.define('USE_GTK_3', 1)
        conf.define('USE_GTKBUILDER', 1)
        if opt.webkit_editor:
            conf.define('USE_WEBKIT_EDITOR', 1)
            env['ENABLE_WEBKIT_EDITOR'] = True
            common_libs += ' "webkitgtk-3.0" '
        else:
            if conf.check_cfg(modversion='libgtkhtml-4.0',
                              msg='Checking for libgtkhtml4',
                              okmsg='ok',
                              errmsg='fail'):
                common_libs += ' "libgtkhtml-4.0" '
            if conf.check_cfg(modversion="gtkhtml-editor-4.0",
                              msg="Checking for GtkHTML Editor 4.0",
                              okmsg='ok',
                              errmsg='fail',
                              mandatory=True):
                common_libs += ' "gtkhtml-editor-4.0" '
            if opt.gtkver == '3' and opt.webkit2 and conf.check_cfg(modversion='webkit2gtk-4.0',
                              msg='Checking for WebKit2',
                              okmsg='ok',
                              errmsg='fail'):
                common_libs += ' "webkit2gtk-4.0" '
                conf.define('USE_WEBKIT2', 1)
            elif conf.check_cfg(modversion='webkitgtk-3.0',
                                msg='Checking for WebKit',
                                okmsg='ok',
                                errmsg='fail'):
                common_libs += ' "webkitgtk-3.0" '
    
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
                   args='"sword >= 1.7.3" --cflags --libs',
                   uselib_store='SWORD',
                   mandatory=True)
    env.append_value('ALL_LIBS', 'SWORD')

    conf.check_cfg(package='icu-i18n',
                   args='--cflags --libs',
                   uselib_store='ICUI18N',
                   errmsg='fail',
                   mandatory=True)
    env.append_value('ALL_LIBS', 'ICUI18N')

    conf.check_cfg(package='biblesync',
                   args='"biblesync >= 1.1.2" --cflags --libs',
                   uselib_store='BIBLESYNC',
                   errmsg='fail',
                   mandatory=True)
    env.append_value('ALL_LIBS', 'BIBLESYNC')

    if not env["IS_WIN32"]:
        conf.check_cfg(package='uuid',
                       args='--cflags --libs',
                       uselib_store='UUID',
                       mandatory=True)
        env.append_value('ALL_LIBS', 'UUID')

    #
    # Random defines... legacy from autotools. Can we drop these?
    # Define to 1 if you have the ANSI C header files. */
    dfn('STDC_HEADERS', 1)
    # Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
    dfn('TIME_WITH_SYS_TIME', 1)
    dfn('ENABLE_NLS', 1)
    dfn('HAVE_BIND_TEXTDOMAIN_CODESET', 1)
    dfn('HAVE_GETTEXT', 1)
    dfn('HAVE_DCGETTEXT', 1)

    # Check for header files
    for h in _headers:
        conf.check(header_name=h)

    # Define to 1 if you have the `strcasestr' function.
    # this function is part of some glibc, string.h
    # could be missing in win32
    conf.check_cc(msg='Checking for function strcasestr', define_name="HAVE_STRCASESTR",
            fragment='int main() {strcasestr("hello","he");}\n')

    # enable the GS_message-style tracing printfs.
    if opt.chatty:
        dfn('CHATTY', 1)

    # appropriate cflags
    env.append_value('CXXFLAGS', env['CXXFLAGS_%s' % opt.debug_level.upper()])
    env.append_value('CCFLAGS', env['CCFLAGS_%s' % opt.debug_level.upper()])

    # stop the insanity. NULLs mean something. shocking, i know, but seriously...
    env.append_value('CXXFLAGS', ['-fno-delete-null-pointer-checks'])
    env.append_value('CCFLAGS', ['-fno-delete-null-pointer-checks'])

    # add a linker option to stop very recent dbus linkage nightmare.
    if not env['IS_WIN32']:
        env.append_value('LINKFLAGS', '-Wl,-z,muldefs')

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
        README.md
	RELEASE-NOTES
	COPYING
	AUTHORS
	ChangeLog
	INSTALL.md
	TODO
	Xiphos.ogg
    """)

    bld.install_files('${PACKAGE_PIXMAPS_DIR}',bld.path.ant_glob('pixmaps/*.png'))
    bld.install_files('${PACKAGE_PIXMAPS_DIR}',bld.path.ant_glob('pixmaps/*.ico'))
    bld.install_files('${PACKAGE_PIXMAPS_DIR}',bld.path.ant_glob('pixmaps/*.xpm'))    
    bld.install_files('${DATAROOTDIR}/icons/hicolor/scalable/apps','pixmaps/xiphos.svg')
    if not env["IS_WIN32"]:
        bld.install_files('${PACKAGE_APPDATA_DIR}','xiphos.appdata.xml')

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

import shutil
import subprocess
import glob

def dist_hook():
    shutil.rmtree('win32')

def run(ctx):
    '''Execute xiphos binary from build directory'''
    subprocess.call(os.path.join(blddir,'default/src/gnome2/xiphos'))


def test(ctx):
    '''Run unit tests'''
    subprocess.call(os.path.join(srcdir,'tests/build.py'))

def dist(ctx):
    import Scripting
    Scripting.dist()
    Scripting.g_gz = 'gz'
    Scripting.dist()

def dist_dfsg(ctx):
    import Scripting
    global dist_hook
    global VERSION

    def dist_hook2():
        shutil.rmtree('win32')
        shutil.copytree(os.path.join(glob.glob('../.waf-*')[0],'wafadmin'), 'wafadmin', ignore=shutil.ignore_patterns('*.pyc'))
        lines = []
        with open('waf') as f:
            lines = f.readlines()
        with open('waf','w') as f:
            f.writelines(lines[:-3])

    VERSION = VERSION + '+dfsg'
    dist_hook=dist_hook2
    Scripting.dist()
