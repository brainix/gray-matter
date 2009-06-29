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

using namespace std;

// C++ stuff:
#include <list>
#include <vector>
#include <string>
#include <sstream>

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "bitboard.h"
#include "state.h"
#include "move.h"

// Castling statuses:
#define CAN_CASTLE      0
#define CANT_CASTLE     1
#define HAS_CASTLED     2
#define CASTLE_STATS    3

// Castling requirements:
#define UNOCCUPIED      0 // Squares which mustn't be occupied.
#define UNATTACKED      1 // Squares which mustn't be attacked.
#define REQS            2

// Game statuses:
#define IN_PROGRESS     0 // Still in progress.
#define STALEMATE       1 // Drawn by stalemate.
#define INSUFFICIENT    2 // Drawn by insufficient material.
#define THREE           3 // Drawn by threefold repetition.
#define FIFTY           4 // Drawn by fifty move rule.
#define CHECKMATE       5 // Checkmated.
#define ILLEGAL         6 // Post-checkmated (king captured).
#define GAME_STATS      7

// Game phases:
#define OPENING         0
#define MIDGAME         1
#define ENDGAME         2
#define PHASES          3

// Rotated BitBoard maps:
#define MAP             0
#define UNMAP           1
#define MAPS            2

// Rotated BitBoard angles:
#define L45             0
#define ZERO            1
#define R45             2
#define R90             3
#define ANGLES          4

// Board coordinates:
#define X               0 // x-coordinate (file).
#define Y               1 // y-coordinate (rank).
#define COORDS          2

// List positions:
#define FRONT           0
#define BACK            1
#define POSITIONS       2

/// This macro represents the color currently on move.
#define ON_MOVE         (state.on_move)

/// This macro represents the color currently off move.
#define OFF_MOVE        (!state.on_move)

/// This class represents the board and generates moves.
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
    virtual value_t evaluate() const = 0; // Force sub-classes to override.
    virtual bool check(bool off_move = false) const;
    //virtual bool zugzwang() const;
    virtual string to_string() const;

    // These methods generate, make, and take back moves.
    virtual bool generate(moveArray& l, bool only_legal_moves = false, bool only_captures = false);
    virtual bool make(move_t m);
    virtual bool unmake();
    virtual move_t san_to_coord(string& san);
    virtual void coord_to_san(move_t m, string& san);
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

    list<state_t> states;                           ///< Previous states.
    state_t state;                                  ///< Current state.
    list<bitboard_t> rotations[ANGLES][COLORS + 1]; ///< Previous rotated BitBoards.
    bitboard_t rotation[ANGLES][COLORS + 1];        ///< Current rotated BitBoards.
    list<bitboard_t> hashes;                        ///< Previous Zobrist hash keys.
    bitboard_t hash;                                ///< Current Zobrist hash key.
    list<bitboard_t> pawn_hashes;                   ///< Previous pawn hash keys.
    bitboard_t pawn_hash;                           ///< Current pawn hash key.
    mutex_t mutex;                                  ///< Lock.
    bool generated_king_capture;

    // These methods start up games.
    virtual void init_state();
    virtual void init_rotation();
    virtual void init_hash();
    virtual void precomp_key() const;

    // These methods generate moves.
    virtual void generate_king(moveArray& l, bool only_captures = false);
    virtual void generate_queen(moveArray& l, bool only_captures = false);
    virtual void generate_rook(moveArray& l, bool only_captures = false);
    virtual void generate_bishop(moveArray& l, bool only_captures = false);
    virtual void generate_knight(moveArray& l, bool only_captures = false);
    virtual void generate_pawn(moveArray& l, bool only_captures = false);
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
    virtual void insert(int x, int y, bitboard_t b, int angle, moveArray& l, 
      bool pos);
};

#endif
