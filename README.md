# nip2 - a user interface for the VIPS image processing library

nip2 is a GUI for the [VIPS image processing 
library](http://www.vips.ecs.soton.ac.uk). It's a little like a spreadsheet:
you create a set of formula connecting your objects together, and on a change
nip2 recalculates.

# Building nip2 from source

In the nip2 directory you should just be able to do:

	$ ./configure
	$ make
	$ sudo make install

By default this will install files to `/usr/local`.

See the Dependencies section below for a list of the things that
nip2 needs in order to be able to build.

We have detailed guides on the wiki for [building on
Windows](http://www.vips.ecs.soton.ac.uk/index.php?title=Build_on_windows)
and [building on OS
X](http://www.vips.ecs.soton.ac.uk/index.php?title=Build_on_OS_X).

If you downloaded from GIT you'll need:

	$ ./bootstrap.sh

first to build the configure system. 

# Dependencies

nip2 needs vips, gtk2 and libxml2 at runtime and flex/bison at compile time.

If you have fftw3, gsl, goffice, libgvc you get extra optional, but useful,
features. 

# Tips 

production build with

	./configure --prefix=/home/john/vips 

debug build with

	CFLAGS="-g -Wall" ./configure --prefix=/home/john/vips 

(--enable-debug turns on and off automatically with development / production
minor version numbers)

leak check

	export G_DEBUG=gc-friendly 
	export G_SLICE=always-malloc 
	valgrind --suppressions=/home/john/nip2.supp \
	  --leak-check=yes \
	  nip2 ... > nip2-vg.log 2>&1

memory access check

	valgrind --suppressions=/home/john/nip2.supp \
	  --leak-check=no --db-attach=yes \
	  nip2 ... > nip2-vg.log 2>&1

or put "--suppressions=/home/john/nip2.supp" into ~/.valgrindrc 

profiling

	valgrind --tool=callgrind \
	  --suppressions=/home/john/nip2.supp \
	  nip2 ... > nip2-vg.log 2>&1

Disclaimer: No guarantees of performance accompany this software, nor is any
responsibility assumed on the part of the authors. Please read the licence
agreement.

