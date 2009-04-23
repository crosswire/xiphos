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


def get_pkgvar(conf, pkg, var):
    """
    Read a variable from package using pkg-config
    """
    ret = conf.check_cfg(package=pkg, args='--variable=%s' % var, okmsg=myokmsg,
            msg='Checking for var %s in %s' % (var, pkg)).strip()
    return ret


def check_pkg(conf, name, version='', mandatory=False, var=None):
    """
    Check package and its version
    """
    if not var:
        var = name.split ('-')[0].upper ()
    conf.check_cfg (package=name, uselib_store=var, args='--cflags --libs',
        atleast_version=version, mandatory=mandatory)

def check_pkg_msg(conf, name, version='', mandatory=False, var=None, msg=''):
    """
    Check package and its version
    """
    if not var:
        var = name.split ('-')[0].upper ()
    conf.check_cfg (package=name, uselib_store=var, args='--cflags --libs',
        atleast_version=version, mandatory=mandatory, msg=msg)


def escape(string):
    """
    Escape string
    Useful to use for string and paths before writing them to config.h
    """
    return string.decode('string-escape')

def escpath(path):
    """
    Escape path - useful in win32
    """
    from os.path import normpath
    # double backslash '\\' needs to be written to config.h in paths in win32
    return repr(normpath(path))
    

