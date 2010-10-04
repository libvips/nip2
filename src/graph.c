/* display workspaces with graphviz
 */

/*

    Copyright (C) 1991-2003 The National Gallery

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

#include "ip.h"

static void *
graph_write_row_child( Link *link )
{
	if( link->child->expr && link->child->expr->row ) {
		printf( "\t\t%s -> %s;\n", 
			IOBJECT( link->child )->name,
			IOBJECT( link->parent )->name ); 
	}

	return( NULL );
}

static void *
graph_write_row( Row *row )
{
	slist_map( row->sym->topchildren, 
		(SListMapFn) graph_write_row_child, NULL );

	return( NULL );
}

static void *
graph_write_column( Column *col )
{
	static int n = 0;

	printf( "\tsubgraph cluster_%d {\n", n++ );
	printf( "\t\tlabel = \"%s", IOBJECT( col )->name );
	if( IOBJECT( col )->caption )
		printf( " - %s", IOBJECT( col )->caption );
	printf( "\"\n" );

	printf( "\t\tstyle=filled;\n" );
	printf( "\t\tcolor=lightgrey;\n" );
	printf( "\t\tnode [style=filled,color=white];\n" );

	(void) column_map( col, 
		(row_map_fn) graph_write_row, NULL, NULL );
	printf( "\t}\n" );

	return( NULL );
}

void
graph_write( Workspace *ws )
{
	printf( "digraph G {\n" );
	workspace_map_column( ws, 
		(column_map_fn) graph_write_column, NULL );
	printf( "}\n" );
}
