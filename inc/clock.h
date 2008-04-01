/*----------------------------------------------------------------------------*\
 |	clock.h - chess clock interface					      |
 |									      |
 |	Copyright © 2005-2008, The Gray Matter Team, original authors.	      |
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

#ifndef CLOCK_H
#define CLOCK_H

// C++ stuff:
#include <string>
#include <sstream>

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Time control styles:
#define CONVENTIONAL	0
#define INCREMENTAL	1
#define SECS_PER_MOVE	2

typedef void (*clock_callback_t)(void *data);

/// Chess clock.
class chess_clock
{
public:
	chess_clock(int o);
	void set_mode(int color, int new_moves, int new_csecs, int new_inc);
	std::string to_string(int color);
	void update_remaining_csecs(int color, int new_csecs);
	void dec_remaining_moves(int color);
	void inc_remaining_moves(int color);
	void set_callback(clock_callback_t cb, void *data);
	void set_alarm(int color) const;
	void cancel_alarm() const;
	void note_time();
	int get_elapsed() const;
	void swap_clocks();
private:
	int total_moves[COLORS];         ///<
	int remaining_moves[COLORS];     ///<
	int remaining_csecs[COLORS];     ///<
	int inc[COLORS];                 ///<
	int overhead;                    ///<
	clock_t noted_time;              ///<
	clock_callback_t clock_callback; ///<
	void *clock_callback_data;       ///<

	static void sound_alarm(void *data);
};

#endif
