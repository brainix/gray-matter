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

/* Function prototypes: */
bool compare(move_t m1, move_t m2);

/*----------------------------------------------------------------------------*\
 |				    search()				      |
\*----------------------------------------------------------------------------*/
search::search()
{

/* Constructor. */

	max_time = INT_MAX;
	max_depth = DEPTH;
	output = false;
}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
search& search::operator=(const search& that)
{

/* Overloaded assignment operator. */

	if (this == &that)
		return *this;

	pv = that.pv;
	max_time = that.max_time;
	max_depth = that.max_depth;
	nodes = that.nodes;
	output = that.output;
	bind(that.board_ptr, that.table_ptr, that.history_ptr, that.xboard_ptr);
	return *this;
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
move_t search::iterate()
{
	move_t m;
	m.value = m.promo = m.new_y = m.new_x = m.old_y = m.old_x = 0;

	/* Set the alarm. */
	timeout = false;
	signal(SIGALRM, handle);
	alarm(max_time);
	nodes = 0;

	/* Perform iterative deepening until the alarm sounds or we reach the
	 * maximum depth. */
	for (int depth = 0; !timeout && depth <= max_depth; depth++)
	{
		move_t tmp = negamax(depth, -WEIGHT_KING, WEIGHT_KING);
		if (timeout)
			break;
		extract(m = tmp);
		if (output)
			xboard_ptr->print_output(depth + 1, m.value, alarm(alarm(0)) * 100, nodes, pv);
		if (m.value == WEIGHT_KING || m.value == -WEIGHT_KING)
			break;
	}

	/* Clear the alarm. */
	alarm(0);
	return m;
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
 |				   negamax()				      |
\*----------------------------------------------------------------------------*/
move_t search::negamax(int depth, int alpha, int beta)
{

/* Search for the best move. */

	list<move_t> l;
	list<move_t>::iterator it;
	move_t m;
	int type = ALPHA;

	if (table_ptr->probe(board_ptr->get_hash(), &m, depth, alpha, beta) != USELESS)
		return m;

	m.promo = m.new_y = m.new_x = m.old_y = m.old_x = 0;
/*
	switch (board_ptr->get_status())
	{
		case IN_PROGRESS :                         break;
		default          : m.value =  CONTEMPT;    return m;
		case CHECKMATE   : m.value = -WEIGHT_KING; return m;
		case ILLEGAL     : m.value = -WEIGHT_KING; return m;
	}
 */

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
void search::extract(move_t m)
{

/* Extract the principal variation from the transposition table. */

	pv.clear();

	while (table_ptr->probe(board_ptr->get_hash(), &m, 0))
	{
		pv.push_back(m);
		board_ptr->make(m);
	}

	for (int j = 0; j < pv.length(); j++)
		board_ptr->unmake();
}

/*----------------------------------------------------------------------------*\
 |				   compare()				      |
\*----------------------------------------------------------------------------*/
bool compare(move_t m1, move_t m2)
{

/* Pass this function to l.sort() to sort the move list in descending order by
 * score. */

	return m1.value > m2.value;
}
