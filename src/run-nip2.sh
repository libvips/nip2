#!/bin/bash
#
# Start script for nip ... from the vips start script

# need extended regexps
shopt -s extglob 
# set -x

# name we were invoked as
bname=`basename $0`

# try to extract the prefix from a path to an executable
# eg. "/home/john/vips/bin/fred" -> "/home/john/vips"
function find_prefix () {
	# try to canonicalise the path
	ep_canon=$1

	# relative path? prefix with pwd
	if [ ${ep_canon:0:1} != "/" ]; then
		ep_canon=`pwd`/$ep_canon
	fi

	# replace any "/./" with "/"
	ep_canon=${ep_canon//\/.\//\/}

	# any "xxx/../" can go 
	ep_canon=${ep_canon//+([^\/])\/..\//}

	# trailing "xxx/.." can go 
	ep_canon=${ep_canon/%+([^\/])\/../}

	# remove trailing "/bin/xxx" to get the prefix
	ep_prefix=${ep_canon/%\/bin\/+([^\/])/}

	# was there anything to remove in that final step? if not, the path 
	# must be wrong
	if [ x$ep_prefix == x$ep_canon ]; then
		return 1
	fi

	echo $ep_prefix;

	return 0
}

# try to guess the install prefix from $0
function guess_prefix () {
	# $0 is a file? must be us
	if [ -f $0 ]; then
		find_prefix $0
		return 
	fi

	# nope, extract program name from $0 and try looking along the
	# searchpath for it
	name=`basename $0`

	fred=$PATH
	while [ x$fred != x"" ]; do
		path=${fred/:*/}/$name
		fred=${fred/*([^:])?(:)/}

		if [ -f $path ]; then
			find_prefix $path
			return 
		fi
	done

	# not found on path either ... give up!
	return 1
}

# set VIPSHOME
prefix=`guess_prefix`;

if [ $? != 0 ]; then
	echo "unable to find $0 from the file name, or from your PATH"
	echo "either run directly, or add the install bin area to "
	echo "your PATH"
	exit 1
fi

export VIPSHOME=$prefix

# add the VIPS lib area to the library path
case `uname` in
HPUX)
	export SHLIB_PATH=$VIPSHOME/lib:$SHLIB_PATH
	;;

Darwin)
	export DYLD_LIBRARY_PATH=$VIPSHOME/lib:$DYLD_LIBRARY_PATH
	;;
 
*)
	export LD_LIBRARY_PATH=$VIPSHOME/lib:$LD_LIBRARY_PATH
	;;
esac

# stop LD_PRELOAD messing up our libraries
unset LD_PRELOAD

# ask for xft font rendering from pango
export GDK_USE_XFT=1

# how odd, need to say where font.conf is
export FONTCONFIG_FILE=/etc/fonts/fonts.conf

# run, passing in args we were passed
exec $VIPSHOME/bin/nip2 $*
