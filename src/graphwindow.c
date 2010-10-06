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
graph_write_row_child( Link *link, VipsBuf *buf )
{
	if( link->child->expr && link->child->expr->row ) {
		vips_buf_appendf( buf, "\t\t%s -> %s;\n", 
			IOBJECT( link->child )->name,
			IOBJECT( link->parent )->name ); 
	}

	return( NULL );
}

static void *
graph_write_row( Row *row, VipsBuf *buf )
{
	slist_map( row->sym->topchildren, 
		(SListMapFn) graph_write_row_child, buf );

	return( NULL );
}

static void *
graph_write_column( Column *col, VipsBuf *buf )
{
	static int n = 0;

	vips_buf_appendf( buf, "\tsubgraph cluster_%d {\n", n++ );
	vips_buf_appendf( buf, "\t\tlabel = \"%s", IOBJECT( col )->name );
	if( IOBJECT( col )->caption )
		vips_buf_appendf( buf, " - %s", IOBJECT( col )->caption );
	vips_buf_appends( buf, "\"\n" );

	vips_buf_appends( buf, "\t\tstyle=filled;\n" );
	vips_buf_appends( buf, "\t\tcolor=lightgrey;\n" );
	vips_buf_appends( buf, "\t\tnode [style=filled,color=white];\n" );

	(void) column_map( col, 
		(row_map_fn) graph_write_row, buf, NULL );
	vips_buf_appends( buf, "\t}\n" );

	return( NULL );
}

/* Generate the workspace in dot format.
 */
static void
graph_write_dot( Workspace *ws, VipsBuf *buf )
{
	vips_buf_appends( buf, "digraph G {\n" );
	workspace_map_column( ws, 
		(column_map_fn) graph_write_column, buf );
	vips_buf_appends( buf, "}\n" );
}

/* Print the workspace in dot format. Display with something like:
 * $ dot test1.dot -o test1.png -Tpng:cairo -v
 * $ eog test1.png
 */
void
graph_write( Workspace *ws )
{
	char txt[1024];
	VipsBuf buf = VIPS_BUF_STATIC( txt );

	graph_write_dot( ws, &buf );
	printf( "%s", vips_buf_all( &buf ) );
}


