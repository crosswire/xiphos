#!/usr/bin/env python
# encoding: utf-8

# Inspiration from gnome.py from waf

"Create help files"

import os, re
import TaskGen, Utils, Runner, Task, Build, Options, Logs
import cc
from Logs import error
from TaskGen import taskgen, before, after, feature


class xml_doc_taskgen(TaskGen.task_gen):
    def __init__(self, *k, **kw):
            TaskGen.task_gen.__init__(self, *k, **kw)

@feature('xml_doc')
def init_xml_doc(self):
    if self.env['IS_WIN32']:
        self.default_install_path = '${PREFIX}/share/help'
    else:
        self.default_install_path = '${PREFIX}/share/gnome/help/xiphos'

@feature('xml_doc')
@after('init_xml_doc')
def apply_xml_doc(self):
    lst = self.to_list(self.doc_linguas)
    bld = self.bld
    for x in lst:
        # xml2po - localized help
        tsk = self.create_task('xml2po')
        node = self.path.find_resource(os.path.join(x, x+'.po'))
        src = self.path.find_resource(os.path.join('C', self.doc_module+'.xml'))
        out = self.path.find_or_declare(os.path.join(x, self.doc_module+'.xml'))
        tsk.set_inputs([node, src])
        tsk.set_outputs(out)

        # FIXME - implement making CHM help
        if self.env['IS_WIN32']:
            pass
            # xsltproc - create html files

            #tsk2 = self.create_task('xsltproc')
            #out2 = self.path.find_or_declare('%s/%s-%s.omf' % (x, self.doc_module, x))
            #tsk2.set_outputs(out2)
            #node = self.path.find_resource(self.doc_module+".omf.in")
            #tsk2.inputs = [node, out]

            # htmlhelp - MS help in CHM format

            #tsk2.run_after.append(tsk)
            #tsk3.run_after.append(tsk2)

        if bld.is_install:
            path = os.path.join(self.install_path, x)
            for y in self.to_list(self.doc_figures):
                try:
                    os.stat(os.path.join(self.path.abspath(), x, y))
                    bld.install_as(path + '/' + y, self.path.abspath() + '/' + x + '/' + y)
                except:
                    bld.install_as(path + '/' + y, self.path.abspath() + '/C/' + y)
            bld.install_as(path + '/%s.xml' % self.doc_module, out.abspath(self.env))

    # install help for 'C'
    if bld.is_install:
        path = os.path.join(self.install_path, 'C')
        lst = self.to_list(self.doc_includes) + self.to_list(self.doc_figures)
        for y in lst:
            bld.install_as(path + '/' + y, self.path.abspath() + '/' + 'C' + '/' + y)
        # install 'xiphos.xml' and 'legal.xml'
        bld.install_as(path + '/%s.xml' % self.doc_module,
                self.path.abspath() + '/C/%s.xml' % self.doc_module)
        bld.install_as(path + '/%s.xml' % self.doc_entities,
                self.path.abspath() + '/C/%s' % self.doc_entities)



Task.simple_task_type('xml2po', '${XML2PO} ${XML2PO_FLAGS} ${SRC} > ${TGT}', color='BLUE')

# how do you expect someone to understand this?!
#xslt_magic = """${XSLTPROC2PO} -o ${TGT[0].abspath(env)} \
#--stringparam db2omf.basename ${APPNAME} \
#--stringparam db2omf.format docbook \
#--stringparam db2omf.lang C \
#--stringparam db2omf.dtd '-//OASIS//DTD DocBook XML V4.3//EN' \
#--stringparam db2omf.omf_dir ${PREFIX}/share/omf \
#--stringparam db2omf.help_dir ${PREFIX}/share/gnome/help \
#--stringparam db2omf.omf_in ${SRC[0].abspath(env)} \
#--stringparam db2omf.scrollkeeper_cl ${SCROLLKEEPER_DATADIR}/Templates/C/scrollkeeper_cl.xml \
#${DB2OMF} ${SRC[1].abspath(env)}"""

#Task.simple_task_type('xsltproc2po', xslt_magic, color='BLUE')

def detect(conf):
    #conf.check_tool('gnu_dirs glib2 dbus')
    #sgml2man = conf.find_program('docbook2man', var='SGML2MAN')

    #def getstr(varname):
            #return getattr(Options.options, varname, '')

    # addefine also sets the variable to the env
    #conf.define('GNOMELOCALEDIR', os.path.join(conf.env['DATADIR'], 'locale'))

    # FIXME - add python to find_program - on win32 'xml2po' is not executable
    #         find_file
    xml2po = conf.find_program('xml2po', var='XML2PO')

    # flags ensure that translated XML will be in one file
    conf.env['XML2PO_FLAGS'] = '-e -p'

    if conf.env['IS_WIN32']:
        # FIXME - any xsltproc flags necessary?
        xsltproc = conf.find_program('xsltproc', var='XSLTPROC')
        htmlhelpc = conf.find_program('hhc', var='HTMLHELPC')
        
