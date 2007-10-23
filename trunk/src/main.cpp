/*----------------------------------------------------------------------------*\
 |	main.cpp							      |
 |									      |
 |	Copyright � 2005-2007, The Gray Matter Team, original authors.	      |
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
	int pawn_table_mb = PAWN_TABLE_MB;
	string book_name = BOOK_NAME;
	int book_moves = BOOK_MOVES;
	int overhead = OVERHEAD;

	// Parse the command-line arguments and possibly change the default
	// settings.
	for (int c; (c = getopt(argc, argv, "e:x:p:n:m:o:")) != -1;)
		switch (c)
		{
			case 'e':
				// Specifying the move search engine to use.
				search_engine = optarg;
				if (search_engine != "BogoSearch" &&
				    search_engine != "MTD(f)")
				{
					cout << "move search engine must be "
					     << "BogoSearch or MTD(f)"
					     << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 'x':
				// Specifying the size of the transposition
				// table.
				if ((xpos_table_mb = atoi(optarg)) < 1)
				{
					cout << "transposition table must be "
					     << ">= 1 MB"
					     << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 'p':
				// Specifying the size of the pawn table.
				if ((pawn_table_mb = atoi(optarg)) < 1)
				{
					cout << "pawn table must be >= 1 MB"
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
				// game from the opening book.
				if ((book_moves = atoi(optarg)) < 1)
				{
					cout << "number of book moves must be "
					     << ">= 1 ply"
					     << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case 'o':
				// Specifying the move search overhead.
				if ((overhead = atoi(optarg)) < 1)
				{
					cout << "move search overhead must be "
					     << ">= 1 centisecond"
					     << endl;
					exit(EXIT_FAILURE);
				}
				break;
			default:
				// Specifying the user doesn't know how to read.
				cout << "unknown option: -" << optopt << endl;
				exit(EXIT_FAILURE);
				break;
		}
	if (optind < argc)
	{
		// Specifying the user doesn't know how to read.
		cout << "unknown argument: " << argv[optind] << endl;
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
	// and cast it as a generic version.
	search_base *s;
	if (search_engine == "BogoSearch")
		s = new search_bogo(&t, &h, &c, &x);
	else if (search_engine == "MTD(f)")
		s = new search_mtdf(&t, &h, &c, &x);

	// Launch the event loop.
	x.loop(s, &c, &o);
	return 0;
}
