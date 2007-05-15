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
mutex_t timeout_mutex;  // The lock that protects...
bool timeout_flag;      // ...the flag that determines when to stop thinking or pondering!  :-D

mutex_t search_mutex;   // The lock that protects...
cond_t search_cond;     // ...the condition that controls...
thread_t search_thread; // ...the search thread via...
int search_status;      // ...the search status!  :-D

/*----------------------------------------------------------------------------*\
 |				    search()				      |
\*----------------------------------------------------------------------------*/
search::search(table *t, xboard *x)
{

/* Constructor. */

	max_time = INFINITY;
	max_depth = DEPTH;
	output = false;

	table_ptr = t;
	xboard_ptr = x;

	mutex_create(&timeout_mutex);
	timer_function(handle);
	mutex_create(&search_mutex);
	cond_create(&search_cond, NULL);
	thread_create(&search_thread, (entry_t) start, this);

}

/*----------------------------------------------------------------------------*\
 |				   ~search()				      |
\*----------------------------------------------------------------------------*/
search::~search()
{

/* Destructor. */

	cond_destroy(&search_cond);
	mutex_destroy(&search_mutex);
	mutex_destroy(&timeout_mutex);
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
	xboard_ptr = that.xboard_ptr;

	return *this;
}

/*----------------------------------------------------------------------------*\
 |				    handle()				      |
\*----------------------------------------------------------------------------*/
void search::handle()
{

/* The alarm has sounded.  Handle it. */

	mutex_lock(&timeout_mutex);
	timeout_flag = true;
	mutex_unlock(&timeout_mutex);
}

/*----------------------------------------------------------------------------*\
 |				    clear()				      |
\*----------------------------------------------------------------------------*/
void search::clear() const
{
	table_ptr->clear();
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
	return search_thread;
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
	int old_search_status = search_status = IDLING;

	do
	{
		/* Wait for the status to change. */
		mutex_lock(&search_mutex);
		while (old_search_status == search_status)
			cond_wait(&search_cond, &search_mutex);
		old_search_status = search_status;
		mutex_unlock(&search_mutex);

		/* Do the requested work - idle, think, ponder, or quit. */
		if (search_status == THINKING || search_status == PONDERING)
		{
			mutex_lock(&timeout_mutex);
			timeout_flag = false;
			mutex_unlock(&timeout_mutex);
			search_ptr->iterate(search_status);
		}
	} while (search_status != QUITTING);

	thread_destroy(NULL);
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

	mutex_lock(&timeout_mutex);
	timeout_flag = true;
	mutex_unlock(&timeout_mutex);

	if (s == THINKING || s == PONDERING)
	{
		b.lock();
		b = now;
		b.unlock();
	}

	mutex_lock(&search_mutex);
	search_status = s;
	cond_signal(&search_cond);
	mutex_unlock(&search_mutex);
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
	 | Note the start time.  If we're to think, set the alarm.  Initialize
	 | the number of nodes searched.
	 */
	clock_t start = clock();
	if (s == THINKING)
		timer_set(max_time);
	nodes = 0;
	move_t m, tmp_move;
	SET_NULL_MOVE(m);
	m.value = 0;

	/*
	 | Perform iterative deepening until the alarm has sounded (if we're
	 | thinking), our opponent has moved (if we're pondering), or we've
	 | reached the maximum depth (either way).
	 */
	b.lock();
	for (int depth = 1; depth <= max_depth; depth++)
	{
//		tmp_move = mtdf(depth, m.value);
		tmp_move = minimax(depth, -INFINITY, +INFINITY);
		if (timeout_flag && depth || IS_NULL_MOVE(tmp_move))
			/*
			 | Oops.  Either the alarm has interrupted this
			 | iteration (and the results are incomplete and
			 | unreliable), or there's no legal move in this
			 | position (and the game must've ended).
			 */
			break;
		m = tmp_move;
		extract(s);
		if (output)
			xboard_ptr->print_output(depth, m.value, (clock() - start) / CLOCKS_PER_SEC, nodes, pv);
		if (m.value == WEIGHT_KING || m.value == -WEIGHT_KING)
			/*
			 | Oops.  The game will be over at this depth.  There's
			 | no point in searching deeper.  Eyes on the prize.
			 */
			break;
	}
	b.unlock();

	/*
	 | If we've just finished thinking, clear the alarm and inform XBoard of
	 | our favorite move.
	 */
	if (s == THINKING)
		timer_cancel();
	if (s == THINKING && search_status != QUITTING)
		xboard_ptr->print_result(m);
}

/*----------------------------------------------------------------------------*\
 |				     mtdf()				      |
\*----------------------------------------------------------------------------*/
move_t search::mtdf(int depth, int guess)
{

/*
 | From the current position, search for the best move.  This method implements
 | the MTD(f) algorithm.
 */

	move_t m;
	SET_NULL_MOVE(m);
	m.value = guess;
	int upper = +INFINITY, lower = -INFINITY, beta;

	do
	{
		if (m.value == lower)
			beta = m.value + WEIGHT_INCREMENT;
		else
			beta = m.value;
		m = minimax(depth, beta - WEIGHT_INCREMENT, beta);
		if (m.value < beta)
			upper = m.value;
		else
			lower = m.value;
	} while (!timeout_flag && lower < upper);
	return m;
}

/*----------------------------------------------------------------------------*\
 |				   minimax()				      |
\*----------------------------------------------------------------------------*/
move_t search::minimax(int depth, int alpha, int beta)
{

/*
 | From the current position, search for the best move.  This method implements
 | the NegaMax algorithm.  NegaMax produces the same results as MiniMax but is
 | simpler to code.  Instead of juggling around two players, Max and Min,
 | NegaMax treats both players as Max and negates the scores on each recursive
 | call.
 |
 | On top of NegaMax, this method implements alpha-beta pruning.
 */

	/* Local variables: */
	int status = b.get_status(false); // In this position, whether or not the game is over.
	move_t m;                         // From this position, the best move.
	bitboard_t hash = b.get_hash();   // This position's hash.
	int upper = +INFINITY;            // For this position, the upper bound on the MiniMax value.
	int lower = -INFINITY;            // For this position, the lower bound on the MiniMax value.
	int tmp_alpha;                    // Scratch variable for us to use so as to not clobber alpha.
	list<move_t> l;                   // From this position, the move list.
	list<move_t>::iterator it;        // The iterator through the move list.

	/* Increment the number of positions searched. */
	nodes++;

	/*
	 | If this position is terminal (the end of the game), there's no legal
	 | move.  All we have to do is determine if the game is drawn or lost.
	 | Check for this case.  Subtle!  We couldn't have just won because our
	 | opponent moved last.
	 */
	if (depth <= 0 || status != IN_PROGRESS)
	{
		/*
		 | If this position is a draw:
		 |     We want to discourage players from forcing a premature
		 |     draw.  That's why we score this position as +WEIGHT_CONTEMPT.
		 |     Therefore, when the NegaMax recursion unrolls, we score
		 |     the move that leads to this position as -WEIGHT_CONTEMPT.
		 |
		 | If this position is a mate:
		 |     We want to encourage players to mate.  ;-)  That's why
		 |     we score this position as -WEIGHT_KING.  Therefore, when
		 |     the NegaMax recursion unrolls, we score the move that
		 |     leads to this position as +WEIGHT_KING.
		 */
		SET_NULL_MOVE(m);
		switch (status)
		{
			case IN_PROGRESS  : m.value = -b.evaluate();    break;
			case STALEMATE    : m.value = +WEIGHT_CONTEMPT; break;
			case INSUFFICIENT : m.value = +WEIGHT_CONTEMPT; break;
			case THREE        : m.value = +WEIGHT_CONTEMPT; break;
			case FIFTY        : m.value = +WEIGHT_CONTEMPT; break;
			case CHECKMATE    : m.value = -WEIGHT_KING;     break;
			case ILLEGAL      : m.value = -WEIGHT_KING;     break;
		}
		return m;
	}

	/*
	 | Before anything else, do some Research Re: search & Research.  ;-)
	 | (Apologies to Aske Plaat.)  If we've already sufficiently examined
	 | this position, return the best move from our previous search.
	 | Otherwise, if we can, reduce the size of our alpha-beta window.
	 */
	if (table_ptr->probe(hash, depth, &m, UPPER))
		if ((upper = m.value) <= alpha)
			return m;
	if (table_ptr->probe(hash, depth, &m, EXACT))
		return m;
	if (table_ptr->probe(hash, depth, &m, LOWER))
		if ((lower = m.value) >= beta)
			return m;
	tmp_alpha = alpha = GREATER(alpha, lower);
	beta = LESSER(beta, upper);

	/* Generate and re-order the move list. */
	b.generate(l);
	if ((it = find(l.begin(), l.end(), m)) != l.end())
	{
		/*
		 | According to the transposition table, a previous search from
		 | this position determined this move to be best.  In this
		 | search, this move could be good too.  Place this move at the
		 | front of the list to score it first to hopefully cause an
		 | earlier cutoff.
		 */
		l.erase(it);
		l.push_front(m);
	}

	/* Score each move in the list. */
	for (it = l.begin(); !timeout_flag && it != l.end(); it++)
	{
		b.make(*it);
		it->value = -minimax(depth - 1, -beta, -tmp_alpha).value;
		b.unmake();

		/* Perform alpha-beta pruning. */
		if ((tmp_alpha = GREATER(tmp_alpha, it->value)) >= beta)
		{
			(m = *it).value = tmp_alpha;
			break;
		}

		/* Keep track of the best move so far. */
		if (it == l.begin() || it->value > m.value)
			m = *it;
	}

	if (!timeout_flag)
	{
		if (m.value <= alpha)
			table_ptr->store(hash, depth, m, UPPER);
		else if (m.value > alpha && m.value < beta)
			table_ptr->store(hash, depth, m, EXACT);
		else if (m.value >= beta)
			table_ptr->store(hash, depth, m, LOWER);
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
	while (table_ptr->probe(b.get_hash(), 0, &m, UPPER) && b.get_status(true) == IN_PROGRESS)
	{
		if (IS_NULL_MOVE(m))
			break;
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
	else if (s == PONDERING && pv.size() >= 1)
		hint = pv.front();
	else
		SET_NULL_MOVE(hint);
}
