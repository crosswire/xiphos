from Configure import conf

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

@conf
def check_pkg(self, name, version='', mandatory=False, var=None):
    """
    Check package version and its cflags/libs
    """
    if not var:
        var = name.split ('-')[0].upper ()

    # for cross-compilation it is necessary to prepend custom
    # prefix for include paths
    if self.env['IS_CROSS_WIN32']:
        self.check_cfg (package=name, uselib_store=var, args='--cflags --libs --define-variable=prefix=%s'%self.env['PKG_CONFIG_PREFIX'],
            atleast_version=version, mandatory=mandatory)
    else:
        self.check_cfg (package=name, uselib_store=var, args='--cflags --libs',
            atleast_version=version, mandatory=mandatory)

    # make available libs to all source files
    #(compile code with cflags from pkgconf e.g. -fshort-wchar, -mms-bitfields)
    self.env.append_value('ALL_LIBS', var)

@conf
def check_pkg_msg(self, name, version='', mandatory=False, var=None, msg=''):
    """
    Check package version and its cflags/libs
    """
    if not var:
        var = name.split ('-')[0].upper ()
    self.check_cfg (package=name, uselib_store=var, args='--cflags --libs',
        atleast_version=version, mandatory=mandatory, msg=msg)

    # make available libs to all source files
    #(compile code with cflags from pkgconf e.g. -fshort-wchar, -mms-bitfields)
    self.env.append_value('ALL_LIBS', var)

@conf
def get_pkgvar(self, pkg, var):
    """
    Read a variable from package using pkg-config
    """

    # for cross-compilation it is necessary to prepend custom
    # prefix for include paths
    if self.env['IS_CROSS_WIN32']:
        ret = self.check_cfg(package=pkg, args='--variable=%s --define-variable=prefix=%s'%(var, self.env['PKG_CONFIG_PREFIX'] ), okmsg=myokmsg,
                msg='Checking for var %s in %s' % (var, pkg)).strip()
    else:
        ret = self.check_cfg(package=pkg, args='--variable=%s' % var, okmsg=myokmsg,
                msg='Checking for var %s in %s' % (var, pkg)).strip()
    return ret

@conf
def check_pkgver(self, name, version='', mandatory=False, var=None):
    """
    Check package version without cflags/libs
    """
    if not var:
        var = name.split ('-')[0].upper ()
    self.check_cfg (package=name, uselib_store=var,
        atleast_version=version, mandatory=mandatory)

@conf
def check_pkgver_msg(self, name, version='', mandatory=False, var=None, msg=''):
    """
    Check package version without cflags/libs
    """
    if not var:
        var = name.split ('-')[0].upper ()
    self.check_cfg (package=name, uselib_store=var,
        atleast_version=version, mandatory=mandatory, msg=msg)


def escape(string):
    """
    Escape string
    Useful to use for string and paths before writing them to config.h
    """
    return string.decode('string-escape')

@conf
def escpath(self, path):
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
    

