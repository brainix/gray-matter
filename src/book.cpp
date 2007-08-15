/*----------------------------------------------------------------------------*\
 |	book.cpp - opening book implementation				      |
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

#include "gray.h"
#include "book.h"

/*----------------------------------------------------------------------------*\
 |				     book()				      |
\*----------------------------------------------------------------------------*/
book::book(table *t, char *file_name, int n)
{
	ifstream stream;

	board_ptr = new board_heuristic();
	table_ptr = t;
	num_moves = n;

	/* Open the file. */
	stream.open(file_name);
	if (stream.fail())
	{
		cout << "couldn't find opening book: " << file_name << endl;
		stream.close();
		return;
	}

	/* Populate the token and move lists. */
	populate_tokens(stream);
	populate_moves();

	/* Close the file. */
	stream.close();
}

/*----------------------------------------------------------------------------*\
 |				     read()				      |
\*----------------------------------------------------------------------------*/
void book::read()
{
	table_ptr->clear(); // Clear the transposition table.
	populate_table();   // Populate the transposition table.
}

/*----------------------------------------------------------------------------*\
 |			       populate_tokens()			      |
\*----------------------------------------------------------------------------*/
void book::populate_tokens(istream& stream)
{
	string token;

	while (!stream.eof())
		switch (tokenize(stream, token))
		{
			case TOKEN_SPACE       :                          break;
			case TOKEN_STRING      :                          break;
			case TOKEN_INTEGER     :                          break;
			case TOKEN_PUNCTUATION :                          break;
			case TOKEN_GLYPH       :                          break;
			case TOKEN_SYMBOL      : tokens.push_back(token); break;
			default                :                          break;
		}
}

/*----------------------------------------------------------------------------*\
 |				populate_moves()			      |
\*----------------------------------------------------------------------------*/
void book::populate_moves()
{
	for (list<string>::iterator it = tokens.begin(); it != tokens.end(); it++)
		moves.push_back(board_ptr->san_to_coord(*it));
}

/*----------------------------------------------------------------------------*\
 |				populate_table()			      |
\*----------------------------------------------------------------------------*/
void book::populate_table()
{
	for (list<move_t>::iterator it = moves.begin(); it != moves.end(); it++)
		if (IS_NULL_MOVE(*it))
			while (board_ptr->unmake())
				;
		else
			if (board_ptr->get_num_moves() < num_moves)
			{
				table_ptr->store(board_ptr->get_hash(), MAX_DEPTH, BOOK, *it);
				board_ptr->make(*it);
			}
}

/*----------------------------------------------------------------------------*\
 |				   tokenize()				      |
\*----------------------------------------------------------------------------*/
int book::tokenize(istream& stream, string& token)
{
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
	return TOKEN_UNKNOWN;
}

/*----------------------------------------------------------------------------*\
 |				tokenize_space()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_space(istream& stream, string& token)
{
	token.erase(0, token.length());
	for (int c; isspace(c = stream.peek()); stream.ignore())
		token += c;
	token += '\0';
	return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |			       tokenize_string()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_string(istream& stream, string& token)
{
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
 |			       tokenize_integer()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_integer(istream& stream, string& token)
{
	token.erase(0, token.length());
	for (int c; isdigit(c = stream.peek()); stream.ignore())
		token += c;
	token += '\0';
	return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |			     tokenize_punctuation()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_punctuation(istream& stream, string& token)
{
	token.erase(0, token.length());
	int c = stream.peek();
	if (c == '.' || c == '*' || c == '[' || c == ']' || c == '<' || c == '>')
	{
		token += c;
		stream.ignore();
	}
	token += '\0';
	return token.length() >= 2;
}

/*----------------------------------------------------------------------------*\
 |				tokenize_glyph()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_glyph(istream& stream, string& token)
{
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
 |			       tokenize_symbol()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_symbol(istream& stream, string& token)
{
	token.erase(0, token.length());
	if (isalnum(stream.peek()))
		for (int c = stream.peek(); IS_SYMBOL(c); stream.ignore(), c = stream.peek())
			token += c;
	token += '\0';
	return token.length() >= 2;
}
