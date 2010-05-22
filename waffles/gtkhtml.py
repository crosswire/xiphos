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
        #success = False
        success = self.try_api314()

        #if not success:
            #success = self.try_38()

        return success



    def try_api314(self):

        ret = False
        dfn = self.conf.define

        check_pkg(self.conf, 'libgtkhtml-3.14', var='GTKHTML')

        if self.env['HAVE_GTKHTML']:
            #dfn('USE_GTKHTML38', 1)
            dfn('USE_GTKHTML3_14', 1)
            self.set_vars('3.14')
            self.check_version323()
            ret = True

        return ret


    #def try_api38(self):
 
        #ret = False
#
        #check_pkg(self.conf, 'libgtkhtml-3.8', mandatory=True, var='GTKHTML')

        #if self.env['GTKHTML']:
            #self.conf.define('USE_GTKHTML38', 1)
            #self.set_vars('3.8')
            #ret = True

        #return ret


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
        check_pkgver(self.conf, 'libgtkhtml-3.14', '3.23', var='EDITOR_IDL')

        if self.conf.env['HAVE_EDITOR_IDL']:
            check_pkg(self.conf, 'gtkhtml-editor', mandatory=True, var='GTKHTML_EDITOR')
            self.conf.define('USE_GTKHTML3_14_23', 1)


