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

    # for cross-compilation it is necessary to prepend custom
    # prefix for include paths
    if conf.env['IS_CROSS_WIN32']:
        ret = conf.check_cfg(package=pkg, args='--variable=%s --define-variable=prefix=%s'%(var, conf.env['PKG_CONFIG_PREFIX'] ), okmsg=myokmsg,
                msg='Checking for var %s in %s' % (var, pkg)).strip()
    else:
        ret = conf.check_cfg(package=pkg, args='--variable=%s' % var, okmsg=myokmsg,
                msg='Checking for var %s in %s' % (var, pkg)).strip()
    return ret


def check_pkg(conf, name, version='', mandatory=False, var=None):
    """
    Check package version and its cflags/libs
    """
    if not var:
        var = name.split ('-')[0].upper ()

    # for cross-compilation it is necessary to prepend custom
    # prefix for include paths
    if conf.env['IS_CROSS_WIN32']:
        conf.check_cfg (package=name, uselib_store=var, args='--cflags --libs --define-variable=prefix=%s'%conf.env['PKG_CONFIG_PREFIX'],
            atleast_version=version, mandatory=mandatory)
    else:
        conf.check_cfg (package=name, uselib_store=var, args='--cflags --libs',
            atleast_version=version, mandatory=mandatory)

    # make available libs to all source files
    #(compile code with cflags from pkgconf e.g. -fshort-wchar, -mms-bitfields)
    conf.env.append_value('ALL_LIBS', var)


def check_pkg_msg(conf, name, version='', mandatory=False, var=None, msg=''):
    """
    Check package version and its cflags/libs
    """
    if not var:
        var = name.split ('-')[0].upper ()
    conf.check_cfg (package=name, uselib_store=var, args='--cflags --libs',
        atleast_version=version, mandatory=mandatory, msg=msg)

    # make available libs to all source files
    #(compile code with cflags from pkgconf e.g. -fshort-wchar, -mms-bitfields)
    conf.env.append_value('ALL_LIBS', var)




def check_pkgver(conf, name, version='', mandatory=False, var=None):
    """
    Check package version without cflags/libs
    """
    if not var:
        var = name.split ('-')[0].upper ()
    conf.check_cfg (package=name, uselib_store=var,
        atleast_version=version, mandatory=mandatory)


def check_pkgver_msg(conf, name, version='', mandatory=False, var=None, msg=''):
    """
    Check package version without cflags/libs
    """
    if not var:
        var = name.split ('-')[0].upper ()
    conf.check_cfg (package=name, uselib_store=var,
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
    normpath(path)

    # double backslashes '\\' needs to be written to config.h in win32 paths in win32
    path = '%r' % path # convert string to raw form - '%r'
    max = len(path) - 1
    path = path[1:max] # trim apostrophes at begin and end

    return path
    

