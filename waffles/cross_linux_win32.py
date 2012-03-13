#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2008 (ita)

import os
import Utils
import ccroot
from Configure import conftest

@conftest
def find_mingw_cc(conf):
	v = conf.env
	v['CC'] = None
	cc = conf.find_program('%sgcc' % (v['CROSS'],), var='CC')
	if not cc: cc = conf.find_program('i686-pc-mingw32-gcc', var='CC')
	if not cc: cc = conf.find_program('mingw32-cc', var='CC')
	if not cc: cc = conf.find_program('i586-mingw32msvc-gcc', var='CC')
	if not cc: cc = conf.find_program('i586-mingw32msvc-cc', var='CC')
	if not cc: conf.fatal('mingw32-gcc was not found')
	try:
		if Utils.cmd_output('%s --version' % cc).find('mingw') < 0:
			conf.fatal('mingw32-gcc was not found, see the result of gcc --version')
	except ValueError:
		conf.fatal('gcc --version could not be executed')
	v['CC'] = v['LINK_CC'] = cc
	v['CC_NAME'] = 'gcc'
	ccroot.get_cc_version(conf, [cc], 'CC_VERSION')

@conftest
def find_mingw_ar(conf):
	v = conf.env
	v['AR'] = None
	ar = conf.find_program('%sar' % (v['CROSS'],), var='AR')
	if not ar: ar = conf.find_program('i686-pc-mingw32-ar', var='AR')
	if not ar: ar = conf.find_program('i586-mingw32msvc-ar', var='AR')
	if not ar: conf.fatal('mingw32-ar was not found')
	try:
		if Utils.cmd_output('%s --version' % ar).find('ar') < 0:
			conf.fatal('mingw32-ar was not found, see the result of %s --version' % ar)
	except ValueError:
		conf.fatal('ar --version could not be executed')
	v['AR_NAME'] = 'ar'

@conftest
def find_mingw_ranlib(conf):
	v = conf.env
	v['RANLIB'] = None
	ranlib = conf.find_program('%sranlib' % (v['CROSS'],), var='RANLIB')
	if not ranlib: ranlib = conf.find_program('i686-pc-mingw32-ranlib', var='RANLIB')
	if not ranlib: ranlib = conf.find_program('i586-mingw32msvc-ranlib', var='RANLIB')
	if not ranlib: conf.fatal('mingw32-ranlib was not found')
	try:
		if Utils.cmd_output('%s --version' % ranlib).find('ranlib') < 0:
			conf.fatal('mingw32-ranlib was not found, see the result of mingw32-ranlib --version')
	except ValueError:
		conf.fatal('ranlib --version could not be executed')
	v['RANLIB_NAME'] = 'ranlib'


@conftest
def find_mingw_cxx_cpp(conf):
	v = conf.env
	v['CXX'] = None
	cxx = conf.find_program('%sg++' % (v['CROSS'],), var='CXX')
	if not cxx: cxx = conf.find_program('i686-pc-mingw32-g++', var='CXX')
	if not cxx: cxx = conf.find_program('mingw32-c++', var='CXX')
	if not cxx: cxx = conf.find_program('i586-mingw32msvc-g++', var='CXX')
	if not cxx: cxx = conf.find_program('i586-mingw32msvc-c++', var='CXX')
	if not cxx: conf.fatal('mingw32-g++ was not found')
	try:
		if Utils.cmd_output('%s --version' % cxx).find('mingw') < 0:
			conf.fatal('mingw32-g++ was not found, see the result of g++ --version')
	except ValueError:
		conf.fatal('g++ --version could not be executed')
	v['CXX'] = v['LINK_CXX'] = v['COMPILER_CXX'] = v['CPP'] = v['LINK_CPP'] = cxx
	v['CXX_NAME'] = 'gcc'

@conftest
def mingw_flags(conf):
	# As we have to change target platform after the tools
	# have been loaded there are a few variables that needs
	# to be initiated if building for win32.
	# Make sure we don't have -fPIC and/or -DPIC in our CCFLAGS
	conf.env['shlib_CXXFLAGS'] = []

	# Setup various target file patterns
	conf.env['staticlib_PATTERN'] = '%s.lib'
	conf.env['shlib_PATTERN'] = '%s.dll'
	conf.env['program_PATTERN'] = '%s.exe'

@conftest
def mingw_libpath(conf):
	# We must add the library and includes paths of Windows libs
	msvc_path = conf.env['MSVC_LIBPATH'] or os.environ.get('MSVC_LIBPATH', '')
	if not os.path.isdir(msvc_path): msvc_path = '/usr/i686-pc-mingw32'
	if not os.path.isdir(msvc_path): msvc_path = '/usr/mingw32'
	if not os.path.isdir(msvc_path): msvc_path = '/usr/i586-mingw32msvc'
	if not os.path.isdir(msvc_path):
		conf.fatal('Could not find the msvc root dir - set it through MSVC_LIBPATH in the environment')
	conf.env.prepend_value("LIBPATH", os.path.join(msvc_path, "lib"))
	conf.env.prepend_value("CPPPATH", os.path.join(msvc_path, "include"))

detect = 'find_mingw_ar find_mingw_ranlib find_mingw_cc find_mingw_cxx_cpp mingw_flags mingw_libpath'

