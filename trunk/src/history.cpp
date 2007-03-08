/*----------------------------------------------------------------------------*\
 |	history.cpp - history table implementation			      |
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

#include "gray.h"
#include "history.h"

/*----------------------------------------------------------------------------*\
 |				   history()				      |
\*----------------------------------------------------------------------------*/
history::history(xboard *x)
{

/* Constructor. */

	xboard_ptr = x;

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
		xboard_ptr->vomit("Couldn't allocate history table.");
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
 |				    clear()				      |
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
 |				    probe()				      |
\*----------------------------------------------------------------------------*/
int history::probe(bool color, move_t move)
{
	return data[color][move.old_x][move.old_y][move.new_x][move.new_y];
}

/*----------------------------------------------------------------------------*\
 |				    store()				      |
\*----------------------------------------------------------------------------*/
void history::store(bool color, move_t move, int depth)
{

/* Gray Matter has searched to the specified depth and determined the specified
 * move for the specified color to be the best.  Note this. */

	data[color][move.old_x][move.old_y][move.new_x][move.new_y] += 1 << depth;
}