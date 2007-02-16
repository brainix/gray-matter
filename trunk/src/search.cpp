/*----------------------------------------------------------------------------*\
 |	search.cpp - move search implementation				      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
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
pthread_t thread;      /* */
pthread_mutex_t mutex; /* */
pthread_cond_t cond;   /* */
int status;            /* */
bool timeout;          /* */

/*----------------------------------------------------------------------------*\
 |				    search()				      |
\*----------------------------------------------------------------------------*/
search::search()
{

/* Constructor. */

	max_time = INT_MAX;
	max_depth = DEPTH;
	output = false;

	signal(SIGALRM, handle);
	status = IDLING;
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&thread, NULL, start, NULL);
}

/*----------------------------------------------------------------------------*\
 |				   ~search()				      |
\*----------------------------------------------------------------------------*/
search::~search()
{

/* Destructor. */

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
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
 |				   get_hint()				      |
\*----------------------------------------------------------------------------*/
move_t search::get_hint() const
{
	return hint;
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
	while (true)
	{
		status = IDLING;

		pthread_mutex_lock(&mutex);
		while (status == IDLING)
			pthread_cond_wait(&cond, &mutex);
		pthread_mutex_unlock(&mutex);

		switch (status)
		{
			case THINKING:
			case PONDERING: iterate(status);    break;
			case QUITTING:  pthread_exit(NULL); break;
		}
	}
}

/*----------------------------------------------------------------------------*\
 |				    change()				      |
\*----------------------------------------------------------------------------*/
void search::change(int s)
{

/* Change the search status (to idling, thinking, pondering, or quitting). */

	pthread_mutex_lock(&mutex);
	if (status != s && !(timeout = (status = s) == IDLING))
		pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				   iterate()				      |
\*----------------------------------------------------------------------------*/
void search::iterate(int s)
{

/* Perform iterative deepening.  This method handles both thinking (on our own
 * time) and pondering (on our opponent's time) since they're so similar. */

	/* Note the start time. */
	clock_t start = clock();

	if (s == THINKING)
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
		assert(pv.size() >= 2);
		pv.pop_front();
		board_ptr->make(pv.front());
		pv.pop_front();
	}

	/* Perform iterative deepening until the alarm has sounded (if we're
	 * thinking), our opponent has moved (if we're pondering), or we've
	 * reached the maximum depth (either way). */
	for (int depth = s == THINKING ? 0 : pv.size(); depth <= max_depth; depth++)
	{
		negascout(depth, -WEIGHT_KING, WEIGHT_KING);
		if (timeout && depth)
			/* Oops.  The alarm has interrupted this iteration; the
			 * current results are incomplete and unreliable.  Go
			 * with the last iteration's results. */
			break;
		extract(s);
		if (output)
			xboard_ptr->print_output(depth + 1, pv.front().value, (clock() - start) / CLOCKS_PER_SEC, nodes, pv);
		if (pv.front().value == WEIGHT_KING || pv.front().value == -WEIGHT_KING)
			/* Oops.  The game will be over at this depth.  There's
			 * no point in searching deeper. */
			break;
	}

	/* Return the best move. */
	if (s == PONDERING)
		board_ptr->unmake();
	xboard_ptr->print_result(pv.front());
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
 | the rest of the nodes with a null window where alpha and beta are equal.  If
 | the test fails, NegaScout assumes the node at which it failed is in the
 | principal variation and so on.  NegaScout works best when there is good move
 | re-ordering and typically yields a 10% performance increase.
 */

	list<move_t> l;
	list<move_t>::iterator it;
	move_t m;
	int type = ALPHA;
	bool whose = board_ptr->get_whose();
	bitboard_t hash = board_ptr->get_hash();

	/* Before anything else, do some Research Re: search & Research.  ;-)
	 * (Apologies to Aske Plaat.)  If we've already sufficiently examined
	 * this position, return the best move from our previous search. */
	if (table_ptr->probe(hash, &m, depth, alpha, beta) != USELESS)
		return m;

	/* If this position is terminal (the end of the game), there's no legal
	 * move.  All we have to do is determine if the game is drawn or lost.
	 * Check for this case.  Subtle!  We couldn't have just won because our
	 * opponent moved last. */
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
		it->value = history_ptr->probe(whose, *it);
	l.sort(compare);

	/* Score each move in the list. */
	for (it = l.begin(); !timeout && it != l.end(); it++)
	{
		board_ptr->make(*it);
		if (!depth)
			/* Base case. */
			it->value = board_ptr->evaluate();
		else
		{
			/* Recursive case - null window. */
			if (type == EXACT)
				it->value = -negascout(depth - 1, -alpha - WEIGHT_PAWN, -alpha).value;
			/* Recursive case - full alpha-beta window. */
			if (type != EXACT || alpha < it->value && it->value < beta)
				it->value = -negascout(depth - 1, -beta, -alpha).value;
		}
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
			table_ptr->store(hash, *it, depth, BETA);
			return *it;
		}
	}

	/* Find the best move in the list.  This loop seems wasteful.  Can
	 * anyone think of a better way? */
	for (m = l.front(), it = l.begin(); it != l.end(); it++)
		if (it->value > m.value)
			m = *it;
	table_ptr->store(hash, m, depth, type);
	history_ptr->store(whose, m, depth);
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

	while (table_ptr->probe(board_ptr->get_hash(), &m, 0))
	{
		pv.push_back(m);
		board_ptr->make(m);
		if (pv.size() == (unsigned) max_depth)
			break;
	}

	for (size_t j = 0; j < pv.size(); j++)
		board_ptr->unmake();

	if (pv.size() >= 2 && s == THINKING)
	{
		list<move_t>::iterator it = pv.begin();
		it++;
		hint = *it;
	}
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
