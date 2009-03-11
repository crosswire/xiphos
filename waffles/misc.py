"""
General useful functions
"""

def myokmsg(kw):
    """
    some customization for displaying pkg-config values on the line with ok msg
    to be able use this function there is need for file ./wafadmin/Tools/config_c.py
    from svn in r5753
    """
    return 'ok %s' % kw['success'].strip()


# pkgconfig utils

def check_pkg (conf, name, version='', mandatory=False, var=None):
    """
    Check package and its version
    """
    if not var:
        var = name.split ('-')[0].upper ()
    conf.check_cfg (package=name, uselib_store=var, args='--cflags --libs',
        atleast_version=version, mandatory=mandatory)



# TODO: remove this function - unreliable
def check_package(conf, pkg, mandatory=False):
    """
    Check package existence
    """
    ret = False

    if conf.check_cfg(package=pkg, args='--modversion', mandatory=mandatory):
        ret = True

    return ret

def check_pkgver(conf, pkg, ver, mandatory=False):
    """
    Check package version
    """
    return conf.check_cfg(package=pkg, args='--modversion',
            atleast_version=ver, mandatory=mandatory)

def check_pkgver_msg(conf, pkg, ver, msg, mandatory=False):
    """
    Check package version
    """
    return conf.check_cfg(package=pkg, args='--modversion',
            atleast_version=ver, msg=msg, mandatory=mandatory)



def check_pkgver_errmsg(conf, pkg, ver, errmsg, mandatory=False):
    """
    Check package version
    """
    return conf.check_cfg(package=pkg, args='--modversion',
            atleast_version=ver, errmsg=errmsg, mandatory=mandatory)



def get_pkgvar(conf, pkg, var):
    """
    Read a variable from package using pkg-config
    """
    ret = conf.check_cfg(package=pkg, args='--variable=%s' % var, okmsg=myokmsg,
            msg='Checking for var %s in %s' % (var, pkg)).strip()
    return ret


def get_pkgcflags(conf, pkg, lib):
    """
    Read cflags and libs of a package and store them
    """
    msg = 'Checking for %s cflags/libs' % pkg
    ret = False

    if conf.check_cfg(package=pkg, uselib_store=lib,
            args='--cflags --libs', msg=msg, okmsg=myokmsg):
        ret = True

    return ret


