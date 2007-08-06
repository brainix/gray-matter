/*----------------------------------------------------------------------------*\
 |	clock.cpp - chess clock implementation				      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
\*----------------------------------------------------------------------------*/

/*
 | This program is free software: you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 3 of the License, or (at your option) any later
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
#include "clock.h"

/* Global variables: */

/*----------------------------------------------------------------------------*\
 |				 chess_clock()				      |
\*----------------------------------------------------------------------------*/
chess_clock::chess_clock()
{

/* Constructor. */

	for (int color = WHITE; color <= BLACK; color++)
		set_mode(color, 40, 5 * 60 * 100, 0);
	timer_function(sound_alarm, this);
	note_time();
}

/*----------------------------------------------------------------------------*\
 |				   set_mode()				      |
\*----------------------------------------------------------------------------*/
void chess_clock::set_mode(int color, int new_moves, int new_csecs, int new_inc)
{

/*
 | Set the mode.  We allow different modes for white and black.  A mode is
 | specified as a number of moves, which must be made in a number of seconds,
 | with an increment of time added to the clock after each move.
 */

	total_moves[color] = new_moves;
	remaining_moves[color] = new_moves;
	remaining_csecs[color] = new_csecs;
	inc[color] = new_inc;
}

/*----------------------------------------------------------------------------*\
 |			    update_remaining_csecs()			      |
\*----------------------------------------------------------------------------*/
void chess_clock::update_remaining_csecs(int color, int new_csecs)
{
	remaining_csecs[color] = new_csecs;
}

/*----------------------------------------------------------------------------*\
 |			     dec_remaining_moves()			      |
\*----------------------------------------------------------------------------*/
void chess_clock::dec_remaining_moves(int color)
{
	if (--remaining_moves[color] == 0)
		remaining_moves[color] = total_moves[color];
}

/*----------------------------------------------------------------------------*\
 |			     inc_remaining_moves()			      |
\*----------------------------------------------------------------------------*/
void chess_clock::inc_remaining_moves(int color)
{
	if (++remaining_moves[color] > total_moves[color])
		remaining_moves[color] = 1;
}

/*----------------------------------------------------------------------------*\
 |				 set_callback()				      |
\*----------------------------------------------------------------------------*/
void chess_clock::set_callback(clock_callback_t cb, void *data)
{
	clock_callback = cb;
	clock_callback_data = data;
}

/*----------------------------------------------------------------------------*\
 |				  set_alarm()				      |
\*----------------------------------------------------------------------------*/
void chess_clock::set_alarm(int color) const
{

/* Set the alarm. */

	timer_set(GREATER(remaining_csecs[color] / (remaining_moves[color] ? remaining_moves[color] : 40) + inc[color] - OVERHEAD, 1));
}

/*----------------------------------------------------------------------------*\
 |				 sound_alarm()				      |
\*----------------------------------------------------------------------------*/
void chess_clock::sound_alarm(void *data)
{

/* Sound the alarm. */

	chess_clock *clock = (chess_clock*)data;

	clock->clock_callback(clock->clock_callback_data);
}

/*----------------------------------------------------------------------------*\
 |				 cancel_alarm()				      |
\*----------------------------------------------------------------------------*/
void chess_clock::cancel_alarm() const
{

/* Cancel the alarm. */

	timer_cancel();
}

/*----------------------------------------------------------------------------*\
 |				  note_time()				      |
\*----------------------------------------------------------------------------*/
void chess_clock::note_time()
{

/* Note the time. */

	noted_time = clock();
}

/*----------------------------------------------------------------------------*\
 |				 get_elapsed()				      |
\*----------------------------------------------------------------------------*/
int chess_clock::get_elapsed() const
{

/* Return the number of seconds elapsed since the last noted time. */

	return (clock() - noted_time) * 100 / CLOCKS_PER_SEC;
}

/*----------------------------------------------------------------------------*\
 |				 swap_clocks()				      |
\*----------------------------------------------------------------------------*/
void chess_clock::swap_clocks()
{
	total_moves[BLACK] ^= total_moves[WHITE];
	total_moves[WHITE] ^= total_moves[BLACK];
	total_moves[BLACK] ^= total_moves[WHITE];

	remaining_moves[BLACK] ^= remaining_moves[WHITE];
	remaining_moves[WHITE] ^= remaining_moves[BLACK];
	remaining_moves[BLACK] ^= remaining_moves[WHITE];

	remaining_csecs[BLACK] ^= remaining_csecs[WHITE];
	remaining_csecs[WHITE] ^= remaining_csecs[BLACK];
	remaining_csecs[BLACK] ^= remaining_csecs[WHITE];

	inc[BLACK] ^= inc[WHITE];
	inc[WHITE] ^= inc[BLACK];
	inc[BLACK] ^= inc[WHITE];
}
