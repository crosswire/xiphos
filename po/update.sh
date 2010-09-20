#!/bin/sh
intltool-update --pot -g xiphos
langs=`ls *.po | sed "s/\(.*\).po/\1/"`
for l in $langs; do
    intltool-update -g xiphos $l
done;
