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

	bool operator<(uint64_t d) const;
	bool operator<=(uint64_t d) const;
	bool operator==(uint64_t d) const;
	bool operator>=(uint64_t d) const;
	bool operator>(uint64_t d) const;
	bool operator!=(uint64_t d) const;

	bool operator<(const bitboard &that) const;
	bool operator<=(const bitboard &that) const;
	bool operator==(const bitboard &that) const;
	bool operator>=(const bitboard &that) const;
	bool operator>(const bitboard &that) const;
	bool operator!=(const bitboard &that) const;

	int bit_idx(int x, int y) const;
	uint64_t bit_msk(int x, int y) const;
	bool bit_get(int x, int y) const;
	uint64_t bit_clr(int x, int y);
	uint64_t bit_set(int x, int y);
	uint64_t bit_mov(int x1, int y1, int x2, int y2);

	int row_num(int x, int y) const;
	int row_loc(int x, int y) const;
	int row_idx(int n) const;
	uint64_t row_msk(int n) const;
	uint8_t row_get(int n) const;
	uint64_t row_clr(int n);
	uint64_t row_set(int n, uint8_t r);

	int col_idx(n) const;
	uint64_t col_msk(n) const;
	uint64_t col_clr(n);

	int diag_num(int x, int y) const;
	int diag_loc(int x, int y) const;
	int diag_len(int n) const;
	int diag_idx(int n) const;
	uint64_t diag_msk(int n) const;
	uint8_t diag_get(int n) const;
	uint64_t diag_clr(int n);
	uint64_t diag_set(int n, uint8_t d);

	int fst() const;

private:
	int angle;
	uint64_t data;

	// This array maps coordinates between rotated BitBoards:
	static const int coord[MAPS][ANGLES][8][8][COORDS] =
	{
		// From 0° to 45° left:
		{{{{0,0},{1,0},{3,0},{6,0},{2,1},{7,1},{5,2},{4,3}},
		  {{2,0},{4,0},{7,0},{3,1},{0,2},{6,2},{5,3},{4,4}},
		  {{5,0},{0,1},{4,1},{1,2},{7,2},{6,3},{5,4},{3,5}},
		  {{1,1},{5,1},{2,2},{0,3},{7,3},{6,4},{4,5},{1,6}},
		  {{6,1},{3,2},{1,3},{0,4},{7,4},{5,5},{2,6},{6,6}},
		  {{4,2},{2,3},{1,4},{0,5},{6,5},{3,6},{7,6},{2,7}},
		  {{3,3},{2,4},{1,5},{7,5},{4,6},{0,7},{3,7},{5,7}},
		  {{3,4},{2,5},{0,6},{5,6},{1,7},{4,7},{6,7},{7,7}}},

		// From 0° to 0°:
		 {{{0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7}},
		  {{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7}},
		  {{2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7}},
		  {{3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7}},
		  {{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{4,7}},
		  {{5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{5,7}},
		  {{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{6,7}},
		  {{7,0},{7,1},{7,2},{7,3},{7,4},{7,5},{7,6},{7,7}}},

		// From 0° to 45° right:
		 {{{4,3},{4,4},{3,5},{1,6},{6,6},{2,7},{5,7},{7,7}},
		  {{5,2},{5,3},{5,4},{4,5},{2,6},{7,6},{3,7},{6,7}},
		  {{7,1},{6,2},{6,3},{6,4},{5,5},{3,6},{0,7},{4,7}},
		  {{2,1},{0,2},{7,2},{7,3},{7,4},{6,5},{4,6},{1,7}},
		  {{6,0},{3,1},{1,2},{0,3},{0,4},{0,5},{7,5},{5,6}},
		  {{3,0},{7,0},{4,1},{2,2},{1,3},{1,4},{1,5},{0,6}},
		  {{1,0},{4,0},{0,1},{5,1},{3,2},{2,3},{2,4},{2,5}},
		  {{0,0},{2,0},{5,0},{1,1},{6,1},{4,2},{3,3},{3,4}}},

		// From 0° to 90° right:
		 {{{7,0},{6,0},{5,0},{4,0},{3,0},{2,0},{1,0},{0,0}},
		  {{7,1},{6,1},{5,1},{4,1},{3,1},{2,1},{1,1},{0,1}},
		  {{7,2},{6,2},{5,2},{4,2},{3,2},{2,2},{1,2},{0,2}},
		  {{7,3},{6,3},{5,3},{4,3},{3,3},{2,3},{1,3},{0,3}},
		  {{7,4},{6,4},{5,4},{4,4},{3,4},{2,4},{1,4},{0,4}},
		  {{7,5},{6,5},{5,5},{4,5},{3,5},{2,5},{1,5},{0,5}},
		  {{7,6},{6,6},{5,6},{4,6},{3,6},{2,6},{1,6},{0,6}},
		  {{7,7},{6,7},{5,7},{4,7},{3,7},{2,7},{1,7},{0,7}}}},

		// From 45° left to 0°:
		{{{{0,0},{2,1},{1,4},{3,3},{4,3},{5,3},{7,2},{6,5}},
		  {{0,1},{3,0},{2,3},{4,2},{5,2},{6,2},{3,7},{7,4}},
		  {{1,0},{0,4},{3,2},{5,1},{6,1},{7,1},{4,6},{5,7}},
		  {{0,2},{1,3},{4,1},{6,0},{7,0},{2,7},{5,5},{6,6}},
		  {{1,1},{2,2},{5,0},{0,7},{1,7},{3,6},{6,4},{7,5}},
		  {{2,0},{3,1},{0,6},{1,6},{2,6},{4,5},{7,3},{6,7}},
		  {{0,3},{4,0},{1,5},{2,5},{3,5},{5,4},{4,7},{7,6}},
		  {{1,2},{0,5},{2,4},{3,4},{4,4},{6,3},{5,6},{7,7}}},

		// From 0° to 0°:
		 {{{0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7}},
		  {{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7}},
		  {{2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7}},
		  {{3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7}},
		  {{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{4,7}},
		  {{5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{5,7}},
		  {{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{6,7}},
		  {{7,0},{7,1},{7,2},{7,3},{7,4},{7,5},{7,6},{7,7}}},

		// From 45° right to 0°:
		 {{{7,0},{6,2},{3,1},{4,3},{4,4},{4,5},{5,7},{2,6}},
		  {{6,0},{7,3},{4,2},{5,4},{5,5},{5,6},{0,3},{3,7}},
		  {{7,1},{3,0},{5,3},{6,5},{6,6},{6,7},{1,4},{0,5}},
		  {{5,0},{4,1},{6,4},{7,6},{7,7},{0,2},{2,5},{1,6}},
		  {{6,1},{5,2},{7,5},{0,0},{0,1},{1,3},{3,6},{2,7}},
		  {{7,2},{6,3},{1,0},{1,1},{1,2},{2,4},{4,7},{0,6}},
		  {{4,0},{7,4},{2,1},{2,2},{2,3},{3,5},{0,4},{1,7}},
		  {{5,1},{2,0},{3,2},{3,3},{3,4},{4,6},{1,5},{0,7}}},

		// From 90° right to 0°:
		 {{{0,7},{1,7},{2,7},{3,7},{4,7},{5,7},{6,7},{7,7}},
		  {{0,6},{1,6},{2,6},{3,6},{4,6},{5,6},{6,6},{7,6}},
		  {{0,5},{1,5},{2,5},{3,5},{4,5},{5,5},{6,5},{7,5}},
		  {{0,4},{1,4},{2,4},{3,4},{4,4},{5,4},{6,4},{7,4}},
		  {{0,3},{1,3},{2,3},{3,3},{4,3},{5,3},{6,3},{7,3}},
		  {{0,2},{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{7,2}},
		  {{0,1},{1,1},{2,1},{3,1},{4,1},{5,1},{6,1},{7,1}},
		  {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0}}}}
	};

	static const int     diag_idx_array[15] = {   0,    1,    3,    6,   10,   15,   21,   28,   36,   43,   49,   54,   58,   61,   63};
	static const uint8_t diag_msk_array[15] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
};

#endif
