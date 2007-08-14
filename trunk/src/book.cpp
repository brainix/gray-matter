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
int book::tokenize(char *buffer)
{
	if ((buffer[0] = file.get()) == EOF)
		buffer[0] = '\0';
	if (tokenize_space(buffer))
		return TOKEN_SPACE;
	if (tokenize_string(buffer))
		return TOKEN_STRING;
	if (tokenize_integer(buffer))
		return TOKEN_INTEGER;
	if (tokenize_punctuation(buffer))
		return TOKEN_PUNCTUATION;
	if (tokenize_glyph(buffer))
		return TOKEN_GLYPH;
	if (tokenize_symbol(buffer))
		return TOKEN_SYMBOL;
	return TOKEN_UNKNOWN;
}

/*----------------------------------------------------------------------------*\
 |				tokenize_space()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_space(char *buffer)
{
	int index = 1;
	if (!isspace(buffer[index - 1]))
		return false;
	for (int c; isspace(c = file.peek()); file.ignore())
		buffer[index++] = c;
	buffer[index++] = '\0';
	return true;
}

/*----------------------------------------------------------------------------*\
 |			       tokenize_string()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_string(char *buffer)
{
	int index = 1;
	if (buffer[index - 1] != '\"')
		return false;
	while (true)
		if ((buffer[index++] = file.get()) == EOF)
		{
			buffer[index - 1] = '\0';
			return false;
		}
		else if (buffer[index - 1] == '\"')
			break;
	buffer[index++] = '\0';
	return true;
}

/*----------------------------------------------------------------------------*\
 |			       tokenize_integer()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_integer(char *buffer)
{
	int index = 1;
	if (!isdigit(buffer[index - 1]))
		return false;
	for (int c; isdigit(c = file.peek()); file.ignore())
		buffer[index++] = c;
	buffer[index++] = '\0';
	return true;
}

/*----------------------------------------------------------------------------*\
 |			     tokenize_punctuation()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_punctuation(char *buffer)
{
	if (buffer[0] != '.' && buffer[0] != '*' &&
	    buffer[0] != '[' && buffer[0] != ']' &&
	    buffer[0] != '<' && buffer[0] != '>')
		return false;
	buffer[1] = '\0';
	return true;
}

/*----------------------------------------------------------------------------*\
 |				tokenize_glyph()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_glyph(char *buffer)
{
	int index = 1;
	if (buffer[index - 1] != '$')
		return false;
	for (int c; isdigit(c = file.peek()); file.ignore())
		buffer[index++] = c;
	buffer[index++] = '\0';
	return true;
}

/*----------------------------------------------------------------------------*\
 |			       tokenize_symbol()			      |
\*----------------------------------------------------------------------------*/
bool book::tokenize_symbol(char *buffer)
{
	int index = 1;
	if (!isalnum(buffer[index - 1]))
		return false;
	for (int c = file.peek(); IS_SYMBOL(c); file.ignore(), c = file.peek())
		buffer[index++] = c;
	buffer[index++] = '\0';
	return true;
}
