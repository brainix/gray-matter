/*----------------------------------------------------------------------------*\
 |	types.h - data types						      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 2 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include "config.h"

/*
 | A bitboard is a brilliant data structure based on this observation: there
 | are 64 bits in an unsigned long long integer, there are 64 squares on a chess
 | board.  See where I'm going?  A bitboard is an unsigned 64-bit integer in
 | which every bit corresponds to a square.
 |
 | A single bitboard can't represent the entire state of the board.  A single
 | bit can only hold a value of 0 or 1 - enough to describe the absence or
 | presence of a piece on a square, but not enough to describe a piece's color
 | or type.  We therefore need 12 bitboards to represent the entire state of the
 | board:
 |
 |		· white pawns		· black pawns
 |		· white knights		· black knights
 |		· white bishops		· black bishops
 |		· white rooks		· black rooks
 |		· white queens		· black queens
 |		· white kings		· black kings
 |
 | Gray Matter introduces a new data structure, a bitrow.  A bitrow is an
 | unsigned 8-bit integer which represents up to 8 adjacent squares: a row in a
 | 0° bitboard, a column in a 90° bitboard, or a diagonal in a 45° bitboard.
 */
typedef uint64_t bitboard_t;
typedef uint8_t bitrow_t;

/*
 | This structure describes the entire state of the board.  It contains the
 | aforementioned 12 bitboards along with castling statuses, en passant
 | vulnerability, and the color on move.
 */
typedef struct state
{
	bitboard_t piece[COLORS][SHAPES]; // Aforementioned 12 bitboards.
	int castle[COLORS][SIDES];        // Castling statuses.
	int en_passant;                   // En passant vulnerability.
	bool whose;                       // Color on move.
} state_t;

/*
 | This structure describes a move.  It contains the from and to coordinates,
 | pawn promotion information, and the negamax score.
 */
typedef struct move
{
	unsigned old_x   :  3; // From x coordinate.
	unsigned old_y   :  3; // From y coordinate.
	unsigned new_x   :  3; // To x coordinate.
	unsigned new_y   :  3; // To y coordinate.
	unsigned promo   :  3; // Pawn promotion information.
	unsigned padding :  1; // Unused.
	  signed value   : 16; // Negamax score.
} __attribute__((packed)) move_t;

/*
 |
 */
#define IS_NULL_MOVE(m)		(!(m).promo && !(m).new_y && !(m).new_x && !(m).old_y && !(m).old_x)
#define SET_NULL_MOVE(m)	((m).promo = (m).new_y = (m).new_x = (m).old_y = (m).old_x = 0)

/*
 | This structure describes a transposition table slot.
 */
typedef struct xpos_slot
{
	bitboard_t hash; // Zobrist hash key.
	uint32_t depth;  // Depth.
	move_t move;     // Best move.
} __attribute__((packed)) xpos_slot_t;

/*
 | This structure describes a pawn table slot.
 */
typedef struct pawn_slot
{
	bitboard_t hash; // Zobrist hash key.
	int value;       //
} __attribute__((packed)) pawn_slot_t;

#endif
