#!/usr/bin/env python

#Copyright (c) 2009 Simon Meers

#Permission is hereby granted, free of charge, to any person
#obtaining a copy of this software and associated documentation
#files (the "Software"), to deal in the Software without
#restriction, including without limitation the rights to use,
#copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the
#Software is furnished to do so, subject to the following
#conditions:

#The above copyright notice and this permission notice shall be
#included in all copies or substantial portions of the Software.

#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#OTHER DEALINGS IN THE SOFTWARE.

pattern = "*.png"

scm = """
(define (save_indexed_png in_file out_file)
	(let* 
		(
			(image (car (gimp-file-load RUN-NONINTERACTIVE in_file in_file)))
			(drawable (car (gimp-image-get-active-layer image)))
		)
		(print in_file)
		(if (= (car (gimp-drawable-is-indexed drawable)) FALSE)
			(begin
				(print "converting")
				(gimp-image-convert-indexed image 0 0 255 0 1 "none")
				(file-png-save RUN-NONINTERACTIVE image drawable out_file out_file 0 9 1 0 0 1 1)
			)
			(print "already indexed")
		)
		(gimp-image-delete image)
	)
)

(define (save_indexed_png_glob pattern)
	(let* 
		(
			(filelist (cadr (file-glob pattern 1)))
		)
		(while (not (null? filelist))
			(save_indexed_png (car filelist) (car filelist))
			(set! filelist (cdr filelist))
		)
	)
)
(save_indexed_png_glob "%s")
(gimp-quit 0)
""" % (pattern, )

import sys
def y_or_n():
	while True:
		response = sys.stdin.readline().strip()
		if response == 'y': return True
		elif response == 'n': return False
		else: print "Please enter 'y' or 'n'"
		
sys.stdout.write("""WARNING: 
  This script will permanently convert (and reduce the quality of)
  all files matching the pattern "%s" in this directory. 
  Please ensure you have backed up the originals. 
  PNGs already indexed will not be affected. 
  Are you sure you wish to proceed? (y/n) """ % (pattern, ))

if y_or_n():
	from subprocess import *
	from tempfile import TemporaryFile
	t = TemporaryFile()
	t.writelines(scm)
	t.seek(0)
	p = Popen("gimp -ib -", shell=True, stdin=t)
	p.communicate() #wait for completion
else:
	print "Conversion aborted."

