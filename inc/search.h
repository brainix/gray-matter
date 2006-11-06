/*----------------------------------------------------------------------------*\
 |	search.h - move search interface				      |
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

#ifndef SEARCH_H
#define SEARCH_H

using namespace std;

#include <board.h>
#include <table.h>
#include <history.h>
#include <xboard.h>
#include <types.h>

class search
{
public:
	search();
	void bind(board *b, table *t, history *h, xboard *x);
	void set_time(int t);
	void set_depth(int d);
	void set_output(bool o);
	move_t iterate();
	static void handle(int num);
private:
	list<move_t> pv;      /* Principal variation.                        */
	int max_time;         /* Maximum search time.                        */
	int max_depth;        /* Maximum search depth.                       */
	int nodes;            /* Number of nodes searched.                   */
//	bool timeout;         /* Whether the search has timed out.           */
	bool output;          /* Whether to print thinking output.           */

	board *board_ptr;     /* Board representation object.                */
	table *table_ptr;     /* Transposition table object.                 */
	history *history_ptr; /* History table object.                       */
	xboard *xboard_ptr;   /* Chess Engine Communication Protocol object. */

	move_t negamax(int depth, int alpha, int beta);
	void extract(move_t m);
};

#endif
