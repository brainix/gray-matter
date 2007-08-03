/*----------------------------------------------------------------------------*\
 |	board_heuristic.cpp - board representation implementation	      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 2 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
 */

#include "gray.h"
#include "board_heuristic.h"

/*----------------------------------------------------------------------------*\
 |			       board_heuristic()			      |
\*----------------------------------------------------------------------------*/
board_heuristic::board_heuristic() : board_base()
{

/* Constructor. */

}

/*----------------------------------------------------------------------------*\
 |			       ~board_heuristic()			      |
\*----------------------------------------------------------------------------*/
board_heuristic::~board_heuristic()
{

/* Destructor. */

}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
board_heuristic& board_heuristic::operator=(const board_heuristic& that)
{

/* Overloaded assignment operator. */

	if (this == &that)
		return *this;
	board_base::operator=(that);
	return *this;
}
