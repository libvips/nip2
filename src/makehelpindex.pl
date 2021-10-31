#!/usr/bin/perl

# html docs in $VIPSHOME/share/nip4/doc/html include extra anchor tags
# generated from \mylabel{} stuff in doc src (nip-xx/doc/src/nipguide)
#
# latex source
#
#    	\section{Image view window}
#    	\mylabel{sec:view}
#
# generates html which includes
#
#	<a NAME="nip_label_sec:view"> </a>
#
# scan all html files in $VIPSHOME/share/nip4/doc/html for patterns like this,
# and generate C along the lines of:
#
#	{ "sec:view", "node4.html#nip_label_sec:view" },
#
# this is includes in boxes.c ... then on 
#
#	box_help( par, "sec:view" )
# 
# we can pop up a web browser pointing at the right place in the docs

$prefix = @ARGV[0];
$docbase = "$prefix/share/doc/nip4/html";

opendir( SDIR, "$docbase" );

while( $filename = readdir SDIR ) {
	if( $filename =~ /.html$/ ) {
		open( HTMLFILE, "$docbase/$filename" );

		while( <HTMLFILE> ) {
			if( /"nip_label_([^"]*)"/ ) {
				print "{ \"$1\", \"$filename#nip_label_" .
					"$1\" },\n";
			}
		}

		close( HTMLFILE );
	}
}

closedir( SDIR );

