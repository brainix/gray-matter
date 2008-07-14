/*----------------------------------------------------------------------------*\
 |  bitboard.h - BitBoard interface and implementation                        |
 |                                                                            |
 |  Copyright © 2005-2008, The Gray Matter Team, original authors.            |
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

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

/// A BitBoard is a brilliant data structure based on this observation: there
/// are 64 bits in a uint64_t integer, and there are 64 squares on a chess
/// board.  Do you see where I'm going with this?  A BitBoard is an unsigned 64-
/// bit integer in which every bit corresponds to a square.
///
/// A single BitBoard can't represent the entire state of the board.  A single
/// bit can only hold a value of 0 or 1 - enough to describe the absence or
/// presence of a piece on a square, but not enough to describe the piece's
/// color or shape.  Therefore, we need 12 BitBoards to represent the entire
/// state of the board:
///
///		· white pawns		· black pawns
///		· white knights		· black knights
///		· white bishops		· black bishops
///		· white rooks		· black rooks
///		· white queens		· black queens
///		· white kings		· black kings
typedef uint64_t bitboard_t;

/// A BitRow is an unsigned 8-bit integer which represents up to 8 adjacent
/// squares: a row in a 0° BitBoard, a column in a 90° BitBoard, or a diagonal
/// in a 45° BitBoard.
typedef uint8_t bitrow_t;

// These macros manipulate bits in BitBoards.
#define BIT_IDX(x, y)               ((y) << 3 | (x))
#define BIT_MSK(x, y)               (1ULL << BIT_IDX(x, y))
#define BIT_GET(b, x, y)            ((b) >> BIT_IDX(x, y) & 1)
#define BIT_CLR(b, x, y)            ((b) &= ~BIT_MSK(x, y))
#define BIT_SET(b, x, y)            ((b) |= BIT_MSK(x, y))
#define BIT_MOV(b, x1, y1, x2, y2)  ((b) ^= BIT_MSK(x1, y1) | BIT_MSK(x2, y2))

// These macros manipulate rows in 0° rotated BitBoards and columns in 90°
// rotated BitBoards.
#define ROW_NUM(x, y, a)            ((a) == ZERO ? (y) : (x))
#define ROW_LOC(x, y, a)            ((a) == ZERO ? (x) : 7 - (y))
#define ROW_IDX(n)                  (BIT_IDX(0, n))
#define ROW_MSK(n)                  (0xFFULL << ROW_IDX(n))
#define ROW_GET(b, n)               ((b) >> ROW_IDX(n) & 0xFF)
#define ROW_CLR(b, n)               ((b) &= ~ROW_MSK(n))
#define ROW_SET(b, n, r)            ((b) |= (bitboard_t) (r) << ROW_IDX(n))

// These macros manipulate columns in 0° rotated BitBoards and rows in 90°
// rotated BitBoards.
#define COL_IDX(n)                  (BIT_IDX(n, 0))
#define COL_MSK(n)                  (0x0101010101010101ULL << COL_IDX(n))
#define COL_CLR(b, n)               ((b) &= ~COL_MSK(n))

// These macros manipulate adjacent bits in 45° rotated BitBoards, which
// correspond to diagonals in 0° and 90° rotated BitBoards.
#define DIAG_NUM(x, y, a)           ((a) == L45 ? (x) + (y) : 7 - (x) + (y))
#define DIAG_LOC(x, y, a)           (BIT_IDX(coord[MAP][a][x][y][X], coord[MAP][a][x][y][Y]) - diag_index[DIAG_NUM(x, y, a)])
#define DIAG_LEN(n)                 (8 - abs(7 - (n)))
#define DIAG_IDX(n)                 (diag_index[n])
#define DIAG_MSK(n)                 ((bitboard_t) diag_mask[n] << diag_index[n])
#define DIAG_GET(b, n)              ((b) >> diag_index[n] & diag_mask[n])
#define DIAG_CLR(b, n)              ((b) &= ~DIAG_MSK(n))
#define DIAG_SET(b, n, d)           ((b) |= (bitboard_t) (d) << diag_index[n])

// This macro finds the first set bit in a BitBoard.
#define FST(b)                      (find_64(b) - 1)

// Convenient BitBoards:
#define SQUARES_CENTER          0x0000001818000000ULL // 4 center squares.
#define SQUARES_EXPANDED_CENTER 0x00003C3C3C3C0000ULL // 16 center squares.
#define SQUARES_PRINCIPAL_DIAG  0x8142241818244281ULL // 16 principal diagonal squares.
#define SQUARES_WHITE_SIDE      0x00000000FFFFFFFFULL // 32 white side squares.
#define SQUARES_BLACK_SIDE      0xFFFFFFFF00000000ULL // 32 black side squares.
#define SQUARES_WHITE           0x55AA55AA55AA55AAULL // 32 white squares.
#define SQUARES_BLACK           0xAA55AA55AA55AA55ULL // 32 black squares.
#define SQUARES_QUEEN_SIDE      0x0F0F0F0F0F0F0F0FULL // 32 queen side squares.
#define SQUARES_KING_SIDE       0xF0F0F0F0F0F0F0F0ULL // 32 king side squares.
#define SQUARES_CORNER          0x8100000000000081ULL // 4 corner squares.

#endif
