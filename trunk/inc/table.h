/*----------------------------------------------------------------------------*\
 |	table.h - transposition and pawn table interfaces		      |
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

#ifndef TABLE_H
#define TABLE_H

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"

class table
{
public:
	table(int mb = XPOS_TABLE_MB);
	~table();
	void clear();
	bool probe(bitboard_t hash, int depth, move_t *move_ptr, int bound) const;
	void store(bitboard_t hash, int depth, move_t move, int bound);
private:
	uint64_t slots;
	xpos_slot_t **data;
};

class pawn
{
public:
	pawn(int mb = PAWN_TABLE_MB);
	~pawn();
	void clear();
	bool probe(bitboard_t hash, int *value_ptr) const;
	void store(bitboard_t hash, int value);
private:
	uint64_t slots;
	pawn_slot_t *data;
};

#endif
