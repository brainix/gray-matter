/*----------------------------------------------------------------------------*\
 |	table.cpp - transposition and pawn table implementations	      |
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

	bitboard_t info;
	xpos_info_t *info_ptr = &info;
	info_ptr->depth = 0;
	SET_NULL_MOVE(info_ptr->move);
	info_ptr->move.value = +INFINITY;
	info_ptr->lower = -INFINITY;

	for (uint64_t index = 0; index < slots; index++)
	{
		data[index].hash = 0;
		data[index].info = info;
	}
}

/*----------------------------------------------------------------------------*\
 |				 table::probe()				      |
\*----------------------------------------------------------------------------*/
bool table::probe(bitboard_t hash, int depth, move_t *move_ptr, int type) const
{
	uint64_t index = hash % slots;
	xpos_info_t *info_ptr = &data[index].info;

	if (data[index].hash ^ data[index].info != hash)
	{
		SET_NULL_MOVE(*move_ptr);
		return false;
	}
	*move_ptr = info_ptr->move;
	if (type == EXACT && info_ptr->move.value != info_ptr->lower)
		return false;
	if (type == LOWER)
		move_ptr->value = info_ptr->lower;
	return info_ptr->depth >= depth;
}

/*----------------------------------------------------------------------------*\
 |				 table::store()				      |
\*----------------------------------------------------------------------------*/
void table::store(bitboard_t hash, int depth, move_t move, int type)
{
	uint64_t index = hash % slots;
	xpos_info_t *info_ptr = &data[index].info;
	bool hash_match = data[index].hash ^ data[index].info == hash;
	bool upper = type == UPPER || type == EXACT;
	bool lower = type == EXACT || type == LOWER;
	xpos_info_t info;

	info.depth = hash_match ? LESSER(info_ptr->depth, depth) : depth;
	info.move = move;
	info.move.value = upper ? move.value : hash_match ? info_ptr->move.value : +INFINITY;
	info.lower = lower ? move.value : hash_match ? info_ptr->move.value : -INFINITY;

	data[index].hash = hash ^ info;
	data[index].info = info;
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
