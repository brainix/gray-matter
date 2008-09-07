/*----------------------------------------------------------------------------*\
 |  main.cpp                                                                  |
 |                                                                            |
 |  Copyright � 2005-2008, The Gray Matter Team, original authors.            |
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
#include "configfile.h"
#include "gray.h"

// Extra Gray Matter stuff:
#include "table.h"
#include "clock.h"
#include "xboard.h"
#include "search_base.h"
#include "search_mtdf.h"
#include "book.h"
#include "testing.h"

using namespace std;

// Function prototypes:
int main(int argc, char **argv);

/*----------------------------------------------------------------------------*\
 |                                   main()                                   |
\*----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
    // First parse the configuration file, if it exists.
    ConfigFile config;
    config.dump(cout);

    // Parse the command-line arguments and possibly change the default
    // settings.
    for (int c; (c = getopt(argc, argv, "e:x:n:m:o:p:")) != -1;)
        switch (c)
        {
            case 'e':
                // Specifying which move search engine to use.
                if (strcmp(optarg, "MTD(f)"))
                {
                    cerr << "move search engine must be MTD(f)" << endl;
                    exit(EXIT_FAILURE);
                }
                config.set("engine", optarg);
                break;
            case 'x':
                // Specifying the size of the transposition table.
                if (atoi(optarg) < 1)
                {
                    cerr << "transposition table must be >= 1 MB" << endl;
                    exit(EXIT_FAILURE);
                }
                config.set("xpos_table_mb", atoi(optarg));
                break;
            case 'n':
                // Specifying the file name of the opening book.
                config.set("book_name", optarg);
                break;
            case 'm':
                // Specifying the number of moves to read per game in the
                // opening book.
                if (atoi(optarg) < 1)
                {
                    cerr << "number of book moves must be >= 1 ply" << endl;
                    exit(EXIT_FAILURE);
                }
                config.set("book_moves", atoi(optarg));
                break;
            case 'o':
                // Specifying the move search overhead.
                if (atoi(optarg) < 1)
                {
                    cerr << "move search overhead must be >= 1 centisecond"
                         << endl;
                    exit(EXIT_FAILURE);
                }
                config.set("overhead", atoi(optarg));
                break;
            case 'p':
            {
                // Specifying correctness and performance tests.
                testing t(optarg);
                t.start();
                break;
            }
            default:
                // Specifying that the user doesn't know how to read.
                cerr << "unknown option: -"
                     << static_cast<char>(optopt)
                     << endl;
                exit(EXIT_FAILURE);
                break;
        }
    if (optind < argc)
    {
        // Specifying that the user doesn't know how to read.
        cerr << "unknown argument: " << argv[optind] << endl;
        exit(EXIT_FAILURE);
    }

    // Seed the random number generator.
    srand(time(NULL));

    // Instantiate the classes.
    /// Transposition table object.
    table t(config.getInt("xpos_table_mb"));
    /// History table object.
    history h;
    /// Chess clock object.
    chess_clock c(config.getInt("overhead"));
    /// XBoard object.
    xboard x;
    /// Opening book object.
    book o(&t, config.getString("book_name"), config.getInt("book_moves"));

    // Based on the -s command-line option, choose the move search engine
    // and cast it as a generic version.  Thus far, we've only implemented
    // one move search engine, MTD(f).
    search_base *s = 0;
    if (config.getString("engine") == "MTD(f)")
        s = new search_mtdf(&t, &h, &c, &x);

    // Launch the event loop.
    x.loop(s, &c, &o);
    return 0;
}
