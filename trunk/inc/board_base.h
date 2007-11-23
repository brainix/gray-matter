/*----------------------------------------------------------------------------*\
 |	board_base.h - board representation interface			      |
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

#ifndef BOARD_BASE_H
#define BOARD_BASE_H

using namespace std;

// C++ stuff:
#include <list>
#include <string>
#include <sstream>

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Castling statuses:
#define CAN_CASTLE	0
#define CANT_CASTLE	1
#define HAS_CASTLED	2
#define CASTLE_STATS	3

// Castling requirements:
#define UNOCCUPIED	0 // Squares which mustn't be occupied.
#define UNATTACKED	1 // Squares which mustn't be attacked.
#define REQS		2

// Game statuses:
#define IN_PROGRESS	0 // Still in progress.
#define STALEMATE	1 // Drawn by stalemate.
#define INSUFFICIENT	2 // Drawn by insufficient material.
#define THREE		3 // Drawn by threefold repetition.
#define FIFTY		4 // Drawn by fifty move rule.
#define CHECKMATE	5 // Checkmated.
#define ILLEGAL		6 // Post-checkmated (king captured).
#define GAME_STATS	7

// Game phases:
#define OPENING		0
#define MIDGAME		1
#define ENDGAME		2
#define PHASES		3

// Rotated BitBoard maps:
#define MAP		0
#define UNMAP		1
#define MAPS		2

// Rotated BitBoard angles:
#define L45		0
#define ZERO		1
#define R45		2
#define R90		3
#define ANGLES		4

// Board coordinates:
#define X		0 // x-coordinate (file).
#define Y		1 // y-coordinate (rank).
#define COORDS		2

// List positions:
#define FRONT		0
#define BACK		1
#define POSITIONS	2



/*----------------------------------------------------------------------------*\
 |				    BitBoard				      |
\*----------------------------------------------------------------------------*/

// A BitBoard is a brilliant data structure based on this observation: there
// are 64 bits in a uint64_t integer and 64 squares on a chess board.  See where
// I'm going?  A BitBoard is an unsigned 64-bit integer in which every bit
// corresponds to a square.
//
// A single BitBoard can't represent the entire state of the board.  A single
// bit can only hold a value of 0 or 1 - enough to describe the absence or
// presence of a piece on a square, but not enough to describe the piece's color
// or type.  Therefore, we need 12 BitBoards to represent the entire state of
// the board:
//
//		· white pawns		· black pawns
//		· white knights		· black knights
//		· white bishops		· black bishops
//		· white rooks		· black rooks
//		· white queens		· black queens
//		· white kings		· black kings
//
// Similarly, BitRow is an unsigned 8-bit integer which represents up to 8
// adjacent squares: a row in a 0° bitboard, a column in a 90° bitboard, or a
// diagonal in a 45° bitboard.
typedef uint64_t bitboard_t;
typedef uint8_t bitrow_t;

// These macros manipulate bits in BitBoards.
#define BIT_IDX(x, y)			((y) * 8 + (x))
#define BIT_MSK(x, y)			(1ULL << BIT_IDX(x, y))
#define BIT_GET(b, x, y)		((b) >> BIT_IDX(x, y) & 1)
#define BIT_CLR(b, x, y)		((b) &= ~BIT_MSK(x, y))
#define BIT_SET(b, x, y)		((b) |= BIT_MSK(x, y))
#define BIT_MOV(b, x1, y1, x2, y2)	((b) ^= BIT_MSK(x1, y1) | BIT_MSK(x2, y2))

// These macros manipulate rows in 0° rotated BitBoards and columns in 90°
// rotated BitBoards.
#define ROW_NUM(x, y, a)		((a) == ZERO ? (y) : (x))
#define ROW_LOC(x, y, a)		((a) == ZERO ? (x) : 7 - (y))
#define ROW_IDX(n)			(BIT_IDX(0, n))
#define ROW_MSK(n)			(0xFFULL << ROW_IDX(n))
#define ROW_GET(b, n)			((b) >> ROW_IDX(n) & 0xFF)
#define ROW_CLR(b, n)			((b) &= ~ROW_MSK(n))
#define ROW_SET(b, n, r)		((b) |= (bitboard_t) (r) << ROW_IDX(n))

// These macros manipulate columns in 0° rotated BitBoards and rows in 90°
// rotated BitBoards.
#define COL_IDX(n)			(BIT_IDX(n, 0))
#define COL_MSK(n)			(0x0101010101010101ULL << COL_IDX(n))
#define COL_CLR(b, n)			((b) &= ~COL_MSK(n))

// These macros manipulate adjacent bits in 45° rotated BitBoards, which
// correspond to diagonals in 0° and 90° rotated BitBoards.
#define DIAG_NUM(x, y, a)		((a) == L45 ? (x) + (y) : 7 - (x) + (y))
#define DIAG_LOC(x, y, a)		(BIT_IDX(coord[MAP][a][x][y][X], coord[MAP][a][x][y][Y]) - diag_index[DIAG_NUM(x, y, a)])
#define DIAG_LEN(n)			(8 - abs(7 - (n)))
#define DIAG_IDX(n)			(diag_index[n])
#define DIAG_MSK(n)			((bitboard_t) diag_mask[n] << diag_index[n])
#define DIAG_GET(b, n)			((b) >> diag_index[n] & diag_mask[n])
#define DIAG_CLR(b, n)			((b) &= ~DIAG_MSK(n))
#define DIAG_SET(b, n, d)		((b) |= (bitboard_t) (d) << diag_index[n])

// This macro finds the first set bit in a BitBoard.
#define FST(b)				(find_64(b) - 1)

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



/*----------------------------------------------------------------------------*\
 |				     State				      |
\*----------------------------------------------------------------------------*/

// This structure describes the entire state of the board.  It contains the
// aforementioned 12 BitBoards along with castling statuses, en passant
// vulnerability, and the color on move.
typedef struct state
{
	bitboard_t piece[COLORS][SHAPES]; // Aforementioned 12 BitBoards.
	int castle[COLORS][SIDES];        // Castling statuses.
	int en_passant;                   // En passant vulnerability.
	bool whose;                       // Color on move.
	int fifty;                        // 50 move rule counter.
} state_t;

// These macros represent the colors on and off move.
#define ON_MOVE		(state.whose)
#define OFF_MOVE	(!state.whose)

// This macro assembles a BitBoard which contains all of a color's pieces.
#define ALL(s, c)	((s).piece[c][PAWN] | (s).piece[c][KNIGHT] | (s).piece[c][BISHOP] | (s).piece[c][ROOK] | (s).piece[c][QUEEN] | (s).piece[c][KING])



/*----------------------------------------------------------------------------*\
 |				      Move				      |
\*----------------------------------------------------------------------------*/

// This structure describes a move.  It contains the from and to coordinates,
// the pawn promotion information, and the MiniMax score.  We use a BitField to
// tightly pack this information into 32 bits because some of our methods return
// this structure (rather than a pointer to this structure or other similar
// ugliness).
typedef struct move
{
	unsigned x1      :  3; // From x coordinate.              3 bits
	unsigned y1      :  3; // From y coordinate.           +  3 bits
	unsigned x2      :  3; // To x coordinate.             +  3 bits
	unsigned y2      :  3; // To y coordinate.             +  3 bits
	unsigned promo   :  3; // Pawn promotion information.  +  3 bits
	unsigned padding :  1; // The Evil Bit (TM).           +  1 bit
	  signed value   : 16; // MiniMax score.               + 16 bits
	                       //                              = 32 bits

	// Overloaded equality test operator.
	bool operator==(const struct move that) const
	{
		return this->x1 == that.x1 && this->y1 == that.y1 &&
		       this->x2 == that.x2 && this->y2 == that.y2 &&
		       this->promo == that.promo;
	};

	// Overloaded inequality test operator.
	bool operator!=(const struct move that) const
	{
		return this->x1 != that.x1 || this->y1 != that.y1 ||
		       this->x2 != that.x2 || this->y2 != that.y2 ||
		       this->promo != that.promo;
	};

	// Overloaded assignment operator.
	struct move& operator=(const struct move& that)
	{
		x1 = that.x1;
		y1 = that.y1;
		x2 = that.x2;
		y2 = that.y2;
		promo = that.promo;
		value = that.value;
		return *this;
	};
} __attribute__((packed)) move_t;

#define IS_NULL_MOVE(m)		(!(m).promo && !(m).y2 && !(m).x2 && !(m).y1 && !(m).x1)
#define SET_NULL_MOVE(m)	((m).promo = (m).y2 = (m).x2 = (m).y1 = (m).x1 = 0)



/*----------------------------------------------------------------------------*\
 |				     Board				      |
\*----------------------------------------------------------------------------*/

class board_base
{
public:
	// These methods set information.
	board_base();
	virtual ~board_base();
	virtual board_base& operator=(const board_base& that);
	virtual void set_board();
	virtual bool set_board_fen(string& fen);
	virtual bool set_board_fen_error(string& fen, string reason, int x, int y);
	virtual void lock();
	virtual void unlock();

	// These methods get information.
	virtual bool get_whose() const;
	virtual bitboard_t get_hash() const;
	virtual int get_status(bool mate_test);
	virtual int get_num_moves() const;
	virtual int evaluate(int depth) const = 0; // Force sub-classes to override.
	virtual bool check() const;
	virtual bool zugzwang() const;

	// These methods generate, make, and take back moves.
	virtual void generate(list<move_t> &l, bool only_legal_moves = false, bool only_captures = false);
	virtual bool make(move_t m);
	virtual bool unmake();
	virtual move_t san_to_coord(string& san);
	virtual void coord_to_san(move_t m, string& san);
	virtual int perft(int depth);

protected:
	list<state_t> states;                           // Previous states.
	state_t state;                                  // Current state.
	list<bitboard_t> rotations[ANGLES][COLORS + 1]; // Previous rotated BitBoards.
	bitboard_t rotation[ANGLES][COLORS + 1];        // Current rotated BitBoard.
	list<bitboard_t> hashes;                        // Previous Zobrist hash keys.
	bitboard_t hash;                                // Current Zobrist hash key.
	list<bitboard_t> pawn_hashes;                   // Previous pawn hash keys.
	bitboard_t pawn_hash;                           // Current pawn hash key.
	mutex_t mutex;				        // Lock.

	// These methods start up games.
	virtual void init_state();
	virtual void init_rotation();
	virtual void init_hash();
	virtual void precomp_key() const;

	// These methods generate moves.
	virtual void generate_king(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_queen(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_rook(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_bishop(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_knight(list<move_t> &l, bool only_captures = false) const;
	virtual void generate_pawn(list<move_t> &l, bool only_captures = false) const;
	virtual void precomp_king() const;
	virtual void precomp_row() const;
	virtual void precomp_knight() const;
	virtual void precomp_pawn() const;

	// These methods test for various conditions.
	virtual int mate();
	virtual bool check(bitboard_t b1, bool color) const;
	virtual bool insufficient() const;
	virtual bool three() const;
	virtual bool fifty() const;

	// These methods manipulate BitBoards.
	virtual bitboard_t rotate(bitboard_t b1, int map, int angle) const;
	virtual void insert(int x, int y, bitboard_t b, int angle, list<move_t> &l, bool pos) const;
};



#endif
