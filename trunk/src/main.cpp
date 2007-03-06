/*----------------------------------------------------------------------------*\
 |	main.cpp							      |
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
#include "board.h"
#include "config.h"
#include "history.h"
#include "opening.h"
#include "search.h"
#include "table.h"
#include "types.h"
#include "xboard.h"

/* Global variables: */
int mb = TABLE_MB; /* The size of the transposition (in megabytes). */

/* Function prototypes: */
int main(int argc, char **argv);

/*----------------------------------------------------------------------------*\
 |				     main()				      |
\*----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	/* Seed the random number generator. */
	srand(time(NULL));

	/* Instantiate the classes. */
	history h;
	opening o;
	class search s;
	table t(mb);
	xboard x;

	/* Bind the objects. */
	o.bind(&t);
	s.bind(&t, &h, &x);
	x.bind(&s);

	/* Launch the event loop. */
	x.loop();
	return 0;
}
