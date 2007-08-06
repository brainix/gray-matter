/*----------------------------------------------------------------------------*\
 |	search_base.h - move search base iterface			      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
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
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
 */

#ifndef SEARCH_BASE_H
#define SEARCH_BASE_H

#include "board_base.h"
#include "board_heuristic.h"
#include "table.h"
#include "clock.h"

/* Forward declarations: */
class xboard;

class search_base
{
public:
	search_base(table *t, history *h, chess_clock *c, xboard *x);
	virtual ~search_base();
	virtual search_base& operator=(const search_base& that);

	virtual void move_now();
	virtual move_t get_hint() const;
	virtual void change(int s, const board_base& now);
	virtual thread_t get_thread() const;
	virtual void set_depth(int d);
	virtual void set_output(bool o);

protected:
	static void _handle(void *arg);  // Proxy clock callback.
	virtual void handle();           // C++ clock callback.
	static void *_start(void *arg);  // Proxy thread entry point.
	virtual void start();            // C++ thread entry point.
	virtual void iterate(int s) = 0; // Force sub-classes to override.

	list<move_t> pv;        // Principal variation.
	move_t hint;            // Opponent's best move.
	int max_depth;          // Maximum search depth.
	int nodes;              // Number of nodes searched.
	bool output;            // Whether to print thinking output.

	board_base *board_ptr;  // Board representation object.
	table *table_ptr;       // Transposition table object.
	history *history_ptr;   // History table object.
	chess_clock *clock_ptr; // Chess clock object.
	xboard *xboard_ptr;     // Chess Engine Communication Protocol object.

	mutex_t timeout_mutex;  // The lock that protects...
	bool timeout_flag;      // ...one flag that determines when to stop
	                        // thinking or pondering!  :-D

	mutex_t depth_mutex;    // The lock that protects...
	bool depth_flag;        // ...the other flag that determines when to 
	                        // stop thinking or pondering!  :-D

	mutex_t search_mutex;   // The lock that protects...
	cond_t search_cond;     // ...the condition that controls...
	thread_t search_thread; // ...the search thread via...
	int search_status;      // ...the search status!  :-D

	/* Prevent the class from being created without the proper construction. */
	search_base();
};

#endif
