/*----------------------------------------------------------------------------*\
 |	search.cpp - move search implementation				      |
 |									      |
 |	Copyright � 2005-2007, The Gray Matter Team, original authors.	      |
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

#include "gray.h"
#include "search.h"

/* Global variables: */
thread_t thread; // The thread in which this...
mutex_t mutex;   // ...mutex protects this...
cond_t cond;     // ...condition which watches the...
int status;      // ...search status!  :-D
bool timeout;    // Whether to stop thinking or pondering.

/*----------------------------------------------------------------------------*\
 |				    search()				      |
\*----------------------------------------------------------------------------*/
search::search(xboard *x, table *t, history *h)
{

/* Constructor. */

	max_time = INT_MAX;
	max_depth = DEPTH;
	output = false;

	xboard_ptr = x;
	table_ptr = t;
	history_ptr = h;

#if defined(LINUX) || defined(OS_X)
	signal(SIGALRM, handle);
#endif

	mutex_init(&mutex);
	cond_init(&cond, NULL);
	thread_create(&thread, (entry_t) start, this);
}

/*----------------------------------------------------------------------------*\
 |				   ~search()				      |
\*----------------------------------------------------------------------------*/
search::~search()
{

/* Destructor. */

	cond_destroy(&cond);
	mutex_destroy(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
class search& search::operator=(const search& that)
{

/* Overloaded assignment operator. */

	if (this == &that)
		return *this;

	pv = that.pv;
	hint = that.hint;
	max_time = that.max_time;
	max_depth = that.max_depth;
	nodes = that.nodes;
	output = that.output;

	b = that.b;
	table_ptr = that.table_ptr;
	history_ptr = that.history_ptr;
	xboard_ptr = that.xboard_ptr;

	return *this;
}

/*----------------------------------------------------------------------------*\
 |				    handle()				      |
\*----------------------------------------------------------------------------*/
void search::handle(int num)
{

/* The alarm has sounded.  Handle it. */

	timeout = true;
}

/*----------------------------------------------------------------------------*\
 |				    clear()				      |
\*----------------------------------------------------------------------------*/
void search::clear() const
{
	table_ptr->clear();
	history_ptr->clear();
}

/*----------------------------------------------------------------------------*\
 |				   get_hint()				      |
\*----------------------------------------------------------------------------*/
move_t search::get_hint() const
{
	return hint;
}

/*----------------------------------------------------------------------------*\
 |				  get_thread()				      |
\*----------------------------------------------------------------------------*/
thread_t search::get_thread() const
{
	return thread;
}

/*----------------------------------------------------------------------------*\
 |				   set_time()				      |
\*----------------------------------------------------------------------------*/
void search::set_time(int t)
{

/* Set the maximum search time. */

	max_time = t;
}

/*----------------------------------------------------------------------------*\
 |				  set_depth()				      |
\*----------------------------------------------------------------------------*/
void search::set_depth(int d)
{

/* Set the maximum search depth. */

	max_depth = d;
}

/*----------------------------------------------------------------------------*\
 |				  set_output()				      |
\*----------------------------------------------------------------------------*/
void search::set_output(bool o)
{

/* Set whether to print thinking output. */

	output = o;
}

/*----------------------------------------------------------------------------*\
 |				    start()				      |
\*----------------------------------------------------------------------------*/
void *search::start(void *arg)
{

/* Think of this method as main() for the search thread. */

	class search *search_ptr = (class search *) arg;
	int tmp = status = IDLING;

	do
	{
		/* Wait for the status to change. */
		mutex_lock(&mutex);
		while (tmp == status)
			cond_wait(&cond, &mutex);
		tmp = status;
		mutex_unlock(&mutex);

		/* Do the requested work - idle, think, ponder, or quit. */
		if (status == THINKING || status == PONDERING)
		{
			timeout = false;
			search_ptr->iterate(status);
		}
	} while (status != QUITTING);

	thread_exit();
	return NULL;
}

/*----------------------------------------------------------------------------*\
 |				    change()				      |
\*----------------------------------------------------------------------------*/
void search::change(int s, const board& now)
{

/*
 | Synchronize the board to the position we're to search from (if necessary) and
 | change the search status (to idling, thinking, pondering, or quitting).
 |
 | Subtle!  start() and change() operate on the same search object (therefore
 | the same board object) but are called from different threads.  Unless we take
 | care to avoid this race condition, start() could ponder and go nuts on the
 | board while change() could simultaneously set the board to a different
 | position.  We avoid this naughty situation by using the search's timeout
 | mechanism and the board's locking mechanism to guarantee the events occur in
 | the following sequence:
 |
 |	time		search thread		I/O thread
 |	----		-------------		----------
 |	  0		grab board
 |	  1		start pondering
 |	  2					force pondering timeout
 |	  3					wait for board
 |	  4		stop pondering
 |	  5		release board
 |	  6		wait for command
 |	  7					grab board
 |	  8					set board position
 |	  9					release board
 |	 10					send thinking command
 |	 11		grab board
 |	 12		start thinking
 */

	timeout = true;
	if (s == THINKING || s == PONDERING)
	{
		b.lock();
		b = now;
		b.unlock();
	}

	mutex_lock(&mutex);
	status = s;
	cond_signal(&cond);
	mutex_unlock(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				   iterate()				      |
\*----------------------------------------------------------------------------*/
void search::iterate(int s)
{

/*
 | Perform iterative deepening.  This method handles both thinking (on our own
 | time) and pondering (on our opponent's time) since they're so similar.
 */

	/*
	 | Initialize the number of nodes searched and note the start time.  If
	 | we're to think, set the alarm.
	 */
	nodes = 0;
	clock_t start = clock();
	if (s == THINKING)
	{
#if defined(LINUX) || defined(OS_X)
		struct itimerval itimerval;
		itimerval.it_interval.tv_sec = 0;
		itimerval.it_interval.tv_usec = 0;
		itimerval.it_value.tv_sec = max_time;
		itimerval.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &itimerval, NULL);
#elif defined(WINDOWS)
#endif
	}

	/*
	 | Perform iterative deepening until the alarm has sounded (if we're
	 | thinking), our opponent has moved (if we're pondering), or we've
	 | reached the maximum depth (either way).
	 */
	b.lock();
	for (int depth = 0; depth <= max_depth; depth++)
	{
		negascout(depth, -WEIGHT_KING, WEIGHT_KING);
		if (timeout && depth)
			/*
			 | Oops.  The alarm has interrupted this iteration; the
			 | current results are incomplete and unreliable.  Go
			 | with the last iteration's results.
			 */
			break;
		extract(s);
		if (output)
			xboard_ptr->print_output(depth + 1, pv.front().value, (clock() - start) / CLOCKS_PER_SEC, nodes, pv);
		if (pv.front().value == WEIGHT_KING || pv.front().value == -WEIGHT_KING)
			/*
			 | Oops.  The game will be over at this depth.  There's
			 | no point in searching deeper.
			 */
			break;
	}
	b.unlock();

	/*
	 | If we've just finished thinking, clear the alarm and inform XBoard of
	 | our favorite move.
	 */
	if (s == THINKING)
	{
#if defined(LINUX) || defined(OS_X)
		struct itimerval itimerval;
		itimerval.it_interval.tv_sec = 0;
		itimerval.it_interval.tv_usec = 0;
		itimerval.it_value.tv_sec = 0;
		itimerval.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &itimerval, NULL);
#elif defined(WINDOWS)
#endif
		if (status != QUITTING)
			xboard_ptr->print_result(pv.front());
	}
}

/*----------------------------------------------------------------------------*\
 |				  negascout()				      |
\*----------------------------------------------------------------------------*/
move_t search::negascout(int depth, int alpha, int beta)
{

/*
 | From the current position, search for the best move.  This method implements
 | the NegaMax algorithm.  NegaMax produces the same results as MiniMax but is
 | simpler to code.  Instead of juggling around two players, Max and Min,
 | NegaMax treats both players as Max and negates the scores and negates and
 | swaps the values of alpha and beta on each recursive call.
 |
 | On top of NegaMax, this method implements the NegaScout algorithm.  NegaScout
 | assumes the first node is in the principal variation and searches this node
 | with a full alpha-beta window.  NegaScout tests its assumption by searching
 | the rest of the nodes with a minimal (scout) window where alpha and beta are
 | close in value.  If the test fails, NegaScout assumes the node at which it
 | failed is in the principal variation and so on.  NegaScout works best when
 | there is good move ordering and typically yields a 10% performance increase.
 */

	list<move_t> l;
	list<move_t>::iterator it;
	move_t m;
	int type = ALPHA;
	bool whose = b.get_whose();
	bitboard_t hash = b.get_hash();

	/*
	 | Before anything else, do some Research Re: search & Research.  ;-)
	 | (Apologies to Aske Plaat.)  If we've already sufficiently examined
	 | this position, return the best move from our previous search.
	 | Otherwise, if we can, reduce the size of our alpha-beta window.
	 */
	switch (table_ptr->probe(hash, &m, depth, alpha, beta))
	{
		case EXACT :                  return m;
		case BETA  : beta  = m.value; break;
		case ALPHA : alpha = m.value; break;
	}
	if (alpha >= beta)
		return m;

	/*
	 | If this position is terminal (the end of the game), there's no legal
	 | move.  All we have to do is determine if the game is drawn or lost.
	 | Check for this case.  Subtle!  We couldn't have just won because our
	 | opponent moved last.
	 */
	switch (b.get_status(false))
	{
		case IN_PROGRESS:
			break;
		default:
			m.value =  CONTEMPT;
			m.promo = m.new_y = m.new_x = m.old_y = m.old_x = 0;
			return m;
		case CHECKMATE:
		case ILLEGAL:
			m.value = -WEIGHT_KING;
			m.promo = m.new_y = m.new_x = m.old_y = m.old_x = 0;
			return m;
	}

	/*
	 | Generate and re-order the move list.  The board class already
	 | generates the list in an order: pawn captures and promotions, knight
	 | captures, bishop captures, rook captures, queen captures, king
	 | captures, king moves, queen moves, rook moves, bishop moves, knight
	 | moves, and pawn moves.  Here, we assign preliminary scores to the
	 | moves, sort by those scores, and hope the STL's sort algorithm is
	 | stable so as not to disturb the board class' move ordering.  ;-)
	 */
	nodes++;
	b.generate(l);
	for (it = l.begin(); it != l.end(); it++)
		if (it->old_x == m.old_x && it->old_y == m.old_y &&
		    it->new_x == m.new_x && it->new_y == m.new_y &&
		    it->promo == m.promo)
			/*
			 | According to the transposition table, a previous
			 | search from this position determined this move to be
			 | best.  In this search, this move could be good too.
			 | Give it a high score to force it to the front of the
			 | list to score it first to (hopefully) cause an
			 | earlier cutoff.
			 */
			it->value = WEIGHT_KING;
		else
			it->value = history_ptr->probe(whose, *it);
	l.sort(compare);

	/* Score each move in the list. */
	for (it = l.begin(); !timeout && it != l.end(); it++)
	{
		b.make(*it);
		if (depth <= 0)
			/* Base case. */
			it->value = b.evaluate();
		else
		{
			/* Recursive case: minimal (scout) window. */
			if (type == EXACT)
				it->value = -negascout(depth - 1, -alpha - WEIGHT_PAWN, -alpha).value;
			/* Recursive case: full alpha-beta window. */
			if (type != EXACT || alpha < it->value && it->value < beta)
				it->value = -negascout(depth - 1, -beta, -alpha).value;
		}
		b.unmake();

		if (it->value >= beta)
		{
			it->value = beta;
			table_ptr->store(hash, *it, depth, BETA);
			return *it;
		}
		if (it->value > alpha)
		{
			alpha = it->value;
			type = EXACT;
		}
		if (it == l.begin() || it->value > m.value)
			m = *it;
	}

	if (!timeout)
	{
		table_ptr->store(hash, m, depth, type);
		history_ptr->store(whose, m, depth);
	}
	return m;
}

/*----------------------------------------------------------------------------*\
 |				   extract()				      |
\*----------------------------------------------------------------------------*/
void search::extract(int s)
{

/* Extract the principal variation and hint from the transposition table. */

	move_t m;
	pv.clear();

	/* Get the principal variation. */
	while (table_ptr->probe(b.get_hash(), &m, 0))
	{
		pv.push_back(m);
		b.make(m);
		if (pv.size() == (unsigned) max_depth)
			break;
	}
	for (size_t j = 0; j < pv.size(); j++)
		b.unmake();

	/* Get the hint. */
	if (s == THINKING && pv.size() >= 2)
	{
		list<move_t>::iterator it = pv.begin();
		hint = *++it;
	}
	if (s == PONDERING && pv.size() >= 1)
		hint = pv.front();
}

/*----------------------------------------------------------------------------*\
 |				   compare()				      |
\*----------------------------------------------------------------------------*/
bool search::compare(move_t m1, move_t m2)
{

/* Pass this method to l.sort() to sort the move list in descending order by
 * score. */

	return m1.value > m2.value;
}