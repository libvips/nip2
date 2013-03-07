/* Declarations for predicate.c
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

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 */

/*

    These files are distributed with VIPS - http://www.vips.ecs.soton.ac.uk

 */

gboolean is_system( Symbol *sym );
gboolean is_separator( Symbol *sym );
gboolean is_member( Symbol *sym );
gboolean is_class( Compile *compile );
gboolean is_super( Symbol *sym );
gboolean is_this( Symbol *sym );
gboolean is_member_enclosing( Compile *compile, Symbol *sym );
gboolean is_top( Symbol *sym );
gboolean is_scope( Symbol *sym );
gboolean is_memberfunc( Compile *compile );
gboolean is_value( Symbol *sym );
gboolean is_ancestor( Symbol *context, Symbol *sym );
gboolean is_menuable( Symbol *sym );
