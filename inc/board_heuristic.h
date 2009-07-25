/*----------------------------------------------------------------------------*\
 |  board_heuristic.h - heuristic evaluation interface                        |
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

#ifndef BOARD_HEURISTIC_H
#define BOARD_HEURISTIC_H

#include "board_base.h"
#include "table.h"

/// Heuristic evaluation.
class board_heuristic : public board_base
{
public:
    board_heuristic();
    ~board_heuristic();
    board_heuristic& operator=(const board_heuristic& that);
    value_t evaluate() const;

private:
    // The values of the pieces:
    static const value_t value_material[SHAPES];

    // The values of having different pieces on different squares:
    static const value_t value_position[SHAPES][8][8];

    // The values of having the white king on different squares during an
    // endgame with pawns (of both colors) only on the queen side:
    static const value_t value_king_position[8][8];
    
    //in the end game, the value of being in the middle or not
    static const value_t value_king_middle[8][8];
    
    // The values of various pawn structure features:
    static const value_t value_pawn_passed[8];
    static const value_t value_pawn_doubled[9];
    static const value_t value_pawn_isolated[9];
    static const value_t value_pawn_doubled_isolated[9];
    static const value_t value_pawn_duo;

    //
    static const value_t value_knight_outpost[8][8];

    //
    static const value_t value_bishop_over_knight;
    static const value_t value_bishop_trapped;

    //
    static const value_t value_rook_on_7th;
    static const value_t value_rooks_on_7th;

    //
    static const value_t value_queen_rook_on_7th;
    static const value_t value_queen_offside;

    // The penalty for giving up castling:
    static const value_t value_king_cant_castle;

    //
    static bool precomputed_board_heuristic;
    static bitboard_t squares_pawn_duo[8][8];
    static bitboard_t squares_pawn_potential_attacks[COLORS][8][8];

    // Since pawn structure remains relatively static, we maintain a hash table
    // of previous pawn structure evaluations.  According to my tests, this hash
    // table sustains a hit rate of around 97%.  This enables us to perform
    // sophisticated pawn structure analysis almost for free.
    static PawnTable pawn_table;

    value_t evaluate_pawns() const;
    value_t evaluate_knights() const;
    value_t evaluate_bishops() const;
    value_t evaluate_rooks() const;
    value_t evaluate_queens() const;
    value_t evaluate_kings() const;
    void precomp_pawn();
};

#endif
