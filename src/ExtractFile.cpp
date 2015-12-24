/*
**      SWISH++
**      src/ExtractFile.cpp
**
**      Copyright (C) 2000  Paul J. Lucas
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

// local
#include "config.h"
#include "ExtractFile.h"
#include "util.h"                               /* for new_strdup() */

// standard
#include <cstring>

using namespace std;

//*****************************************************************************
//
// SYNOPSIS
//
        void ExtractFile::parse_value( char *line )
//
// DESCRIPTION
//
//      Parse the line of text by splitting it into words that are separated by
//      whitespace.
//
// PARAMETERS
//
//      line    The line of text to be parsed.
//
//*****************************************************************************
{
    for ( char const *s; (s = ::strtok( line, " \r\t" )); line = 0 )
        insert( new_strdup( s ), 0 );
}
/* vim:set et sw=4 ts=4: */
