/*----------------------------------------------------------------------------*\
 |	main.cpp							      |
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

/* C stuff: */
#include "gray.h"

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"

/* Extra Gray Matter stuff: */
#include "table.h"
#include "xboard.h"
#include "clock.h"
#include "search_mtdf.h"

/* Function prototypes: */
int main(int argc, char **argv);

/*----------------------------------------------------------------------------*\
 |				     main()				      |
\*----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	char search_engine[32] = "MTD(f)";
	int xpos_table_size = XPOS_TABLE_MB;

	/* Parse the command-line arguments. */
	for (int c; (c = getopt(argc, argv, "s:x:")) != -1;)
		switch (c)
		{
			case 's':
				/* Specifying which search engine to use. */
				strncpy(search_engine, optarg, sizeof(search_engine));
				if (strncasecmp(search_engine, "MTD(f)", 6) != 0)
				{
					printf("unknown search engine: %s\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 'x':
				/* Specifying the size of the transposition table. */
				if ((xpos_table_size = atoi(optarg)) < 1)
				{
					printf("transposition table must be >= 1 MB\n");
					exit(EXIT_FAILURE);
				}
				break;
			default:
				printf("unknown option: -%c\n", optopt);
				exit(EXIT_FAILURE);
				break;
		}
	if (optind < argc)
	{
		for (int index = optind; index < argc; index++)
			printf("unknown argument: %s\n", argv[index]);
		exit(EXIT_FAILURE);
	}

	/* Seed the random number generator. */
	srand(time(NULL));

	/* Instantiate the classes. */
	table t(xpos_table_size);
	history h;
	xboard x;
	chess_clock c;

	/*
	 | Based on the -s command-line option, choose the move search engine
	 | and cast it as a generic version.  Thus far, we've only implemented
	 | one move search engine, MTD(f).
	 */
	search_base *s = new search_mtdf(&t, &h, &c, &x);

	/* Launch the event loop. */
	x.loop(s, &c);
	return 0;
}
