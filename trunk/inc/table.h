/*----------------------------------------------------------------------------*\
 |	table.h - transposition table interface				      |
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

#ifndef TABLE_H
#define TABLE_H

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"

/* Extra Gray Matter stuff: */
#include "xboard.h"

/* Forward declarations: */
class xboard;

class table
{
public:
	table(xboard *x, int mb = TABLE_MB);
	~table();
	void clear();
	int probe(bitboard_t hash, move_t *move_ptr, int depth, int alpha = INT_MIN, int beta = INT_MAX);
	void store(bitboard_t hash, move_t move, int depth, int type);
private:
	xboard *xboard_ptr; // Chess Engine Communication Protocol object.
	uint64_t slots;     //
	slot_t **data;      //
};

#endif