# A debhelper build system class for handling Waf based projects.
# Copyright: © 2009 Dmitrijs Ledkovs
# License: GPL-2+

package Debian::Debhelper::Buildsystem::waf_src;

use strict;
use base "Debian::Debhelper::Buildsystem::waf";

sub DESCRIPTION {
	"waf using source directory script";
}

sub check_auto_buildable {
	my ( $this, $step ) = @_;
	#TODO check that waf is executable or run as "python waf"?
	return -e $this->get_sourcepath("waf") && -e $this->get_sourcepath("wscript") ? 1 : 0;
}

sub _do_waf {
	my $this = shift;
	return $this->doit_in_sourcedir( "./waf", "--nocache", @_ );
}

1
