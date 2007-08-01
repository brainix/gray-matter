/*----------------------------------------------------------------------------*\
 |	search_mtdf.cpp - move search implementation			      |
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

#include "gray.h"
#include "search_mtdf.h"

/* Global variables: */

/*----------------------------------------------------------------------------*\
 |				 search_mtdf()				      |
\*----------------------------------------------------------------------------*/
search_mtdf::search_mtdf(table *t, history *h, chess_clock *c, xboard *x) :
	search_base(t, h, c, x)
{
}

/*----------------------------------------------------------------------------*\
 |				 ~search_mtdf()				      |
\*----------------------------------------------------------------------------*/
search_mtdf::~search_mtdf()
{
}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
class search_mtdf& search_mtdf::operator=(const search_mtdf& that)
{

/* Overloaded assignment operator. */

	if (this == &that)
		return *this;
	search_base::operator=(that);
	pv = that.pv;
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				   iterate()				      |
\*----------------------------------------------------------------------------*/
void search_mtdf::iterate(int s)
{

/*
 | Perform iterative deepening.  This method handles both thinking (on our own
 | time) and pondering (on our opponent's time) since they're so similar.
 */

	int depth;
	move_t guess[2], m;

	/* Wait for the board, then grab the board. */
	b.lock();

	/*
	 | Note the start time.  If we're to think, set the alarm.  Initialize
	 | the number of nodes searched.
	 */
	clock_ptr->note_time();
	if (s == THINKING)
	{
		clock_ptr->set_alarm(b.get_whose());
		history_ptr->clear();
	}
	nodes = 0;
	for (depth = 0; depth <= 1; depth++)
	{
		SET_NULL_MOVE(guess[depth]);
		guess[depth].value = 0;
	}

	/*
	 | Perform iterative deepening until the alarm has sounded (if we're
	 | thinking), our opponent has moved (if we're pondering), or we've
	 | reached the maximum depth (either way).
	 */
	for (depth = 1; depth <= max_depth; depth++)
	{
		guess[depth & 1] = mtdf(depth, guess[depth & 1].value);
		if (timeout_flag && depth_flag || IS_NULL_MOVE(guess[depth & 1]))
			/*
			 | Oops.  Either the alarm has interrupted this
			 | iteration (and the results are incomplete and
			 | unreliable), or there's no legal move in this
			 | position (and the game must've ended).
			 */
			break;
		m = guess[depth & 1];
		extract(s);
		if (output)
			xboard_ptr->print_output(depth, m.value, clock_ptr->get_elapsed(), nodes, pv);
		if (ABS(m.value) >= WEIGHT_KING - MAX_DEPTH)
			/*
			 | Oops.  The game will be over at this depth.  There's
			 | no point in searching deeper.  Eyes on the prize.
			 */
			break;
		if (depth == MIN_DEPTH)
		{
			mutex_lock(&depth_mutex);
			depth_flag = true;
			mutex_unlock(&depth_mutex);
		}
	}

	/* Release the board. */
	b.unlock();

	/*
	 | If we've just finished thinking, cancel the alarm and inform XBoard
	 | of our favorite move.
	 */
	if (s == THINKING)
		clock_ptr->cancel_alarm();
	if (s == THINKING && search_status != QUITTING)
		xboard_ptr->print_result(m);
}

/*----------------------------------------------------------------------------*\
 |				     mtdf()				      |
\*----------------------------------------------------------------------------*/
move_t search_mtdf::mtdf(int depth, int guess)
{

/*
 | From the current position, search for the best move.  This method implements
 | the MTD(f) algorithm.
 */

	move_t m;
	SET_NULL_MOVE(m);
	m.value = guess;
	int upper = +INFINITY, lower = -INFINITY, beta;

	while (upper > lower && (!timeout_flag || !depth_flag))
	{
		beta = m.value + (m.value == lower);
		m = minimax(depth, 0, beta - 1, beta);
		upper = m.value < beta ? m.value : upper;
		lower = m.value < beta ? lower : m.value;
	}
	return m;
}

/*----------------------------------------------------------------------------*\
 |				   minimax()				      |
\*----------------------------------------------------------------------------*/
move_t search_mtdf::minimax(int depth, int shallowness, int alpha, int beta)
{

/*
 | From the current position, search for the best move.  This method implements
 | the MiniMax algorithm.
 |
 | On top of MiniMax, this method implements NegaMax.  NegaMax produces the same
 | results as MiniMax but is simpler to code.  Instead of juggling around two
 | players, Max and Min, NegaMax treats both players as Max and negates the
 | scores on each recursive call.
 |
 | On top of NegaMax, this method implements AlphaBeta.  AlphaBeta produces the
 | same results as NegaMax but far more efficiently.
 |
 | On top of AlphaBeta, this method implements FailSoft.  FailSoft returns more
 | information than AlphaBeta.
 */

	/* Local variables: */
	bool whose = b.get_whose();       // In this position, the color on move.
	bitboard_t hash = b.get_hash();   // This position's hash.
	int status = b.get_status(false); // In this position, whether or not the game is over.
	int upper = +INFINITY;            // For this position, the upper bound on the MiniMax score.
	int lower = -INFINITY;            // For this position, the lower bound on the MiniMax score.
	int current = alpha;              // Scratch variable for us to use so as to not clobber alpha.
	list<move_t> l;                   // From this position, the move list.
	list<move_t>::iterator it;        // The iterator through the move list.
	move_t m;                         // From this position, the best move.

	/* Increment the number of positions searched. */
	nodes++;

	/*
	 | If we've reached the maximum search depth, this node is a leaf - all
	 | we have to do is apply the static evaluator.  If this position is
	 | terminal (the end of the game), there's no legal move - all we have
	 | to do is determine if the game is drawn or lost.  (Subtle!  We
	 | couldn't have just won because our opponent moved last.)  Check for
	 | these cases.
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
			case STALEMATE    : m.value = +WEIGHT_CONTEMPT; break; // XXX: This should never happen.
			case INSUFFICIENT : m.value = +WEIGHT_CONTEMPT; break;
			case THREE        : m.value = +WEIGHT_CONTEMPT; break;
			case FIFTY        : m.value = +WEIGHT_CONTEMPT; break;
			case CHECKMATE    : m.value = -WEIGHT_KING;     break; // XXX: This should never happen.
			case ILLEGAL      : m.value = -WEIGHT_ILLEGAL;  break; // XXX: This should never happen.
		}
		return m;
	}

	/*
	 | If we've already sufficiently examined this position, return the best
	 | move from our previous search.  Otherwise, if we can, reduce the size
	 | of our alpha-beta window.
	 */
	if (shallowness)
	{
		if (table_ptr->probe(hash, depth, UPPER, &m))
			if ((upper = m.value) <= alpha)
				return m;
		if (table_ptr->probe(hash, depth, LOWER, &m))
			if ((lower = m.value) >= beta)
				return m;
		current = alpha = GREATER(alpha, lower);
		beta = LESSER(beta, upper);
	}

	/* Generate and re-order the move list. */
	b.generate(l, !shallowness);
//	l.sort(shuffle);
	for (it = l.begin(); it != l.end(); it++)
		/*
		 | According to the transposition table, a previous search from
		 | this position determined this move to be best.  In this
		 | search, this move could be good too.  Give this move a high
		 | score to force it to the front of the list to score it first
		 | to hopefully cause an earlier cutoff.
		 */
		it->value = *it == m ? WEIGHT_KING : history_ptr->probe(whose, *it);
	l.sort(descend);

	/* Score each move in the list. */
	for (m.value = -INFINITY, it = l.begin(); it != l.end(); it++)
	{
		b.make(*it);
		it->value = -minimax(depth - 1, shallowness + 1, -beta, -current).value;
		b.unmake();
		if (it->value == -WEIGHT_ILLEGAL)
			continue;
		if (it->value > m.value)
			current = GREATER(current, (m = *it).value);
		if (it->value >= beta || timeout_flag && depth_flag)
			break;
	}

	if (m.value == -INFINITY)
	{
		SET_NULL_MOVE(m);
		m.value = !b.check() ? +WEIGHT_CONTEMPT : -(WEIGHT_KING - shallowness);
		return m;
	}
	if (!timeout_flag || !depth_flag)
	{
		if (m.value <= alpha)
			table_ptr->store(hash, depth, UPPER, m);
		else if (m.value >= beta)
			table_ptr->store(hash, depth, LOWER, m);
		history_ptr->store(whose, m, depth);
	}
	return m;
}

/*----------------------------------------------------------------------------*\
 |				   shuffle()				      |
\*----------------------------------------------------------------------------*/
bool search_mtdf::shuffle(move_t m1, move_t m2)
{

/*
 | Pass this method as the comparison function to l.sort() to randomize the move
 | list.  This is a magnificent hack.
 |
 | Note: This hack wouldn't work for O(n²) list sort algorithms.  But if your
 | STL's list sort algorithm is O(n²), you don't deserve for this hack to work
 | anyway.
 */

	return rand() & 1;
}

/*----------------------------------------------------------------------------*\
 |				   descend()				      |
\*----------------------------------------------------------------------------*/
bool search_mtdf::descend(move_t m1, move_t m2)
{

/*
 | Pass this method as the comparison function to l.sort() to sort the move list
 | from highest to lowest by score.
 */

	return m1.value > m2.value;
}

/*----------------------------------------------------------------------------*\
 |				   extract()				      |
\*----------------------------------------------------------------------------*/
void search_mtdf::extract(int s)
{

/* Extract the principal variation and hint from the transposition table. */

	move_t m;
	pv.clear();

	/* Get the principal variation. */
	for (table_ptr->probe(b.get_hash(), 0, EXACT, &m); !IS_NULL_MOVE(m) && b.get_status(true) == IN_PROGRESS; table_ptr->probe(b.get_hash(), 0, EXACT, &m))
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
	else if (s == PONDERING && pv.size() >= 1)
		hint = pv.front();
	else
		SET_NULL_MOVE(hint);
}
