/*----------------------------------------------------------------------------*\
 |	xboard.cpp - Chess Engine Communication Protocol implementation	      |
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

#include "gray.h"
#include "xboard.h"

/*----------------------------------------------------------------------------*\
 |				    xboard()				      |
\*----------------------------------------------------------------------------*/
xboard::xboard()
{

/* Constructor. */

	/* Turn I/O buffering off. */
	setbuf(stdout, NULL);
	setbuf(stdin, NULL);

	/* Initialize the variables. */
	ponder = true;
	force = false;
	draw = false;
}

/*----------------------------------------------------------------------------*\
 |				     bind()				      |
\*----------------------------------------------------------------------------*/
void xboard::bind(class search *s)
{
	search_ptr = s;
}

/*----------------------------------------------------------------------------*\
 |				     loop()				      |
\*----------------------------------------------------------------------------*/
void xboard::loop()
{
	for (fgets(s, 80, stdin); strncmp(s, "quit", 4); fgets(s, 80, stdin))
	{
		if (!strncmp(s, "xboard", 6))
			do_xboard();
		else if (!strncmp(s, "protover", 8))
			do_protover();
		else if (!strncmp(s, "accepted", 8))
			do_accepted();
		else if (!strncmp(s, "rejected", 8))
			do_rejected();
		else if (!strncmp(s, "new", 3))
			do_new();
		else if (!strncmp(s, "force", 5))
			do_force();
		else if (!strncmp(s, "go", 2))
			do_go();
		else if (!strncmp(s, "playother", 9))
			do_playother();
		else if (!strncmp(s, "level", 5))
			do_level();
		else if (!strncmp(s, "st", 2))
			do_st();
		else if (!strncmp(s, "sd", 2))
			do_sd();
		else if (!strncmp(s, "usermove", 8))
			do_usermove();
		else if (!strncmp(s, "?", 1))
			do_question();
		else if (!strncmp(s, "ping", 4))
			do_ping();
		else if (!strncmp(s, "draw", 4))
			do_draw();
		else if (!strncmp(s, "hint", 4))
			do_hint();
		else if (!strncmp(s, "undo", 4))
			do_undo();
		else if (!strncmp(s, "remove", 6))
			do_remove();
		else if (!strncmp(s, "hard", 4))
			do_hard();
		else if (!strncmp(s, "easy", 4))
			do_easy();
		else if (!strncmp(s, "post", 4))
			do_post();
		else if (!strncmp(s, "nopost", 6))
			do_nopost();
		else
			printf("Error (unknown command): %s\n", s);
	}
	do_quit();
}

/*----------------------------------------------------------------------------*\
 |				 print_output()				      |
\*----------------------------------------------------------------------------*/
void xboard::print_output(int ply, int value, int time, int nodes, list<move_t> &pv) const
{

/* Print thinking output. */

	printf("%d %d %d %d", ply, value, time, nodes);
	for (list<move_t>::iterator it = pv.begin(); it != pv.end(); it++)
	{
		printf(" ");
		print_move(*it);
	}
	printf("\n");
}

/*----------------------------------------------------------------------------*\
 |				 print_result()				      |
\*----------------------------------------------------------------------------*/
void xboard::print_result(move_t m)
{
	if (m.value == -WEIGHT_KING)
	{
		/* Bloody hell.  At this point, even our best response leads to
		 * a loss; there's no hope.  We might as well take it like
		 * samurais and eviscerate ourselves.  If our opponent has
		 * offered a draw, accept it.  Otherwise, resign. */
		if (draw)
			printf("offer draw\n");
		else
		{
			printf("resign\n");
			return;
		}
	}

	b.make(m);
	search_ptr->change(ponder ? PONDERING : IDLING, b);

	printf("move ");
	print_move(m);
	printf("\n");
	game_over();
}

/*----------------------------------------------------------------------------*\
 |				  print_move()				      |
\*----------------------------------------------------------------------------*/
void xboard::print_move(move_t m) const
{
	printf("%c%c", m.old_x + 'a', m.old_y + '1');
	printf("%c%c", m.new_x + 'a', m.new_y + '1');
	switch (m.promo)
	{
		case KNIGHT : printf("n"); break;
		case BISHOP : printf("b"); break;
		case ROOK   : printf("r"); break;
		case QUEEN  : printf("q"); break;
	}
}

/*----------------------------------------------------------------------------*\
 |				    vomit()				      |
\*----------------------------------------------------------------------------*/
void xboard::vomit(char *message) const
{

/* Houston, we have a problem... */

	printf("telluser %s\n", message);
	exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------------*\
 |				  do_xboard()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_xboard() const
{
}

/*----------------------------------------------------------------------------*\
 |				 do_protover()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_protover() const
{
	printf("feature ping=1\n");
	printf("feature playother=1\n");
	printf("feature usermove=1\n");
	printf("feature time=0\n");
	printf("feature draw=1\n");
	printf("feature sigint=0\n");
	printf("feature analyze=0\n");
	printf("feature myname=\"Gray Matter\"\n");
	printf("feature variants=\"normal\"\n");
	printf("feature colors=0\n");
	printf("feature done=1\n");
}

/*----------------------------------------------------------------------------*\
 |				 do_accepted()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_accepted() const
{
}

/*----------------------------------------------------------------------------*\
 |				 do_rejected()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_rejected() const
{
	exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------------*\
 |				    do_new()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_new()
{
	ponder = true;
	force = false;
	draw = false;
	b.set_board();
	search_ptr->change(IDLING, b);
	search_ptr->clear();
	search_ptr->set_depth(DEPTH);
}

/*----------------------------------------------------------------------------*\
 |				   do_quit()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_quit() const
{
	search_ptr->change(QUITTING, b);
	assert(!pthread_join(search_ptr->get_thread(), NULL));
}

/*----------------------------------------------------------------------------*\
 |				   do_force()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_force()
{
	force = true;
	search_ptr->change(IDLING, b);
}

/*----------------------------------------------------------------------------*\
 |				    do_go()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_go()
{
	force = false;
	search_ptr->change(THINKING, b);
}

/*----------------------------------------------------------------------------*\
 |				 do_playother()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_playother()
{
	force = false;
	search_ptr->change(ponder ? PONDERING : IDLING, b);
}

/*----------------------------------------------------------------------------*\
 |				   do_level()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_level() const
{
	const char *p = &s[6];
	int moves = str_to_num(p);
	while (*p++ != ' ')
		;
	int secs = str_to_secs(p);
	while (*p++ != ' ')
		;
	int inc = str_to_num(p);
	search_ptr->set_time(secs / (moves ? moves : 40) + inc);
}

/*----------------------------------------------------------------------------*\
 |				    do_st()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_st() const
{

/* Set the maximum search time. */

	search_ptr->set_time(str_to_num(&s[3]));
}

/*----------------------------------------------------------------------------*\
 |				    do_sd()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_sd() const
{

/* Set the maximum search depth. */

	search_ptr->set_depth(str_to_num(&s[3]));
}

/*----------------------------------------------------------------------------*\
 |				 do_usermove()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_usermove()
{

/* Our opponent has moved.  If the move was legal, we're not in force mode, and
 * the last move didn't just end the game: formulate a response. */

	move_t m;

	/* Was the move legal? */
	m.old_x = s[ 9] - 'a'; m.old_y = s[10] - '1';
	m.new_x = s[11] - 'a'; m.new_y = s[12] - '1';
	m.promo = char_to_shape(s[13]);
	if (!test_move(m))
	{
		/* No!  The gypsy was trying to pull a fast one on us! */
		printf("Illegal move: ");
		print_move(m);
		printf("\n");
		return;
	}

	b.make(m);

	/* Alright, so the move was legal.  Are we in force mode, or did the
	 * move just end the game? */
	if (force || game_over())
		/* Yes, either we're in force mode, or the move just ended the
		 * game.  Either way, we're not to respond. */
		return;

	/* Alright, so the move was legal, we're not in force mode, and the move
	 * didn't just end the game.  Formulate a response. */
	search_ptr->change(THINKING, b);
}

/*----------------------------------------------------------------------------*\
 |				 do_question()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_question() const
{
	search_ptr->change(IDLING, b);
}

/*----------------------------------------------------------------------------*\
 |				   do_ping()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_ping()
{

/* XBoard has sent a ping request (to make sure we're not on drugs).  Send a
 * pong reply. */

	s[1] = 'o';
	printf("%s\n", s);
}

/*----------------------------------------------------------------------------*\
 |				   do_draw()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_draw()
{
	draw = true;
}

/*----------------------------------------------------------------------------*\
 |				   do_hint()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_hint() const
{

/* Aww, poor baby.  Our opponent needs us to hold her hand.  Give her a hint. */

	printf("Hint: ");
	print_move(search_ptr->get_hint());
	printf("\n");
}

/*----------------------------------------------------------------------------*\
 |				   do_undo()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_undo()
{

/* Take back one ply. */

	b.unmake();
}

/*----------------------------------------------------------------------------*\
 |				  do_remove()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_remove()
{

/* Take back two plies. */

	b.unmake();
	b.unmake();
	search_ptr->change(ponder ? PONDERING : IDLING, b);
}

/*----------------------------------------------------------------------------*\
 |				   do_hard()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_hard()
{

/* Turn on pondering.  No sleep for the wicked. */

	ponder = true;
}

/*----------------------------------------------------------------------------*\
 |				   do_easy()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_easy()
{

/* Turn off pondering.  Grab a piña colada. */

	ponder = false;
}

/*----------------------------------------------------------------------------*\
 |				   do_post()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_post() const
{

/* Turn on thinking output. */

	search_ptr->set_output(true);
}

/*----------------------------------------------------------------------------*\
 |				  do_nopost()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_nopost() const
{

/* Turn off thinking output. */

	search_ptr->set_output(false);
}

/*----------------------------------------------------------------------------*\
 |				  game_over()				      |
\*----------------------------------------------------------------------------*/
int xboard::game_over()
{
	int status = b.get_status(true);

	switch (status)
	{
		case STALEMATE    : printf("1/2-1/2 {Stalemate}\n");                                              break;
		case INSUFFICIENT : printf("1/2-1/2 {Insufficient material}\n");                                  break;
		case THREE        : printf("1/2-1/2 {Threefold repetition}\n");                                   break;
		case FIFTY        : printf("1/2-1/2 {Fifty move rule}\n");                                        break;
		case CHECKMATE    : printf("%s mates}\n", !b.get_whose() == WHITE ? "1-0 {White" : "0-1 {Black"); break;
	}
	return status;
}

/*----------------------------------------------------------------------------*\
 |				  str_to_num()				      |
\*----------------------------------------------------------------------------*/
int xboard::str_to_num(const char *p) const
{
	int n = 0;

	while (*p >= '0' && *p <= '9')
		n = n * 10 + *p++ - '0';
	return n;
}

/*----------------------------------------------------------------------------*\
 |				 str_to_secs()				      |
\*----------------------------------------------------------------------------*/
int xboard::str_to_secs(const char *p) const
{
	int mins = 0, secs = 0;

	mins = str_to_num(p);
	while (*p != ' ' && *p != ':')
		p++;
	secs = *p++ == ' ' ? 0 : str_to_num(p);
	return mins * 60 + secs;
}

/*----------------------------------------------------------------------------*\
 |				char_to_shape()				      |
\*----------------------------------------------------------------------------*/
int xboard::char_to_shape(char c) const
{
	switch (c)
	{
		case 'n' : return KNIGHT;
		case 'b' : return BISHOP;
		case 'r' : return ROOK;
		case 'q' : return QUEEN;
		default  : return PAWN;
	}
}

/*----------------------------------------------------------------------------*\
 |				  test_move()				      |
\*----------------------------------------------------------------------------*/
bool xboard::test_move(move_t m) const
{

/* In the current position, is the specified move legal? */

	list<move_t> l;

	b.generate(l);
	for (list<move_t>::iterator it = l.begin(); it != l.end(); it++)
		if (it->old_x == m.old_x && it->old_y == m.old_y &&
		    it->new_x == m.new_x && it->new_y == m.new_y &&
		    it->promo == m.promo)
			return true;
	return false;
}
