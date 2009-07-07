/*----------------------------------------------------------------------------*\
 |  book.h - opening book interface                                           |
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

#ifndef BOOK_H
#define BOOK_H

// C++ stuff:
#include <fstream>
#include <string>
#include <list>

#include "move.h"

// Token types:
#define TOKEN_UNKNOWN       0 // Unknown.
#define TOKEN_SPACE         1 // Whitespace.
#define TOKEN_STRING        2 // String.
#define TOKEN_INTEGER       3 // Integer.
#define TOKEN_PUNCTUATION   4 // Punctuation.
#define TOKEN_GLYPH         5 // Numeric Annotation Glyph (NAG).
#define TOKEN_SYMBOL        6 // Symbol.

// Is the specified token punctuation (as defined by the PGN specification)?
#define IS_PUNCT(c) ((c) == (int) '.' || (c) == (int) '*' || \
                     (c) == (int) '[' || (c) == (int) ']' || \
                     (c) == (int) '<' || (c) == (int) '>')

// Is the specified token a symbol (as defined by the PGN specification)?
#define IS_SYMBOL(c)    (isalnum((c))     || (c) == (int) '_' || \
                         (c) == (int) '+' || (c) == (int) '#' || \
                         (c) == (int) '=' || (c) == (int) ':' || \
                         (c) == (int) '-')

// Forward declarations:
class board_base;
class table;

/// Opening book.
class book
{
public:
    book(table *t, std::string file_name, int n);
    void read();

private:
    int num_moves;                       ///<
    std::list<std::list<Move> > games; ///< Game list.

    board_base *board_ptr;               ///< Board representation object.
    table *table_ptr;                    ///< Transposition table object.

    // Based on the PGN file, the following methods populate the token list,
    // game list, and transposition table.
    void populate_tokens(std::istream& stream, std::list<std::string>& tokens);
    void populate_games(std::list<std::string>& tokens);
    void populate_table();

    //
    int tokenize(std::istream& stream, std::string& token);
    bool tokenize_space(std::istream& stream, std::string& token);
    bool tokenize_string(std::istream& stream, std::string& token);
    bool tokenize_integer(std::istream& stream, std::string& token);
    bool tokenize_punctuation(std::istream& stream, std::string& token);
    bool tokenize_glyph(std::istream& stream, std::string& token);
    bool tokenize_symbol(std::istream& stream, std::string& token);
};

#endif
