/*----------------------------------------------------------------------------*\
 |	book.h - opening book interface					      |
 |									      |
 |	Copyright © 2005-2008, The Gray Matter Team, original authors.	      |
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

// C++ stuff:
#include <fstream>
#include <string>

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "board_base.h"
#include "board_heuristic.h"
#include "table.h"

// Token types:
#define TOKEN_UNKNOWN		0 // Unknown.
#define TOKEN_SPACE		1 // Whitespace.
#define TOKEN_STRING		2 // String.
#define TOKEN_INTEGER		3 // Integer.
#define TOKEN_PUNCTUATION	4 // Punctuation.
#define TOKEN_GLYPH		5 // Numeric Annotation Glyph (NAG).
#define TOKEN_SYMBOL		6 // Symbol.

// Is the specified token punctuation (as defined by the PGN specification)?
#define IS_PUNCT(c)	((c) == (int) '.' || (c) == (int) '*' || \
			 (c) == (int) '[' || (c) == (int) ']' || \
			 (c) == (int) '<' || (c) == (int) '>')

// Is the specified token a symbol (as defined by the PGN specification)?
#define IS_SYMBOL(c)	(isalnum((c))     || (c) == (int) '_' || \
			 (c) == (int) '+' || (c) == (int) "#" || \
			 (c) == (int) '=' || (c) == (int) ':' || \
			 (c) == (int) '-')

// Forward declarations:
class board_base;
class table;

class book
{
public:
	book(table *t, string& file_name, int n);
	void read();
private:
	int num_moves;             ///<
	list<list<move_t> > games; ///< Game list.

	board_base *board_ptr;     ///< Board representation object.
	table *table_ptr;          ///< Transposition table object.

	// Based on the PGN file, the following methods populate the token list,
	// game list, and transposition table.
	void populate_tokens(istream& stream, list<string>& tokens);
	void populate_games(list<string>& tokens);
	void populate_table();

	//
	int tokenize(istream& stream, string& token);
	bool tokenize_space(istream& stream, string& token);
	bool tokenize_string(istream& stream, string& token);
	bool tokenize_integer(istream& stream, string& token);
	bool tokenize_punctuation(istream& stream, string& token);
	bool tokenize_glyph(istream& stream, string& token);
	bool tokenize_symbol(istream& stream, string& token);

	// The following method is the most magnificent hack of my life.
	static bool shuffle(list<move_t> l1, list<move_t> l2);
};

#endif
