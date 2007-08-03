/*----------------------------------------------------------------------------*\
 |	board_base.h - board representation interface			      |
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

#ifndef BOARD_BASE_H
#define BOARD_BASE_H

using namespace std;

/* C++ stuff: */
#include <list>

/* Default Gray Matter stuff: */
#include "config.h"
#include "types.h"
#include "thread.h"

/* These macros represent the colors on and off move. */
#define ON_MOVE			(state.whose)
#define OFF_MOVE		(!state.whose)

/* These macros manipulate bits in BitBoards. */
#define BIT_IDX(x, y)		((y) * 8 + (x))
#define BIT_MSK(x, y)		(1ULL << BIT_IDX(x, y))
#define BIT_GET(b, x, y)	((b) >> BIT_IDX(x, y) & 1)
#define BIT_CLR(b, x, y)	((b) &= ~BIT_MSK(x, y))
#define BIT_SET(b, x, y)	((b) |= BIT_MSK(x, y))
#define BIT_MOV(b, x1, y1, x2, y2) \
				((b) ^= BIT_MSK(x1, y1) | BIT_MSK(x2, y2))

/* These macros manipulate rows in 0° rotated BitBoards and columns in 90°
 * rotated BitBoards. */
#define ROW_NUM(x, y, a)	((a) == ZERO ? (y) : (x))
#define ROW_LOC(x, y, a)	((a) == ZERO ? (x) : 7 - (y))
#define ROW_IDX(n)		(BIT_IDX(0, n))
#define ROW_MSK(n)		(0xFFULL << ROW_IDX(n))
#define ROW_GET(b, n)		((b) >> ROW_IDX(n) & 0xFF)
#define ROW_CLR(b, n)		((b) &= ~ROW_MSK(n))
#define ROW_SET(b, n, r)	((b) |= (bitboard_t) (r) << ROW_IDX(n))

/* These macros manipulate columns in 0° rotated BitBoards and rows in 90°
 * rotated BitBoards. */
#define COL_IDX(n)		(BIT_IDX(n, 0))
#define COL_MSK(n)		(0x0101010101010101ULL << COL_IDX(n))
#define COL_CLR(b, n)		((b) &= ~COL_MSK(n))

/* These macros manipulate adjacent bits in 45° rotated BitBoards, which
 * correspond to diagonals in 0° and 90° rotated BitBoards. */
#define DIAG_NUM(x, y, a)	((a) == L45 ? (x) + (y) : 7 - (x) + (y))
#define DIAG_LOC(x, y, a)	(BIT_IDX(coord[MAP][a][x][y][X], coord[MAP][a][x][y][Y]) - diag_index[DIAG_NUM(x, y, a)])
#define DIAG_LEN(n)		(8 - abs(7 - (n)))
#define DIAG_IDX(n)		(diag_index[n])
#define DIAG_MSK(n)		((bitboard_t) diag_mask[n] << diag_index[n])
#define DIAG_GET(b, n)		((b) >> diag_index[n] & diag_mask[n])
#define DIAG_CLR(b, n)		((b) &= ~DIAG_MSK(n))
#define DIAG_SET(b, n, d)	((b) |= (bitboard_t) (d) << diag_index[n])

/* This macro represents a BitBoard which contains all of a color's pieces. */
#define ALL(s, c)		((s).piece[c][PAWN] | (s).piece[c][KNIGHT] | (s).piece[c][BISHOP] | (s).piece[c][ROOK] | (s).piece[c][QUEEN] | (s).piece[c][KING])

/* This macro finds the first set bit in a BitBoard. */
#define FST(b)			(find_64(b) - 1)

class board_base
{
public:
	/* These methods set information. */
	board_base();
	virtual ~board_base();
	virtual board_base& operator=(const board_base& that);
	virtual void set_board();
	virtual void lock();
	virtual void unlock();

	/* These methods get information. */
	virtual bool get_whose() const;
	virtual bitboard_t get_hash() const;
	virtual int get_status(bool mate_test);
	virtual bool check() const;
	virtual bool zugzwang() const;

	/* These methods generate, make, and take back moves. */
	virtual void generate(list<move_t> &l, bool only_legal_moves = false, bool only_captures = false);
	virtual bool make(move_t m);
	virtual void unmake();
	virtual bool make(char *p);
	virtual int perft(int depth);

private:
	list<state_t> states;                           // Previous states.
	state_t state;                                  // Current state.
	list<bitboard_t> rotations[ANGLES][COLORS + 1]; // Previous rotated BitBoards.
	bitboard_t rotation[ANGLES][COLORS + 1];        // Current rotated BitBoard.
	list<bitboard_t> hashes;                        // Previous Zobrist hash keys.
	bitboard_t hash;                                // Current Zobrist hash key.
	list<bitboard_t> pawn_hashes;                   // Previous pawn hash keys.
	bitboard_t pawn_hash;                           // Current pawn hash key.
	mutex_t mutex;				        // Lock.

	/* These methods start up games. */
	virtual void init_state();
	virtual void init_rotation();
	virtual void init_hash();
	virtual void precomp_key() const;

	/* These methods generate moves. */
	virtual void generate_king(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_queen(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_rook(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_bishop(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_knight(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_pawn(list<move_t> &l, bool only_captures = false) const;
	virtual void precomp_king() const;
	virtual void precomp_row() const;
	virtual void precomp_knight() const;

	/* These methods test for various conditions. */
	virtual int mate();
	virtual bool check(bitboard_t b1, bool color) const;
	virtual bool insufficient() const;
	virtual bool three() const;
	virtual bool fifty() const;

	/* These methods manipulate BitBoards. */
	virtual int count(bitboard_t b) const;
	virtual int find_64(int64_t n) const;
	virtual int find_32(int32_t n) const;
	virtual bitboard_t rotate(bitboard_t b1, int map, int angle) const;
	virtual uint64_t randomize() const;
	virtual void insert(int x, int y, bitboard_t b, int angle, list<move_t> &l, bool pos) const;

protected:
	virtual int evaluate() const = 0;
};

#endif
