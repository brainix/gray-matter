/*----------------------------------------------------------------------------*\
 |  board_base.h - board representation and move generation interface         |
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

#ifndef BOARD_BASE_H
#define BOARD_BASE_H

#include <iostream>

using namespace std;

#include "bitboard.h"
#include "state.h"
#include "move.h"

/// A BitRow is an unsigned 8-bit integer which represents up to 8 adjacent
/// squares: a row in a 0° BitBoard, a column in a 90° BitBoard, or a diagonal
/// in a 45° BitBoard.
typedef uint8_t bitrow_t;

/// This class represents the board and generates moves.
class board_base
{
public:
  inline static int         BIT_IDX(int x, int y) {return ((y) << 3 | (x));}
  inline static bitboard_t  BIT_MSK(int x, int y) {return (1ULL << BIT_IDX(x, y));}
  inline static bool        BIT_GET(bitboard_t b, int x, int y) {return ((b) >> BIT_IDX(x, y) & 1);}
  inline static void        BIT_CLR(bitboard_t& b, unsigned x, unsigned y) {(b) &= ~BIT_MSK(x, y);}
  inline static void        BIT_SET(bitboard_t& b, int x, int y) {(b) |= BIT_MSK(x, y);}
  inline static bitboard_t  BIT_MOV(bitboard_t& b, int x1, int y1, int x2, int y2) {return ((b) ^= BIT_MSK(x1, y1) | BIT_MSK(x2, y2));}

  inline static int         ROW_NUM(int x,int y,int a){return ((a) == ZERO ? (y) : (x));}
  inline static int         ROW_LOC(int x,int y,int a){return ((a) == ZERO ? (x) : 7 - (y));}
  inline static int         ROW_IDX(int n){return (BIT_IDX(0, n));}
  inline static bitboard_t  ROW_MSK(int n){return (0xFFULL << ROW_IDX(n));}
  inline static bitrow_t    ROW_GET(bitboard_t b, int n){return ((bitrow_t)((b) >> ROW_IDX(n) & 0xFF));}
  inline static bitboard_t  ROW_CLR(bitboard_t& b, int n){return ((b) &= ~ROW_MSK(n));}
  inline static bitboard_t  ROW_SET(bitboard_t& b, int n, bitrow_t r){return ((b) |= (bitboard_t) (r) << ROW_IDX(n));}

  // These macros manipulate columns in 0° rotated BitBoards and rows in 90°
  // rotated BitBoards.
  inline static int         COL_IDX(int n){return (BIT_IDX(n, 0));}
  inline static bitboard_t  COL_MSK(int n){return (0x0101010101010101ULL << COL_IDX(n));}
  inline static bitboard_t  COL_CLR(bitboard_t& b, int n){return ((b) &= ~COL_MSK(n));}

  // These macros manipulate adjacent bits in 45° rotated BitBoards, which
  // correspond to diagonals in 0° and 90° rotated BitBoards.
  inline static int         DIAG_NUM(int x, int y, int a){return ((a) == L45 ? (x) + (y) : 7 - (x) + (y));}
  inline static int         DIAG_LOC(int x, int y, int a){return (BIT_IDX(coord[MAP][a][x][y][X], coord[MAP][a][x][y][Y]) - diag_index[DIAG_NUM(x, y, a)]);}
  inline static int         DIAG_LEN(int n){return (8 - abs(7 - (n)));}
  inline static int         DIAG_IDX(int n){return (diag_index[n]);}
  inline static bitboard_t  DIAG_MSK(int n){return ((bitboard_t) diag_mask[n] << diag_index[n]);}
  inline static bitboard_t  DIAG_CLR(bitboard_t& b, int n) {return ((b) &= ~DIAG_MSK(n));}
  inline static bitrow_t    DIAG_GET(bitboard_t b, int n) {return ((bitrow_t)((b) >> diag_index[n] & diag_mask[n]));}
  inline static bitboard_t  DIAG_SET(int n, bitrow_t d){return ((bitboard_t) (d) << diag_index[n]);}

  // This macro finds the first set bit in a BitBoard.
  inline static int FST(bitboard_t b){return (find_64(b) - 1);}

  // Convenient BitBoards:
  static const bitboard_t SQUARES_CENTER           = 0x0000001818000000ULL; // 4 center squares.
  static const bitboard_t SQUARES_EXPANDED_CENTER  = 0x00003C3C3C3C0000ULL; // 16 center squares.
  static const bitboard_t SQUARES_PRINCIPAL_DIAG   = 0x8142241818244281ULL; // 16 principal diagonal squares.
  static const bitboard_t SQUARES_WHITE_SIDE       = 0x00000000FFFFFFFFULL; // 32 white side squares.
  static const bitboard_t SQUARES_BLACK_SIDE       = 0xFFFFFFFF00000000ULL; // 32 black side squares.
  static const bitboard_t SQUARES_WHITE            = 0x55AA55AA55AA55AAULL; // 32 white squares.
  static const bitboard_t SQUARES_BLACK            = 0xAA55AA55AA55AA55ULL; // 32 black squares.
  static const bitboard_t SQUARES_QUEEN_SIDE       = 0x0F0F0F0F0F0F0F0FULL; // 32 queen side squares.
  static const bitboard_t SQUARES_KING_SIDE        = 0xF0F0F0F0F0F0F0F0ULL; // 32 king side squares.
  static const bitboard_t SQUARES_CORNER           = 0x8100000000000081ULL; // 4 corner squares.


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
    virtual unsigned get_num_pieces(const bool color) const;
    virtual value_t evaluate() const = 0; // Force sub-classes to override.
    virtual bool check(bool off_move = false) const;
    virtual bool zugzwang() const;
    virtual string to_string() const;

    // These methods generate, make, and take back moves.
    virtual bool generate(MoveArray& l, bool only_legal_moves = false, bool only_captures = false);
    virtual bool make(Move m);
    virtual bool unmake();
    virtual Move san_to_coord(string& san);
    virtual void coord_to_san(Move m, string& san);
    virtual uint64_t perft(int depth);

protected:
    // This array maps coordinates between rotated BitBoards:
    static const int coord[MAPS][ANGLES][8][8][COORDS];

    //
    static const int diag_index[15];
    static const bitrow_t diag_mask[15];

    // Whether the moves and hash keys have been pre-computed:
    static bool precomputed_board_base;

    // Pre-computed moves:
    static bitboard_t squares_king[8][8];
    static bitrow_t squares_row[8][256];
    static bitboard_t squares_knight[8][8];
    static const bitboard_t squares_castle[COLORS][SIDES][REQS];
    static bitboard_t squares_adj_cols[8];
    static bitboard_t squares_pawn_attacks[COLORS][8][8];

    // Zobrist hash keys:
    static bitboard_t key_piece[COLORS][SHAPES][8][8];
    static bitboard_t key_castle[COLORS][SIDES][CASTLE_STATS];
    static bitboard_t key_no_en_passant;
    static bitboard_t key_en_passant[8];
    static bitboard_t key_on_move;

    stateArray states;                            ///< Previous states.
    state_t state;                                  ///< Current state.
    bitBoardArray rotations[ANGLES][COLORS + 1];    ///< Previous rotated BitBoards.
    bitboard_t rotation[ANGLES][COLORS + 1];        ///< Current rotated BitBoards.
    bitBoardArray hashes;                           ///< Previous Zobrist hash keys.
    bitboard_t hash;                                ///< Current Zobrist hash key.
    bitBoardArray pawn_hashes;                      ///< Previous pawn hash keys.
    bitboard_t pawn_hash;                           ///< Current pawn hash key.
    mutex_t mutex;                                  ///< Lock.
    bool generated_king_capture;

    // These methods start up games.
    virtual void init_state();
    virtual void init_rotation();
    virtual void init_hash();
    virtual void precomp_key() const;

    // These methods generate moves.
    virtual void generate_king(MoveArray& l, bool only_captures = false);
    virtual void generate_queen(MoveArray& l, bool only_captures = false);
    virtual void generate_rook(MoveArray& l, bool only_captures = false);
    virtual void generate_bishop(MoveArray& l, bool only_captures = false);
    virtual void generate_knight(MoveArray& l, bool only_captures = false);
    virtual void generate_pawn(MoveArray& l, bool only_captures = false);
    virtual void precomp_king() const;
    virtual void precomp_row() const;
    virtual void precomp_knight() const;
    virtual void precomp_pawn();

    // These methods test for various conditions.
    virtual int mate();
    virtual bool check(bitboard_t b1, bool color) const;
    virtual bool insufficient() const;
    virtual bool three() const;
    virtual bool fifty() const;

    // These methods manipulate BitBoards.
    virtual bitboard_t rotate(bitboard_t b1, int map, int angle) const;
    virtual void insert(int x, int y, bitboard_t b, int angle, MoveArray& l, 
      bool pos);
};

#endif
