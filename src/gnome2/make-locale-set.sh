#!/bin/sh
#
# convert LINGUAS file into a C source file containing full locale names.
# names in LINGUAS are either 5-char full "language_LOCALE" names or
# 2-char "language-only" names.  convert the latter.
#
( echo 'char *locale_set[] = { '
sed -e 's/en_GB/& en_US/' < $1 |
tr ' ' '\n' |
while read l ; do
    case "$l" in
	*_*)
	    ;;
	*)  u="`echo \"$l\" | tr a-z A-Z`"
	    [ "$u" = CS ] && u=CZ	# special case, czech.
	    [ "$u" = FA ] && u=IR	# special case, iran.
	    [ "$u" = ZH ] && u=CN	# special case, chinese.
	    [ "$u" = HE ] && u=IL	# special case, israel.
	    [ "$u" = NB ] && u=NO	# special case, norway.
	    l="$l"_"$u"
	    ;;
    esac
    echo '	"'"$l"'",'
done
echo '        (char*)0'
echo '};' ) |
uniq
