/*----------------------------------------------------------------------------*\
 |	search_mtdf.h - MTD(f) move search interface			      |
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

#ifndef SEARCH_MTDF_H
#define SEARCH_MTDF_H

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

/// MTD(f) move search.
class search_mtdf : public search_base
{
public:
	search_mtdf(table* t, history* h, chess_clock* c, xboard* x);
	~search_mtdf();
	search_mtdf& operator=(const search_mtdf& that);
private:
	void iterate(int s);
	move_t mtdf(int depth, value_t guess = 0);
	move_t minimax(int depth, int shallowness = 0, value_t alpha = -INFINITY, value_t beta = +INFINITY, bool try_null_move = true);
	value_t quiesce(int shallowness, value_t alpha, value_t beta);
};

#endif
