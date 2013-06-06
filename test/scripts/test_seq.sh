#!/bin/bash

# set -x

top_srcdir=$1
tmp=$top_srcdir/test/tmp
test_images=$top_srcdir/test/images
image=$test_images/slanted_oval_vase2.jpg

# make a large PNG, roughly the size of Chicago.png
echo -n "building huge test PNG image ... "
vips replicate $image $tmp/huge.png 80 8
echo "ok"

huge=$tmp/huge.png

echo -n "testing vipsthumbnail ... "
rm -f $tmp/x.png
vipsthumbnail $huge -o $tmp/x.png
if ! header $tmp/x.png &> /dev/null ; then
	echo "vipsthumbnail failed in basic mode"
	exit 1
fi
echo "ok"

if [ ! -d $tmp/readonly ] ; then
	mkdir $tmp/readonly
	chmod ugo-wx $tmp/readonly
fi 
export TMPDIR=$tmp/readonly

echo -n "testing vipsthumbnail does not make temps ... "
rm -f $tmp/x.png
vipsthumbnail $huge -o $tmp/x.png
if ! header $tmp/x.png &> /dev/null ; then
	echo "vipsthumbnail made a temp"
	exit 1
fi
echo "ok"

echo -n "testing shrink does not make temps ... "
rm -f $tmp/x.png
vips shrink $huge $tmp/x.png 230 230 
if ! header $tmp/x.png &> /dev/null; then
	echo "shrink made a temp"
	exit 1
fi
echo "ok"
