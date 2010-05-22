# A debhelper build system class for handling Waf based projects.
# Copyright: © 2009 Ryan Niebur
#            © 2009 Dmitrijs Ledkovs
# License: GPL-2+

package Debian::Debhelper::Buildsystem::waf;

use strict;
use base 'Debian::Debhelper::Buildsystem';

sub DESCRIPTION {
	"waf using system-installed script";
}

sub DEFAULT_BUILD_DIRECTORY {
	my $this=shift;
	return $this->canonpath($this->get_sourcepath("build"));
}

sub check_auto_buildable {
	my ( $this, $step ) = @_;
	return -e $this->get_sourcepath("wscript") ? 1 : 0;
}

sub new {
	my $class = shift;
	my $this  = $class->SUPER::new(@_);
	# Out of source tree building is prefered.
	$this->prefer_out_of_source_building(@_);
	return $this;
}

sub _do_waf {
	my $this = shift;
	return $this->doit_in_sourcedir( "/usr/bin/waf", "--nocache", @_ );
}

sub configure {
	my $this = shift;
	$this->_do_waf( "configure",
			"--prefix=/usr",
			"-b", $this->get_build_rel2sourcedir(),
			@_ );
}

sub test {
	my $this = shift;
	#waf documentation suggestes to perform tests in the shutdown procedure
	#which is run unconditionally on sucessful builds.
	$this->_do_waf( @_ );
}

sub install {
	my $this    = shift;
	my $destdir = shift;
	$this->_do_waf( "install", "--destdir", $destdir, @_ );
}

sub build {
	my $this = shift;
	$this->_do_waf( "build", @_ );
}

sub clean {
	my $this = shift;
	$this->_do_waf( "distclean", @_ );
}

1;
