/*----------------------------------------------------------------------------*\
 |	table.cpp - transposition, history, and pawn table implementations    |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
\*----------------------------------------------------------------------------*/

/*
 | This program is free software: you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation, either version 3 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gray.h"
#include "table.h"

/*----------------------------------------------------------------------------*\
 |				 table::table()				      |
\*----------------------------------------------------------------------------*/
table::table(int mb)
{

// Constructor.

	try
	{
		if ((slots = mb * MB / POLICIES / sizeof(xpos_slot_t)) == 0)
			throw;
		data = new xpos_slot_t*[POLICIES];
		for (int policy = DEEP; policy <= FRESH; policy++)
			data[policy] = new xpos_slot_t[slots];
	}
	catch (...)
	{
		// XXX: We should probably do something here.
	}
	clear();
}

/*----------------------------------------------------------------------------*\
 |				table::~table()				      |
\*----------------------------------------------------------------------------*/
table::~table()
{

// Destructor.

	for (int policy = DEEP; policy <= FRESH; policy++)
		delete[] data[policy];
	delete[] data;
}

/*----------------------------------------------------------------------------*\
 |				 table::clear()				      |
\*----------------------------------------------------------------------------*/
void table::clear()
{

// Clear the transposition table.

	for (int policy = DEEP; policy <= FRESH; policy++)
		for (uint64_t index = 0; index < slots; index++)
		{
			data[policy][index].hash = 0;
			data[policy][index].depth = 0;
			data[policy][index].type = USELESS;
			SET_NULL_MOVE(data[policy][index].move);
			data[policy][index].move.value = 0;
		}
}

/*----------------------------------------------------------------------------*\
 |				 table::probe()				      |
\*----------------------------------------------------------------------------*/
bool table::probe(bitboard_t hash, int depth, int type, move_t *move_ptr)
{
	uint64_t index = hash % slots;
	for (int policy = DEEP; policy <= FRESH; policy++)
		if (data[policy][index].hash == hash)
		{
			*move_ptr = data[policy][index].move;
			return data[policy][index].depth >= depth && (data[policy][index].type == type || data[policy][index].type == EXACT || data[policy][index].type == BOOK);
		}
	SET_NULL_MOVE(*move_ptr);
	move_ptr->value = 0;
	return false;
}

/*----------------------------------------------------------------------------*\
 |				 table::store()				      |
\*----------------------------------------------------------------------------*/
void table::store(bitboard_t hash, int depth, int type, move_t move)
{
	uint64_t index = hash % slots;
	for (int policy = DEEP; policy <= FRESH; policy++)
		if (depth >= data[policy][index].depth || policy == FRESH)
		{
			data[policy][index].hash = hash;
			data[policy][index].depth = depth;
			data[policy][index].type = type;
			data[policy][index].move = move;
		}
}

/*----------------------------------------------------------------------------*\
 |			       history::history()			      |
\*----------------------------------------------------------------------------*/
history::history()
{
	try
	{
		data = new int****[COLORS];
		for (int color = WHITE; color <= BLACK; color++)
		{
			data[color] = new int***[8];
			for (int old_x = 0; old_x <= 7; old_x++)
			{
				data[color][old_x] = new int**[8];
				for (int old_y = 0; old_y <= 7; old_y++)
				{
					data[color][old_x][old_y] = new int*[8];
					for (int new_x = 0; new_x <= 7; new_x++)
						data[color][old_x][old_y][new_x] = new int[8];
				}
			}
		}
	}
	catch (...)
	{
		// XXX: We should probably do something here.
	}
	clear();
}

/*----------------------------------------------------------------------------*\
 |			      history::~history()			      |
\*----------------------------------------------------------------------------*/
history::~history()
{
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
		for (int old_x = 0; old_x <= 7; old_x++)
			for (int old_y = 0; old_y <= 7; old_y++)
				for (int new_x = 0; new_x <= 7; new_x++)
					for (int new_y = 0; new_y <= 7; new_y++)
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

// Gray Matter has searched to the specified depth and determined the specified
// move for the specified color to be the best.  Note this.

	data[color][move.old_x][move.old_y][move.new_x][move.new_y] += 1 << depth;
}

/*----------------------------------------------------------------------------*\
 |				  pawn::pawn()				      |
\*----------------------------------------------------------------------------*/
pawn::pawn(int mb)
{

// Constructor.

	try
	{
		if ((slots = mb * MB / sizeof(pawn_slot_t)) == 0)
			throw;
		data = new pawn_slot_t[slots];
	}
	catch (...)
	{
		// XXX: We should probably do something here.
	}
	clear();
}

/*----------------------------------------------------------------------------*\
 |				 pawn::~pawn()				      |
\*----------------------------------------------------------------------------*/
pawn::~pawn()
{

// Destructor.

	delete[] data;
}

/*----------------------------------------------------------------------------*\
 |				 pawn::clear()				      |
\*----------------------------------------------------------------------------*/
void pawn::clear()
{

// Clear the pawn table.

	for (uint64_t index = 0; index < slots; index++)
	{
		data[index].hash = 0;
		data[index].value = -INFINITY;
	}
}

/*----------------------------------------------------------------------------*\
 |				 pawn::probe()				      |
\*----------------------------------------------------------------------------*/
bool pawn::probe(bitboard_t hash, int *value_ptr)
{

// Given the pawn structure described in hash, check the pawn table to see if
// we've evaluated it before.  If so, save its previous evaluation to the memory
// pointed to by value_ptr and return success.  If not, return failure.

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

// We've just evaluated the pawn structure described in hash.  Save its
// evaluation in the pawn table for future probes.

	uint64_t index = hash % slots;
	data[index].hash = hash;
	data[index].value = value;
}
