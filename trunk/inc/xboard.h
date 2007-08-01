/*----------------------------------------------------------------------------*\
 |	xboard.h - Chess Engine Communication Protocol interface	      |
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

#ifndef XBOARD_H
#define XBOARD_H

using namespace std;

/* C++ stuff: */
#include <list>

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"
#include "thread.h"

/* Extra Gray Matter stuff: */
#include "board.h"
#include "search_base.h"
#include "clock.h"

/* Forward declarations: */
class board;
class search_base;
class chess_clock;

class xboard
{
public:
	xboard();
	~xboard();
	void vomit(char *message) const;
	void loop(search_base *s, chess_clock *c);
	void print_output(int ply, int value, int time, int nodes, list<move_t> &pv) const;
	void print_result(move_t m);
	void print_resignation() const;
private:
	char buffer[80];          // Input buffer.
	bool ponder;              // Whether we're to ponder.
	bool force;               // Whether we're in force mode.
	bool draw;                // Whether our opponent has offered a draw.
	bool sync;                // Whether to sync our clock with XBoard's.

	board b;                  // Board representation object.
	search_base *search_ptr;  // Move search object.
	chess_clock *clock_ptr;   // Chess clock object.

	void print_move(move_t m) const;
	void do_xboard() const;
	void do_protover() const;
	void do_accepted() const;
	void do_rejected() const;
	void do_new();
	void do_quit() const;
	void do_force();
	void do_go();
	void do_playother();
	void do_level() const;
	void do_st();
	void do_sd() const;
	void do_time() const;
	void do_otim() const;
	void do_usermove();
	void do_question() const;
	void do_ping();
	void do_draw();
	void do_hint() const;
	void do_undo();
	void do_remove();
	void do_hard();
	void do_easy();
	void do_post() const;
	void do_nopost() const;

	int game_over();
	int str_to_num(const char *p) const;
	int str_to_secs(const char *p) const;
	int char_to_shape(char c) const;
	bool test_move(move_t m);
};

#endif
