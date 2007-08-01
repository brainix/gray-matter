/*----------------------------------------------------------------------------*\
 |	xboard.cpp - Chess Engine Communication Protocol implementation	      |
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
#include "xboard.h"

/* Global variables: */

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
	sync = true;
}

/*----------------------------------------------------------------------------*\
 |				   ~xboard()				      |
\*----------------------------------------------------------------------------*/
xboard::~xboard()
{

/* Destructor. */
}

/*----------------------------------------------------------------------------*\
 |				    vomit()				      |
\*----------------------------------------------------------------------------*/
void xboard::vomit(char *message) const
{

/* Houston, we have a problem... */

	printf("tellusererror %s\n", message);
	exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------------*\
 |				     loop()				      |
\*----------------------------------------------------------------------------*/
void xboard::loop(search_base *s, chess_clock *c)
{
	search_ptr = s;
	clock_ptr = c;

	for (fgets(buffer, 80, stdin); strncmp(buffer, "quit", 4); fgets(buffer, 80, stdin))
	{
		if (!strncmp(buffer, "xboard", 6))
			do_xboard();
		else if (!strncmp(buffer, "protover", 8))
			do_protover();
		else if (!strncmp(buffer, "accepted", 8))
			do_accepted();
		else if (!strncmp(buffer, "rejected", 8))
			do_rejected();
		else if (!strncmp(buffer, "new", 3))
			do_new();
		else if (!strncmp(buffer, "force", 5))
			do_force();
		else if (!strncmp(buffer, "go", 2))
			do_go();
		else if (!strncmp(buffer, "playother", 9))
			do_playother();
		else if (!strncmp(buffer, "level", 5))
			do_level();
		else if (!strncmp(buffer, "st", 2))
			do_st();
		else if (!strncmp(buffer, "sd", 2))
			do_sd();
		else if (!strncmp(buffer, "time", 4))
			do_time();
		else if (!strncmp(buffer, "otim", 4))
			do_otim();
		else if (!strncmp(buffer, "usermove", 8))
			do_usermove();
		else if (!strncmp(buffer, "?", 1))
			do_question();
		else if (!strncmp(buffer, "ping", 4))
			do_ping();
		else if (!strncmp(buffer, "draw", 4))
			do_draw();
		else if (!strncmp(buffer, "hint", 4))
			do_hint();
		else if (!strncmp(buffer, "undo", 4))
			do_undo();
		else if (!strncmp(buffer, "remove", 6))
			do_remove();
		else if (!strncmp(buffer, "hard", 4))
			do_hard();
		else if (!strncmp(buffer, "easy", 4))
			do_easy();
		else if (!strncmp(buffer, "post", 4))
			do_post();
		else if (!strncmp(buffer, "nopost", 6))
			do_nopost();
		else
		{
			printf("Error (unknown command): %s\n", buffer);
		}
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
	b.make(m);
	clock_ptr->dec_remaining_moves(!b.get_whose());

	printf("move ");
	print_move(m);
	printf("\n");

	int status = game_over();
	search_ptr->change(ponder && status == IN_PROGRESS ? PONDERING : IDLING, b);
}

/*----------------------------------------------------------------------------*\
 |			      print_resignation()			      |
\*----------------------------------------------------------------------------*/
void xboard::print_resignation() const
{
	printf("%s\n", draw ? "offer draw" : "resign");
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
	printf("feature ping=1\n");                     //
	printf("feature playother=1\n");                //
	printf("feature usermove=1\n");                 //
	printf("feature time=1\n");                     //
	printf("feature draw=1\n");                     //
	printf("feature sigint=0\n");                   //
	printf("feature analyze=0\n");                  //
	printf("feature myname=\"Gray Matter 0.0\"\n"); //
	printf("feature variants=\"normal\"\n");        //
	printf("feature colors=0\n");                   //
	printf("feature done=1\n");                     //
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
	sync = true;
	b.set_board();
	search_ptr->change(IDLING, b);
	search_ptr->set_depth(MAX_DEPTH);
}

/*----------------------------------------------------------------------------*\
 |				   do_quit()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_quit() const
{
	thread_t thread = search_ptr->get_thread(); 
	search_ptr->change(QUITTING, b);
	thread_wait(&thread);
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
	const char *p = &buffer[6];
	int moves = str_to_num(p);
	while (*p++ != ' ')
		;
	int secs = str_to_secs(p);
	while (*p++ != ' ')
		;
	int inc = str_to_num(p);
	for (int color = WHITE; color <= BLACK; color++)
		clock_ptr->set_mode(color, moves, secs * 100, inc * 100);
}

/*----------------------------------------------------------------------------*\
 |				    do_st()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_st()
{

/* Set the maximum search time. */

	sync = false;
	for (int color = WHITE; color <= BLACK; color++)
		clock_ptr->set_mode(color, 1, str_to_num(&buffer[3]) * 100, 0);
}

/*----------------------------------------------------------------------------*\
 |				    do_sd()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_sd() const
{

/* Set the maximum search depth. */

	search_ptr->set_depth(str_to_num(&buffer[3]));
}

/*----------------------------------------------------------------------------*\
 |				   do_time()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_time() const
{
	if (sync)
		clock_ptr->update_remaining_csecs(!b.get_whose(), str_to_num(&buffer[5]));
}

/*----------------------------------------------------------------------------*\
 |				   do_otim()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_otim() const
{
	if (sync)
		clock_ptr->update_remaining_csecs(b.get_whose(), str_to_num(&buffer[5]));
}

/*----------------------------------------------------------------------------*\
 |				 do_usermove()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_usermove()
{

/*
 | Our opponent has moved.  If the move was legal, and it didn't just end the
 | game, and we're not in force mode: formulate a response.
 */

	move_t m;

	/* Was the move legal? */
	m.old_x = buffer[ 9] - 'a'; m.old_y = buffer[10] - '1';
	m.new_x = buffer[11] - 'a'; m.new_y = buffer[12] - '1';
	m.promo = char_to_shape(buffer[13]);
	if (!test_move(m))
	{
		/* No!  The gypsy was trying to pull a fast one on us! */
		printf("Illegal move: ");
		print_move(m);
		printf("\n");
		return;
	}

	b.make(m);
	clock_ptr->dec_remaining_moves(!b.get_whose());

	/* Alright, so the move was legal.  Did it just end the game? */
	if (game_over())
	{
		/* Yes.  We're not to respond. */
		search_ptr->change(IDLING, b);
		return;
	}

	/*
	 | Alright, so the move was legal, and it didn't just end the game.  Are
	 | we in force mode?
	 */
	if (force)
		/* Yes.  We're not to respond. */
		return;

	/*
	 | Alright, so the move was legal, and it didn't just end the game, and
	 | we're not in force mode.  Formulate a response.
	 */
	search_ptr->change(THINKING, b);
}

/*----------------------------------------------------------------------------*\
 |				 do_question()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_question() const
{
	search_ptr->move_now();
}

/*----------------------------------------------------------------------------*\
 |				   do_ping()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_ping()
{

/*
 | XBoard has sent a ping request (to make sure we're not on drugs).  Send a
 | pong reply.
 */

	buffer[1] = 'o';
	printf("%s\n", buffer);
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

	move_t m = search_ptr->get_hint();
	if (IS_NULL_MOVE(m))
		return;
	printf("Hint: ");
	print_move(m);
	printf("\n");
}

/*----------------------------------------------------------------------------*\
 |				   do_undo()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_undo()
{

/* Take back one ply. */

	b.unmake();
	clock_ptr->inc_remaining_moves(b.get_whose());
}

/*----------------------------------------------------------------------------*\
 |				  do_remove()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_remove()
{

/* Take back two plies. */

	b.unmake();
	b.unmake();
	for (int color = WHITE; color <= BLACK; color++)
		clock_ptr->inc_remaining_moves(color);
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
bool xboard::test_move(move_t m)
{

/* In the current position, is the specified move legal? */

	list<move_t> l;

	b.generate(l, true);
	for (list<move_t>::iterator it = l.begin(); it != l.end(); it++)
		if (*it == m)
			return true;
	return false;
}
