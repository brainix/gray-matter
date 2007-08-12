/*----------------------------------------------------------------------------*\
 |	board_heuristic.h - heuristic evaluation interface		      |
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

#ifndef BOARD_HEURISTIC_H
#define BOARD_HEURISTIC_H

using namespace std;

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"
#include "thread.h"

/* Extra Gray Matter stuff: */
#include "board_base.h"
#include "table.h"

/* Forward declarations: */
class pawn_table;

class board_heuristic : public board_base
{
public:
	board_heuristic();
	~board_heuristic();
	board_heuristic& operator=(const board_heuristic& that);
#if DEBUG
	void print_stats() const;
#endif
	int evaluate() const;
private:
	int evaluate_material() const;
	int evaluate_tempo() const;
	int evaluate_pawn() const;
	int evaluate_knight() const;
	int evaluate_bishop() const;
	int evaluate_rook() const;
	int evaluate_queen() const;
	int evaluate_king() const;
};

#endif
