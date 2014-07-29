#!/bin/bash

# resize a 1000x1000 image to every size in [500,1000] with every interpolator
# and check for black lines 

# see https://github.com/jcupitt/libvips/issues/131

# set -x

top_srcdir=$1
tmp=$top_srcdir/test/tmp
test_images=$top_srcdir/test/images
image=$test_images/slanted_oval_vase2.jpg

# make a 1000x1000 mono test image
echo building test image ...
vips extract_band $image $tmp/t1.v 1
vips replicate $tmp/t1.v $tmp/t2.v 3 3
vips extract_area $tmp/t2.v $tmp/t1.v 10 10 1000 1000

# is a difference beyond a threshold? return 0 (meaning all ok) or 1 (meaning
# error, or outside threshold)
# 
# use bc since bash does not support fp math
break_threshold() {
	diff=$1
	threshold=$2
	return $(echo "$diff > $threshold" | bc -l)
}

#for interp in nearest bilinear bicubic lbb nohalo vsqbs; do
for interp in nohalo; do
  for size in {500..1000}; do
    echo -n "testing $interp, size to $size ... "
    vipsthumbnail $tmp/t1.v -o $tmp/t2.v --size $size --interpolator $interp
    vips project $tmp/t2.v $tmp/cols.v $tmp/rows.v

    min=$(vips min $tmp/cols.v)
    if break_threshold $min 0; then
      echo failed
      echo vipsthumbnail 1kx1k.v --size $size --interpolator $interp
      echo has a black column
    fi
        
    min=$(vips min $tmp/rows.v)
    if break_threshold $min 0; then
      echo failed
      echo vipsthumbnail 1kx1k.v --size $size --interpolator $interp
      echo has a black row
    fi

    echo ok
  done
done
        


