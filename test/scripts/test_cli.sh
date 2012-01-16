#!/bin/bash

#set -x

top_srcdir=$1
tmp=$top_srcdir/test/tmp
test_images=$top_srcdir/test/images
image=$test_images/slanted_oval_vase2.jpg

# is a difference beyond a threshold? return 0 (meaning all ok) or 1 (meaning
# error, or outside threshold)
# 
# use bc since bash does not support fp math
break_threshold() {
	diff=$1
	threshold=$2
	return $(echo "$diff <= $threshold" | bc -l)
}

# subtract, look for max difference less than a threshold
test_difference() {
	before=$1
	after=$2
	threshold=$3

	vips im_subtract $before $after $tmp/difference.v
	vips im_abs $tmp/difference.v $tmp/abs.v 
	dif=$(vips im_max $tmp/abs.v)

	if break_threshold $dif $threshold; then
		echo "difference is $dif"
		exit 1
	fi
}

echo -n "testing bicubic bilinear nohalo lbb $(basename $image) ... "

# 90 degree clockwise rotate 
vips im_affinei_all $image $tmp/t1.v bicubic 0 1 1 0 0 0
vips im_affinei_all $tmp/t1.v $tmp/t2.v bilinear 0 1 1 0 0 0
vips im_affinei_all $tmp/t2.v $tmp/t1.v nohalo 0 1 1 0 0 0
vips im_affinei_all $tmp/t1.v $tmp/t2.v lbb 0 1 1 0 0 0

test_difference $image $tmp/t2.v 1

echo "ok"
