#!/bin/sh
#
# convert LINGUAS file into a C source file containing full locale names.
# names in LINGUAS are either 5-char full "language_LOCALE" names or
# 2-char "language-only" names.  convert the latter.
#
echo 'char *locale_set[] = { '
sed -e 's/en_GB/& en_US/' < ../../po/LINGUAS |
tr ' ' '\n' |
while read l ; do
    case "$l" in
	*_*)
	    ;;
	*)  u="`echo \"$l\" | tr a-z A-Z`"
	    [ "$u" = FA ] && u=IR	# special case, iran.
	    l="$l"_"$u"
	    ;;
    esac
    echo '	"'"$l.UTF-8"'",'
done
echo '        (char*)0'
echo '};'
