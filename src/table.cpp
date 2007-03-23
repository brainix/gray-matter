/*----------------------------------------------------------------------------*\
 |	table.cpp - transposition table implementation			      |
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
 |				    table()				      |
\*----------------------------------------------------------------------------*/
table::table(int mb)
{

/* Constructor. */

	try
	{
		if ((slots = mb * MB / sizeof(slot_t)) == 0)
			throw;
		data = new slot_t *[POLICIES];
		for (int policy = DEEP; policy <= FRESH; policy++)
			data[policy] = new slot_t[slots / 2];
	}
	catch (...)
	{
	}

	clear();
}

/*----------------------------------------------------------------------------*\
 |				    ~table()				      |
\*----------------------------------------------------------------------------*/
table::~table()
{

/* Destructor. */

	for (int policy = DEEP; policy <= FRESH; policy++)
		delete[] data[policy];
	delete[] data;
}

/*----------------------------------------------------------------------------*\
 |				    clear()				      |
\*----------------------------------------------------------------------------*/
void table::clear()
{
	for (int policy = DEEP; policy <= FRESH; policy++)
		for (uint64_t index = 0; index < slots / 2; index++)
		{
			data[policy][index].hash = 0;
			SET_NULL_MOVE(data[policy][index].move);
			data[policy][index].depth = 0;
			data[policy][index].type = USELESS;
		}
}

/*----------------------------------------------------------------------------*\
 |				    probe()				      |
\*----------------------------------------------------------------------------*/
int table::probe(bitboard_t hash, move_t *move_ptr, int depth, int alpha, int beta) const
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
	return type;
}

/*----------------------------------------------------------------------------*\
 |				    store()				      |
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
