"""
General useful functions
"""

def okmsg(kw):
    """
    some customization for displaying pkg-config values on the line with ok msg
    to be able use this function there is need for file ./wafadmin/Tools/config_c.py
    from svn in r5753
    """
    return 'ok %s' % kw['success'].strip()


# pkgconfig utils


def get_pkgvar(conf, pkg, var):
    """
    Read a variable from package using pkg-config
    """
    ret = conf.check_cfg(package=pkg, args='--variable=%s' % var, okmsg=okmsg,
            msg='Checking for var %s in %s' % (var, pkg)).strip()
    return ret


def check_package(conf, pkg, lib, mandatory=False):
    """
    Check existence of a package (pkg) in system and
    store its cflags (lib)
    """
    if conf.check_cfg(package=pkg, uselib_store=lib,
            args='--cflags --libs', okmsg=okmsg, mandatory=mandatory):
        return True
    else:
        return False


def check_pkgver(conf, pkg, ver):
    """
    Check package version
    """
    return conf.check_cfg(package=pkg, args='--modversion',
            atleast_version=ver)

