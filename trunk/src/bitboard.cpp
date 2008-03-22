/*----------------------------------------------------------------------------*\
 |	bitboard.cpp - BitBoard implementation				      |
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

#include "gray.h"
#include "bitboard.h"

/*----------------------------------------------------------------------------*\
 |				   bitboard()				      |
\*----------------------------------------------------------------------------*/
bitboard::bitboard(int a, uint64_t d)
{
	angle = a;
	data = d;
}

/*----------------------------------------------------------------------------*\
 |				   bit_idx()				      |
\*----------------------------------------------------------------------------*/
int bitboard::bit_idx(int x, int y)
{
	return y << 3 | x;
}

/*----------------------------------------------------------------------------*\
 |				   bit_msk()				      |
\*----------------------------------------------------------------------------*/
uint64_t bitboard::bit_msk(int x, int y)
{
	return 1ULL << bit_idx(x, y);
}

/*----------------------------------------------------------------------------*\
 |				   bit_get()				      |
\*----------------------------------------------------------------------------*/
bool bitboard::bit_get(int x, int y)
{
	return data >> bit_idx(x, y) & 1;
}

/*----------------------------------------------------------------------------*\
 |				   bit_clr()				      |
\*----------------------------------------------------------------------------*/
uint64_t bitboard::bit_clr(int x, int y)
{
	return data &= ~bit_msk(x, y);
}

/*----------------------------------------------------------------------------*\
 |				   bit_set()				      |
\*----------------------------------------------------------------------------*/
uint64_t bitboard::bit_set(int x, int y)
{
	return data |= bit_msk(x, y);
}

/*----------------------------------------------------------------------------*\
 |				   bit_mov()				      |
\*----------------------------------------------------------------------------*/
uint64_t bitboard::bit_mov(int x1, int y1, int x2, int y2)
{
	return data ^= bit_msk(x1, y1) | bit_msk(x2, y2);
}
