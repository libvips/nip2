#!/bin/bash

top_srcdir=$1
tmp=$top_srcdir/test/tmp
image=$top_srcdir/share/nip2/data/examples/businesscard/slanted_oval_vase2.jpg

# write with a certain tilesize and number of threads, test avg 
write_test() {
	image=$1
	threads=$2
	tilew=$3
	tileh=$4
	avg=$5

	args="--vips-concurrency=$threads \
		--vips-tile-width=$tilew \
		--vips-tile-height=$tileh"

	echo -n "testing threads=$threads, tilew=$tilew, tileh=$tileh ... "

	if ! vips $args im_rot90 $image $tmp/t1.v ; then
		echo "write failed"
		exit 1
	fi

	result=`vips $args im_avg $tmp/t1.v`

	if [ $result != $avg ] ; then
		echo "result should be $avg, was $result"
		exit 1
	fi

	echo ok
}

avg=`vips im_avg $image`

write_test $image 1 64 64 $avg
write_test $image 1 256 256 $avg
write_test $image 10 256 256 $avg
write_test $image 1024 256 256 $avg
write_test $image 1024 16 16 $avg

