"""
Stuff to detect GTKHTML
and generate Editor-* sources for
component editor
"""

from waffles.misc import *

# TODO: adjust for compilation with GTKHTML backend
class Gtkhtml(object):

    def __init__(self, conf):
        self.conf = conf
        self.env = conf.env

    def detect(self):
        pass
        """
        Detect gtkhtml in system
        """
        success = False
        success = self.try_api314()

        if not success:
            success = self.try_38()

        return success



    def try_api314(self):

        ret = False
        dfn = self.conf.define

        if check_package(self.conf, 'libgtkhtml-3.14', 'GTKHTML'):
            dfn('USE_GTKHTML38', 1)
            dfn('USE_GTKHTML3_14', 1)
            self.set_vars('3.14')
            ret = True

        return ret


    def try_api38(self):
 
        ret = False

        if check_package(self.conf, 'libgtkhtml-3.8', 'GTKHTML'):
            self.conf.define('USE_GTKHTML38', 1)
            self.set_vars('3.8')
            ret = True

        return ret


    def set_vars(self, api_ver):

        dfn = self.conf.define
        api = get_pkgvar(self.conf, 'libgtkhtml-%s' % api_ver,
                'gtkhtml_apiversion')
        data_dir = get_pkgvar(self.conf, 'libgtkhtml-%s' % api_ver,
                'gtkhtml_datadir')

        self.env['GTKHTML_DATA'] = data_dir
        dfn('GTKHTML_API_VERSION', api)
        dfn('GTKHTML_DATA_DIR', data_dir)


    def check_version323(self):
        """
        gtkhtml >=3.23 don't need generating sources form Editor.idl
        """
        if check_pkgver(self.conf, 'libgtkhtml-3.14', '3.23'):
            check_package(self.conf, 'gtkhtml-editor', 'GTKHTML_EDITOR', True)
            env['HAVE_GTKHTML3_23'] = True
            self.conf.define('USE_GTKHTML3_14_23', 1)
        else:
            env['HAVE_GTKHTML3_23'] = False


