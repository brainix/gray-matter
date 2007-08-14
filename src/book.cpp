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
	int index = 0;

	if ((buffer[index++] = file.get()) == EOF)
	{
		buffer[index - 1] = '\0';
		return UNKNOWN;
	}

	if (isspace(buffer[index - 1]))
	{
		for (int c; isspace(c = file.peek()); file.ignore())
			buffer[index++] = c;
		buffer[index++] = '\0';
		return SPACE;
	}

	if (buffer[index - 1] == '\"')
		while (true)
			if ((buffer[index++] = file.get()) == EOF)
			{
				buffer[index - 1] = '\0';
				return UNKNOWN;
			}
			else if (buffer[index - 1] == '\"')
			{
				buffer[index++] = '\0';
				return TOK_STR;
			}

	if (isdigit(buffer[index - 1]))
	{
		for (int c; isdigit(c = file.peek()); file.ignore())
			buffer[index++] = c;
		buffer[index++] = '\0';
		return TOK_INT;
	}

	if (buffer[index - 1] == '.' || buffer[index - 1] == '*' ||
	    buffer[index - 1] == '[' || buffer[index - 1] == ']' ||
	    buffer[index - 1] == '<' || buffer[index - 1] == '>')
	{
		buffer[index++] = '\0';
		return TOK_PUNC;
	}

	if (buffer[index - 1] == '$')
	{
		for (int c; isdigit(c = file.peek()); file.ignore())
			buffer[index++] = c;
		buffer[index++] = '\0';
		return TOK_NAG;
	}

	if (isalnum(buffer[index - 1]))
	{
		for (int c = file.peek(); IS_SYM(c); file.ignore(), c = file.peek())
			buffer[index++] = c;
		buffer[index++] = '\0';
		return TOK_SYM;
	}

	return UNKNOWN;
}
