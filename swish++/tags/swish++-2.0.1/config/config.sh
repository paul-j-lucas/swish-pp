#!/bin/sh
##
#	SWISH++
#	config.sh -- Configuration script
#
#	Copyright (C) 1998  Paul J. Lucas
#
#	This program is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
# 
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
# 
#	You should have received a copy of the GNU General Public License
#	along with this program; if not, write to the Free Software
#	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##

##
#	This code is Bourne Shell for maximal portability.
##

########### You shouldn't have to change anything below this line. ############

ME=$0; ME=`expr $ME : '.*/\(.*\)'`
USAGE="usage: $ME target CC_c_ext CC_o_ext compiler [ compiler-options ]"
N=4

[ $# -lt $N ] && { echo $USAGE >&2; exit 1; }

TARGET=$1
CCCEXT=$2
CCOEXT=$3
CC=$4
shift $N
CFLAGS="$* -c"

echo
echo "$ME: checking C++ compiler $CC"
echo

trap "x=$?; rm -f *$CCOEXT $TARGET; exit $x" 0 1 2 15

cat > $TARGET <<!
/*
**	PJL C++ Library
**	platform.h
**
**	This program is free software; you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation; either version 2 of the License, or
**	(at your option) any later version.
** 
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
** 
**	You should have received a copy of the GNU General Public License
**	along with this program; if not, write to the Free Software
**	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
**	Note: This configuration file was automatically generated for the C++
**	compiler $CC on `date`.
*/

#ifndef PJL_platform_H
#define PJL_platform_H

!

for f in src/*$CCCEXT
do
	echo "$ME: checking for \"`grep TEST $f | cut -f3-`\"..."
	$CC $CFLAGS $f >&- 2>&- ||
		echo "#define `grep DEFINE $f | cut -f3-`" >> $TARGET
done

echo

cat >> $TARGET <<!

#endif	/* PJL_platform_H */
!

trap "x=$?; rm -f *$CCOEXT; exit $x" 0 1 2 15
