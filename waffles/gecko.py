"""
Stuff to detect GECKO
"""

from os.path import join

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

from waffles.misc import okmsg
from Configure import conf

#dfn = conf.define
#env = conf.env
#pkgcf = conf.check_cfg

GECKOS = [
        'libxul-embedding', # xul 1.9
        'xulrunner-gtkmozembed', # xul 1.8
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
        self.pkgcf = conf.check_cfg
        

    def detect(self):
        """
        Detect gecko in system
        TODO: ignore pkgconfig when supplied custom
              gecko incldir and libdir
        """
        self.conf.check_cxx(cxxflags='-fshort-wchar',
                errmsg='fail flag is needed for libxul 1.9')
        self.env['GECKO_CCFLAGS'] = '-fshort-wchar'

        # try first the latest xulrunner version
        success = False
        success = self.try_libxul19()

        if not success:
            success = self.try_xulrunner18()

        return success




    def check_package(self, pkg, lib):
        if self.pkgcf(package=pkg, uselib_store=lib,
                args='--cflags --libs', okmsg=okmsg):
            return True
        else:
            return False


    def check_pkgver(self, pkg, ver):
        return self.pkgcf(package=pkg, args='--modversion',
                atleast_version=ver)


    def check_version(self, name):
        if self.check_pkgver(name, '1.7'):
            self.dfn('HAVE_GECKO_1_7', 1)
        if self.check_pkgver(name, '1.8'):
            self.dfn('HAVE_GECKO_1_8', 1)
        if self.check_pkgver(name, '1.8.1'):
            self.dfn('HAVE_GECKO_1_8_1', 1)
        if self.check_pkgver(name, '1.9'):
            self.dfn('HAVE_GECKO_1_9', 1)

    
    def get_gecko_includes(self, incldir):
        # put gecko subdirs in string with absolute paths
        gecko_include = ''
        for i in gecko18_subdirs:
            gecko_include += join(incldir, i) + ' '
        return gecko_include


    def get_pkgvar(self, pkg, var):
        """
        Read a variable from package using pkg-config
        """
        ret = self.pkgcf(package=pkg, args='--variable=%s' % var, okmsg=okmsg,
                msg='Checking for var %s in %s' % (var, pkg)).strip()
        return ret



    # Various configurations to try



    def try_libxul19(self):
        """
        Should work for:
          Ubuntu 8.04
          Ubuntu 8.10
          Gentoo with xulrunner-1.9
        """
        ret = False
        gecko = 'libxul-embedding'
        gecko_unstable = 'libxul-embedding-unstable'
        lib = 'GTKMOZEMBED'
        lib_unstable = 'GTKMOZEMBED_UNSTABLE'

        if self.check_package(gecko, lib) and self.check_package(gecko_unstable, lib_unstable):

            #incldir = self.get_pkgvar(gecko, 'includedir')
            self.check_version(gecko)
            libdir = self.get_pkgvar(gecko, 'sdkdir')
            self.dfn('GECKO_HOME', libdir)
            self.env['GECKO_INCLUDE'] = '' # header files aren't in subdirs

            # NSS & NSPR
            # TODO: are NSS and NSPR really needed both?
            self.check_package('nss', 'GECKONSS')
            self.check_package('nspr', 'GECKONSPR')

            ret = True

        return ret


    def try_xulrunner18(self):
        """
        Should work for:
            Gentoo with xulrunner-1.8
        """
        ret = False
        gecko = 'xulrunner-gtkmozembed'
        lib = 'GTKMOZEMBED'

        if self.check_package(gecko, lib):

            self.check_version(gecko)
            incldir = self.get_pkgvar(gecko, 'includedir')
            libdir = self.get_pkgvar(gecko, 'libdir')
            gecko_include = self.get_gecko_includes(incldir)
            self.dfn('GECKO_HOME', libdir)
            self.env['GECKO_INCLUDE'] = gecko_include

            self.check_package('xulrunner-nss', 'GECKONSS')
            self.check_package('xulrunner-nspr', 'GECKONSPR')

            ret = True

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

