/*----------------------------------------------------------------------------*\
 |	search_mtdf.cpp - MTD(f) move search implementation		      |
 |									      |
 |	Copyright © 2005-2008, The Gray Matter Team, original authors.	      |
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

/// Constructor.

}

/*----------------------------------------------------------------------------*\
 |				 ~search_mtdf()				      |
\*----------------------------------------------------------------------------*/
search_mtdf::~search_mtdf()
{

/// Destructor.

}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
search_mtdf& search_mtdf::operator=(const search_mtdf& that)
{

/// Overloaded assignment operator.

	if (this != &that)
		search_base::operator=(that);
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				   iterate()				      |
\*----------------------------------------------------------------------------*/
void search_mtdf::iterate(int s)
{

/// Perform iterative deepening.  This method handles analyzing (thinking
/// indefinitely), thinking (on our own time), and pondering (on our opponent's
/// time) since they're so similar.

	move_t guess[2], m;
	bool strong_pondering = false;

	// Wait for the board, then grab the board.
	board_ptr->lock();

	// If we're to think:  For the current position, does the opening book
	// recommend a move?  Or, in the current position, is there only one
	// legal move?
	if (s == THINKING)
	{
		list<move_t> l;
		board_ptr->generate(l, true);
		if (table_ptr->probe(board_ptr->get_hash(), MAX_DEPTH, BOOK, &m) || l.size() == 1)
		{
			// Yes.  Make the move.
			if (l.size() == 1)
				m = l.front();
			extract_pv();
			extract_hint(THINKING);
			board_ptr->unlock();
			xboard_ptr->print_result(m);
			return;
		}
	}

	// Note the start time.  If we're to think, set the alarm.  (If we're to
	// analyze or ponder, there's no need to set the alarm.  We analyze or
	// ponder indefinitely until our opponent has moved.)
	clock_ptr->note_time();
	if (s == THINKING)
		clock_ptr->set_alarm(board_ptr->get_whose());

	// If we're to ponder, pretend our opponent has made the move we think
	// she'll make, then think about our best response.
	if (s == PONDERING && !IS_NULL_MOVE(hint))
	{
		strong_pondering = true;
		board_ptr->make(hint);
	}

	// Initialize the number of nodes searched.
	nodes = 0;
	for (int depth = 0; depth <= 1; depth++)
	{
		SET_NULL_MOVE(guess[depth]);
		guess[depth].value = 0;
	}

	// Perform iterative deepening until the alarm has sounded (if we're
	// thinking), our opponent has moved (if we're analyzing or pondering),
	// or we've reached the maximum depth (in any case).
	for (int depth = 1; depth <= max_depth; depth++)
	{
		DEBUG_SEARCH_INIT(0, "");
//		guess[depth & 1] = mtdf(depth, guess[depth & 1].value);
		guess[depth & 1] = minimax(depth);
		if (timeout_flag || IS_NULL_MOVE(guess[depth & 1]))
			// Oops.  Either the alarm has interrupted this
			// iteration (and the results are incomplete and
			// unreliable), or there's no legal move in this
			// position (and the game must've ended).
			break;
		m = guess[depth & 1];
		extract_pv();
		if (output)
		{
			if (strong_pondering)
				pv.push_front(hint);
			xboard_ptr->print_output(depth, 
				board_ptr->get_whose() ? -m.value : m.value, 
				clock_ptr->get_elapsed(), nodes, pv);
			if (strong_pondering)
				pv.pop_front();
		}
		if (ABS(m.value) >= VALUE_KING - MAX_DEPTH)
			// Oops.  The game will be over at this depth.  There's
			// no point in searching deeper.  Eyes on the prize.
			break;
	}

	// If we've just finished thinking, cancel the alarm.
	if (s == THINKING)
	{
		clock_ptr->cancel_alarm();
		extract_hint(THINKING);
	}

	// If we've just finished pondering, take back the move we thought our
	// opponent would've made.
	if (strong_pondering)
		board_ptr->unmake();

	// Release the board.
	board_ptr->unlock();

	// If we've just finished thinking, inform XBoard of our favorite move.
	if (s == THINKING && search_status != QUITTING)
		xboard_ptr->print_result(m);
}

/*----------------------------------------------------------------------------*\
 |				     mtdf()				      |
\*----------------------------------------------------------------------------*/
move_t search_mtdf::mtdf(int depth, value_t guess)
{

/// From the current position, search for the best move.  This method implements
/// the MTD(f) algorithm.

	move_t m;
	SET_NULL_MOVE(m);
	m.value = guess;
	value_t upper = +INFINITY, lower = -INFINITY, beta;

	while (upper > lower && !timeout_flag)
	{
		beta = m.value + (m.value == lower);
		m = minimax(depth, 0, beta - 1, beta, true);
		upper = m.value < beta ? m.value : upper;
		lower = m.value < beta ? lower : m.value;
	}
	return m;
}

/*----------------------------------------------------------------------------*\
 |				   minimax()				      |
\*----------------------------------------------------------------------------*/
move_t search_mtdf::minimax(int depth, int shallowness, value_t alpha, value_t beta, bool try_null_move)
{

/// From the current position, search for the best move.  This method implements
/// the MiniMax algorithm.
///
/// On top of MiniMax, this method implements NegaMax.  NegaMax produces the
/// same results as MiniMax but is simpler to code.  Instead of juggling around
/// two players, Max and Min, NegaMax treats both players as Max and negates the
/// scores (and negates and swaps the lower and upper bounds - more on that in
/// the next paragraph) on each recursive call.  In other words, NegaMax always
/// views the color on move as Max and the color off move as Min.
///
/// On top of NegaMax, this method implements AlphaBeta.  AlphaBeta produces the
/// same results as NegaMax but far more efficiently.
//
/// On top of AlphaBeta, this method implements FailSoft.  FailSoft returns more
/// information than AlphaBeta.  If the exact score falls outside of the window,
/// AlphaBeta returns either alpha (to represent that the exact score is lower
/// than the window) or beta (to represent that the exact score is higher than
/// the window).  On the other hand, FailSoft returns either an upper bound (<=
/// alpha) or a lower bound (>= beta) on the exact score.
///
/// This method also implements null move pruning.

	// Local variables that pertain to the current position:
	bool whose = board_ptr->get_whose();     // The color on move.
	bitboard_t hash = board_ptr->get_hash(); // This position's hash.
	int status = board_ptr->get_status(0);   // Whether the game is over.
	value_t saved_alpha = alpha;             // Saved lower bound on score.
	value_t saved_beta = beta;               // Saved upper bound on score.
//	move_t null_move;                        // The all-important null move.
	list<move_t> l;                          // The move list.
	list<move_t>::iterator it;               // The move list's iterator.
	move_t m;                                // The best move and score.

	// Increment the number of positions searched.
	nodes++;

	// If this position is terminal (the end of the game), there's no legal
	// move - all we have to do is determine if the game is drawn or lost.
	// (Subtle!  We couldn't have just won because our opponent moved last.)
	if (status != IN_PROGRESS)
	{
		SET_NULL_MOVE(m);
		switch (status)
		{
//			case IN_PROGRESS  : m.value = -quiesce(shallowness, alpha, beta); break;
			case IN_PROGRESS  : m.value = -board_ptr->evaluate();             break;
			case STALEMATE    : m.value = +VALUE_CONTEMPT;                    break;
			case INSUFFICIENT : m.value = +VALUE_CONTEMPT;                    break;
			case THREE        : m.value = +VALUE_CONTEMPT;                    break;
			case FIFTY        : m.value = +VALUE_CONTEMPT;                    break;
			case CHECKMATE    : m.value = -VALUE_KING;                        break;
			case ILLEGAL      : m.value = -VALUE_ILLEGAL;                     break;
//			default           : m.value = -quiesce(shallowness, alpha, beta); break;
			default           : m.value = -board_ptr->evaluate();             break;
		}
		DEBUG_SEARCH_PRINT("terminal state %d.", status);
		return m;
	}

	// If we've already sufficiently examined this position, return the best
	// move from our previous search.  Otherwise, if we can, reduce the size
	// of our AlphaBeta window.
	if (table_ptr->probe(hash, depth, EXACT, &m))
		return m;
//	if (table_ptr->probe(hash, depth, UPPER, &m))
//	{
//		if (m.value <= alpha)
//			return m;
//		// When doing MTD(f) zero-window searches, our window should
//		// never be resized here.  I've only accounted for this in the
//		// interest of robustness.
//		beta = LESSER(beta, m.value);
//	}
//	if (table_ptr->probe(hash, depth, LOWER, &m))
//	{
//		if (m.value >= beta)
//			return m;
//		// When doing MTD(f) zero-window searches, our window should
//		// never be resized here.  I've only accounted for this in the
//		// interest of robustness.
//		alpha = GREATER(alpha, m.value);
//	}

	// If we've reached the maximum search depth, this node is a leaf - all
	// we have to do is apply the static evaluator.
	if (depth <= 0)
	{
		SET_NULL_MOVE(m);
//		m.value = -quiesce(shallowness, alpha, beta);
		m.value = -board_ptr->evaluate();
		table_ptr->store(hash, 0, EXACT, m);
		DEBUG_SEARCH_PRINT("evaluate() says %d.", board_ptr->get_whose() ? -m.value : m.value);
		return m;
	}

	// Perform null move pruning.
//	if (try_null_move && !board_ptr->zugzwang())
//	{
//		SET_NULL_MOVE(null_move);
//		DEBUG_SEARCH_ADD_MOVE(null_move);
//		board_ptr->make(null_move);
//		null_move = minimax(depth - R - 1, shallowness + R + 1, -beta, -beta + 1, false);
//		DEBUG_SEARCH_DEL_MOVE(null_move);
//		board_ptr->unmake();
//		if (-null_move.value >= beta)
//		{
//			null_move.value = beta;
//			return null_move;
//		}
//	}

	// Generate the move list.
	if (!board_ptr->generate(l, !shallowness))
	{
		// There's a move in the list that captures the opponent's king,
		// which means we're in an illegal position.
		SET_NULL_MOVE(m);
		m.value = VALUE_ILLEGAL;
		DEBUG_SEARCH_PRINT("Opponent's king can be captured - illegal position.");
		return m;
	}

	// Re-order the move list.
	for (it = l.begin(); it != l.end(); it++)
		// If according to the transposition table, a previous search
		// from this position determined this move to be best, then in
		// this search, this move could be good too - score this move
		// highly to force it to the front of the list to score it first
		// to hopefully cause an earlier cutoff.  Otherwise, score this
		// move according to the history heuristic.
		it->value = *it == m ? VALUE_KING : history_ptr->probe(whose, *it);
	l.sort(descend);

	// Score each move in the list.
	SET_NULL_MOVE(m);
	m.value = -VALUE_ILLEGAL;
	for (it = l.begin(); it != l.end(); it++)
	{
		DEBUG_SEARCH_ADD_MOVE(*it);
		board_ptr->make(*it);
		it->value = -minimax(depth - 1, shallowness + 1, -beta, -alpha, true).value;
		DEBUG_SEARCH_DEL_MOVE(*it);
		board_ptr->unmake();
		if (ABS(it->value) == VALUE_ILLEGAL)
			continue;
		if (it->value > m.value)
			alpha = GREATER(alpha, (m = *it).value);
		if (it->value >= beta || timeout_flag)
			break;
	}

	// Was there a legal move in the list?
	if (m.value == -VALUE_ILLEGAL)
	{
		// Nope, there was no legal move in the list.  There are three
		// possibilities: the current position is illegal, a draw, or a
		// checkmate.  How can we tell which?  If our opponent is in
		// check, the position is illegal.  If we're not in check, we're
		// drawn.  If we're in check, we're checkmated.
		SET_NULL_MOVE(m);
		if (board_ptr->check(true))
			// Our opponent is in check; the position is illegal;
			// we've already won.
			m.value = VALUE_ILLEGAL;
		else if (!board_ptr->check())
			// We're not in check; the position is a draw.
			m.value = VALUE_CONTEMPT;
		else
			// We're in check; the position is a checkmate; we've
			// lost.
			m.value = -(VALUE_KING - shallowness);
		if (!timeout_flag)
			table_ptr->store(hash, depth, EXACT, m);
		DEBUG_SEARCH_PRINT("%s.", m.value == VALUE_ILLEGAL ? "Illegal position" : m.value == VALUE_CONTEMPT ? "Stalemated" : "Checkmated");
		return m;
	}

	// Was the search interrupted?
	if (!timeout_flag)
	{
		// Nope, the results are complete and reliable.  Save them for
		// progeny.
		if (m.value > saved_alpha && m.value < saved_beta)
			// When doing MTD(f) zero-window searches, our move
			// search should never return an exact score.  I've only
			// accounted for this in the interest of robustness.
			table_ptr->store(hash, depth, EXACT, m);
		else if (m.value <= saved_alpha)
			table_ptr->store(hash, depth, UPPER, m);
		else // m.value >= saved_beta
			table_ptr->store(hash, depth, LOWER, m);
		history_ptr->store(whose, m, depth);
	}
	DEBUG_SEARCH_PRINTM(m, "max of %d children: %d.", l.size(), m.value);
	return m;
}

/*----------------------------------------------------------------------------*\
 |				   quiesce()				      |
\*----------------------------------------------------------------------------*/
value_t search_mtdf::quiesce(int shallowness, value_t alpha, value_t beta)
{
	// Local variables that pertain to the current position:
	value_t value_stand_pat;       //
	list<move_t> l;                // The move list.
	list<move_t>::iterator it;     // The move list's iterator.

	// Increment the number of positions searched.
	nodes++;

	if (shallowness >= MAX_DEPTH)
		return beta;

	//
	value_stand_pat = board_ptr->evaluate(shallowness);
	if (value_stand_pat > alpha)
		alpha = value_stand_pat;
	if (value_stand_pat >= beta)
		return value_stand_pat;

	// Generate the move list.
	board_ptr->generate(l, false, true);

	// Score each move in the list.
	for (it = l.begin(); it != l.end(); it++)
	{
		board_ptr->make(*it);
		it->value = -quiesce(shallowness + 1, -beta, -alpha);
		board_ptr->unmake();
		if (it->value > alpha)
			alpha = it->value;
		if (it->value >= beta)
			return it->value;
		if (timeout_flag)
			return beta;
	}

	return alpha;
}
