/*----------------------------------------------------------------------------*\
 |	table.h - transposition, history, and pawn table interfaces	      |
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

#ifndef TABLE_H
#define TABLE_H

// Default Gray Matter stuff:
#include "config.h"
#include "types.h"

// Transposition table entry replacement policies:
#define DEEP		0 // Replace if same depth or deeper.
#define FRESH		1 // Replace always.
#define POLICIES	2

// Transposition table entry types:
#define USELESS		0 // Useless.
#define BOOK		1 // Prescribed by the opening book.
#define EXACT		2 // Exact MiniMax value.
#define UPPER		3 // Upper bound.
#define LOWER		4 // Lower bound.
#define ENTRY_TYPES	5

/*----------------------------------------------------------------------------*\
 |			      Transposition Table			      |
\*----------------------------------------------------------------------------*/

class table
{
public:
	table(int mb = XPOS_TABLE_MB);
	~table();
	void clear();
	bool probe(bitboard_t hash, int depth, int type, move_t *move_ptr);
	void store(bitboard_t hash, int depth, int type, move_t move);
private:
	uint64_t slots;     // The number of slots.
	xpos_slot_t **data; // The slots themselves.
};

/*----------------------------------------------------------------------------*\
 |				 History Table				      |
\*----------------------------------------------------------------------------*/

class history
{
public:
	history();
	~history();
	void clear();
	int probe(bool color, move_t move) const;
	void store(bool color, move_t move, int depth);
private:
	int *****data;
};

/*----------------------------------------------------------------------------*\
 |				   Pawn Table				      |
\*----------------------------------------------------------------------------*/

class pawn
{
public:
	pawn(int mb = PAWN_TABLE_MB);
	~pawn();
	void clear();
	bool probe(bitboard_t hash, int *value_ptr);
	void store(bitboard_t hash, int value);
private:
	uint64_t slots;    // The number of slots.
	pawn_slot_t *data; // The slots themselves.
};

#endif
