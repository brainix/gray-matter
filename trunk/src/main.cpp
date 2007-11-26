/*----------------------------------------------------------------------------*\
 |	main.cpp							      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
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

// Default Gray Matter stuff:
#include "config.h"
#include "gray.h"

// Extra Gray Matter stuff:
#include "table.h"
#include "clock.h"
#include "xboard.h"
#include "search_base.h"
#include "search_mtdf.h"
#include "book.h"
#include "testing.h"

// Function prototypes:
int main(int argc, char **argv);

/*----------------------------------------------------------------------------*\
 |				     main()				      |
\*----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	// Default settings:
	string search_engine = SEARCH_ENGINE;
	int xpos_table_mb = XPOS_TABLE_MB;
	string book_name = BOOK_NAME;
	int book_moves = BOOK_MOVES;
	int overhead = OVERHEAD;

	// Parse the command-line arguments and possibly change the default
	// settings.
	for (int c; (c = getopt(argc, argv, "e:x:n:m:o:p:")) != -1;)
		switch (c)
		{
			case 'e':
				// Specifying which move search engine to use.
				search_engine = optarg;
				if (search_engine != "MTD(f)")
				{
					cerr << "move search engine "
					     << "must be MTD(f)"
					     << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 'x':
				// Specifying the size of the transposition
				// table.
				if ((xpos_table_mb = atoi(optarg)) < 1)
				{
					cerr << "transposition table "
					     << "must be >= 1 MB"
					     << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 'n':
				// Specifying the file name of the opening book.
				book_name = optarg;
				break;
			case 'm':
				// Specifying the number of moves to read per
				// game in the opening book.
				if ((book_moves = atoi(optarg)) < 1)
				{
					cerr << "number of book moves "
					     << "must be >= 1 ply"
					     << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 'o':
				// Specifying the move search overhead.
				if ((overhead = atoi(optarg)) < 1)
				{
					cerr << "move search overhead "
					     << "must be >= 1 centisecond"
					     << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 'p':
			{
				// Specifying correctness and performance tests.
				testing t(optarg);
				t.start();
				break;
			}
			default:
				// Specifying the user doesn't know how to read.
				cerr << "unknown option: -" << optopt << endl;
				exit(EXIT_FAILURE);
				break;
		}
	if (optind < argc)
	{
		// Specifying the user doesn't know how to read.
		cerr << "unknown argument: " << argv[optind] << endl;
		exit(EXIT_FAILURE);
	}

	// Seed the random number generator.
	srand(time(NULL));

	// Instantiate the classes.
	table t(xpos_table_mb);            // Transposition table object.
	history h;                         // History table object.
	chess_clock c(overhead);           // Chess clock object.
	xboard x;                          // XBoard object.
	book o(&t, book_name, book_moves); // Opening book object.

	// Based on the -s command-line option, choose the move search engine
	// and cast it as a generic version.  Thus far, we've only implemented
	// one move search engine, MTD(f).
	search_base *s = new search_mtdf(&t, &h, &c, &x);

	// Launch the event loop.
	x.loop(s, &c, &o);
	return 0;
}
