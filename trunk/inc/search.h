/*----------------------------------------------------------------------------*\
 |	search.h - move search interface				      |
 |									      |
 |	Copyright � 2005-2007, The Gray Matter Team, original authors.	      |
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

#ifndef SEARCH_H
#define SEARCH_H

using namespace std;

#include <board.h>
#include <table.h>
#include <history.h>
#include <xboard.h>
#include <types.h>

namespace gray
{

class search
{
public:
	search();
	~search();
	search& operator=(const search& that);
	static void handle(int num);
	void bind(table *t, history *h, xboard *x);
	void clear() const;
	move_t get_hint() const;
	pthread_t get_thread() const;
	void set_time(int t);
	void set_depth(int d);
	void set_output(bool o);
	static void *start(void *arg);
	void change(int s, const board& now);
private:
	list<move_t> pv;      /* Principal variation.                        */
	move_t hint;          /* Opponent's best move.                       */
	int max_time;         /* Maximum search time.                        */
	int max_depth;        /* Maximum search depth.                       */
	int nodes;            /* Number of nodes searched.                   */
	bool output;          /* Whether to print thinking output.           */

	board b;              /* Board representation object.                */
	table *table_ptr;     /* Transposition table object.                 */
	history *history_ptr; /* History table object.                       */
	xboard *xboard_ptr;   /* Chess Engine Communication Protocol object. */

	void iterate(int s);
	move_t negascout(int depth, int alpha, int beta);
	void extract(int s);
	static bool compare(move_t m1, move_t m2);
};

};
#endif
