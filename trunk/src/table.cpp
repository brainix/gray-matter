/*----------------------------------------------------------------------------*\
 |	table.cpp - transposition table implementation			      |
 |									      |
 |	Copyright © 2005-2006, The Gray Matter Team, original authors.	      |
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

#include <gray.h>

/*----------------------------------------------------------------------------*\
 |				    table()				      |
\*----------------------------------------------------------------------------*/
table::table()
{

/* Constructor. */

	pthread_mutex_init(&mutex, NULL);
	clear();
}

/*----------------------------------------------------------------------------*\
 |				    clear()				      |
\*----------------------------------------------------------------------------*/
void table::clear()
{
	/* Prevent other threads from shitting on our heads. */
	pthread_mutex_lock(&mutex);

	for (int policy = DEEP; policy <= FRESH; policy++)
		for (bitboard_t index = 0; index < ENTRIES / 2; index++)
		{
			data[policy][index].hash = 0;
			data[policy][index].depth = 0;
			data[policy][index].type = USELESS;
		}

	/* Allow other threads to shit on our heads again. */
	pthread_mutex_unlock(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				    probe()				      |
\*----------------------------------------------------------------------------*/
int table::probe(bitboard_t hash, move_t *move_ptr, int depth, int alpha,
                 int beta)
{
	bitboard_t index = hash % (ENTRIES / 2);
	int type = USELESS;

	/* Prevent other threads from shitting on our heads. */
	pthread_mutex_lock(&mutex);

	for (int policy = DEEP; policy <= FRESH; policy++)
		if (data[policy][index].hash == hash &&
		    data[policy][index].depth >= (unsigned) depth)
		{
			*move_ptr = data[policy][index].move;
			if (data[policy][index].type == ALPHA &&
			    move_ptr->value <= alpha)
			{
				move_ptr->value = alpha;
				type = ALPHA;
			}
			if (data[policy][index].type == BETA &&
			    move_ptr->value >= beta)
			{
				move_ptr->value = beta;
				type = BETA;
			}
			if (data[policy][index].type == EXACT)
				type = EXACT;
		}

	/* Allow other threads to shit on our heads again. */
	pthread_mutex_unlock(&mutex);
	return type;
}

/*----------------------------------------------------------------------------*\
 |				    store()				      |
\*----------------------------------------------------------------------------*/
void table::store(bitboard_t hash, move_t move, int depth, int type)
{
	bitboard_t index = hash % (ENTRIES / 2);

	/* Prevent other threads from shitting on our heads. */
	pthread_mutex_lock(&mutex);

	for (int policy = DEEP; policy <= FRESH; policy++)
		if (policy == FRESH || (unsigned) depth >= data[DEEP][index].depth)
		{
			data[policy][index].hash = hash;
			data[policy][index].move = move;
			data[policy][index].depth = depth;
			data[policy][index].type = type;
		}

	/* Allow other threads to shit on our heads again. */
	pthread_mutex_unlock(&mutex);
}
