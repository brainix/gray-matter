/*----------------------------------------------------------------------------*\
 |	search_mtdf.h - move search interface				      |
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

#ifndef SEARCH_MTDF_H
#define SEARCH_MTDF_H

using namespace std;

// C++ stuff:
#include <algorithm>
#include <list>

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

class search_mtdf : public search_base
{
public:
	search_mtdf(table *t, history *h, chess_clock *c, xboard *x);
	~search_mtdf();
	search_mtdf& operator=(const search_mtdf& that);
private:
	void iterate(int s);
	move_t mtdf(int depth, int guess);
	move_t minimax(int depth, int shallowness, int alpha, int beta);
};

#endif
