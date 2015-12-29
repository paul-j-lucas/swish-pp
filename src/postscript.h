/*
**      SWISH++
**      src/postscript.h
**
**      Copyright (C) 1998-2015  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef postscript_H
#define postscript_H

// local
#include "pjl/pjl_set.h"

///////////////////////////////////////////////////////////////////////////////

/**
 * A %postscript_comment_set is-a char_ptr_set.  The only reason for having a
 * derived class rather than a typedef is so that we can have a custom
 * constructor that initializes itself.
 */
struct postscript_comment_set : PJL::char_ptr_set {
  postscript_comment_set();
};

/**
 * A %postscript_operator_set is-a char_ptr_set used to contain the entire set
 * of Level 2 PostScript operators that are not also English words.  The only
 * reason for having a derived class rather than a typedef is so that we can
 * have a custom constructor that initializes itself.
 */
struct postscript_operator_set : PJL::char_ptr_set {
  postscript_operator_set();
};

///////////////////////////////////////////////////////////////////////////////

#endif /* postscript_H */
/* vim:set et sw=2 ts=2: */
