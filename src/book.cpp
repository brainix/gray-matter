/*----------------------------------------------------------------------------*\
 |  book.cpp - opening book implementation                                    |
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

#include "gray.h"
#include "book.h"
#include "board_heuristic.h"

using namespace std;

/*----------------------------------------------------------------------------*\
 |                                   book()                                   |
\*----------------------------------------------------------------------------*/
book::book(table *t, string file_name, int n)
{
    ifstream stream;     // The PGN opening book file stream.
    list<string> tokens; // The token list.

    board_ptr = new board_heuristic();
    table_ptr = t;
    num_moves = n;

    stream.open(file_name.c_str());  // Open the PGN file.
    populate_tokens(stream, tokens); // Populate the token list.
    stream.close();                  // Close the PGN file.
    populate_games(tokens);          // Populate the game list.
}

/*----------------------------------------------------------------------------*\
 |                                   read()                                   |
\*----------------------------------------------------------------------------*/
void book::read()
{

/// At this point, we've already populated the game list.  Now, we're actually
/// starting a new game.  Based on the game list, populate the transposition
/// table.

    populate_table();
}

/*----------------------------------------------------------------------------*\
 |                             populate_tokens()                              |
\*----------------------------------------------------------------------------*/
void book::populate_tokens(istream& stream, list<string>& tokens)
{

/// Based on the PGN file, populate the token list.

    string token;

    while (!stream.fail() && !stream.eof())
        if (tokenize(stream, token) == TOKEN_SYMBOL)
            tokens.push_back(token);
}

/*----------------------------------------------------------------------------*\
 |                              populate_games()                              |
\*----------------------------------------------------------------------------*/
void book::populate_games(list<string>& tokens)
{

/// Based on the token list, populate the game list.

    list<string>::iterator it;
    Move move;
    list<Move> moves;

    for (it = tokens.begin(); it != tokens.end(); it++)
    {
        move = board_ptr->san_to_coord(*it);
        if (move.is_null())
        {
            if (moves.empty())
                continue;
            games.push_back(moves);
            moves.clear();
            board_ptr->set_board();
            continue;
        }
        moves.push_back(move);
        board_ptr->make(move);
    }
    board_ptr->set_board();
}

/*----------------------------------------------------------------------------*\
 |                              populate_table()                              |
\*----------------------------------------------------------------------------*/
void book::populate_table()
{

/// Based on the game list, populate the transposition table.

    list<list<Move> >::iterator game;
    list<Move> moves;
    list<Move>::iterator move;

    games.sort(shuffle);
    for (game = games.begin(); game != games.end(); game++)
    {
        moves = *game;
        for (move = moves.begin(); move != moves.end(); move++)
        {
            if (board_ptr->get_num_moves() >= num_moves)
                break;
            table_ptr->store(board_ptr->get_hash(), MAX_DEPTH, BOOK, *move);
            board_ptr->make(*move);
        }
        board_ptr->set_board();
    }
}

/*----------------------------------------------------------------------------*\
 |                                 tokenize()                                 |
\*----------------------------------------------------------------------------*/
int book::tokenize(istream& stream, string& token)
{

/// Forward past the PGN opening book file stream's next token, save it (null
/// terminated), and return its type.

    if (tokenize_space(stream, token))
        return TOKEN_SPACE;
    if (tokenize_string(stream, token))
        return TOKEN_STRING;
    if (tokenize_integer(stream, token))
        return TOKEN_INTEGER;
    if (tokenize_punctuation(stream, token))
        return TOKEN_PUNCTUATION;
    if (tokenize_glyph(stream, token))
        return TOKEN_GLYPH;
    if (tokenize_symbol(stream, token))
        return TOKEN_SYMBOL;

    while (!isspace(stream.peek()))
        token += stream.get();
    token += '\0';
    return TOKEN_UNKNOWN;
}

/*----------------------------------------------------------------------------*\
 |                              tokenize_space()                              |
\*----------------------------------------------------------------------------*/
bool book::tokenize_space(istream& stream, string& token)
{

/// If the stream's next token is whitespace (as defined by the PGN
/// specification), then forward the stream past it, save it (null terminated),
/// and return true.  Otherwise, leave the stream untouched, save only the null
/// character, and return false.

    token.erase(0, token.length());
    for (int c; isspace(c = stream.peek()); stream.ignore())
        token += c;
    token += '\0';
    return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |                             tokenize_string()                              |
\*----------------------------------------------------------------------------*/
bool book::tokenize_string(istream& stream, string& token)
{

/// If the stream's next token is a string (as defined by the PGN
/// specification), then forward the stream past it, save it (null terminated),
/// and return true.  Otherwise, leave the stream untouched, save only the null
/// character, and return false.

    token.erase(0, token.length());
    if (stream.peek() == '\"')
        for (token += stream.get(); ; )
        {
            int c = stream.get();
            if (c == EOF)
            {
                token += '\0';
                return false;
            }
            token += c;
            if (c == '\"')
                break;
        }
    token += '\0';
    return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |                             tokenize_integer()                             |
\*----------------------------------------------------------------------------*/
bool book::tokenize_integer(istream& stream, string& token)
{

/// If the stream's next token is an integer (as defined by the PGN
/// specification), then forward the stream past it, save it (null terminated),
/// and return true.  Otherwise, leave the stream untouched, save only the null
/// character, and return false.

    token.erase(0, token.length());
    for (int c; isdigit(c = stream.peek()); stream.ignore())
        token += c;
    token += '\0';
    return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |                           tokenize_punctuation()                           |
\*----------------------------------------------------------------------------*/
bool book::tokenize_punctuation(istream& stream, string& token)
{

/// If the stream's next token is punctuation (as defined by the PGN
/// specification), then forward the stream past it, save it (null terminated),
/// and return true.  Otherwise, leave the stream untouched, save only the null
/// character, and return false.

    token.erase(0, token.length());
    int c = stream.peek();
    if (IS_PUNCT(c))
    {
        token += c;
        stream.ignore();
    }
    token += '\0';
    return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |                              tokenize_glyph()                              |
\*----------------------------------------------------------------------------*/
bool book::tokenize_glyph(istream& stream, string& token)
{

/// If the stream's next token is a Numeric Annotation Glyph (as defined by the
/// PGN specification), then forward the stream past it, save it (null
/// terminated), and return true.  Otherwise, leave the stream untouched, save
/// only the null character, and return false.

    token.erase(0, token.length());
    if (stream.peek() == '$')
    {
        token += stream.get();
        for (int c; isdigit(c = stream.peek()); stream.ignore())
            token += c;
    }
    token += '\0';
    return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |                             tokenize_symbol()                              |
\*----------------------------------------------------------------------------*/
bool book::tokenize_symbol(istream& stream, string& token)
{

/// If the stream's next token is a symbol (as defined by the PGN
/// specification), then forward the stream past it, save it (null terminated),
/// and return true.  Otherwise, leave the stream untouched, save only the null
/// character, and return false.

    token.erase(0, token.length());
    if (isalnum(stream.peek()))
        for (int c; IS_SYMBOL(c = stream.peek()); stream.ignore())
            token += c;
    token += '\0';
    return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |                                 shuffle()                                  |
\*----------------------------------------------------------------------------*/
bool book::shuffle(list<Move> l1, list<Move> l2)
{

/// Pass this method as the comparison function to l.sort() to randomize the
/// game list.  This is a magnificent hack.
///
/// Note: This hack wouldn't work for O(n²) list sort algorithms.  But if your
/// STL's list sort algorithm is O(n²), then you don't deserve for this hack to
/// work anyway.

    return rand() & 1;
}
