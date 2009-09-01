"""
Stuff to detect GECKO
"""

from os.path import join

from Configure import conf

from waffles.misc import *

# needed for gecko 1.8
# in libxul 1.9 all headers are in 2 folders
# ${gecko_incldir}/stable ${gecko_incldir}/unstable
gecko18_subdirs = '''
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

"""
    # TODO: implement gecko detecting (rewrite gecko.m4 in python)

    ## Gecko

    # other HARDCODED
    # TODO: When should be this define defined?
    # Define if mozilla is of the toolkit flavour */
    #dfn('HAVE_MOZILLA_TOOLKIT', 1)


"""


#dfn = conf.define
#env = conf.env

GECKOS = [
        'libxul-embedding', # xul 1.9
        'xulrunner-gtkmozembed', # xul 1.8
        'xulrunner-windows', #windows
    ]


class Gecko(object):
    """
    Stuff related to gecko detection
    """
    #DISTROS = [
            #_try_ubuntu804_xul19,
            #_try_gentoo_xul18,
    #]

    def __init__(self, conf, incldir='', libdir=''):
        self.incldir = incldir
        self.libdir = libdir
        self.conf = conf
        self.dfn = conf.define
        self.env = conf.env
        

    def detect(self):
        """
        Detect gecko in system
        TODO: ignore pkgconfig when supplied custom
              gecko incldir and libdir
        """
        #self.conf.check_cxx(cxxflags='-fshort-wchar',
        #        errmsg='fail flag is needed for libxul 1.9')
        #self.env['GECKO_CCFLAGS'] = '-fshort-wchar'

        success = False

        # try xulwin
        if self.env['IS_WIN32']:
            success = self.try_libxulwin()
        
        else:
            # try first the latest xulrunner version
            success = self.try_libxul19()

            if not success:
                success = self.try_xulrunner18()

        return success


    def check_version(self, name):

        cfg = self.conf
        check_pkgver(cfg, name, '1.7', var='GECKO_1_7')
        check_pkgver(cfg, name, '1.8', var='GECKO_1_8')
        check_pkgver(cfg, name, '1.8.1', var='GECKO_1_8_1')
        check_pkgver(cfg, name, '1.9', var='GECKO_1_9')

    
    def get_gecko_includes(self, incldir):
        # put gecko subdirs in string with absolute paths
        gecko_include = ''
        for i in gecko18_subdirs:
            gecko_include += join(incldir, i) + ' '
        return gecko_include



    # Various configurations to try



    def try_libxul19(self):
        """
        Should work for:
          Ubuntu 8.04
          Ubuntu 8.10
          Gentoo with xulrunner-1.9
        """
        cfg = self.conf
        ret = False
        gecko = 'libxul-embedding'
        gecko_unstable = 'libxul-embedding-unstable'
        lib = 'GTKMOZEMBED'
        lib_unstable = 'GTKMOZEMBED_UNSTABLE'

        check_pkg(cfg, gecko, var=lib)
        check_pkg(cfg, gecko_unstable, var=lib_unstable)

        if self.env['HAVE_GTKMOZEMBED'] and self.env['HAVE_GTKMOZEMBED_UNSTABLE']:
            #incldir = self.get_pkgvar(gecko, 'includedir')
            self.check_version(gecko)
            libdir = get_pkgvar(self.conf, gecko, 'sdkdir')
            self.dfn('GECKO_HOME', libdir)
            self.env['GECKO_INCLUDE'] = '' # header files aren't in subdirs

            # NSS & NSPR
            # TODO: are NSS and NSPR really needed both?
            check_pkg(cfg, 'nss', var='GECKONSS')
            check_pkg(cfg, 'nspr', var='GECKONSPR')

            ret = True

        return ret


    def try_xulrunner18(self):
        """
        Should work for:
            Gentoo with xulrunner-1.8
        """
        cfg = self.conf
        ret = False
        gecko = 'xulrunner-gtkmozembed'
        lib = 'GTKMOZEMBED'

        check_pkg(cfg, gecko, var=lib)

        if self.env['HAVE_GTKMOZEMBED']:
            self.check_version(gecko)
            incldir = get_pkgvar(cfg, gecko, 'includedir')
            libdir = get_pkgvar(cfg, gecko, 'libdir')
            gecko_include = self.get_gecko_includes(incldir)
            self.dfn('GECKO_HOME', libdir)
            self.env['GECKO_INCLUDE'] = gecko_include

            check_pkg(cfg, 'xulrunner-nss', var='GECKONSS')
            check_pkg(cfg, 'xulrunner-nspr', var='GECKONSPR')

            ret = True

        return ret
    
    def try_libxulwin(self):
        v = self.env
        df = self.conf.define

        # FIXME hardcoded values
        #if v['IS_CROSS_WIN32']:
        d = v['MOZILLA_DISTDIR']
        df('HAVE_GTKMOZEMBED', 1)
        df('USE_GTKMOZEMBED', 1)
        df('HAVE_GECKO_1_7', 1)
        df('HAVE_GECKO_1_8', 1)
        df('HAVE_GECKO_1_8_1', 1)
        df('HAVE_GECKO_1_9', 1)
        v['ALL_LIBS'].append('GTKMOZEMBED')
        v['CPPPATH_GTKMOZEMBED'] = ['%s/sdk/include' % d,
                '%s/include' % d,
                '%s/include/widget' % d,
                '%s/include/xpcom' % d,
                '%s/include/dom' % d,
                '%s/include/content' % d,
                '%s/include/layout' % d,
                '%s/include/gfx' % d]
        v['LIBPATH_GTKMOZEMBED'] = ['%s/sdk/lib' % d]
        v['LIB_GTKMOZEMBED'] = ['xpcomglue_s', 'xpcom', 'xul', 'nspr4']
        ret = True

        #else:
            #ret = False
            #gecko = 'libxul'
            #lib = 'GTKMOZEMBED'

            #check_pkg(cfg, gecko, var=lib)
            
            #if self.env['HAVE_GTKMOZEMBED']:
                #self.check_version(gecko)
                #ret = True

        return ret
        

    def try_custom_conf(self):
        """
        Try values for gecko includedir and libdir
        supplied by user.
        """
        pass

    def try_custom_conf_xul19(self):
        """
        Try values for gecko includedir and libdir
        supplied by user.
        """
        pass

