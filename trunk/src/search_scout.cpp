/*----------------------------------------------------------------------------*\
 |	search_scout.cpp - NegaScout move search implementation		      |
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
#include "search_scout.h"

/*----------------------------------------------------------------------------*\
 |				 search_scout()				      |
\*----------------------------------------------------------------------------*/
search_scout::search_scout(table *t, history *h, chess_clock *c, xboard *x) :
	search_base(t, h, c, x)
{

// Constructor.

}

/*----------------------------------------------------------------------------*\
 |				~search_scout()				      |
\*----------------------------------------------------------------------------*/
search_scout::~search_scout()
{

// Destructor.

}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
search_scout& search_scout::operator=(const search_scout& that)
{

// Overloaded assignment operator.

	if (this != &that)
		search_base::operator=(that);
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				   iterate()				      |
\*----------------------------------------------------------------------------*/
void search_scout::iterate(int s)
{

// Perform iterative deepening.  This method handles analyzing (thinking
// indefinitely), thinking (on our own time), and pondering (on our opponent's
// time) since they're so similar.

}
