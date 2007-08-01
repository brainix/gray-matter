/*----------------------------------------------------------------------------*\
 |	search_mtdf.h - move search interface				      |
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

#ifndef SEARCH_MTDF_H
#define SEARCH_MTDF_H

using namespace std;

/* C++ stuff: */
#include <algorithm>
#include <list>

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"
#include "thread.h"

/* Extra Gray Matter stuff: */
#include "board.h"
#include "table.h"
#include "clock.h"
#include "xboard.h"

/* Forward declarations: */
class board;
class table;
class history;
class chess_clock;
class xboard;

class search_mtdf
{
public:
	search_mtdf(table *t, history *h, chess_clock *c, xboard *x);
	~search_mtdf();
	search_mtdf& operator=(const search_mtdf& that);
	static void handle();
	void move_now() const;
	void clear() const;
	move_t get_hint() const;
	thread_t get_thread() const;
	void set_depth(int d);
	void set_output(bool o);
	static void *start(void *arg);
	void change(int s, const board& now);
private:
	list<move_t> pv;        // Principal variation.
	move_t hint;            // Opponent's best move.
	int max_depth;          // Maximum search depth.
	int nodes;              // Number of nodes searched.
	bool output;            // Whether to print thinking output.

	board b;                // Board representation object.
	table *table_ptr;       // Transposition table object.
	history *history_ptr;   // History table object.
	chess_clock *clock_ptr; // Chess clock object.
	xboard *xboard_ptr;     // Chess Engine Communication Protocol object.

	void iterate(int s);
	move_t mtdf(int depth, int guess);
	move_t minimax(int depth, int shallowness, int alpha, int beta);
	static bool shuffle(move_t m1, move_t m2);
	static bool descend(move_t m1, move_t m2);
	void extract(int s);
};

#endif
