# Xiphos

Xiphos is a Bible study application for Linux, UNIX, and Windows operating
systems. It uses Sword to display Bibles, commentaries, dictionary, and other
texts and images. Xiphos includes features such as, searching, biblesync,
bookmarks, parallel study, and original language study.

To install Xiphos, search in your package manager for xiphos and install the
package. If you are using Windows, please download the exe from here:

http://xiphos.org/download/

## Building Xiphos

Please see the INSTALL file.


## History and Acknowledgements

Xiphos started life as the Sword Project's Cheatah program.

Over the course of Xiphos development many ideas and some pieces of code have
come from the Bibletime program and the Sword Project's BibleCS program.

We wish to thank the folks who have put so much time and work into the Sword
Project and then giving it to the world! God Bless you!


## Developing Xiphos


If you want to help develop Xiphos your help will be very appreciated!

The first steps would be to make yourself familiar with the Xiphos build
process, so go read the INSTALL file.

If you are looking for things to do, the best place to find out is to jump into
the #xiphos channel of irc.freenode.net

If you make changes to C or C++ files, please make sure they are cleaned up to
follow our style of coding. The best way to make sure of it is to run this:

	$ clang-format -i -style=file /path/to/edited.c

Where /path/to/edited.c, the C or C++ file you edited. The option '-style=file'
uses a predefined style file called .clang-format in the source tree.


## Thank you!


We hope that your Bible study will be enhanced by Xiphos!

The Xiphos Development Team

## Contributors

This is a list of all the people that have contributed to Xiphos' development.
Mostly just the output of:

	$ git log --format='%aN <%cE>' | sort -u

~~~
Ambrose Li <ambrose.li@gmail.com> 
Andy Piper <andypiperuk@gmail.com> 
Ari Constancio <ari.constancio@gmail.com> 
Barry Drake <bdrake@crosswire.org> 
Brian J Dumont <brian.j.dumont@gmail.com> 
Christopher Bayliss <christopher.j.bayliss@gmail.com> 
Daniel Glassey <dglassey@gmail.com> 
Dmitrij Ledkovskij <xnox@debian.org> 
Dominique Corbex <dominique.corbex@gmail.com> 
Eicke Godehardt <eicke@godehardt.org> 
Gertjan Francke <gwfrancke@gmail.com> 
Greg Hellings <greg.hellings@gmail.com> 
Thomas Abthorpe <thomas@goodking.ca> 
Jan Paul Schmidt <japas@users.sf.net> 
Karl Kleinpaste <karl@kleinpaste.org> 
Karl-Rikard Ländell <kallelandell@gmail.com> 
Konstantin Maslyuk <kostyamaslyuk@gmail.com> 
Laurentiu Nica <nick.laurentiu.nica@gmail.com> 
Mansour <manhid1@yahoo.com> 
Martin Zibricky <mzibr.public@gmail.com> 
Michael Meeks <michael.meeks@collabora.com> 
Moses McKnight <moses@texband.net> 
mwtalbert <ransom1982@gmail.com> 
Peter von Kaehne <refdoc@crosswire.org> 
Pierre Benz <mail@pierrebenz.com> 
Scott Bell <skaught@users.sourceforge.net> 
Simon Meers <simon@simonmeers.com> 
Terry Biggs <tbiggs@users.sf.net> 
Teus Benschop <teusjannette@gmail.com> 
Victor Porton <porton@narod.ru> 
Yotam Benshalom <benshalom@gmail.com> 
Zdenko Podobny <zdposter@gmail.com> 
~~~

We even have an unknown author! ;)

	Unknown Author <nobody@crosswire.orgnobody@crosswire.org>
