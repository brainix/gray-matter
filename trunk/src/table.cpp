/*----------------------------------------------------------------------------*\
 |	table.cpp - transposition, pawn, and history table implementations    |
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
 |				 table::probe()				      |
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
	for (uint64_t index = 0; index < slots; index++)
	{
		data[index].hash = 0;
		data[index].value = INT_MIN;
	}
}

/*----------------------------------------------------------------------------*\
 |				 pawn::probe()				      |
\*----------------------------------------------------------------------------*/
int pawn::probe(bitboard_t hash, int *value_ptr) const
{
	uint64_t index = hash % slots;
	bool found = data[index].hash == hash;
	*value_ptr = found ? data[index].value : 0;
	return found ? EXACT : USELESS;
}

/*----------------------------------------------------------------------------*\
 |				 pawn::store()				      |
\*----------------------------------------------------------------------------*/
void pawn::store(bitboard_t hash, int value)
{
	uint64_t index = hash % slots;
	data[index].hash = hash;
	data[index].value = value;
}

/*----------------------------------------------------------------------------*\
 |			       history::history()			      |
\*----------------------------------------------------------------------------*/
history::history()
{

/* Constructor. */

	try
	{
		data = new int ****[COLORS];
		for (int color = WHITE; color <= BLACK; color++)
		{
			data[color] = new int ***[8];
			for (int old_x = 0; old_x <= 7; old_x++)
			{
				data[color][old_x] = new int **[8];
				for (int old_y = 0; old_y <= 7; old_y++)
				{
					data[color][old_x][old_y] = new int *[8];
					for (int new_x = 0; new_x <= 7; new_x++)
						data[color][old_x][old_y][new_x] = new int[8];
				}
			}
		}
	}
	catch (...)
	{
	}

	clear();
}

/*----------------------------------------------------------------------------*\
 |			      history::~history()			      |
\*----------------------------------------------------------------------------*/
history::~history()
{

/* Destructor. */

	for (int color = WHITE; color <= BLACK; color++)
	{
		for (int old_x = 0; old_x <= 7; old_x++)
		{
			for (int old_y = 0; old_y <= 7; old_y++)
			{
				for (int new_x = 0; new_x <= 7; new_x++)
					delete[] data[color][old_x][old_y][new_x];
				delete[] data[color][old_x][old_y];
			}
			delete[] data[color][old_x];
		}
		delete[] data[color];
	}
	delete[] data;
}

/*----------------------------------------------------------------------------*\
 |				history::clear()			      |
\*----------------------------------------------------------------------------*/
void history::clear()
{
	for (int color = WHITE; color <= BLACK; color++)
		for (int old_y = 0; old_y <= 7; old_y++)
			for (int old_x = 0; old_x <= 7; old_x++)
				for (int new_y = 0; new_y <= 7; new_y++)
					for (int new_x = 0; new_x <= 7; new_x++)
						data[color][old_x][old_y][new_x][new_y] = 0;
}

/*----------------------------------------------------------------------------*\
 |				history::probe()			      |
\*----------------------------------------------------------------------------*/
int history::probe(bool color, move_t move) const
{
	return data[color][move.old_x][move.old_y][move.new_x][move.new_y];
}

/*----------------------------------------------------------------------------*\
 |				history::store()			      |
\*----------------------------------------------------------------------------*/
void history::store(bool color, move_t move, int depth)
{

/* Gray Matter has searched to the specified depth and determined the specified
 * move for the specified color to be the best.  Note this. */

	data[color][move.old_x][move.old_y][move.new_x][move.new_y] += 1 << depth;
}
