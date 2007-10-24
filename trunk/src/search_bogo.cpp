/*----------------------------------------------------------------------------*\
 |	search_bogo.cpp - BogoSearch move search implementation		      |
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
#include "search_bogo.h"

/*----------------------------------------------------------------------------*\
 |				 search_bogo()				      |
\*----------------------------------------------------------------------------*/
search_bogo::search_bogo(table *t, history *h, chess_clock *c, xboard *x) :
	search_base(t, h, c, x)
{

// Constructor.

}

/*----------------------------------------------------------------------------*\
 |				 ~search_bogo()				      |
\*----------------------------------------------------------------------------*/
search_bogo::~search_bogo()
{

// Destructor.

}

/*----------------------------------------------------------------------------*\
 |				   iterate()				      |
\*----------------------------------------------------------------------------*/
void search_bogo::iterate(int s)
{
}
