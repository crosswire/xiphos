def configure(ctx):
    if env['IS_CROSS_WIN32']:
        env['STRIP'] = conf.find_program('i686-w64-mingw32-strip', mandatory=True)
    else:
        env['STRIP'] = conf.find_program('strip', mandatory=True)

    if (not env['IS_LINUX']) and (opt.debug_level in ['release','optimized']):
        env['TO_STRIP'] = True
    else:
        env['TO_STRIP'] = opt.strip


