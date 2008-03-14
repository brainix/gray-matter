/*----------------------------------------------------------------------------*\
 |	search_scout.h - NegaScout move search interface		      |
 |									      |
 |	Copyright © 2005-2008, The Gray Matter Team, original authors.	      |
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

#ifndef SEARCH_SCOUT_H
#define SEARCH_SCOUT_H

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

/// NegaScout move search.
class search_scout : public search_base
{
public:
	search_scout(table *t, history *h, chess_clock *c, xboard *x);
	~search_scout();
	search_scout& operator=(const search_scout& that);
private:
	void iterate(int s);
	move_t scout(int depth, int shallowness = 0, value_t alpha = -INFINITY, value_t beta = +INFINITY, bool try_null_move = true);
};

#endif
