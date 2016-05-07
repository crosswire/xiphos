#!/usr/bin/env python
# encoding: utf-8

"Create help files"

import os, re, shutil
import Configure, TaskGen, Utils, Runner, Task, Build, Options, Logs
import cc
from Logs import error
from TaskGen import taskgen, before, after, feature


class xml_doc_taskgen(TaskGen.task_gen):
    def __init__(self, *k, **kw):
        TaskGen.task_gen.__init__(self, *k, **kw)


@feature('xml_doc')
def init_xml_doc(self):
    self.env['XML2PO_FLAGS'] = getattr(self, 'flags', ['-e', '-p'])

    if self.env['IS_WIN32']:
        self.default_install_path = '${PREFIX}/share/help'
        self.env['HTMLHELP_XSL'] = os.path.normpath(os.path.join(
            self.env['ROOTDIR'], self.env['ROOTDIR'], 'share/docbook/htmlhelp/htmlhelp.xsl'))
    else:
        self.default_install_path = '${PREFIX}/share/gnome/help/xiphos'


@feature('xml_doc')
@after('init_xml_doc')
def apply_xml_doc(self):

    def replace_backslash(node):
        return node.abspath().replace('\\', '/')

    def create_xml2po_task(lang):
        node = self.path.find_resource(lang + os.sep + lang+'.po')
        src = self.path.find_resource('C' + os.sep + self.doc_module+'.xml')
        out = self.path.find_or_declare(lang + os.sep + self.doc_module+'.xml')

        if self.env['IS_WIN32']:
            # FIXME - xml2po on win32 fails with flags containing backslashes '\'
            # failing example:  python xml2po -e -p fr\fr.po C\xiphos.xml
            node_str = replace_backslash(node)
            src_str = replace_backslash(src)

            # it is necessary pas to xml2po only paths with '/'
            Task.simple_task_type('xml2po',
                    '${XML2PO} -e -p %s %s > ${TGT}' % (node_str, src_str),
                    color='BLUE')

        tsk = self.create_task('xml2po')
        tsk.set_inputs([node, src])
        # waf will control output file if it exists, when task is done
        tsk.set_outputs(out)
        return tsk, out

    def create_xsltproc_task(lang, src, after=[]):
        out = self.path.find_or_declare(lang + os.sep + 'htmlhelp.hhp')
        tsk = self.create_task('xsltproc')
        tsk.set_inputs(src)
        tsk.set_outputs(out)
        if after:
            tsk.run_after.append(after)
        return tsk, out

    def create_htmlhelpc_task(lang, src, after=[]):
        out = self.path.find_or_declare(lang + os.sep + 'htmlhelp.chm')
        tsk = self.create_task('htmlhelpc')
        tsk.set_inputs(src)
        tsk.set_outputs(out)
        tsk.run_after.append(after)
        return tsk, out

    def copy_figures(lang, figures, out):
        fig1 = self.path.find_resource(os.path.join(lang, figures[0]))
        srcdir = os.path.dirname(os.path.dirname(fig1.abspath(self.env)))
        destdir = os.path.dirname(out.abspath(self.env))
        for fig in figures:
            shutil.copyfile(os.path.join(srcdir, fig), os.path.join(destdir, fig))

    def install_help(lang, self):
        if self.env['IS_WIN32']:
            chm_help = self.path.find_or_declare(lang + os.sep + 'htmlhelp.chm')
            path = self.install_path
            help_file = self.doc_module + '_' + lang + '.chm'
            self.bld.install_as(os.path.join(path, help_file), chm_help.abspath(self.env))
        else:
            xml_help = self.path.find_or_declare(os.path.join(lang, self.doc_module+'.xml'))
            path = os.path.join(self.install_path, lang)
            bld = self.bld
            # install xiphos.xml
            bld.install_as(os.path.join(path, xml_help.file()), xml_help.abspath(self.env))
            install_figures(lang, self)

    def install_help_C(self):
        if self.env['IS_WIN32']:
            chm_help = self.path.find_or_declare('C' + os.sep + 'htmlhelp.chm')
            path = self.install_path
            self.bld.install_as(os.path.join(path, self.doc_module + '.chm'), chm_help.abspath(self.env))
        else:
            xml_help = self.path.find_resource('C' + os.sep + self.doc_module+'.xml')
            path = self.install_path + os.sep + 'C'
            bld = self.bld
            # lang C needs all xml files
            lst = self.to_list(self.doc_includes)
            for item in lst:
                bld.install_as(os.path.join(path, item),
                        os.path.join(self.path.abspath(), 'C', item))
            bld.install_as(os.path.join(path, self.doc_entities),
                    os.path.join(self.path.abspath(), 'C',  self.doc_entities))
            bld.install_as(os.path.join(path, xml_help.file()), xml_help.abspath(self.env))
            install_figures('C', self)

    def install_figures(lang, self):
        figures = self.to_list(self.doc_figures)
        for fig in figures:
            self.bld.install_as(os.path.join(self.install_path, lang, fig),
                    os.path.join(self.path.abspath(), lang, fig))


    figures = self.to_list(self.doc_figures)
    languages = self.to_list(self.doc_linguas)
    # C help - doesn't need xml2po phase
    # on win32 just xsltproc and htmlhelpc
    if self.env['IS_WIN32']:
        C_src = self.path.find_resource('C' + os.sep + self.doc_module+'.xml')
        C_tsk, C_out = create_xsltproc_task('C', C_src)
        copy_figures('C', figures, C_out)
        create_htmlhelpc_task('C', C_out, C_tsk)

    # translated help
    for lang in languages:
        # xml2po - localized help
        tsk, out = create_xml2po_task(lang)

        if self.env['IS_WIN32']:
            # xsltproc - create html files
            tsk2, out2 = create_xsltproc_task(lang, out, tsk)
            # copy figures - hhc needs figures in the build dir
            copy_figures(lang, figures, out2)
            # CHM help - windows help format
            tsk3, out3 = create_htmlhelpc_task(lang, out2, tsk2)

        if self.bld.is_install:
            install_help(lang, self)

    # install C help
    install_help_C(self)


def exec_xsltproc(self):
    '''
    xsltproc save files to cwd (current working directory).
    CWD in this case is folder where is translated documentation.
    '''
    env = self.env
    input = self.inputs[0].abspath(env)
    cwd = os.path.dirname(self.outputs[0].abspath(env))

    # cmd='xsltproc htmlhelp.xsl xiphos.xml'
    lst = []
    lst.append(env['XSLTPROC'])
    lst.append(env['HTMLHELP_XSL'])
    lst.append(input)

    lst = [lst]
    ret = self.exec_command(*lst, cwd=cwd)

    return ret

def exec_htmlhelpc(self):
    '''
    hhc also saves files to cwd (current working directory).
    '''
    env = self.env
    input = self.inputs[0].abspath(env)
    cwd = os.path.dirname(input)

    lst = []
    lst.append(env['HTMLHELPC'])
    lst.append(input)

    lst = [lst]
    ret = self.exec_command(*lst, cwd=cwd)

    return not ret


Task.simple_task_type('xml2po', '${XML2PO} -o ${TGT} ${XML2PO_FLAGS} ${SRC}', color='BLUE')
Task.task_type_from_func('xsltproc', vars=['XSLTPROC', 'HTMLHELP_XSL'], color='BLUE', func=exec_xsltproc)
Task.task_type_from_func('htmlhelpc', vars=['HTMLHELPC'], color='BLUE', func=exec_htmlhelpc)


def detect(conf):
    # xml2po
    xml2po = conf.find_program('xml2po', var='XML2PO')
    if not xml2po:
        # xml2po: on win32 'xml2po' is not executable thus this hook
        if conf.env['IS_WIN32']:
            python = conf.find_program('python', var='PYTHON')
            if not python:
                conf.fatal('The program python (required by xml2po) could not be found')

            xml2podir = Configure.find_file('xml2po', os.environ['PATH'].split(os.pathsep))
            if not xml2podir:
                conf.fatal('The program xml2po is mandatory!')

            conf.env['XML2PO'] = Utils.to_list(conf.env['PYTHON']) + [xml2podir + os.sep + 'xml2po']
            conf.check_message('xml2po', '', True, ' '.join(conf.env['XML2PO']))

        else:
            conf.fatal('The program xml2po is mandatory!')


    if conf.env['IS_WIN32']:
        xsltproc = conf.find_program('xsltproc', var='XSLTPROC')
        htmlhelpc = conf.find_program('hhc', var='HTMLHELPC')

