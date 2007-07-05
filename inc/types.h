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
 | A BitBoard is a brilliant data structure based on this observation: there
 | are 64 bits in an unsigned long long integer, there are 64 squares on a chess
 | board.  See where I'm going?  A BitBoard is an unsigned 64-bit integer in
 | which every bit corresponds to a square.
 |
 | A single BitBoard can't represent the entire state of the board.  A single
 | bit can only hold a value of 0 or 1 - enough to describe the absence or
 | presence of a piece on a square, but not enough to describe the piece's color
 | or type.  Therefore, we need 12 BitBoards to represent the entire state of
 | the board:
 |
 |		· white pawns		· black pawns
 |		· white knights		· black knights
 |		· white bishops		· black bishops
 |		· white rooks		· black rooks
 |		· white queens		· black queens
 |		· white kings		· black kings
 |
 | Gray Matter introduces a new data structure, a BitRow.  A BitRow is an
 | unsigned 8-bit integer which represents up to 8 adjacent squares: a row in a
 | 0° bitboard, a column in a 90° bitboard, or a diagonal in a 45° bitboard.
 */
typedef uint64_t bitboard_t;
typedef uint8_t bitrow_t;

/*
 | This structure describes the entire state of the board.  It contains the
 | aforementioned 12 BitBoards along with castling statuses, en passant
 | vulnerability, and the color on move.
 */
typedef struct state
{
	bitboard_t piece[COLORS][SHAPES]; // Aforementioned 12 BitBoards.
	int castle[COLORS][SIDES];        // Castling statuses.
	int en_passant;                   // En passant vulnerability.
	bool whose;                       // Color on move.
} state_t;

/*
 | This structure describes a move.  It contains the from and to coordinates,
 | the pawn promotion information, and the MiniMax score.  We use a bitfield to
 | tightly pack this information into 32 bits because some of our methods return
 | this structure (rather than a pointer to this structure or other similar
 | ugliness).
 */
typedef struct move
{
	unsigned old_x   :  3; // From x coordinate.              3 bits
	unsigned old_y   :  3; // From y coordinate.           +  3 bits
	unsigned new_x   :  3; // To x coordinate.             +  3 bits
	unsigned new_y   :  3; // To y coordinate.             +  3 bits
	unsigned promo   :  3; // Pawn promotion information.  +  3 bits
	unsigned padding :  1; // Unused.                      +  1 bit
	  signed value   : 16; // MiniMax score.               + 16 bits
	                       //                              = 32 bits

	/* Overloaded equality test operator. */
	bool operator==(const struct move that) const
	{
		return this->old_x == that.old_x && this->old_y == that.old_y &&
		       this->new_x == that.new_x && this->new_y == that.new_y &&
		       this->promo == that.promo;
	};

	/* Overloaded inequality test operator. */
	bool operator!=(const struct move that) const
	{
		return this->old_x != that.old_x || this->old_y != that.old_y ||
		       this->new_x != that.new_x || this->new_y != that.new_y ||
		       this->promo != that.promo;
	};

	/* Overloaded assignment operator. */
	struct move& operator=(const struct move& that)
	{
		old_x = that.old_x;
		old_y = that.old_y;
		new_x = that.new_x;
		new_y = that.new_y;
		promo = that.promo;
		value = that.value;
		return *this;
	};
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
	bitboard_t hash;               // Zobrist hash key.              64 bits
	uint8_t depth[ENTRY_TYPES];    // Depth of our search.        +  16 bits
	move_t move;                   // Best move and upper bound.  +  32 bits
	uint16_t lower;                // Lower bound.                +  16 bits
} __attribute__((packed)) xpos_slot_t; //                             = 128 bits

/*
 | This structure describes a pawn table slot.
 */
typedef struct pawn_slot
{
	bitboard_t hash;               // Zobrist hash key.    64 bits
	int16_t value;                 // Score.             + 16 bits
} __attribute__((packed)) pawn_slot_t; //                    = 80 bits

#endif
