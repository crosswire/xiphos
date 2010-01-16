#! /usr/bin/env python
# encoding: utf-8

import os, os.path
import intltool


# waf imports
import Utils
import Options
import ccroot


# custom imports
import waffles.misc
import waffles.gnome as gnome

# the following two variables are used by the target "waf dist"
VERSION='3.1.2'
APPNAME='xiphos'
PACKAGE='xiphos'



# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = 'build'

# dir where waf will search custom (additional) files
_tooldir = './waffles/'

_prefix = '/usr/local'

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


    # options provided by the modules
    #opt.tool_options('g++ gcc gnome intltool glib2')
    opt.tool_options('g++ gcc')

    opt.add_option('--enable-gtkhtml', action='store_true', default=False,
            dest='gtkhtml',
            help='Use gtkhtml instead of gtkmozembed [Default: disabled]')

    opt.add_option('--disable-console',
            action='store_true',
            default=False,
            help='Disable console window in win32 [Default: enabled]',
            dest='no_console')

    opt.add_option('-d', '--debug-level',
		action = 'store',
		default = ccroot.DEBUG_LEVELS.ULTRADEBUG,
		help = "Specify the debugging level ['ultradebug', 'debug', 'release', 'optimized']",
		choices = ['ultradebug', 'debug', 'release', 'optimized'],
		dest = 'debug_level')

    opt.add_option('--strip', action='store_true', default=False,
                    help='Strip resulting binary')

    opt.add_option('--enable-delint', action='store_true', default=False,
            dest='delint',
            help='Use -Wall -Werror[Default: disabled]')


    ### cross compilation from Linux/Unix for win32

    opt.add_option('--target-platform-win32', action='store_true', default=False,
            dest='cross_win32',
            help='Cross-compile for win32 [Default: disabled]')

    # FIXME - handle this option
    #opt.add_option('--rootdir',
		#action = 'store',
		#help = 'Specify path where resides msys. This dir is used for finding libraries and other files',
		#dest = 'rootdir')

    opt.add_option('--pkgconf-libdir',
		action = 'store',
		default = os.path.join(ROOTDIR_WIN32, 'local', 'lib', 'pkgconfig'),
		help = "Specify dir with *.pc files for cross-compilation",
		dest = 'pkg_conf_libdir')

    opt.add_option('--pkgconf-prefix',
		action = 'store',
		default = os.path.join(ROOTDIR_WIN32, 'local'),
		help = "Specify prefix with folders for headers and libraries for cross-compilation",
		dest = 'pkg_conf_prefix')

    opt.add_option('--mozilla-distdir',
		action = 'store',
		default = os.path.join(ROOTDIR_WIN32, 'local'),
		help = "Folder 'dist' in unpacked mozilla devel tarball. Mandatory for win32 compilation",
		dest = 'mozilla_distdir')

    opt.add_option('--disable-dbus',
                   action = 'store_true',
                   default = False,
                   help = "Disable the Xiphos dbus API [Default: enabled]",
                   dest = 'without_dbus')

    opt.add_option('--without-gnome',
                   action = 'store_true',
                   default = False,
                   help = "Disable GNOME [Default: enabled]",
                   dest = 'without_gnome')

    opt.add_option('--no-post-install',
                   action = 'store_false',
                   default = True,
                   help = "Disable post-install tasks",
                   dest = 'post_install')

    group = opt.add_option_group ('Localization and documentation', '')
    group.add_option('--helpdir',
		action = 'store',
		default = '${DATAROOTDIR}/gnome/help/${PACKAGE}',
                help = "user documentation [Default: ${DATAROOTDIR}/gnome/help/${PACKAGE}]",
		dest = 'helpdir')
    group.add_option('--disable-help',
            action='store_true',
            default=False,
            help='Disable creating help files',
            dest='disable_help')


def configure(conf):

    env = conf.env
    import Utils
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

    # IS_WIN32 means compiling for win32, not compiling necessary on win32
    if env['IS_WIN32']:
        Utils.pprint('CYAN', "Windows detected")
    elif env['IS_LINUX']:
        Utils.pprint('CYAN', "Linux detected")

    if not (env['IS_LINUX'] or env['IS_WIN32']):
        Utils.pprint('RED', "Assuming %s is UNIX like platform" % platform)
        env['IS_LINUX'] = True

    ## temporary HACKS for win32
    if env['IS_WIN32']:
        env['PREFIX'] = conf.escpath(os.path.abspath('win32/binaries/Xiphos'))
    ##

    if env['IS_WIN32']:
        env['ROOTDIR'] = ROOTDIR_WIN32
    
    conf.check_tool('g++ gcc')
    # cross compiler
    if env['IS_CROSS_WIN32']:
        conf.check_tool('cross_linux_win32', tooldir=_tooldir)

    conf.check_tool('gnu_dirs misc')
    conf.check_tool('intltool') # check for locale.h included

    opt = Options.options
    env['DISABLE_HELP'] = opt.disable_help

    if not opt.disable_help:
        if env['IS_LINUX']:
            conf.check_tool('gnome', tooldir=_tooldir)
        else:
            conf.check_tool('documentation', tooldir=_tooldir) # stuff to create help files

    # DATADIR is defined by intltool in config.h - conflict in win32 (mingw)
    conf.undefine('DATADIR')

    env['POST_INSTALL']=opt.post_install

    if env['IS_WIN32']:
        # tool to link icon with executable
        # use tool modified for cross-compilation support
        env['POST_INSTALL']=False
        conf.check_tool('winres', tooldir=_tooldir)
        # the following line does not work because of a problem with waf
        # conf.check_tool('intltool')
        #env['POCOM'] = conf.find_program('msgfmt')
        #env['INTLTOOL'] = '/usr/local/bin/intltool-merge'
    #else:


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
        env['CXXFLAGS_DEBUG']      = ['-g', '-DDEBUG', '-ftemplate-depth-25']
        env['CXXFLAGS_ULTRADEBUG'] = ['-g3', '-O0', '-DDEBUG', '-ftemplate-depth-25']
    
    if env['IS_WIN32']:
        ## setup for Winsock on Windows (required for read-aloud)
        conf.check(lib='ws2_32', uselib='WSOCK', mandatory=True)
        conf.check(lib='shell32', uselib='SHELLAPI', mandatory=True)
        # this isn't supposed to be necessary
        env['LINKFLAGS'] = ['-lws2_32']
    
    ### cmd line options

    opt = Options.options
    dfn = conf.define
    env = conf.env


    if env['IS_CROSS_WIN32']:
        # allows to use linux pkg-config for cross-compilation
        os.environ['PKG_CONFIG_LIBDIR'] = opt.pkg_conf_libdir
        env['PKG_CONFIG_PREFIX'] = opt.pkg_conf_prefix

    # mozilla distdir mandatory for win32 for using libxul
    if env['IS_WIN32']:
        if opt.mozilla_distdir:
            env['MOZILLA_DISTDIR'] = opt.mozilla_distdir
        else:
            env['MOZILLA_DISTDIR'] = '%s/../..' % env['PKG_CONFIG_LIBDIR']

    # appropriate cflags
    env.append_value('CXXFLAGS', env['CXXFLAGS_%s' % opt.debug_level.upper()])
    env.append_value('CCFLAGS', env['CCFLAGS_%s' % opt.debug_level.upper()])

    if env['IS_WIN32']:
        dfn('WIN32', 1)

    if opt.delint:
        env.append_value('CXXFLAGS', env['CXXFLAGS_DELINT'])
        env.append_value('CCFLAGS', env['CCFLAGS_DELINT'])

    # gtkhtml
    if opt.gtkhtml:
        env['ENABLE_GTKHTML'] = True
        dfn('GTKHTML', 1)

    # disable console window in win32
    if opt.no_console and env['IS_WIN32']:
        env.append_value('LINKFLAGS', '-mwindows')


    # strip xiphos binary
    if env['IS_CROSS_WIN32']:
        env['STRIP'] = conf.find_program('i686-mingw32-strip', mandatory=True)
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
    define('PACKAGE_BUGREPORT','http://sourceforge.net/tracker/?group_id=5528&atid=105528' )
    define('PACKAGE_DATA_DIR', conf.escpath(sub('${DATAROOTDIR}/${PACKAGE}', env)))
    define('PACKAGE_DOC_DIR', conf.escpath(env['DOCDIR']))
    define('PACKAGE_HELP_DIR', conf.escpath(sub('${DATAROOTDIR}/gnome/help/${PACKAGE}', env)))
    define('PACKAGE_LOCALE_DIR', conf.escpath(env['LOCALEDIR']))
    define('PACKAGE_MENU_DIR', conf.escpath(sub('${DATAROOTDIR}/applications', env)))
    define('PACKAGE_PIXMAPS_DIR', conf.escpath(sub('${DATAROOTDIR}/pixmaps/${PACKAGE}', env)))
    define('PACKAGE_SOURCE_DIR', conf.escpath(os.path.abspath(srcdir))) # foder where was wscript executed

    # pkg-config
    conf.check_cfg(atleast_pkgconfig_version='0.9.0')

    # GTK+
    #check_pkg(conf, 'gtk+-x11-2.0', '2.0.0', var='LIBGTK_X11_2_0')
    #if not env['HAVE_LIBGTK_X11_2_0']:
    #    check_pkg(conf, 'gtk+-x11-2.0', '2.0.0', True, var='LIBGTK_WIN32_2_0')
    conf.check_pkg('gtk+-2.0', '2.12', True, var='GTK')
    # glade
    conf.check_pkg('libglade-2.0', '2.0.0', var='GLADE')
        
    conf.check_pkg('gmodule-2.0', '2.0.0', True, var='GMODULEEXP')
    conf.check_pkg('glib-2.0', '2.0.0', True, 'GLIB')

    ## Gnome libs
    if not opt.without_gnome:
        conf.check_pkg('libgnomeui-2.0', '2.0.0', True, var='GNOMEUI')
    else:
        dfn('HAVE_GTK214', 1)

    ## gfs
    conf.check_pkg('libgsf-1', '1.14', True, var='GFS')

    #check_pkg(conf, 'libgnomeprintui-2.2', '2.2', True, var='GNOMEPRINTUI')
    #check_pkg(conf, 'libgnomeprint-2.2', '2.2', True, var='GNOMEPRINT')

    ## Other
    conf.check_pkg('libxml-2.0', '2.0.0', True, var='XML')

    conf.check_pkg('gtk+-unix-print-2.0', '2.0.0', var='UPRINT')
    if env['HAVE_UPRINT']:
        dfn('USE_GTKUPRINT', 1)

    ## Sword
    conf.check_pkg('sword', '1.5.11', True, var='SWORD')

    conf.check_pkgver_msg('sword', '1.5.11.99', var='MULTIVERSE',
            msg='Checking for sword multiverse')
    if env['HAVE_MULTIVERSE']:
        dfn('SWORD_MULTIVERSE', 1)


    #here gtkhtml
    conf.check_pkg('libgtkhtml-3.14', version='3.23', mandatory=True, var='GTKHTML')

    if conf.env['HAVE_GTKHTML']:
	conf.define('USE_GTKHTML3_14', 1)

    conf.define('GTKHTML_API_VERSION', conf.get_pkgvar('libgtkhtml-3.14', 'gtkhtml_apiversion'))

    conf.check_pkg('gtkhtml-editor', mandatory=True)

    ######################
    ### gecko (xulrunner) for html rendering
    # gtkhtml only for editor
    if not env['ENABLE_GTKHTML']:
        if not env["IS_WIN32"]:

            conf.check_cfg (package='libxul-embedding',
                            uselib_store='GECKO',
                            args='--define-variable=includetype=unstable --cflags --libs',
                            atleast_version='1.9.0')
            conf.define('GECKO_HOME', conf.get_pkgvar('libxul-embedding', 'sdkdir'))
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

        env.append_value('ALL_LIBS', 'GECKO')
        conf.define('USE_GTKMOZEMBED', 1)


    ######################


    # TODO: maybe the following checks should be in a more generic module.

    #always defined to indicate that i18n is enabled */
    dfn('ENABLE_NLS', 1)

    # TODO
    #Define to 1 if you have the `bind_textdomain_codeset' function.
    dfn('HAVE_BIND_TEXTDOMAIN_CODESET', 1)


    # TODO
    #Define if the GNU gettext() function is already present or preinstalled.
    dfn('HAVE_GETTEXT', 1)
    #Define to 1 if you have the `dcgettext' function.
    dfn('HAVE_DCGETTEXT', 1)


    # Check for header files
    for h in _headers:
        conf.check(header_name=h)


    # Define to 1 if you have the `strcasestr' function.
    # this function is part of some glibc, string.h
    # could be missing in win32
    conf.check_cc(msg='Checking for function strcasestr', define_name="HAVE_STRCASESTR",
            fragment='int main() {strcasestr("hello","he");}\n')


    # TODO: What's the purpose of STDC? Is xiphos able compile without that?
    # Define to 1 if you have the ANSI C header files. */
    # NOT necessary?
    dfn('STDC_HEADERS', 1)
    # Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
    dfn('TIME_WITH_SYS_TIME', 1)


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
    
    # portable adding define, should work gcc and msvc
    env.append_value('CXXFLAGS', env['CXXDEFINES_ST'] % 'HAVE_CONFIG_H')
    env.append_value('CCFLAGS', env['CCDEFINES_ST'] % 'HAVE_CONFIG_H')

    conf.write_config_header('config.h')

    env.append_value('CCFLAGS', env['CCFLAGS_SAFE'])
    env.append_value('CXXFLAGS', env['CXXFLAGS_SAFE'])

def build(bld):

    env = bld.env
    opt = Options.options

    # process subfolders
    bld.add_subdirs("""
        src/backend
        src/editor
        src/main
        src/gnome2
        ui
    """)
    # use GECKO
    if not env['ENABLE_GTKHTML']:
        if env["IS_WIN32"]:
            bld.add_subdirs('src/geckowin')
        else:
            bld.add_subdirs('src/gecko')

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

    #if env['HAVE_GTKSHARP'] and env['MCS']:
        #bld.add_subdirs('sharp')

    #if env['SGML2MAN']:
    #	bld.add_subdirs('man')

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

def dist(ctx):
    import Scripting
    Scripting.dist()
    Scripting.g_gz = 'gz'
    Scripting.dist()
