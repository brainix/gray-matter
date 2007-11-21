/*----------------------------------------------------------------------------*\
 |	search_base.h - move search base iterface			      |
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

#ifndef SEARCH_BASE_H
#define SEARCH_BASE_H

// C++ stuff:
#include <list>

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "board_base.h"
#include "board_heuristic.h"
#include "table.h"
#include "clock.h"
#include "xboard.h"

// Search statuses:
#define IDLING		0 // Masturbating.
#define ANALYZING	1 // Analyzing (thinking indefinitely).
#define THINKING	2 // Thinking (on our own time).
#define PONDERING	3 // Pondering (on our opponent's time).
#define QUITTING	4 // Terminating search thread.
#define SEARCH_STATS	5

// Forward declarations:
class board_base;
class table;
class history;
class chess_clock;
class xboard;

class search_base
{
public:
	search_base(table *t, history *h, chess_clock *c, xboard *x);
	virtual ~search_base();
	virtual search_base& operator=(const search_base& that);
	virtual move_t get_hint() const;
	virtual thread_t get_thread() const;
	virtual void set_depth(int d);
	virtual void set_output(bool o);
	virtual void move_now();
	virtual void verify_prediction(move_t m);
	virtual void change(int s, const board_base& now);

protected:
	static void _handle(void *arg);            // Proxy clock callback.
	virtual void handle();                     // C++ clock callback.
	static void *_start(void *arg);            // Proxy thread entry point.
	virtual void start();                      // C++ thread entry point.
	virtual void iterate(int s) = 0;           // Force sub-classes to override.
	virtual void extract_pv();
	virtual void extract_hint(int s);
	static bool shuffle(move_t m1, move_t m2);
	static bool descend(move_t m1, move_t m2);

	list<move_t> pv;        // Principal variation.
	move_t hint;            // Opponent's best move.
	int max_depth;          // Maximum search depth.
	int nodes;              // Number of nodes searched.
	bool output;            // Whether to print thinking output.
	int correct_guesses;    //
	int total_guesses;      //

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

	// Prevent the class from being instantiated without the proper
	// construction.
	search_base();
};

#endif
