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
#include <string>

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"

/* Extra Gray Matter stuff: */
#include "board_base.h"
#include "board_heuristic.h"
#include "table.h"

#define MAX_TOKEN_LENGTH	256

/* Token types: */
#define TOKEN_UNKNOWN		0 // Unknown.
#define TOKEN_SPACE		1 // Whitespace.
#define TOKEN_STRING		2 // String.
#define TOKEN_INTEGER		3 // Integer.
#define TOKEN_PUNCTUATION	4 // Punctuation.
#define TOKEN_GLYPH		5 // Numeric Annotation Glyph (NAG).
#define TOKEN_SYMBOL		6 // Symbol.

#define IS_SYMBOL(c)	(isalnum((c))     || (c) == (int) '_' || \
			 (c) == (int) '+' || (c) == (int) "#" || \
			 (c) == (int) '=' || (c) == (int) ':' || \
			 (c) == (int) '-')
#define GAME_OVER(s)	(!strcmp((s), "1-0")     || !strcmp((s), "0-1") || \
			 !strcmp((s), "1/2-1/2") || !strcmp((s), "*"))

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
	int tokenize(string& token);
	bool tokenize_space(string& token);
	bool tokenize_string(string& token);
	bool tokenize_integer(string& token);
	bool tokenize_punctuation(string& token);
	bool tokenize_glyph(string& token);
	bool tokenize_symbol(string& token);
};

#endif
