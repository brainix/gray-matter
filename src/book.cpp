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
bool book::tokenize(char *buffer)
{
	int index = 0;

	if ((buffer[index++] = cin.get()) == EOF)
		goto end_of_file;

	if (buffer[index - 1] == '\"')
		while (true)
			if ((buffer[index++] = cin.get()) == EOF)
				goto end_of_file;
			else if (buffer[index - 1] == '\"')
				goto end_of_token;

	if (buffer[index - 1] == '.' || buffer[index - 1] == '*' ||
	    buffer[index - 1] == '[' || buffer[index - 1] == ']' ||
	    buffer[index - 1] == '<' || buffer[index - 1] == '>')
		goto end_of_token;

end_of_token:
	buffer[index++] = '\0';
	return true;

end_of_file:
	buffer[index - 1] = '\0';
	return false;
}
