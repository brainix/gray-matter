/*----------------------------------------------------------------------------*\
 |	util.cpp							      |
 |									      |
 |	Copyright © 1991-1998, Free Software Foundation, original authors.    |
 |		All rights reserved.					      |
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

/*
 | The functions in this file were shamelessly yoinked from the GNU C Library,
 | version 2.5, originally written by Torbjorn Granlund <tege@sics.se>.
 */

#include "gray.h"
#include "util.h"

/* Function prototypes: */
int first_bit_64(int64_t i);
static int first_bit_32(int32_t i);

/*----------------------------------------------------------------------------*\
 |				 first_bit_64()				      |
\*----------------------------------------------------------------------------*/
int first_bit_64(int64_t i)
{
	uint64_t x = i & -i;

	if (x <= 0xffffffff)
		return first_bit_32(i);
	else
		return 32 + first_bit_32(i >> 32);
}

/*----------------------------------------------------------------------------*\
 |				 first_bit_32()				      |
\*----------------------------------------------------------------------------*/
static int first_bit_32(int32_t i)
{
	static const uint8_t table[] =
	{
		0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
	};

	unsigned int x = i & -i;
	unsigned int a = x <= 0xFFFF ? (x <= 0xFF ? 0 : 8) : (x <= 0xFFFFFF ?  16 : 24);
	return table[x >> a] + a;
}
