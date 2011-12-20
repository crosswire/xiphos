#!/usr/bin/env python
# encoding: utf-8

from waflib.Configure import conf

def options(ctx):
    maingroup = ctx.add_option_group('Main Xiphos Options')

    maingroup.add_option('--gtk',
                   action='store', default='auto', dest='gtkver',
                   choices=['auto', '3', '2'],
                   help="Select gtk+ API ['auto', '3', '2']")

    maingroup.add_option('--backend',
                   action='store', default='auto', dest='backend',
                   choices=['auto', 'webkit', 'gtkhtml'],
                   help="Select rendering backend ['auto', 'webkit', 'xulrunner', 'gtkhtml']")

@conf
def pick_backend(ctx):
    ctx.env.gtkver = ctx.options.gtkver
    ctx.env.backend = ctx.options.backend

    def select_backend(name, option, libs):
        if ctx.env[option] != 'auto':        
            libs = (ctx.env[option], dict(libs)[ctx.env[option]])

        for try_opt, try_lib in libs:
            if ctx.check_cfg(modversion=try_lib, mandatory=False):
                ctx.env[option] = try_opt
                ctx.env.common_libs.append(try_lib)
                break

        if ctx.env[option] == 'auto':
            ctx.fatal('Could not find any sutable %s' % name)

        ctx.msg('Detecting %s' % name, ctx.env[option])
        
    select_backend('GTK+ version', 'gtkver', [('3', 'gtk+-3.0'), ('2', 'gtk+-2.0')])

    if ctx.env.gtkver == '3':
        ctx.define('USE_GTK_3', 1)
	ctx.define('USE_GTKBUILDER', 1)
        ctx.env.common_libs.extend(['libgtkhtml-4.0','gtkhtml-editor-4.0'])
        web_libs = [('webkit', 'webkitgtk-3.0'), ('gtkhtml', 'libgtkhtml-4.0')]
    else:
        ctx.undefine('USE_GTK_3')
        ctx.env.common_libs.extend(['libglade-2.0', 'libgtkhtml-3.14 >= 3.23', 'gtkhtml-editor-3.14'])
        web_libs = [('webkit', 'webkit-1.0'), ('gtkhtml', 'libgtkhtml-3.14')]

    select_backend('rendering backend', 'backend', web_libs)

    if ctx.env.backend == 'webkit':
        ctx.define('USE_WEBKIT', 1)

    if ctx.env.backend == 'gtkhtml':
        ctx.define('ENABLE_GTKHTML', 1)

def configure(ctx):
    ctx.pick_backend()


