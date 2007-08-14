/*----------------------------------------------------------------------------*\
 |	book.h - opening book interface					      |
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

#define MAX_TOK_LEN	256

/* Token types: */
#define UNKNOWN		0 // Unknown.
#define SPACE		1 // Whitespace.
#define TOK_STR		2 // String.
#define TOK_INT		3 // Integer.
#define TOK_PUNC	4 // Punctuation.
#define TOK_NAG		5 // Numeric Annotation Glyph (NAG).
#define TOK_SYM		6 // Symbol.

#define IS_SYM(c)	(isalnum((c))     || (c) == (int) '_' || \
			 (c) == (int) '+' || (c) == (int) "#" || \
			 (c) == (int) '=' || (c) == (int) ':' || \
			 (c) == (int) '-')

/* Forward declarations: */
class board_base;
class table;

class book
{
public:
	book(table *t, char *n, int m);
	void read();
private:
	char name[32];
	int moves;
	ifstream file;

	board_base *board_ptr; // Board representation object.
	table *table_ptr;      // Transposition table object.

	void parse();
	int tokenize(char *buffer);
};

#endif
