/*----------------------------------------------------------------------------*\
 |	bitboard.h - BitBoard interface					      |
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

#ifndef BITBOARD_H
#define BITBOARD_H

using namespace std;

// C++ stuff:
#include <list>

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

class bitboard
{
public:
	bitboard(int a, uint64_t d);

	int bit_idx(int x, int y);
	uint64_t bit_msk(int x, int y);
	bool bit_get(int x, int y);
	uint64_t bit_clr(int x, int y);
	uint64_t bit_set(int x, int y);
	uint64_t bit_mov(int x1, int y1, int x2, int y2);

	int row_num(int x, int y);
	int row_loc(int x, int y);
	int row_idx(int n);
	uint64_t row_msk(int n);
	uint8_t row_get(int n);
	uint64_t row_clr(int n);
	uint64_t row_set(int n, uint8_t r);

protected:
	int angle;
	uint64_t data;
};

#endif
