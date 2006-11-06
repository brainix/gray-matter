/*----------------------------------------------------------------------------*\
 |	main.cpp							      |
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

/* Function prototypes: */
int main();

/*----------------------------------------------------------------------------*\
 |				     main()				      |
\*----------------------------------------------------------------------------*/
int main()
{
	/* Seed the random number generator. */
	srand(time(NULL));

	/* Instantiate the classes. */
	board b;
	table t;
	opening o;
	history h;
	xboard x;
	class search s;

	/* Bind the objects. */
	o.bind(&b, &t);
	x.bind(&b, &t, &h, &s);
	s.bind(&b, &t, &h, &x);

	/* Launch the event loop. */
	x.loop();
	return 0;
}
