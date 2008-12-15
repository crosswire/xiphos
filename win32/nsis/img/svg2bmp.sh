#!/bin/bash

for i in header header-r header-uninstall header-uninstall-r wizard wizard-uninstall
do
	convert $i.svg PPM:- | convert PPM:- $i.bmp
done

#converted via PPM format to remove gAMA and cHRM chunks so resulting 
#BMP is an older BMP3 instead of BMP4, as seems to be required by NSIS.
#-- from http://www.cit.gu.edu.au/~anthony/info/graphics/imagemagick.hints
