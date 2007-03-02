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
int first_bit_64(int64_t signed_num);
int first_bit_32(int32_t signed_num);

/*----------------------------------------------------------------------------*\
 |				 first_bit_64()				      |
\*----------------------------------------------------------------------------*/
int first_bit_64(int64_t signed_num)
{

/* Find the first (least significant) set bit in a 64-bit integer. */

	uint64_t unsigned_num = signed_num & -signed_num;
	int shift = unsigned_num <= 0xFFFFFFFF ? 0 : 32;
	return first_bit_32(signed_num >> shift) + shift;
}

/*----------------------------------------------------------------------------*\
 |				 first_bit_32()				      |
\*----------------------------------------------------------------------------*/
int first_bit_32(int32_t signed_num)
{

/* Find the first (least significant) set bit in a 32-bit integer. */

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

	uint32_t unsigned_num = signed_num & -signed_num;
	int shift = unsigned_num <= 0xFFFF ? (unsigned_num <= 0xFF ? 0 : 8) : (unsigned_num <= 0xFFFFFF ?  16 : 24);
	return table[unsigned_num >> shift] + shift;
}
