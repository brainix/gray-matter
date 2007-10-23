/*----------------------------------------------------------------------------*\
 |	search_bogo.h - BogoSearch move search interface		      |
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

#ifndef SEARCH_BOGO_H
#define SEARCH_BOGO_H

using namespace std;

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "search_base.h"
#include "board_base.h"
#include "table.h"
#include "clock.h"
#include "xboard.h"

// Forward declarations:
class search_base;
class board_base;
class table;
class history;
class chess_clock;
class xboard;

class search_bogo : public search_base
{
public:
	search_bogo(table *t, history *h, chess_clock *c, xboard *x);
	~search_bogo();
private:
	void iterate(int s);
};

#endif
