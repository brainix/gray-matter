/*----------------------------------------------------------------------------*\
 |	board_base.h - board representation interface			      |
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

/// A BitBoard is a brilliant data structure based on this observation: there
/// are 64 bits in a uint64_t integer, and there are 64 squares on a chess
/// board.  Do you see where I'm going with this?  A BitBoard is an unsigned 64-
/// bit integer in which every bit corresponds to a square.
///
/// A single BitBoard can't represent the entire state of the board.  A single
/// bit can only hold a value of 0 or 1 - enough to describe the absence or
/// presence of a piece on a square, but not enough to describe the piece's
/// color or type.  Therefore, we need 12 BitBoards to represent the entire
/// state of the board:
///
///		· white pawns		· black pawns
///		· white knights		· black knights
///		· white bishops		· black bishops
///		· white rooks		· black rooks
///		· white queens		· black queens
///		· white kings		· black kings
///
/// Similarly, BitRow is an unsigned 8-bit integer which represents up to 8
/// adjacent squares: a row in a 0° bitboard, a column in a 90° bitboard, or a
/// diagonal in a 45° bitboard.
typedef uint64_t bitboard_t;
typedef uint8_t bitrow_t;

// These macros manipulate bits in BitBoards.
#define BIT_IDX(x, y)			((y) << 3 | (x))
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

/// This structure describes the entire state of the board.  

/// It contains the aforementioned 12 BitBoards along with castling statuses, 
/// en passant vulnerability, and the color on move.
///
/// Subtle!  In the en passant vulnerability field, we need only store the file
/// of the pawn susceptible to en passant.  Its rank is implied by the color on
/// move (which is also kept in the state).  If white is on move, then the pawn
/// susceptible to en passant must be black and on rank 5.  If black is on move,
/// then the pawn susceptible to en passant must be white and on rank 4.
typedef struct state
{
	bitboard_t piece[COLORS][SHAPES]; ///< Aforementioned 12 BitBoards.
	int castle[COLORS][SIDES];        ///< Castling statuses.
	int en_passant;                   ///< En passant vulnerability.
	bool whose;                       ///< Color on move.
	int fifty;                        ///< 50 move rule counter.
} state_t;

/// This macro assembles a BitBoard that contains all of a color's pieces.
#define ALL(s, c)	((s).piece[c][PAWN]   | (s).piece[c][KNIGHT] | \
			 (s).piece[c][BISHOP] | (s).piece[c][ROOK]   | \
			 (s).piece[c][QUEEN]  | (s).piece[c][KING])

/// This macro represents the color on move.
#define ON_MOVE		(state.whose)

/// This macro represents the color off move.
#define OFF_MOVE	(!state.whose)



/*----------------------------------------------------------------------------*\
 |				      Move				      |
\*----------------------------------------------------------------------------*/

typedef int16_t value_t;

/// This structure describes a move. 

/// It contains the from and to coordinates, the pawn promotion information, 
/// and the MiniMax score.  We use a BitField to tightly pack this information 
/// into 32 bits because some of our methods return this structure (rather 
/// than a pointer to this structure or other similar ugliness).
typedef struct move
{
	unsigned x1      : 3; ///< From x coordinate.              3 bits
	unsigned y1      : 3; ///< From y coordinate.           +  3 bits
	unsigned x2      : 3; ///< To x coordinate.             +  3 bits
	unsigned y2      : 3; ///< To y coordinate.             +  3 bits
	unsigned promo   : 3; ///< Pawn promotion information.  +  3 bits
	unsigned padding : 1; ///< The Evil Bit (TM).           +  1 bit
	value_t  value;       ///< MiniMax score.               + 16 bits
	                      ///                               = 32 bits

	/// Overloaded equality test operator.
	bool operator==(const struct move that) const
	{
		return this->x1 == that.x1 && this->y1 == that.y1 &&
		       this->x2 == that.x2 && this->y2 == that.y2 &&
		       this->promo == that.promo;
	};

	/// Overloaded inequality test operator.
	bool operator!=(const struct move that) const
	{
		return this->x1 != that.x1 || this->y1 != that.y1 ||
		       this->x2 != that.x2 || this->y2 != that.y2 ||
		       this->promo != that.promo;
	};

	/// Overloaded assignment operator.
	struct move &operator=(const struct move &that)
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

/// This class represents the board.
class board_base
{
public:
	// These methods set information.
	board_base();
	virtual ~board_base();
	virtual board_base &operator=(const board_base &that);
	virtual void set_board();
	virtual bool set_board_fen(string &fen);
	virtual bool set_board_fen_error(string &fen, string reason, int x, int y);
	virtual void lock();
	virtual void unlock();

	// These methods get information.
	virtual bool get_whose() const;
	virtual bitboard_t get_hash() const;
	virtual int get_status(bool mate_test);
	virtual int get_num_moves() const;
	virtual value_t evaluate(int depth) const = 0; // Force sub-classes to override.
	virtual bool check(bool off_move = false) const;
	virtual bool zugzwang() const;
	virtual string to_string() const;

	// These methods generate, make, and take back moves.
	virtual bool generate(list<move_t> &l, bool only_legal_moves = false, bool only_captures = false);
	virtual bool make(move_t m);
	virtual bool unmake();
	virtual move_t san_to_coord(string &san);
	virtual void coord_to_san(move_t m, string &san);
	virtual uint64_t perft(int depth);

protected:
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

	//
	static const int      diag_index[15] = {   0,    1,    3,    6,   10,   15,   21,   28,   36,   43,   49,   54,   58,   61,   63};
	static const bitrow_t diag_mask[15]  = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};

	// Whether the moves and hash keys have been pre-computed:
	static bool precomputed_board_base = false;

	// Pre-computed moves:
	bitboard_t squares_king[8][8];
	bitrow_t squares_row[8][256];
	bitboard_t squares_knight[8][8];
	static const bitboard_t squares_castle[COLORS][SIDES][REQS] =
	{
		// The squares which must be unoccupied on the queen side in
		// order for the white king to be able to castle:
		{{0x000000000000000EULL,
		// The squares which must be unattacked on the queen side in
		// order for the white king to be able to castle:
		  0x000000000000001CULL},
		// The squares which must be unoccupied on the king side in
		// order for the white king to be able to castle:
		 {0x0000000000000060ULL,
		// The squares which must be unattacked on the king side in
		// order for the white king to be able to castle:
		  0x0000000000000070ULL}},
		// The squares which must be unoccupied on the queen side in
		// order for the black king to be able to castle:
		{{0x0E00000000000000ULL,
		// The squares which must be unattacked on the queen side in
		// order for the black king to be able to castle:
		  0x1C00000000000000ULL},
		// The squares which must be unoccupied on the king side in
		// order for the black king to be able to castle:
		 {0x6000000000000000ULL,
		// The squares which must be unattacked on the king side in
		// order for the black king to be able to castle.
		  0x7000000000000000ULL}}
	};
	static bitboard_t squares_adj_cols[8];
	static bitboard_t squares_pawn_attacks[COLORS][8][8];

	// Zobrist hash keys:
	static bitboard_t key_piece[COLORS][SHAPES][8][8];
	static bitboard_t key_castle[COLORS][SIDES][CASTLE_STATS];
	static bitboard_t key_no_en_passant;
	static bitboard_t key_en_passant[8];
	static bitboard_t key_whose;

	list<state_t> states;                           ///< Previous states.
	state_t state;                                  ///< Current state.
	list<bitboard_t> rotations[ANGLES][COLORS + 1]; ///< Previous rotated BitBoards.
	bitboard_t rotation[ANGLES][COLORS + 1];        ///< Current rotated BitBoards.
	list<bitboard_t> hashes;                        ///< Previous Zobrist hash keys.
	bitboard_t hash;                                ///< Current Zobrist hash key.
	list<bitboard_t> pawn_hashes;                   ///< Previous pawn hash keys.
	bitboard_t pawn_hash;                           ///< Current pawn hash key.
	mutex_t mutex;				        ///< Lock.
	bool generated_king_capture;

	// These methods start up games.
	virtual void init_state();
	virtual void init_rotation();
	virtual void init_hash();
	virtual void precomp_key() const;

	// These methods generate moves.
	virtual void generate_king(list<move_t> &l, bool only_captures = false);
	virtual void generate_queen(list<move_t> &l, bool only_captures = false);
	virtual void generate_rook(list<move_t> &l, bool only_captures = false);
	virtual void generate_bishop(list<move_t> &l, bool only_captures = false);
	virtual void generate_knight(list<move_t> &l, bool only_captures = false);
	virtual void generate_pawn(list<move_t> &l, bool only_captures = false);
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
	virtual void insert(int x, int y, bitboard_t b, int angle, list<move_t> &l, bool pos);
};



#endif
