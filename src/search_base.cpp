/*----------------------------------------------------------------------------*\
 |  search_base.cpp - move search base implementation                         |
 |                                                                            |
 |  Copyright � 2005-2008, The Gray Matter Team, original authors.            |
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
#include "board_heuristic.h"
#include "clock.h"

using namespace std;

#ifdef DEBUG_SEARCH
int search_base::debug_depth = 0;
int search_base::debug_maxdepth = 0;
string search_base::debug_pv = "";
string search_base::debug_mv = "";
string search_base::debug_pv_prefix = "";
#endif

/*----------------------------------------------------------------------------*\
 |                               search_base()                                |
\*----------------------------------------------------------------------------*/
search_base::search_base(table *t, history *h, chess_clock *c, xboard *x)
{

/// Constructor.  Important!  Seed the random number generator - issue
/// <code>srand(time(NULL));</code> - before instantiating this class!

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
    mutex_create(&search_mutex);
    cond_create(&search_cond, NULL);
    thread_create(&search_thread, (entry_t) _start, this);
}

/*----------------------------------------------------------------------------*\
 |                               ~search_base()                               |
\*----------------------------------------------------------------------------*/
search_base::~search_base()
{

/// Destructor.

    cond_destroy(&search_cond);
    mutex_destroy(&search_mutex);
    mutex_destroy(&timeout_mutex);
}

/*----------------------------------------------------------------------------*\
 |                                     =                                      |
\*----------------------------------------------------------------------------*/
class search_base& search_base::operator=(const search_base& that)
{

/// Overloaded assignment operator.

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
    search_mutex = that.search_mutex;
    search_thread = that.search_thread;
    search_status = that.search_status;
    return *this;
}

/*----------------------------------------------------------------------------*\
 |                                 get_hint()                                 |
\*----------------------------------------------------------------------------*/
Move search_base::get_hint() const
{
    return hint;
}

/*----------------------------------------------------------------------------*\
 |                                get_thread()                                |
\*----------------------------------------------------------------------------*/
thread_t search_base::get_thread() const
{
    return search_thread;
}

/*----------------------------------------------------------------------------*\
 |                                set_depth()                                 |
\*----------------------------------------------------------------------------*/
void search_base::set_depth(int d)
{

/// Set the maximum search depth.

    max_depth = d;
}

/*----------------------------------------------------------------------------*\
 |                                set_output()                                |
\*----------------------------------------------------------------------------*/
void search_base::set_output(bool o)
{

/// Set whether to print thinking output.

    output = o;
}

/*----------------------------------------------------------------------------*\
 |                                 move_now()                                 |
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
 |                            verify_prediction()                             |
\*----------------------------------------------------------------------------*/
void search_base::verify_prediction(Move m)
{
    correct_guesses += hint == m;
    total_guesses++;
}

/*----------------------------------------------------------------------------*\
 |                                  change()                                  |
\*----------------------------------------------------------------------------*/
void search_base::change(int s, const board_base& now)
{

/// Synchronize the board to the position we're to search from (if necessary)
/// and change the search status (to idling, analyzing, thinking, pondering, or
/// quitting).
///
/// Subtle!  start() and change() operate on the same search object (therefore
/// the same board object) but are called from different threads.  Unless we
/// take care to avoid this race condition, start() could ponder and go nuts on
/// the board while change() could simultaneously set the board to a different
/// position.  We avoid this naughty situation by using the search's timeout
/// mechanism and the board's locking mechanism to guarantee the events occur in
/// the following sequence:
///
///	time        search thread       I/O thread
///	----        -------------       ----------
///	  0	        grab board
///	  1	        start pondering
///	  2	                            force pondering timeout
///	  3                             wait for board
///	  4	        stop pondering
///	  5         release board
///	  6         wait for command
///	  7                             grab board
///	  8                             set board position
///	  9                             release board
///	 10                             send thinking command
///	 11         grab board
///	 12         start thinking

    // Force pondering timeout.
    mutex_lock(&timeout_mutex);
    timeout_flag = true;
    mutex_unlock(&timeout_mutex);

    // Wait for the board, grab the board, set the board position, and release
    // the board.
	board_ptr->lock();
	*board_ptr = now;
	extract_pv();
	extract_hint(s);
	board_ptr->unlock();

    // Send the command to think.
    mutex_lock(&search_mutex);
#ifndef _MSDEV_WINDOWS
    DEBUG_SEARCH_PRINTA("search_base::change changes state from %s to %s.",
        status_to_string(search_status).c_str(), status_to_string(s).c_str());
#endif
    search_status = s;
    token_update++;
    cond_signal(&search_cond);
    mutex_unlock(&search_mutex);
}

/*----------------------------------------------------------------------------*\
 |                                 _handle()                                  |
\*----------------------------------------------------------------------------*/
void search_base::_handle(void *arg)
{
    ((search_base *) arg)->handle();
}

/*----------------------------------------------------------------------------*\
 |                                  handle()                                  |
\*----------------------------------------------------------------------------*/
void search_base::handle()
{

/// The alarm has sounded.  Handle it.

    mutex_lock(&timeout_mutex);
#ifndef _MSDEV_WINDOWS    
	DEBUG_SEARCH_PRINT("Setting timeout flag");
#endif
    timeout_flag = true;
    mutex_unlock(&timeout_mutex);
}

/*----------------------------------------------------------------------------*\
 |                                  _start()                                  |
\*----------------------------------------------------------------------------*/
void *search_base::_start(void *arg)
{
    ((search_base *) arg)->start();
    return NULL;
}

/*----------------------------------------------------------------------------*\
 |                                  start()                                   |
\*----------------------------------------------------------------------------*/
void search_base::start()
{

/// Think of this method as main() for the search thread.  Wait for either the
/// status or the board to change, then do the requested work.  Rinse, lather,
/// and repeat, until XBoard commands us to quit.

    search_status = IDLING;

    do
    {
        // Wait for either the status or the board to change.
        mutex_lock(&search_mutex);
        while (!token_update)
            cond_wait(&search_cond, &search_mutex);
        token_update = 0;
        mutex_unlock(&search_mutex);

        // Do the requested work - idle, analyze, think, ponder, or quit.
#ifndef _MSDEV_WINDOWS
        bitboard_t board_hash = board_ptr->get_hash();
        DEBUG_SEARCH_PRINTA("search_base::start doing requested work (hash = %llx, status = %s).",
            (long long unsigned)board_hash, status_to_string(search_status).c_str());
#endif

        if (search_status == ANALYZING ||
            search_status == THINKING  ||
            search_status == PONDERING)
        {
            mutex_lock(&timeout_mutex);
            timeout_flag = false;
            mutex_unlock(&timeout_mutex);
            iterate(search_status);
        }
    } while (search_status != QUITTING);

    thread_destroy(NULL);
}

/*----------------------------------------------------------------------------*\
 |                                extract_pv()                                |
\*----------------------------------------------------------------------------*/
void search_base::extract_pv()
{
  /// Extract the principal variation from the transposition table.
    Move m;
    pv.clear();

    for (table_ptr->probe(board_ptr->get_hash(), 0, EXACT, &m); 
         !m.is_null() && board_ptr->get_status(true) == IN_PROGRESS;
         table_ptr->probe(board_ptr->get_hash(), 0, EXACT, &m))
    {
        pv.addMove(m);
        board_ptr->make(m);
        if (pv.size() == (unsigned) max_depth)
            break;
    }
    for (size_t j = 0; j < pv.size(); j++)
        board_ptr->unmake();
}

/*----------------------------------------------------------------------------*\
 |                               extract_hint()                               |
\*----------------------------------------------------------------------------*/
void search_base::extract_hint(int s)
{

/// Extract the hint (what we think that our opponent should do) from the
/// principal variation.  We call this method after extracting the principal
/// variation either at various times during analyzing and thinking or just once
/// before pondering.

    if (s == ANALYZING && !(pv.size() == 0))
        // We're analyzing.
        hint = pv.theArray[0]; // .front();
    else if (s == THINKING && pv.size() >= 2)
    {
        // We're thinking.  That means that the principal variation's 1st move
        // is what we think that we should do, and its 2nd move is what we think
        // that our opponent should do.
        //list<Move>::iterator it = pv.begin();
        hint = pv.theArray[1]; //*++it;
    }
    else if (s == PONDERING && !(pv.size() == 0))
        // We're about to ponder.  That means that the principal variation's 1st
        // move is what we think that our opponent should do.
        hint = pv.theArray[0]; //pv.front();
    else
        // The principal variation isn't long enough.  We don't know what our
        // opponent should do.
        hint.set_null();
}

/*----------------------------------------------------------------------------*\
 |                             status_to_string()                             |
\*----------------------------------------------------------------------------*/
string search_base::status_to_string(int status)
{

/// Convert status to a string.

    string str;
    switch(status) 
    {
        case IDLING    : str = "IDLING";         break;
        case ANALYZING : str = "ANALYZING";      break;
        case THINKING  : str = "THINKING";       break;
        case PONDERING : str = "PONDERING";      break;
        case QUITTING  : str = "QUITTING";       break;
        default        : str = "ILLEGAL_STATUS"; break;
    }
    return str;
}

void search_base::reset()
{
  table_ptr->clear();
  history_ptr->clear();
}
