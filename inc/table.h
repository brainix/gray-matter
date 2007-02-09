/*----------------------------------------------------------------------------*\
 |	table.h - transposition table interface				      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
 |		All rights reserved.					      |
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

using namespace std;

#include <config.h>
#include <types.h>

class table
{
public:
	table(int mb = 0);
	~table();
	void clear();
	int probe(bitboard_t hash, move_t *move_ptr, int depth, int alpha = INT_MIN, int beta = INT_MAX);
	void store(bitboard_t hash, move_t move, int depth, int type);
private:
	pthread_mutex_t mutex;
	bitboard_t entries;
	entry_t **data;
};

#endif
