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
		data = new xpos_slot_t *[POLICIES];
		for (int policy = DEEP; policy <= FRESH; policy++)
			data[policy] = new xpos_slot_t[slots / 2];
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

	for (int policy = DEEP; policy <= FRESH; policy++)
		delete[] data[policy];
	delete[] data;
}

/*----------------------------------------------------------------------------*\
 |				 table::clear()				      |
\*----------------------------------------------------------------------------*/
void table::clear()
{

/* Clear the transposition table. */

	for (int policy = DEEP; policy <= FRESH; policy++)
		for (uint64_t index = 0; index < slots / 2; index++)
		{
			data[policy][index].hash = 0;
			SET_NULL_MOVE(data[policy][index].move);
			data[policy][index].depth = 0;
			data[policy][index].type = USELESS;
		}
#if DEBUG
	hits = 0;
	misses = 0;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 table::probe()				      |
\*----------------------------------------------------------------------------*/
int table::probe(bitboard_t hash, move_t *move_ptr, int depth, int alpha, int beta)
{
	uint64_t index = hash % (slots / 2);
	int type = USELESS;

	SET_NULL_MOVE(*move_ptr);
	for (int policy = DEEP; policy <= FRESH; policy++)
		if (data[policy][index].hash == hash && data[policy][index].depth >= (unsigned) depth)
		{
			*move_ptr = data[policy][index].move;
			if (data[policy][index].type == ALPHA && move_ptr->value >= alpha)
			{
				move_ptr->value = alpha;
				type = ALPHA;
			}
			if (data[policy][index].type == BETA && move_ptr->value <= beta)
			{
				move_ptr->value = beta;
				type = BETA;
			}
			if (data[policy][index].type == EXACT)
				type = EXACT;
		}
#if DEBUG
	if (type != USELESS)
		hits++;
	else
		misses++;
#endif
	return type;
}

/*----------------------------------------------------------------------------*\
 |				 table::store()				      |
\*----------------------------------------------------------------------------*/
void table::store(bitboard_t hash, move_t move, int depth, int type)
{
	uint64_t index = hash % (slots / 2);

	if (IS_NULL_MOVE(move))
		return;

	for (int policy = DEEP; policy <= FRESH; policy++)
		if (policy == FRESH || (unsigned) depth >= data[DEEP][index].depth)
		{
			data[policy][index].hash = hash;
			data[policy][index].move = move;
			data[policy][index].depth = depth;
			data[policy][index].type = type;
		}
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
		data[index].value = INT_MIN;
	}
#if DEBUG
	hits = 0;
	misses = 0;
#endif
}

/*----------------------------------------------------------------------------*\
 |				 pawn::probe()				      |
\*----------------------------------------------------------------------------*/
int pawn::probe(bitboard_t hash, int *value_ptr)
{

/*
 | Given the pawn structure described in hash, check the pawn table to see if
 | we've evaluated it before.  If so, save its previous evaluation to the memory
 | pointed to by value_ptr and return success.  If not, return failure.
 */

	uint64_t index = hash % slots;
	bool found = data[index].hash == hash;
	*value_ptr = found ? data[index].value : 0;
#if DEBUG
	if (found)
		hits++;
	else
		misses++;
#endif
	return found ? EXACT : USELESS;
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
