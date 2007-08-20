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

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "board_base.h"
#include "table.h"

// Convenient BitBoards:
#define SQUARES_CENTER		0x0000001818000000ULL //  4 center squares.
#define SQUARES_EXPANDED_CENTER	0x00003C3C3C3C0000ULL // 16 center squares.
#define SQUARES_PRINCIPAL_DIAG	0x8142241818244281ULL // 16 principal diagonal squares.
#define SQUARES_WHITE_SIDE	0x00000000FFFFFFFFULL // 32 white side squares.
#define SQUARES_BLACK_SIDE	0xFFFFFFFF00000000ULL // 32 black side squares.
#define SQUARES_WHITE		0x55AA55AA55AA55AAULL // 32 white squares.
#define SQUARES_BLACK		0xAA55AA55AA55AA55ULL // 32 black squares.
#define SQUARES_QUEEN_SIDE	0x0F0F0F0F0F0F0F0FULL // 32 queen side squares.
#define SQUARES_KING_SIDE	0xF0F0F0F0F0F0F0F0ULL // 32 king side squares.
#define SQUARES_CORNER		0x8100000000000081ULL //  4 corner squares.

// Forward declarations:
class pawn_table;

class board_heuristic : public board_base
{
public:
	board_heuristic();
	~board_heuristic();
	board_heuristic& operator=(const board_heuristic& that);
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
