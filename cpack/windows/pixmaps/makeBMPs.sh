#!/bin/bash

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

function makeBMP { convert $1 PPM:- | convert PPM:- $2; }

#converted via PPM format to remove gAMA and cHRM chunks so resulting 
#BMP is an older BMP3 instead of BMP4, as seems to be required by NSIS.
#-- from http://www.cit.gu.edu.au/~anthony/info/graphics/imagemagick.hints

makeBMP header.svg header.bmp
makeBMP header-r.svg header-r.bmp
cp header.bmp header-uninstall.bmp
cp header-r.bmp header-uninstall-r.bmp
makeBMP wizard.png wizard.bmp
cp wizard.bmp wizard-uninstall.bmp

