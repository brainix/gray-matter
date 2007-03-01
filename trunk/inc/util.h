/*----------------------------------------------------------------------------*\
 |	util.h								      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
 |		All rights reserved.					      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 2 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
 */

#ifndef UTIL_H
#define UTIL_H

#include <string.h> // should declare ffsll on supported platforms

/*
 | WIN32 doesn't have ffsll, so we implement our own if the symbol isn't
 | defined at this point.  gray_ffsll is in types.cpp
 | 
 | I can't seem to keep gray_ffsll from being used on linux.. help?
 | I'm disabling it for now.
 */
#ifndef ffsll
int first_bit_64(long long int i);
#define ffsll(i) first_bit_64(i)
#endif

#endif
