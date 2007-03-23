/*----------------------------------------------------------------------------*\
 |	pawn.cpp							      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
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

#include "gray.h"
#include "pawn.h"

/*----------------------------------------------------------------------------*\
 |				  pawn_table()				      |
\*----------------------------------------------------------------------------*/
pawn_table::pawn_table(int mb)
{

/* Constructor. */

	try
	{
		if ((slots = mb * MB / sizeof(pawn_slot_t)) == 0)
			throw;
		data = new pawn_slot_t[slots]
	}
	catch (...)
	{
	}

	clear();
}

/*----------------------------------------------------------------------------*\
 |				 ~pawn_table()				      |
\*----------------------------------------------------------------------------*/
pawn_table::~pawn_table()
{

/* Destructor. */

	delete[] data;
}

/*----------------------------------------------------------------------------*\
 |				    clear()				      |
\*----------------------------------------------------------------------------*/
void pawn_table::clear()
{
	for (uint64_t index = 0; index < slots; index++)
	{
		data[index].pawn_hash = 0;
		data[index].value = INT_MIN;
	}
}

/*----------------------------------------------------------------------------*\
 |				    probe()				      |
\*----------------------------------------------------------------------------*/
int pawn_table::probe(bitboard_t pawn_hash) const
{
	uint64_t index = pawn_hash % slots;
	return data[index].pawn_hash == pawn_hash ? data[index].value : INT_MIN;
}

/*----------------------------------------------------------------------------*\
 |				    store()				      |
\*----------------------------------------------------------------------------*/
void pawn_table::store(bitboard_t pawn_hash, int value)
{
	uint64_t index = pawn_hash % slots;
	data[index].pawn_hash = pawn_hash;
	data[index].value = value;
}
