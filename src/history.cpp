/*----------------------------------------------------------------------------*\
 |	history.cpp - history table implementation			      |
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

#include <gray.h>

/*----------------------------------------------------------------------------*\
 |				   history()				      |
\*----------------------------------------------------------------------------*/
history::history()
{

/* Constructor. */

	pthread_mutex_init(&mutex, NULL);
	assert(data = (int *****) malloc(COLORS * sizeof(int ****)));
	for (int color = WHITE; color <= BLACK; color++)
	{
		assert(data[color] = (int ****) malloc(8 * sizeof(int ***)));
		for (int old_x = 0; old_x <= 7; old_x++)
		{
			assert(data[color][old_x] = (int ***) malloc(8 * sizeof(int **)));
			for (int old_y = 0; old_y <= 7; old_y++)
			{
				assert(data[color][old_x][old_y] = (int **) malloc(8 * sizeof(int *)));
				for (int new_x = 0; new_x <= 7; new_x++)
					assert(data[color][old_x][old_y][new_x] = (int *) malloc(8 * sizeof(int)));
			}
		}
	}
	clear();
}

/*----------------------------------------------------------------------------*\
 |				   ~history()				      |
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
					free(data[color][old_x][old_y][new_x]);
				free(data[color][old_x][old_y]);
			}
			free(data[color][old_y]);
		}
		free(data[color]);
	}
	free(data);
}

/*----------------------------------------------------------------------------*\
 |				    clear()				      |
\*----------------------------------------------------------------------------*/
void history::clear()
{
	/* Prevent other threads from shitting on our heads. */
//	pthread_mutex_lock(&mutex);

	for (int color = WHITE; color <= BLACK; color++)
		for (int old_y = 0; old_y <= 7; old_y++)
			for (int old_x = 0; old_x <= 7; old_x++)
				for (int new_y = 0; new_y <= 7; new_y++)
					for (int new_x = 0; new_x <= 7; new_x++)
						data[color][old_x][old_y][new_x][new_y] = 0;

	/* Allow other threads to shit on our heads again. */
//	pthread_mutex_unlock(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				    probe()				      |
\*----------------------------------------------------------------------------*/
int history::probe(bool color, move_t move)
{
	int value;

	/* Prevent other threads from shitting on our heads. */
//	pthread_mutex_lock(&mutex);

	value = data[color][move.old_x][move.old_y][move.new_x][move.new_y];

	/* Allow other threads to shit on our heads again. */
//	pthread_mutex_unlock(&mutex);
	return value;
}

/*----------------------------------------------------------------------------*\
 |				    store()				      |
\*----------------------------------------------------------------------------*/
void history::store(bool color, move_t move, int depth)
{

/* Gray Matter has searched to the specified depth and determined the specified
 * move for the specified color to be the best.  Note this. */

	/* Prevent other threads from shitting on our heads. */
//	pthread_mutex_lock(&mutex);

	data[color][move.old_x][move.old_y][move.new_x][move.new_y] += 1 << depth;

	/* Allow other threads to shit on our heads again. */
//	pthread_mutex_unlock(&mutex);
}
