/*----------------------------------------------------------------------------*\
 |	search_base.cpp - move search base implementation		      |
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

#include "search_base.h"

/*----------------------------------------------------------------------------*\
 |				 search_base()				      |
\*----------------------------------------------------------------------------*/
search_base::search_base(table *t, history *h, chess_clock *c, xboard *x)
{

// Constructor.  Important!  Seed the random number generator - issue
// srand(time(NULL)); - before instantiating this class!

	max_depth = MAX_DEPTH;
	output = false;
	correct_guesses = 0;
	total_guesses = 0;

	board_ptr = new board_heuristic();
	table_ptr = t;
	history_ptr = h;
	clock_ptr = c;
	xboard_ptr = x;

	mutex_create(&timeout_mutex);
	clock_ptr->set_callback((clock_callback_t) _handle, this);
	mutex_create(&depth_mutex);
	mutex_create(&search_mutex);
	cond_create(&search_cond, NULL);
	thread_create(&search_thread, (entry_t) _start, this);
}

/*----------------------------------------------------------------------------*\
 |				 ~search_base()				      |
\*----------------------------------------------------------------------------*/
search_base::~search_base()
{

// Destructor.

	cond_destroy(&search_cond);
	mutex_destroy(&search_mutex);
	mutex_destroy(&depth_mutex);
	mutex_destroy(&timeout_mutex);
}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
class search_base& search_base::operator=(const search_base& that)
{

// Overloaded assignment operator.

	if (this == &that)
		return *this;

	hint = that.hint;
	max_depth = that.max_depth;
	nodes = that.nodes;
	output = that.output;

	*board_ptr = *that.board_ptr;
	table_ptr = that.table_ptr;
	history_ptr = that.history_ptr;
	clock_ptr = that.clock_ptr;
	xboard_ptr = that.xboard_ptr;

	timeout_mutex = that.timeout_mutex;
	depth_flag = that.depth_flag;
	search_mutex = that.search_mutex;
	search_thread = that.search_thread;
	search_status = that.search_status;
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				   get_hint()				      |
\*----------------------------------------------------------------------------*/
move_t search_base::get_hint() const
{
	return hint;
}

/*----------------------------------------------------------------------------*\
 |				  get_thread()				      |
\*----------------------------------------------------------------------------*/
thread_t search_base::get_thread() const
{
	return search_thread;
}

/*----------------------------------------------------------------------------*\
 |				  set_depth()				      |
\*----------------------------------------------------------------------------*/
void search_base::set_depth(int d)
{

// Set the maximum search depth.

	max_depth = d;
}

/*----------------------------------------------------------------------------*\
 |				  set_output()				      |
\*----------------------------------------------------------------------------*/
void search_base::set_output(bool o)
{

// Set whether to print thinking output.

	output = o;
}

/*----------------------------------------------------------------------------*\
 |				   move_now()				      |
\*----------------------------------------------------------------------------*/
void search_base::move_now()
{
	if (search_status != THINKING)
		return;
	mutex_lock(&timeout_mutex);
	timeout_flag = true;
	mutex_unlock(&timeout_mutex);
}

/*----------------------------------------------------------------------------*\
 |			      verify_prediction()			      |
\*----------------------------------------------------------------------------*/
void search_base::verify_prediction(move_t m)
{
	correct_guesses += hint == m;
	total_guesses++;
}

/*----------------------------------------------------------------------------*\
 |				    change()				      |
\*----------------------------------------------------------------------------*/
void search_base::change(int s, const board_base& now)
{

// Synchronize the board to the position we're to search from (if necessary) and
// change the search status (to idling, analyzing, thinking, pondering, or
// quitting).
//
// Subtle!  start() and change() operate on the same search object (therefore
// the same board object) but are called from different threads.  Unless we take
// care to avoid this race condition, start() could ponder and go nuts on the
// board while change() could simultaneously set the board to a different
// position.  We avoid this naughty situation by using the search's timeout
// mechanism and the board's locking mechanism to guarantee the events occur in
// the following sequence:
//
//	time		search thread		I/O thread
//	----		-------------		----------
//	  0		grab board
//	  1		start pondering
//	  2					force pondering timeout
//	  3					wait for board
//	  4		stop pondering
//	  5		release board
//	  6		wait for command
//	  7					grab board
//	  8					set board position
//	  9					release board
//	 10					send thinking command
//	 11		grab board
//	 12		start thinking

	// Force pondering timeout.
	mutex_lock(&timeout_mutex);
	timeout_flag = true;
	mutex_unlock(&timeout_mutex);

	// Wait for the board, grab the board, set the board position, and
	// release the board.
	if (s == ANALYZING || s == THINKING || s == PONDERING)
	{
		board_ptr->lock();
		*board_ptr = now;
		extract_pv();
		extract_hint(s);
		board_ptr->unlock();
	}

	// Send the command to think.
	mutex_lock(&search_mutex);
	search_status = s;
	cond_signal(&search_cond);
	mutex_unlock(&search_mutex);
}

/*----------------------------------------------------------------------------*\
 |				    _handle()				      |
\*----------------------------------------------------------------------------*/
void search_base::_handle(void *arg)
{
	((search_base *) arg)->handle();
}

/*----------------------------------------------------------------------------*\
 |				    handle()				      |
\*----------------------------------------------------------------------------*/
void search_base::handle()
{

// The alarm has sounded.  Handle it.

	mutex_lock(&timeout_mutex);
	timeout_flag = true;
	mutex_unlock(&timeout_mutex);
}

/*----------------------------------------------------------------------------*\
 |				    _start()				      |
\*----------------------------------------------------------------------------*/
void *search_base::_start(void *arg)
{
	((search_base *) arg)->start();
	return NULL;
}

/*----------------------------------------------------------------------------*\
 |				    start()				      |
\*----------------------------------------------------------------------------*/
void search_base::start()
{

// Think of this method as main() for the search thread.  Wait for the status to
// change, then do the requested work.  Rinse, lather, and repeat, until XBoard
// commands us to quit.

	int old_search_status = search_status = IDLING;

	do
	{
		// Wait for the status to change.
		mutex_lock(&search_mutex);
		while (old_search_status == search_status)
			cond_wait(&search_cond, &search_mutex);
		old_search_status = search_status;
		mutex_unlock(&search_mutex);

		// Do the requested work - idle, analyze, think, ponder, or
		// quit.
		if (search_status == ANALYZING ||
		    search_status == THINKING  ||
		    search_status == PONDERING)
		{
			mutex_lock(&timeout_mutex);
			timeout_flag = false;
			mutex_unlock(&timeout_mutex);
			mutex_lock(&depth_mutex);
			depth_flag = false;
			mutex_unlock(&depth_mutex);
			iterate(search_status);
		}
	} while (search_status != QUITTING);

	thread_destroy(NULL);
}

/*----------------------------------------------------------------------------*\
 |				  extract_pv()				      |
\*----------------------------------------------------------------------------*/
void search_base::extract_pv()
{

// Extract the principal variation from the transposition table.

	move_t m;
	pv.clear();

	for (table_ptr->probe(board_ptr->get_hash(), 0, EXACT, &m); !IS_NULL_MOVE(m) && board_ptr->get_status(true) == IN_PROGRESS; table_ptr->probe(board_ptr->get_hash(), 0, EXACT, &m))
	{
		pv.push_back(m);
		board_ptr->make(m);
		if (pv.size() == (unsigned) max_depth)
			break;
	}
	for (size_t j = 0; j < pv.size(); j++)
		board_ptr->unmake();
}

/*----------------------------------------------------------------------------*\
 |				 extract_hint()				      |
\*----------------------------------------------------------------------------*/
void search_base::extract_hint(int s)
{

// Extract the hint (what we think our opponent should do) from the principal
// variation.  We call this method after extracting the principal variation
// either at various times during analyzing and thinking or just once before
// pondering.

	if (s == ANALYZING && !pv.empty())
		// We're analyzing.
		hint = pv.front();
	else if (s == THINKING && pv.size() >= 2)
	{
		// We're thinking.  That means the principal variation's 1st
		// move is what we think we should do, and the 2nd move is what
		// we think our opponent should do.
		list<move_t>::iterator it = pv.begin();
		hint = *++it;
	}
	else if (s == PONDERING && !pv.empty())
		// We're about to ponder.  That means the principal variation's
		// 1st move is what we think our opponent should do.
		hint = pv.front();
	else
		// The principal variation isn't long enough.  We don't know
		// what our opponent should do.
		SET_NULL_MOVE(hint);
}

/*----------------------------------------------------------------------------*\
 |				   shuffle()				      |
\*----------------------------------------------------------------------------*/
bool search_base::shuffle(move_t m1, move_t m2)
{

// Pass this method as the comparison function to l.sort() to randomize the move
// list.  This is a magnificent hack.
//
// Note: This hack wouldn't work for O(n²) list sort algorithms.  But if your
// STL's list sort algorithm is O(n²), you don't deserve for this hack to work
// anyway.

	return rand() & 1;
}

/*----------------------------------------------------------------------------*\
 |				   descend()				      |
\*----------------------------------------------------------------------------*/
bool search_base::descend(move_t m1, move_t m2)
{

// Pass this method as the comparison function to l.sort() to sort the move list
// from highest to lowest by score.

	return m1.value > m2.value;
}
