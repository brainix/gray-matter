/*----------------------------------------------------------------------------*\
 |	table.cpp - transposition and pawn table implementations	      |
 |									      |
 |	Copyright � 2005-2007, The Gray Matter Team, original authors.	      |
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
#include "table.h"

/*----------------------------------------------------------------------------*\
 |				 table::table()				      |
\*----------------------------------------------------------------------------*/
table::table(int mb)
{

/* Constructor. */

	try
	{
		if ((slots = mb * MB / sizeof(xpos_slot_t)) == 0)
			throw;
		data = new xpos_slot_t[slots];
	}
	catch (...)
	{
	}
	clear();
}

/*----------------------------------------------------------------------------*\
 |				table::~table()				      |
\*----------------------------------------------------------------------------*/
table::~table()
{

/* Destructor. */

	delete[] data;
}

/*----------------------------------------------------------------------------*\
 |				 table::clear()				      |
\*----------------------------------------------------------------------------*/
void table::clear()
{

/* Clear the transposition table. */

	for (uint64_t index = 0; index < slots; index++)
	{
		data[index].hash = 0;
		data[index].depth = 0;
		data[index].type = USELESS;
		SET_NULL_MOVE(data[index].move);
		data[index].move.value = 0;
	}
}

/*----------------------------------------------------------------------------*\
 |				 table::probe()				      |
\*----------------------------------------------------------------------------*/
bool table::probe(bitboard_t hash, int depth, int type, move_t *move_ptr) const
{
	uint64_t index = hash % slots;
	if (data[index].hash != hash)
	{
		SET_NULL_MOVE(*move_ptr);
		move_ptr->value = 0;
		return false;
	}
	*move_ptr = data[index].move;
	return data[index].depth >= depth && (data[index].type == EXACT || data[index].type == type);
}

/*----------------------------------------------------------------------------*\
 |				 table::store()				      |
\*----------------------------------------------------------------------------*/
void table::store(bitboard_t hash, int depth, int type, move_t move)
{
	uint64_t index = hash % slots;
	data[index].hash = hash;
	data[index].depth = depth;
	data[index].type = type;
	data[index].move = move;
}

/*----------------------------------------------------------------------------*\
 |				  pawn::pawn()				      |
\*----------------------------------------------------------------------------*/
pawn::pawn(int mb)
{

/* Constructor. */

	try
	{
		if ((slots = mb * MB / sizeof(pawn_slot_t)) == 0)
			throw;
		data = new pawn_slot_t[slots];
	}
	catch (...)
	{
	}
	clear();
}

/*----------------------------------------------------------------------------*\
 |				 pawn::~pawn()				      |
\*----------------------------------------------------------------------------*/
pawn::~pawn()
{

/* Destructor. */

	delete[] data;
}

/*----------------------------------------------------------------------------*\
 |				 pawn::clear()				      |
\*----------------------------------------------------------------------------*/
void pawn::clear()
{

/* Clear the pawn table. */

	for (uint64_t index = 0; index < slots; index++)
	{
		data[index].hash = 0;
		data[index].value = -INFINITY;
	}
}

/*----------------------------------------------------------------------------*\
 |				 pawn::probe()				      |
\*----------------------------------------------------------------------------*/
bool pawn::probe(bitboard_t hash, int *value_ptr) const
{

/*
 | Given the pawn structure described in hash, check the pawn table to see if
 | we've evaluated it before.  If so, save its previous evaluation to the memory
 | pointed to by value_ptr and return success.  If not, return failure.
 */

	uint64_t index = hash % slots;
	bool found = data[index].hash == hash;
	*value_ptr = found ? data[index].value : 0;
	return found;
}

/*----------------------------------------------------------------------------*\
 |				 pawn::store()				      |
\*----------------------------------------------------------------------------*/
void pawn::store(bitboard_t hash, int value)
{

/*
 | We've just evaluated the pawn structure described in hash.  Save its
 | evaluation in the pawn table for future probes.
 */

	uint64_t index = hash % slots;
	data[index].hash = hash;
	data[index].value = value;
}
