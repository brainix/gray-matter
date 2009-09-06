/*----------------------------------------------------------------------------*\
 |  main.cpp                                                                  |
 |                                                                            |
 |  Copyright © 2005-2008, The Gray Matter Team, original authors.            |
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

#include <iostream>

#include "configfile.h"
#include "testing.h"
#include "xboard.h"
#include "search_mtdf.h"

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

    for(int i=1;i<argc;++i)
    {
      switch (argv[i][1])  //assumes -eXXX or similar
      {
        case 'e':
          // Specifying which move search engine to use.
          if (strcmp(&argv[i][2], "MTD(f)"))
          {
              cerr << "move search engine must be MTD(f)" << endl;
              exit(EXIT_FAILURE);
          }
          cout << "Setting engine to MTD(f)" << endl;
          config.set("engine", &argv[i][2]);
          break;
        case 'x':
          {
            int xPosTableSize = atoi(&argv[i][2]);    //assumes -x999
            if (xPosTableSize < 1)
            {
              cerr << "transposition table must be >= 1 MB" << endl;
              exit(EXIT_FAILURE);
            }
            cout << "Setting transposition size to " << xPosTableSize 
                 << endl;
            config.set("xpos_table_mb", xPosTableSize);
            break;
          }
        case 'n':
          // Specifying the file name of the opening book.
          cout << "Setting opening book to: " << &argv[i][2] << endl;
          config.set("book_name", &argv[i][2]);
          break;
        case 'm':
          {
            // Specifying the number of moves to read per game in the
            // opening book.
            int iMoves = atoi(&argv[i][2]);
            if (iMoves < 1)
            {
              cerr << "number of book moves must be >= 1 ply" << endl;
              exit(EXIT_FAILURE);
            }
            cout << "Setting moves to read in book to " << iMoves
                 << endl;
            config.set("book_moves", iMoves);
            break;
          }
        case 'o':
          {
            // Specifying the move search overhead.
            int iOverhead = atoi(&argv[i][2]);
            if (iOverhead < 1)
            {
                cerr << "move search overhead must be >= 1 centisecond"
                     << endl;
                exit(EXIT_FAILURE);
            }
            cout << "Setting move search overhead to " << iOverhead
                 << endl;
            config.set("overhead", iOverhead);
            break;
          }
        case 'p':
          {
            // Specifying correctness and performance tests.
            testing t(&argv[i][2]);
            t.start();
            break;
          }
        default:
          // Specifying that the user doesn't know how to read.
          cerr << "ERROR: could not interpret option "
               << argv[i]
               << endl;
          exit(EXIT_FAILURE);
          break;
        }
    }

	// Dump configuration items.
    config.dump(cout);

    // Seed the random number generator.
    srand((unsigned int)time(NULL));

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
