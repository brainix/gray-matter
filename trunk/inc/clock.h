/*----------------------------------------------------------------------------*\
 |	clock.h - chess clock interface					      |
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

#ifndef CLOCK_H
#define CLOCK_H

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"
#include "thread.h"

class chess_clock
{
public:
	chess_clock();
	void set_mode(int color, int new_moves, int new_secs, int new_inc);
	void update_remaining_secs(int color, int new_secs);
	void dec_remaining_moves(int color);
	void inc_remaining_moves(int color);
	void set_alarm(int color);
	static void sound_alarm();
	void cancel_alarm() const;
	void note_time();
	int get_elapsed() const;
private:
	int total_moves[COLORS];
	int remaining_moves[COLORS];
	int remaining_secs[COLORS];
	int inc[COLORS];
	clock_t noted_time;
};

#endif
