/*----------------------------------------------------------------------------*\
 |	search.cpp - move search implementation				      |
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

#include <gray.h>

/* Global variables: */
bool timeout;

/*----------------------------------------------------------------------------*\
 |				    search()				      |
\*----------------------------------------------------------------------------*/
search::search()
{

/* Constructor. */

	signal(SIGALRM, handle);
	max_time = INT_MAX;
	max_depth = DEPTH;
	output = false;
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
 |				     bind()				      |
\*----------------------------------------------------------------------------*/
void search::bind(board *b, table *t, history *h, xboard *x)
{
	board_ptr = b;
	table_ptr = t;
	history_ptr = h;
	xboard_ptr = x;
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
 |				   iterate()				      |
\*----------------------------------------------------------------------------*/
move_t search::iterate(bool pondering)
{

/* Perform iterative deepening.  This method handles both thinking (on our own
 * time) and pondering (on our opponent's time) since they're so similar. */

	/* Note the start time. */
	timeout = false;
	clock_t start = clock();

	if (!pondering)
	{
		/* OK, we're thinking (on our own time).  Initialize the number
		 * of nodes searched and set the alarm. */
		nodes = 0;
		struct itimerval itimerval;
		itimerval.it_interval.tv_sec = 0;
		itimerval.it_interval.tv_usec = 0;
		itimerval.it_value.tv_sec = max_time;
		itimerval.it_value.tv_usec = 0;
		setitimer(ITIMER_REAL, &itimerval, NULL);
	}
	else
	{
		/* OK, we're pondering (on our opponent's time).  Currently, the
		 * principal variation starts with the move we just made and the
		 * move we think our opponent will make.  Get rid of the move we
		 * just made and assume our opponent will make the move we think
		 * she'll make. */
		if (pv.size() < 2)
			return;
		pv.pop_front();
		board_ptr->make(pv.front());
		pv.pop_front();
	}

	/* Perform iterative deepening until the alarm has sounded (if we're
	 * thinking), our opponent has moved (if we're pondering), or we've
	 * reached the maximum depth (either way). */
	for (int depth = !pondering ? 0 : pv.size(); depth <= max_depth; depth++)
	{
		negamax(depth, -WEIGHT_KING, WEIGHT_KING);
		if (timeout)
		{
			/* Oops.  The alarm has interrupted this iteration; the
			 * current results are incomplete and unreliable.  Go
			 * with the last iteration's results. */
			assert(depth);
			break;
		}
		extract(pondering);
		if (output)
			xboard_ptr->print_output(depth + 1, pv.front().value, (clock() - start) / CLK_TCK, nodes, pv);
		if (pv.front().value == WEIGHT_KING || pv.front().value == -WEIGHT_KING)
			/* Oops.  The game will be over at this depth.  There's
			 * no point in searching deeper. */
			break;
	}

	/* Return the best move. */
	if (pondering)
		board_ptr->unmake();
	return pv.front();
}

/*----------------------------------------------------------------------------*\
 |				   negamax()				      |
\*----------------------------------------------------------------------------*/
move_t search::negamax(int depth, int alpha, int beta)
{

/* From the current position, search for the best move.  This method implements
 * the clever negamax algorithm.  Negamax produces the same results as minimax
 * but is simpler to code.  Instead of juggling around two players, max and min,
 * negamax treats both players as max and negates and swaps the values of alpha
 * and beta on each recursive call. */

	list<move_t> l;
	list<move_t>::iterator it;
	move_t m;
	int type = ALPHA;

	/* Before anything else, do some research to avoid re-search.  ;-)  If
	 * we've already sufficiently examined this position, return the best
	 * move from our previous search. */
	if (table_ptr->probe(board_ptr->get_hash(), &m, depth, alpha, beta) != USELESS)
		return m;

	/* If this position is terminal (the end of the game), there's no legal
	 * move.  All we have to do is determine if we've won, the game is
	 * drawn, or our opponent has won.  Check for this case. */
	m.promo = m.new_y = m.new_x = m.old_y = m.old_x = 0;
	switch (board_ptr->get_status())
	{
		case IN_PROGRESS :                         break;
		default          : m.value =  CONTEMPT;    return m;
		case CHECKMATE   : m.value = -WEIGHT_KING; return m;
		case ILLEGAL     : m.value = -WEIGHT_KING; return m;
	}

	/* Generate and re-order the move list. */
	nodes++;
	board_ptr->generate(l);
	for (it = l.begin(); it != l.end(); it++)
		it->value = history_ptr->probe(board_ptr->get_whose(), *it);
	l.sort(compare);

	/* Score each move in the list. */
	for (it = l.begin(); !timeout && it != l.end(); it++)
	{
		board_ptr->make(*it);
		it->value = depth ? -negamax(depth - 1, -beta, -alpha).value : board_ptr->evaluate();
		board_ptr->unmake();

		if (it->value > alpha)
		{
			alpha = it->value;
			m = *it;
			type = EXACT;
		}
		if (it->value >= beta)
		{
			it->value = beta;
			table_ptr->store(board_ptr->get_hash(), *it, depth, BETA);
			return *it;
		}
	}

	/* Find the best move in the list. */
	for (m = l.front(), it = l.begin(); it != l.end(); it++)
		if (it->value > m.value)
			m = *it;
	table_ptr->store(board_ptr->get_hash(), m, depth, type);
	history_ptr->store(board_ptr->get_whose(), m, depth);
	return m;
}

/*----------------------------------------------------------------------------*\
 |				   extract()				      |
\*----------------------------------------------------------------------------*/
void search::extract(bool pondering)
{

/* Extract the principal variation and hint from the transposition table. */

	move_t m;
	pv.clear();

	while (table_ptr->probe(board_ptr->get_hash(), &m, 0))
	{
		pv.push_back(m);
		board_ptr->make(m);
	}

	for (size_t j = 0; j < pv.size(); j++)
		board_ptr->unmake();

	if (pv.size() >= 2 && !pondering)
	{
		list<move_t>::iterator it = pv.begin();
		it++;
		hint = *it;
	}
}

/*----------------------------------------------------------------------------*\
 |				   get_hint()				      |
\*----------------------------------------------------------------------------*/
move_t search::get_hint() const
{
	return hint;
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
