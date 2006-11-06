/*----------------------------------------------------------------------------*\
 |	xboard.cpp - Chess Engine Communication Protocol implementation	      |
 |									      |
 |	Copyright � 2005-2006, The Gray Matter Team, original authors.	      |
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
	force = false;
	output = false;
}

/*----------------------------------------------------------------------------*\
 |				     bind()				      |
\*----------------------------------------------------------------------------*/
void xboard::bind(board *b, table *t, history *h, class search *s)
{
	board_ptr = b;
	table_ptr = t;
	history_ptr = h;
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
		else if (!strncmp(s, "quit", 4))
			do_quit();
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
		else if (!strncmp(s, "ping", 4))
			do_ping();
		else if (!strncmp(s, "hint", 4))
			do_hint();
		else if (!strncmp(s, "undo", 4))
			do_undo();
		else if (!strncmp(s, "remove", 6))
			do_remove();
		else if (!strncmp(s, "post", 4))
			do_post();
		else if (!strncmp(s, "nopost", 6))
			do_nopost();
		else
			printf("Error (unknown command): %s\n", s);
	}
}

/*----------------------------------------------------------------------------*\
 |				 print_output()				      |
\*----------------------------------------------------------------------------*/
void xboard::print_output(int ply, int value, int time, int nodes, list<move_t> &pv) const
{

/* Print thinking output. */

	printf("%d ", ply);
	printf("%d ", value);
	printf("%d ", time);
	printf("%d",  nodes);

	for (list<move_t>::iterator it = pv.begin(); it != pv.end(); it++)
	{
		printf(" ");
		print_move(*it);
	}
	printf("\n");
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
	printf("feature ping=1\n");
	printf("feature playother=1\n");
	printf("feature usermove=1\n");
	printf("feature time=0\n");
	printf("feature draw=0\n");
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
	board_ptr->set_board();
	table_ptr->clear();
	history_ptr->clear();
	search_ptr->set_depth(DEPTH);
	force = false;
}

/*----------------------------------------------------------------------------*\
 |				   do_quit()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_quit() const
{
}

/*----------------------------------------------------------------------------*\
 |				   do_force()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_force()
{
	force = true;
}

/*----------------------------------------------------------------------------*\
 |				    do_go()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_go()
{
	move_t m;

	force = false;
	if ((m = search_ptr->iterate()).value == -WEIGHT_KING)
	{
		printf("resign\n");
		return;
	}
	board_ptr->make(m);
	printf("move ");
	print_move(m);
	printf("\n");
	game_over();
}

/*----------------------------------------------------------------------------*\
 |				 do_playother()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_playother()
{
	force = false;
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
	search_ptr->set_time(secs / moves + inc);
}

/*----------------------------------------------------------------------------*\
 |				    do_st()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_st() const
{
	search_ptr->set_time(str_to_num(&s[3]));
}

/*----------------------------------------------------------------------------*\
 |				    do_sd()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_sd() const
{
	search_ptr->set_depth(str_to_num(&s[3]));
}

/*----------------------------------------------------------------------------*\
 |				 do_usermove()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_usermove() const
{
	move_t m;

	m.old_x = s[ 9] - 'a'; m.old_y = s[10] - '1';
	m.new_x = s[11] - 'a'; m.new_y = s[12] - '1';
	m.promo = char_to_shape(s[13]);
	if (!test_move(m))
	{
		printf("Illegal move: ");
		print_move(m);
		printf("\n");
		return;
	}

	board_ptr->make(m);
	if (force || game_over())
		return;

	if ((m = search_ptr->iterate()).value == -WEIGHT_KING)
	{
		printf("resign\n");
		return;
	}
	board_ptr->make(m);
	printf("move ");
	print_move(m);
	printf("\n");
	game_over();
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
 |				   do_hint()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_hint() const
{
	move_t m = search_ptr->iterate();
	printf("Hint: ");
	print_move(m);
	printf("\n");
}

/*----------------------------------------------------------------------------*\
 |				   do_undo()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_undo() const
{
	board_ptr->unmake();
}

/*----------------------------------------------------------------------------*\
 |				  do_remove()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_remove() const
{
	board_ptr->unmake();
	board_ptr->unmake();
}

/*----------------------------------------------------------------------------*\
 |				   do_post()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_post() const
{
	search_ptr->set_output(true);
}

/*----------------------------------------------------------------------------*\
 |				  do_nopost()				      |
\*----------------------------------------------------------------------------*/
void xboard::do_nopost() const
{
	search_ptr->set_output(false);
}

/*----------------------------------------------------------------------------*\
 |				  game_over()				      |
\*----------------------------------------------------------------------------*/
int xboard::game_over() const
{
	int status;

	switch (status = board_ptr->get_status())
	{
		case STALEMATE    : printf("1/2-1/2 {Stalemate}\n");                                                       break;
		case INSUFFICIENT : printf("1/2-1/2 {Insufficient material}\n");                                           break;
		case THREE        : printf("1/2-1/2 {Threefold repetition}\n");                                            break;
		case FIFTY        : printf("1/2-1/2 {Fifty move rule}\n");                                                 break;
		case CHECKMATE    : printf("%s mates}\n", !board_ptr->get_whose() == WHITE ? "1-0 {White" : "0-1 {Black"); break;
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
	list<move_t> l;

	board_ptr->generate(l);
	for (list<move_t>::iterator it = l.begin(); it != l.end(); it++)
		if (it->old_x == m.old_x && it->old_y == m.old_y &&
		    it->new_x == m.new_x && it->new_y == m.new_y &&
		    it->promo == m.promo)
			return true;
	return false;
}
