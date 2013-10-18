#!/bin/sh

# set -x

# remove /everything/ ready to remake
rm -f Makefile Makefile.in aclocal.m4 config.* configure depcomp
rm -rf autom4te.cache
rm -f install-sh intltool-* ltmain.sh missing mkinstalldirs
rm -f src/*.o src/nip2 src/Makefile src/Makefile.in 

# glib-gettextize asks us to copy these files to m4 if they aren't there
# I don't have $ACDIR/isc-posix.m4, how mysterious
ACDIR=`aclocal --print-ac-dir`

# OS X with brew sets ACDIR to
# /usr/local/Cellar/automake/1.13.1/share/aclocal, the staging area, which is
# totally wrong argh
if [ ! -d $ACDIR ]; then
	ACDIR=/usr/local/share/aclocal
fi

mkdir -p m4
cp $ACDIR/codeset.m4 m4
cp $ACDIR/gettext.m4 m4
cp $ACDIR/glibc21.m4 m4
cp $ACDIR/iconv.m4 m4
cp $ACDIR/isc-posix.m4 m4
cp $ACDIR/lcmessage.m4 m4
cp $ACDIR/progtest.m4 m4

# some systems need libtoolize, some glibtoolize ... how annoying
echo testing for glibtoolize ...
if glibtoolize --version >/dev/null 2>&1; then 
  LIBTOOLIZE=glibtoolize
  echo using glibtoolize 
else 
  LIBTOOLIZE=libtoolize
  echo using libtoolize 
fi

aclocal 
glib-gettextize --force --copy
test -r aclocal.m4 && chmod u+w aclocal.m4
autoconf
autoheader
$LIBTOOLIZE --copy --force --automake
automake --add-missing --copy

