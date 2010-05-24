#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2005-2008 (ita)

import sys, random, time, traceback, os
import Build, Utils, Logs, Options
from Logs import debug, error
from Constants import *

class TaskConsumer(object):
	consumers = 1

def process(tsk):
	m = tsk.master
	if m.stop:
		m.out.put(tsk)
		return

	try:
		tsk.generator.bld.printout(tsk.display())
		if tsk.__class__.stat: ret = tsk.__class__.stat(tsk)
		# actual call to task's run() function
		else: ret = tsk.call_run()
	except Exception, e:
		tsk.err_msg = Utils.ex_stack()
		tsk.hasrun = EXCEPTION

		# TODO cleanup
		m.error_handler(tsk)
		m.out.put(tsk)
		return

	if ret:
		tsk.err_code = ret
		tsk.hasrun = CRASHED
	else:
		try:
			tsk.post_run()
		except Utils.WafError:
			pass
		except Exception:
			tsk.err_msg = Utils.ex_stack()
			tsk.hasrun = EXCEPTION
		else:
			tsk.hasrun = SUCCESS
	if tsk.hasrun != SUCCESS:
		m.error_handler(tsk)

	m.out.put(tsk)

def start(self):

	while not self.stop:

		self.refill_task_list()

		# consider the next task
		tsk = self.get_next()
		if not tsk:
			if self.count:
				# tasks may add new ones after they are run
				continue
			else:
				# no tasks to run, no tasks running, time to exit
				break

		if tsk.hasrun:
			# if the task is marked as "run", just skip it
			self.processed += 1
			self.manager.add_finished(tsk)
			continue

		try:
			st = tsk.runnable_status()
		except Exception, e:
			self.processed += 1
			if self.stop and not Options.options.keep:
				tsk.hasrun = SKIPPED
				self.manager.add_finished(tsk)
				continue
			self.error_handler(tsk)
			self.manager.add_finished(tsk)
			tsk.hasrun = EXCEPTION
			tsk.err_msg = Utils.ex_stack()
			continue

		if st == ASK_LATER:
			self.postpone(tsk)
		elif st == SKIP_ME:
			self.processed += 1
			tsk.hasrun = SKIPPED
			self.manager.add_finished(tsk)
		else:
			# run me: put the task in ready queue
			tsk.position = (self.processed, self.total)
			self.count += 1
			self.processed += 1
			tsk.master = self

			process(tsk)

	# self.count represents the tasks that have been made available to the consumer threads
	# collect all the tasks after an error else the message may be incomplete
	while self.error and self.count:
		self.get_out()

	#print loop
	assert (self.count == 0 or self.stop)


# enable nothreads if -j1 is used from the makefile
if os.environ.get('JOBS') == '1' or sys.platform == 'linux2-hppa':
	import Runner
	Runner.Parallel.start = start
