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
book::book(table *t, char *n, int m)
{
	board_ptr = new board_heuristic();
	table_ptr = t;
	strncpy(name, n, sizeof(name));
	moves = m;
}

/*----------------------------------------------------------------------------*\
 |				     read()				      |
\*----------------------------------------------------------------------------*/
void book::read()
{
	/* Clear the transposition table. */
	table_ptr->clear();
	if (moves == 0)
		return;

	/* Open the file. */
	file.open(name);
	if (file.fail())
	{
		cout << "couldn't find open book: " << name << endl;
		file.close();
		return;
	}

	/* Parse the input. */
	parse();

	/* Close the file. */
	file.close();
}

/*----------------------------------------------------------------------------*\
 |				    parse()				      |
\*----------------------------------------------------------------------------*/
void book::parse()
{
}

/*----------------------------------------------------------------------------*\
 |				   tokenize()				      |
\*----------------------------------------------------------------------------*/
int book::tokenize(string& token)
{
	if (tokenize_space(token))
		return TOKEN_SPACE;
	if (tokenize_string(token))
		return TOKEN_STRING;
	if (tokenize_integer(token))
		return TOKEN_INTEGER;
	if (tokenize_punctuation(token))
		return TOKEN_PUNCTUATION;
	if (tokenize_glyph(token))
		return TOKEN_GLYPH;
	if (tokenize_symbol(token))
		return TOKEN_SYMBOL;
	return TOKEN_UNKNOWN;
}

/*----------------------------------------------------------------------------*\
 |				tokenize_space()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_space(string& token)
{
	token.erase(0, token.length());
	for (int c; isspace(c = file.peek()); file.ignore())
		token += c;
	token += '\0';
	return token.length() >= 1;
}

/*----------------------------------------------------------------------------*\
 |			       tokenize_string()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_string(string& token)
{
	token.erase(0, token.length());
	if (file.peek() == '\"')
		for (token += file.get(); ; )
		{
			int c = file.get();
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
	return token.length() >= 1;
}

/*----------------------------------------------------------------------------*\
 |			       tokenize_integer()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_integer(string& token)
{
	token.erase(0, token.length());
	for (int c; isdigit(c = file.peek()); file.ignore())
		token += c;
	token += '\0';
	return token.length() >= 1;
}

/*----------------------------------------------------------------------------*\
 |			     tokenize_punctuation()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_punctuation(string& token)
{
	token.erase(0, token.length());
	int c = file.peek();
	if (c == '.' || c == '*' || c == '[' || c == ']' || c == '<' || c == '>')
		token += c;
	token += '\0';
	return token.length() >= 1;
}

/*----------------------------------------------------------------------------*\
 |				tokenize_glyph()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_glyph(string& token)
{
	token.erase(0, token.length());
	if (file.peek() == '$')
	{
		token += file.get();
		for (int c; isdigit(c = file.peek()); file.ignore())
			token += c;
	}
	token += '\0';
	return token.length() >= 1;
}

/*----------------------------------------------------------------------------*\
 |			       tokenize_symbol()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_symbol(string& token)
{
	token.erase(0, token.length());
	if (isalnum(file.peek()))
		for (int c = file.peek(); IS_SYMBOL(c); file.ignore(), c = file.peek())
			token += c;
	token += '\0';
	return token.length() >= 1;
}
