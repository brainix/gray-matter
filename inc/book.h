/*----------------------------------------------------------------------------*\
 |	book.h - opening book interface					      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 3 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BOOK_H
#define BOOK_H

using namespace std;

/* C++ stuff: */
#include <fstream>

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"

/* Extra Gray Matter stuff: */
#include "board_base.h"
#include "board_heuristic.h"
#include "table.h"

/* Forward declarations: */
class board_base;
class table;

class book
{
public:
	book(table *t, char *n);
	void read();
private:
	char name[32];
	ifstream file;

	board_base *board_ptr; // Board representation object.
	table *table_ptr;      // Transposition table object.
};

#endif
