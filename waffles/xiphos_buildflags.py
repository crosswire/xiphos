    miscgroup = ctx.add_option_group('Miscellaneous Options')

    miscgroup.add_option('-d', '--debug-level',
                action = 'store',
		default = 'ultradebug',
		help = "Specify the debugging level ['ultradebug', 'debug', 'release', 'optimized']",
		choices = ['ultradebug', 'debug', 'release', 'optimized'],
		dest = 'debug_level')

    miscgroup.add_option('--strip', action='store_true', default=False,
                    help='Strip resulting binary')

    miscgroup.add_option('--enable-delint', action='store_true', default=False,
            dest='delint',
            help='Use -Wall -Werror [default: disabled]')


    # appropriate cflags
    env.append_value('CXXFLAGS', env['CXXFLAGS_%s' % opt.debug_level.upper()])
    env.append_value('CCFLAGS', env['CCFLAGS_%s' % opt.debug_level.upper()])

    if opt.delint:
        env.append_value('CXXFLAGS', env['CXXFLAGS_DELINT'])
        env.append_value('CCFLAGS', env['CCFLAGS_DELINT'])

    # portable adding define, should work gcc and msvc
    env.append_value('CXXFLAGS', env['CXXDEFINES_ST'] % 'HAVE_CONFIG_H')
    env.append_value('CCFLAGS', env['CCDEFINES_ST'] % 'HAVE_CONFIG_H')

    env.append_value('CCFLAGS', env['CCFLAGS_SAFE'])
    env.append_value('CXXFLAGS', env['CXXFLAGS_SAFE'])
