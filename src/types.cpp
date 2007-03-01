/*----------------------------------------------------------------------------*\
 |	types.cpp - data types						      |
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

#include <string.h> // this should declare ffsll if it's available
//
// don't compile our version of ffsll if it is already defined.
//
#ifndef ffsll

#include "types.h"

static int gray_ffsb(uint8_t b)
{
	const static uint8_t byte_masks[8] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
	};
	int i;
	for(i = 0; i < 8; i++)
		if(b & byte_masks[i])
			return i+1; // LSB == 1
	return 0; // no bits set
}

//int ffsl (long int i);

int gray_ffsll(long long int i)
{
	const static uint64_t search_masks[8] = {
		0xFFFFFFFFFFFFFFFFULL,
		0x00000000FFFFFFFFULL,
		0x000000000000FFFFULL,
		0x00000000000000FFULL,
		0x0000000000FF0000ULL,
		0x0000FFFF00000000ULL,
		0x000000FF00000000ULL,
		0x00FF000000000000ULL,
	};
	if(!(i & search_masks[0]))
		return 0; // no bits set

	if(i & search_masks[1])
	{
		if(i & search_masks[2])
		{
			if(i & search_masks[3])
				return gray_ffsb((uint8_t)i);
			else
				return gray_ffsb((uint8_t)(i >> 8)) + 8;
		}
		else if(i & search_masks[4])
			return gray_ffsb((uint8_t)(i >> 16)) + 16;
		else
			return gray_ffsb((uint8_t)(i >> 24)) + 24;
	}
	else
	{
		if(i & search_masks[5])
		{
			if(i & search_masks[6])
				return gray_ffsb((uint8_t)(i >> 32)) + 32;
			else
				return gray_ffsb((uint8_t)(i >> 40)) + 40;
		}
		else if(i & search_masks[7])
			return gray_ffsb((uint8_t)(i >> 48)) + 48;
		else
			return gray_ffsb((uint8_t)(i >> 56)) + 56;
	}
	return 0; // satisfy the compiler, this should never occur
}

#endif

