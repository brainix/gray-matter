/*----------------------------------------------------------------------------*\
 |	search_mtdf.cpp - move search implementation			      |
 |									      |
 |	Copyright � 2005-2007, The Gray Matter Team, original authors.	      |
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

#include "gray.h"
#include "search_mtdf.h"

/*----------------------------------------------------------------------------*\
 |				 search_mtdf()				      |
\*----------------------------------------------------------------------------*/
search_mtdf::search_mtdf(table *t, history *h, chess_clock *c, xboard *x) :
	search_base(t, h, c, x)
{

// Constructor.

}

/*----------------------------------------------------------------------------*\
 |				 ~search_mtdf()				      |
\*----------------------------------------------------------------------------*/
search_mtdf::~search_mtdf()
{

// Destructor.

}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
search_mtdf& search_mtdf::operator=(const search_mtdf& that)
{

// Overloaded assignment operator.

	if (this != &that)
		search_base::operator=(that);
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				   iterate()				      |
\*----------------------------------------------------------------------------*/
void search_mtdf::iterate(int s)
{

// Perform iterative deepening.  This method handles both thinking (on our own
// time) and pondering (on our opponent's time) since they're so similar.

	bool strong_pondering = s == PONDERING && pv.size() >= 2 && !IS_NULL_MOVE(hint);
	int depth;
	move_t guess[2], m;

	// For the current position, does the opening book recommend a move?
	if (s == THINKING)
		if (table_ptr->probe(board_ptr->get_hash(), MAX_DEPTH, BOOK, &m))
		{
			// Yes.  Just make the prescribed move.
			xboard_ptr->print_result(m);
			return;
		}

	// Wait for the board, then grab the board.
	board_ptr->lock();

	// Note the start time.  If we're to think, set the alarm.  (If we're to
	// ponder, there's no need to set the alarm.  We ponder indefinitely
	// until the opponent has moved.)
	clock_ptr->note_time();
	if (s == THINKING)
		clock_ptr->set_alarm(board_ptr->get_whose());

	//
	if (strong_pondering)
		board_ptr->make(hint);

	// Initialize the number of nodes searched.
	nodes = 0;
	for (depth = 0; depth <= 1; depth++)
	{
		SET_NULL_MOVE(guess[depth]);
		guess[depth].value = 0;
	}

	// Perform iterative deepening until the alarm has sounded (if we're
	// thinking), our opponent has moved (if we're pondering), or we've
	// reached the maximum depth (either way).
	for (depth = 1; depth <= max_depth; depth++)
	{
		guess[depth & 1] = mtdf(depth, guess[depth & 1].value);
		if (timeout_flag && depth_flag || IS_NULL_MOVE(guess[depth & 1]))
			// Oops.  Either the alarm has interrupted this
			// iteration (and the results are incomplete and
			// unreliable), or there's no legal move in this
			// position (and the game must've ended).
			break;
		m = guess[depth & 1];
		extract(s, !strong_pondering);
		if (output)
			xboard_ptr->print_output(depth, m.value, clock_ptr->get_elapsed(), nodes, pv);
		if (ABS(m.value) >= WEIGHT_KING - MAX_DEPTH)
			// Oops.  The game will be over at this depth.  There's
			// no point in searching deeper.  Eyes on the prize.
			break;
		if (depth >= MIN_DEPTH)
		{
			mutex_lock(&depth_mutex);
			depth_flag = true;
			mutex_unlock(&depth_mutex);
		}
	}

	//
	if (strong_pondering)
		board_ptr->unmake();

	// Release the board.
	board_ptr->unlock();

	// If we've just finished thinking, cancel the alarm and inform XBoard
	// of our favorite move.
	if (s == THINKING)
	{
		clock_ptr->cancel_alarm();
		if (search_status != QUITTING)
			xboard_ptr->print_result(m);
	}
}

/*----------------------------------------------------------------------------*\
 |				     mtdf()				      |
\*----------------------------------------------------------------------------*/
move_t search_mtdf::mtdf(int depth, int guess)
{

// From the current position, search for the best move.  This method implements
// the MTD(f) algorithm.

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

// From the current position, search for the best move.  This method implements
// the MiniMax algorithm.
//
// On top of MiniMax, this method implements NegaMax.  NegaMax produces the same
// results as MiniMax but is simpler to code.  Instead of juggling around two
// players, Max and Min, NegaMax treats both players as Max and negates the
// scores on each recursive call.
//
// On top of NegaMax, this method implements AlphaBeta.  AlphaBeta produces the
// same results as NegaMax but far more efficiently.
//
// On top of AlphaBeta, this method implements FailSoft.  FailSoft returns more
// information than AlphaBeta.

	// Local variables that pertain to the current position:
	bool whose = board_ptr->get_whose();       // The color on move.
	bitboard_t hash = board_ptr->get_hash();   // This position's hash.
	int status = board_ptr->get_status(false); // Whether the game is over.
	int upper = +INFINITY, lower = -INFINITY;  // The upper & lower bounds.
	int current = alpha;                       // Scratch alpha variable.
	list<move_t> l;                            // The move list.
	list<move_t>::iterator it;                 // The iterator.
	move_t m;                                  // The best move.

	// Increment the number of positions searched.
	nodes++;

	// If we've reached the maximum search depth, this node is a leaf - all
	// we have to do is apply the static evaluator.  If this position is
	// terminal (the end of the game), there's no legal move - all we have
	// to do is determine if the game is drawn or lost.  (Subtle!  We
	// couldn't have just won because our opponent moved last.)  Check for
	// these cases.
	if (depth <= 0 || status != IN_PROGRESS)
	{
		// If this position is a draw:
		//     We want to discourage players from forcing a premature
		//     draw.  That's why we score this position as +WEIGHT_CONTEMPT.
		//     Therefore, when the NegaMax recursion unrolls, we score
		//     the move that leads to this position as -WEIGHT_CONTEMPT.
		SET_NULL_MOVE(m);
		m.value = status == IN_PROGRESS ? -board_ptr->evaluate() : status >= INSUFFICIENT && status <= FIFTY ? +WEIGHT_CONTEMPT : -WEIGHT_ILLEGAL;
		return m;
	}

	// If we've already sufficiently examined this position, return the best
	// move from our previous search.  Otherwise, if we can, reduce the size
	// of our alpha-beta window.
	if (shallowness)
	{
		if (table_ptr->probe(hash, depth, EXACT, &m))
			return m;
		if (table_ptr->probe(hash, depth, UPPER, &m))
		{
			if ((upper = m.value) <= alpha)
				return m;
			beta = LESSER(beta, upper);
		}
		else if (table_ptr->probe(hash, depth, LOWER, &m))
		{
			if ((lower = m.value) >= beta)
				return m;
			current = alpha = GREATER(alpha, lower);
		}
	}

	// Generate and re-order the move list.
	board_ptr->generate(l, !shallowness);
	for (it = l.begin(); it != l.end(); it++)
		// If according to the transposition table, a previous search
		// from this position determined this move to be best, then in
		// this search, this move could be good too - score this move
		// highly to force it to the front of the list to score it first
		// to hopefully cause an earlier cutoff.  Otherwise, score this
		// move according to the history heuristic.
		it->value = *it == m ? WEIGHT_KING : history_ptr->probe(whose, *it);
	l.sort(descend);

	// Score each move in the list.
	SET_NULL_MOVE(m);
	m.value = -WEIGHT_ILLEGAL;
	for (it = l.begin(); it != l.end(); it++)
	{
		board_ptr->make(*it);
		it->value = -minimax(depth - 1, shallowness + 1, -beta, -current).value;
		board_ptr->unmake();
		if (it->value > m.value)
			current = GREATER(current, (m = *it).value);
		if (it->value >= beta || timeout_flag && depth_flag)
			break;
	}

	// Was there a legal move in the list?
	if (m.value == -WEIGHT_ILLEGAL)
	{
		// Nope, there was no legal move in the list.  The current
		// position must either be stalemate or checkmate.  How can we
		// tell which?  Easily.  If we're not in check, we're
		// stalemated; if we're in check, we're checkmated.
		SET_NULL_MOVE(m);
		m.value = !board_ptr->check() ? +WEIGHT_CONTEMPT : -WEIGHT_KING + shallowness;
		if (!timeout_flag || !depth_flag)
			table_ptr->store(hash, depth, EXACT, m);
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
