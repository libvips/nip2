#!/bin/bash

top_srcdir=$1
tmp=$top_srcdir/test/tmp
image=$top_srcdir/share/nip2/data/examples/businesscard/slanted_oval_vase2.jpg

# make a mono image
vips im_extract_band $image $tmp/mono.v 1
mono=$tmp/mono.v

# make a radiance image
vips im_float2rad $image $tmp/rad.v 
rad=$tmp/rad.v

# make a cmyk image
vips im_bandjoin $image $tmp/mono.v $tmp/t1.v
vips im_copy_set $tmp/t1.v $tmp/cmyk.v 15 1 1 0 0 
cmyk=$tmp/cmyk.v

# save to t1.format, load as back.v
save_load() {
	in=$1
	format=$2
	mode=$3

	if ! vips im_copy $in $tmp/t1.$format$mode ; then
		echo "write to $out failed"
		exit 1
	fi

	if ! vips im_copy $tmp/t1.$format $tmp/back.v ; then
		echo "read from $out failed"
		exit 1
	fi
}

# save to the named file in tmp, convert back to vips again, subtract, look
# for max difference less than a threshold
test_format() {
	in=$1
	format=$2
	threshold=$3
	mode=$4

	echo -n "testing $in $format$mode ... "

	save_load $in $format $mode

	vips im_subtract $in $tmp/back.v $tmp/difference.v
	vips im_abs $tmp/difference.v $tmp/abs.v 
	dif=`vips im_max $tmp/abs.v`

	if (( $dif > $threshold )) ; then
		echo "save / load difference is $dif"
		exit 1
	fi

	echo "ok"
}

# as above, but hdr format
# this is a coded format, so we need to rad2float before we can test for
# differences
test_rad() {
	in=$1

	echo -n "testing $in hdr ... "

	save_load $in hdr

	vips im_rad2float $in $tmp/before.v
	vips im_rad2float $tmp/back.v $tmp/after.v
	vips im_subtract $tmp/before.v $tmp/after.v $tmp/difference.v
	vips im_abs $tmp/difference.v $tmp/abs.v
	dif=`vips im_max $tmp/abs.v`

	if (( $dif > 0 )) ; then
		echo "save / load difference is $dif"
		exit 1
	fi

	echo "ok"
}

test_format $image v 0
test_format $image tif 0
test_format $image tif 10 :jpeg
test_format $image tif 0 :deflate
test_format $image tif 0 :packbits
test_format $image tif 10 :jpeg,tile
test_format $image tif 10 :jpeg,tile,pyramid
test_format $image png 0
test_format $image png 0 :9,1
test_format $image jpg 10
test_format $image ppm 0
test_format $image pfm 0

# csv can only do mono
test_format $mono csv 0

# cmyk jpg is a special path
test_format $cmyk jpg 10
test_format $cmyk tif 0
test_format $cmyk tif 10 :jpeg
test_format $cmyk tif 10 :jpeg,tile
test_format $cmyk tif 10 :jpeg,tile,pyramid

test_rad $rad 

# we have loaders but not savers for other formats, eg. mat and of course all
# the libMagick formats -- add these when we get the savers done

