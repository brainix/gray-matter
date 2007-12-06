/*----------------------------------------------------------------------------*\
 |	xboard.h - Chess Engine Communication Protocol interface	      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
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

#ifndef XBOARD_H
#define XBOARD_H

using namespace std;

// C++ stuff:
#include <list>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "board_base.h"
#include "board_heuristic.h"
#include "search_base.h"
#include "clock.h"
#include "book.h"

#define BUFFER_SIZE	80

// Forward declarations:
class board_base;
class search_base;
class chess_clock;
class book;

class xboard
{
public:
	xboard();
	~xboard();
	void vomit(char *message) const;
	void loop(search_base *s, chess_clock *c, book *o);
	void print_output(int ply, int value, int time, int nodes, list<move_t> &pv) const;
	void print_result(move_t m);
	void print_resignation();
private:
	char buffer[BUFFER_SIZE]; // Input buffer.
	bool analyze;             // Whether we're analyzing.
	bool ponder;              // Whether we're to ponder.
	bool force;               // Whether we're in force mode.
	bool draw;                // Whether our opponent has offered a draw.
	bool sync;                // Whether to sync our clock with XBoard's.

	board_base *board_ptr;    // Board representation object.
	search_base *search_ptr;  // Move search engine object.
	chess_clock *clock_ptr;   // Chess clock object.
	book *book_ptr;           // Opening book object.

	vector <string> ts_fen;	  // Test Suite Data
	vector <string> ts_sol;
	bool ts_mode;
	int ts_erroneous;		  // Test Suite statistics.
	int ts_success, ts_failure;

	void print_move(move_t m, bool san = false) const;
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
	void do_setboard(string argfen="") const;
	void do_hint() const;
	void do_undo();
	void do_remove();
	void do_hard();
	void do_easy();
	void do_post() const;
	void do_nopost() const;
	void do_analyze();
	void do_exit();
	void do_display() const;
	void do_test();
	void do_unknown() const;

	int game_over();
	int str_to_num(const char *p) const;
	int str_to_secs(const char *p) const;
	int char_to_shape(char c) const;
	bool test_move(move_t m);
	void test_suite_next();
};

#endif
