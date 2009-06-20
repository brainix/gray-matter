/*----------------------------------------------------------------------------*\
 |  board_heuristic.cpp - heuristic evaluation implementation                 |
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

#include "gray.h"
#include "board_heuristic.h"

/// The values of the pieces:
const value_t board_heuristic::value_material[SHAPES] = {VALUE_PAWN,
                                                         VALUE_KNIGHT,
                                                         VALUE_BISHOP,
                                                         VALUE_ROOK,
                                                         VALUE_QUEEN,
                                                         VALUE_KING};

/// The values of having different pieces on different squares:
const value_t board_heuristic::value_position[SHAPES][8][8] =
{
            // White pawns:
    /* A */ {{  0,   0,   1,   3,   6,  10,  40,   0},
    /* B */  {  0,   0,   1,   3,   6,  10,  40,   0},
    /* C */  {  0,   0,   1,   3,   6,  10,  40,   0},
    /* D */  {  0, -10,  10,  13,  16,  30,  40,   0},
    /* E */  {  0, -10,  10,  13,  16,  30,  40,   0},
    /* F */  {  0,   0,   1,   3,   6,  10,  40,   0},
    /* G */  {  0,   0,   1,   3,   6,  10,  40,   0},
    /* H */  {  0,   0,   1,   3,   6,  10,  40,   0}},
            //  1    2    3    4    5    6    7    8

            // Knights:
    /* A */ {{-60, -30, -30, -30, -30, -30, -30, -60},
    /* B */  {-30, -24,  -6,  -6,  -6,  -6, -24, -30},
    /* C */  {-30, -10,  -6,  -2,  -2,  -6, -10, -30},
    /* D */  {-30, -10,  -6,   0,   0,  -6, -10, -30},
    /* E */  {-30, -10,  -6,   0,   0,  -6, -10, -30},
    /* F */  {-30, -10,  -6,  -2,  -2,  -6, -10, -30},
    /* G */  {-30, -24,  -6,  -6,  -6,  -6, -24, -30},
    /* H */  {-60, -30, -30, -30, -30, -30, -30, -60}},
            //  1    2    3    4    5    6    7    8

            // Bishops:
    /* A */ {{-20, -20, -20, -20, -20, -20, -20, -20},
    /* B */  {-20,   6,   6,   3,   3,   6,   6, -20},
    /* C */  {-20,   6,   8,   6,   6,   8,   6, -20},
    /* D */  {-20,   3,   6,  10,  10,   6,   3, -20},
    /* E */  {-20,   3,   6,  10,  10,   6,   3, -20},
    /* F */  {-20,   6,   8,   6,   6,   8,   6, -20},
    /* G */  {-20,   6,   6,   3,   3,   6,   6, -20},
    /* H */  {-20, -20, -20, -20, -20, -20, -20, -20}},
            //  1    2    3    4    5    6    7    8

            // Rooks:
    /* A */ {{-10, -10, -10, -10, -10, -10, -10, -10},
    /* B */  { -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6},
    /* C */  { -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2},
    /* D */  {  2,   2,   2,   2,   2,   2,   2,   2},
    /* E */  {  2,   2,   2,   2,   2,   2,   2,   2},
    /* F */  { -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2},
    /* G */  { -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6},
    /* H */  {-10, -10, -10, -10, -10, -10, -10, -10}},
            //  1    2    3    4    5    6    7    8

            // Queens:
    /* A */ {{-20, -20,   0,   0,   0,   0, -20, -20},
    /* B */  {-20,   0,   8,   8,   8,   8,   0, -20},
    /* C */  {  0,   8,   8,  12,  12,   8,   8,   0},
    /* D */  {  0,   8,  12,  16,  16,  12,   8,   0},
    /* E */  {  0,   8,  12,  16,  16,  12,   8,   0},
    /* F */  {  0,   8,   8,  12,  12,   8,   8,   0},
    /* G */  {-20,   0,   8,   8,   8,   8,   0, -20},
    /* H */  {-20, -20,   0,   0,   0,   0, -20, -20}},
            //  1    2    3    4    5    6    7    8

            // White king:
    /* A */ {{-40, -20, -20, -20, -20, -20, -20, -40},
    /* B */  {-20,   0,  20,  20,  30,  30,  30, -20},
    /* C */  {-20,   0,  20,  40,  60,  60,  40, -20},
    /* D */  {-20,   0,  20,  40,  60,  60,  40, -20},
    /* E */  {-20,   0,  20,  40,  60,  60,  40, -20},
    /* F */  {-20,   0,  20,  40,  60,  60,  40, -20},
    /* G */  {-20,   0,  20,  20,  30,  30,  30, -20},
    /* H */  {-40, -20, -20, -20, -20, -20, -20, -40}}
            //  1    2    3    4    5    6    7    8
};

/// The values of having the white king on different squares during an endgame
/// with pawns (of both colors) only on the queen side:
const value_t board_heuristic::value_king_position[8][8] =
{
    /* A */ {-20,   0,  20,  40,  40,  40,  40, -20},
    /* B */ {-20,   0,  20,  40,  60,  60,  40, -20},
    /* C */ {-20,   0,  20,  40,  60,  60,  40, -20},
    /* D */ {-20,   0,  20,  40,  60,  60,  40, -20},
    /* E */ {-20,   0,  20,  20,  20,  20,  20, -20},
    /* F */ {-20, -20, -20, -20, -20, -20, -20, -20},
    /* G */ {-40, -40, -40, -40, -40, -40, -40, -40},
    /* H */ {-60, -60, -60, -60, -60, -60, -60, -60}
            // 1    2    3    4    5    6    7    8
};

// The values of various pawn structure features:
const value_t board_heuristic::value_pawn_passed[8]           = {0, 12,  20,  48,  72, 120, 150,   0};
const value_t board_heuristic::value_pawn_doubled[9]          = {0,  0,  -4,  -7, -10, -10, -10, -10, -10};
const value_t board_heuristic::value_pawn_isolated[9]         = {0, -8, -20, -40, -60, -70, -80, -80, -80};
const value_t board_heuristic::value_pawn_doubled_isolated[9] = {0, -5, -10, -15, -15, -15, -15, -15, -15};
const value_t board_heuristic::value_pawn_duo = 2;

//
const value_t board_heuristic::value_knight_outpost[8][8] =
{
    /* A */ {  0,   0,   0,   0,   0,   0,   0,   0},
    /* B */ {  0,   0,   0,   5,   5,   0,   0,   0},
    /* C */ {  0,   0,   0,  10,  10,  10,   0,   0},
    /* D */ {  0,   0,   0,  20,  24,  24,   0,   0},
    /* E */ {  0,   0,   0,  20,  24,  24,   0,   0},
    /* F */ {  0,   0,   0,  10,  10,  10,   0,   0},
    /* G */ {  0,   0,   0,   5,   5,   0,   0,   0},
    /* H */ {  0,   0,   0,   0,   0,   0,   0,   0}
            // 1    2    3    4    5    6    7    8
};

//
const value_t board_heuristic::value_bishop_over_knight = 36;
const value_t board_heuristic::value_bishop_trapped = -174;

//
const value_t board_heuristic::value_rook_on_7th = 24;
const value_t board_heuristic::value_rooks_on_7th = 10;

//
const value_t board_heuristic::value_queen_rook_on_7th = 50;
const value_t board_heuristic::value_queen_offside = -30;

// The penalty for giving up castling:
const value_t board_heuristic::value_king_cant_castle = -20;

bool board_heuristic::precomputed_board_heuristic = false;
bitboard_t board_heuristic::squares_pawn_duo[8][8];
bitboard_t board_heuristic::squares_pawn_potential_attacks[COLORS][8][8];
pawn board_heuristic::pawn_table;

/*----------------------------------------------------------------------------*\
 |                             board_heuristic()                              |
\*----------------------------------------------------------------------------*/
board_heuristic::board_heuristic() : board_base()
{

/// Constructor.

    if (!precomputed_board_heuristic)
    {
        precomp_pawn();
        precomputed_board_heuristic = true;
    }
}

/*----------------------------------------------------------------------------*\
 |                             ~board_heuristic()                             |
\*----------------------------------------------------------------------------*/
board_heuristic::~board_heuristic()
{

/// Destructor.

}

/*----------------------------------------------------------------------------*\
 |                                     =                                      |
\*----------------------------------------------------------------------------*/
board_heuristic& board_heuristic::operator=(const board_heuristic& that)
{

/// Overloaded assignment operator.

    if (this != &that)
        board_base::operator=(that);
    return *this;
}

/*----------------------------------------------------------------------------*\
 |                                 evaluate()                                 |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate(int depth) const
{

/// Evaluate the current state.  For simplicity's sake, evaluate from the
/// perspective of the player who's just moved (the color that's off move).

    value_t sum = 0;

    if (!state.piece[ON_MOVE][KING])
        return VALUE_ILLEGAL;

    sum += evaluate_pawns();
    sum += evaluate_knights();
    sum += evaluate_bishops();
    sum += evaluate_rooks();
    sum += evaluate_queens();
    sum += evaluate_kings(depth);
    return sum;
}

/*----------------------------------------------------------------------------*\
 |                              evaluate_pawns()                              |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_pawns() const
{

/// Evaluate pawn structure.

    value_t sign, sum = 0;

    // If we've already evaluated this pawn structure, return our previous
    // evaluation.
    if (pawn_table.probe(pawn_hash, &sum))
        goto end;

    for (int color = WHITE; color <= BLACK; color++)
    {
        sign = !color ? 1 : -1;
        bitboard_t b = state.piece[color][PAWN];
        int num_isolated = 0;
        for (int n, x, y; (n = FST(b)) != -1; BIT_CLR(b, x, y))
        {
            x = n & 0x7;
            y = n >> 3;

            // Reward material.
            sum += sign * value_material[PAWN];

            // Penalize bad position or reward good position.
            if (color == WHITE)
              sum += sign * value_position[PAWN][x][y];
            else
              sum += sign * value_position[PAWN][7-x][7-y];

            //
            bitboard_t pawns = state.piece[color][PAWN];
            bitboard_t pawns_on_col = pawns & COL_MSK(y);
            int num_on_col = count_64(pawns_on_col);

            if (!(pawns & squares_adj_cols[x]))
            {
                // Count isolated pawns and penalize isolated doubled pawns.
                num_isolated++;
                sum += sign * value_pawn_doubled_isolated[num_on_col];
            }
            else
            {
                // TODO: Penalize weak pawns.

                // Penalize doubled pawns.
                sum += sign * value_pawn_doubled[num_on_col];

                // Reward pawn duos.
                if (pawns & squares_pawn_duo[x][y])
                    sum += sign * value_pawn_duo;
            }

            // Reward passed pawns.
            if (!(state.piece[!color][PAWN] & squares_pawn_potential_attacks[!color][x][y]))
                sum += sign * value_pawn_passed[!color ? y : 7 - y];

            // TODO: Reward hidden passed pawns.
        }

        // Penalize isolated pawns.
        sum += sign * value_pawn_isolated[num_isolated];
    }

    pawn_table.store(pawn_hash, sum);
end:
    sign = !OFF_MOVE ? 1 : -1;
    return sign * sum;
}

/*----------------------------------------------------------------------------*\
 |                             evaluate_knights()                             |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_knights() const
{

///

    value_t sign, sum = 0;
    bitboard_t b;

    for (int color = WHITE; color <= BLACK; color++)
    {
        sign = color == OFF_MOVE ? 1 : -1;
        b = state.piece[color][KNIGHT];
        for (int n, x, y; (n = FST(b)) != -1; BIT_CLR(b, x, y))
        {
            x = n & 0x7;
            y = n >> 3;

            // Reward material.
            sum += sign * value_material[KNIGHT];

            // Penalize bad position or reward good position.
            sum += sign * value_position[KNIGHT][x][y];

            // Reward outposts.
            bitboard_t pawn_potential_attacks =
                squares_pawn_potential_attacks[!color][x][y] &
                state.piece[!color][PAWN];
            if (!pawn_potential_attacks)
            {
                bitboard_t pawn_defenses =
                    squares_pawn_attacks[color][x][y] &
                    state.piece[color][PAWN];
                if (pawn_defenses)
                {
                  if (color == WHITE)
                    sum += sign * value_knight_outpost[x][y];
                  else
                    sum += sign * value_knight_outpost[7-x][7-y];
                }
            }
            // TODO: Reward blocking center pawns.
        }
    }
    return sum;
}

/*----------------------------------------------------------------------------*\
 |                             evaluate_bishops()                             |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_bishops() const
{

///

    value_t sign, sum = 0;
    bitboard_t b, squares_both_sides, all_pawns;
    bool pawns_both_sides, enemy_bishop_present;

    for (int color = WHITE; color <= BLACK; color++)
    {
        sign = color == OFF_MOVE ? 1 : -1;
        b = state.piece[color][BISHOP];
        for (int n, x, y; (n = FST(b)) != -1; BIT_CLR(b, x, y))
        {
            x = n & 0x7;
            y = n >> 3;

            // Reward material.
            sum += sign * value_material[BISHOP];

            // Penalize bad position or reward good position.
            sum += sign * value_position[BISHOP][x][y];

            // TODO: Reward blocking center pawns.

            // Reward bishops (over knights) during endgames with pawns on both
            // sides of the board.
            int friendly_piece_count = count_64(ALL(state, color));
            bool endgame = friendly_piece_count < 7;
            if (!endgame)
                goto no_bishop_over_knight;
            enemy_bishop_present = state.piece[!color][BISHOP];
            if (enemy_bishop_present)
                goto no_bishop_over_knight;
            all_pawns = state.piece[WHITE][PAWN] | state.piece[BLACK][PAWN];
            squares_both_sides = COL_MSK(0) | COL_MSK(1) | COL_MSK(2) |
                                 COL_MSK(5) | COL_MSK(6) | COL_MSK(7);
            pawns_both_sides = all_pawns & squares_both_sides;
            if (!pawns_both_sides)
                goto no_bishop_over_knight;
            sum += sign * value_bishop_over_knight;
no_bishop_over_knight:

            // Penalize trapped or potentially trapped bishops.
            if (color == WHITE)
            {
                if ((x == 0 && y == 6 && BIT_GET(state.piece[BLACK][PAWN], 1, 5)) ||
                    (x == 1 && y == 7 && BIT_GET(state.piece[BLACK][PAWN], 2, 6)) ||
                    (x == 7 && y == 6 && BIT_GET(state.piece[BLACK][PAWN], 6, 5)) ||
                    (x == 6 && y == 7 && BIT_GET(state.piece[BLACK][PAWN], 5, 6)))
                    sum += sign * value_bishop_trapped;
            }
            else // color == BLACK
            {
                if ((x == 0 && y == 1 && BIT_GET(state.piece[WHITE][PAWN], 1, 2)) ||
                    (x == 1 && y == 0 && BIT_GET(state.piece[WHITE][PAWN], 2, 1)) ||
                    (x == 7 && y == 1 && BIT_GET(state.piece[WHITE][PAWN], 6, 2)) ||
                    (x == 6 && y == 0 && BIT_GET(state.piece[WHITE][PAWN], 5, 1)))
                    sum += sign * value_bishop_trapped;
            }

            // TODO: Penalize bad bishops.

            // TODO: Reward having two bishops.
        }
    }
    return sum;
}

/*----------------------------------------------------------------------------*\
 |                              evaluate_rooks()                              |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_rooks() const
{

///

    value_t sign, sum = 0;
    bitboard_t b, rooks_on_7th, rooks, enemy_pawns, seventh_row, enemy_king,
        eighth_row;
    int num_rooks_on_7th, eighth;
    bool is_enemy_pawn_on_7th, is_enemy_king_on_8th;

    for (int color = WHITE; color <= BLACK; color++)
    {
        sign = color == OFF_MOVE ? 1 : -1;
        b = state.piece[color][ROOK];
        for (int n, x, y; (n = FST(b)) != -1; BIT_CLR(b, x, y))
        {
            x = n & 0x7;
            y = n >> 3;

            // Reward material.
            sum += sign * value_material[ROOK];

            // Penalize bad position or reward good position.
            sum += sign * value_position[ROOK][x][y];

            //
            int seventh = color == WHITE ? 6 : 1;
            bool is_rook_on_7th = y == seventh;
            if (!is_rook_on_7th)
                goto no_rook_on_7th;
            enemy_pawns = state.piece[!color][PAWN];
            seventh_row = ROW_MSK(seventh);
            is_enemy_pawn_on_7th = enemy_pawns & seventh_row;
            enemy_king = state.piece[!color][KING];
            eighth = color == WHITE ? 7 : 0;
            eighth_row = ROW_MSK(eighth);
            is_enemy_king_on_8th = enemy_king & eighth_row;
            if (!is_enemy_pawn_on_7th && !is_enemy_king_on_8th)
                goto no_rook_on_7th;
            sum += sign * value_rook_on_7th;
            rooks = state.piece[color][ROOK];
            rooks_on_7th = rooks & seventh_row;
            num_rooks_on_7th = count_64(rooks_on_7th);
            if (num_rooks_on_7th < 2)
                goto no_rook_on_7th;
            sum += sign * value_rooks_on_7th;
no_rook_on_7th: 
            ;
        }
    }
    return sum;
}

/*----------------------------------------------------------------------------*\
 |                             evaluate_queens()                              |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_queens() const
{

///

    value_t sign, sum = 0;
    bitboard_t b, rooks, enemy_pawns, seventh_row, enemy_king, eighth_row;
    int seventh, eighth;
    bool is_rook_on_7th, is_queen_on_7th, is_enemy_pawn_on_7th,
        is_enemy_king_on_8th;

    for (int color = WHITE; color <= BLACK; color++)
    {
        sign = color == OFF_MOVE ? 1 : -1;
        b = state.piece[color][QUEEN];
        for (int n, x, y; (n = FST(b)) != -1; BIT_CLR(b, x, y))
        {
            x = n & 0x7;
            y = n >> 3;

            // Reward material.
            sum += sign * value_material[QUEEN];

            // Penalize bad position or reward good position.
            sum += sign * value_position[QUEEN][x][y];

            //
            seventh = color == WHITE ? 6 : 1;
            is_queen_on_7th = y == seventh;
            if (!is_queen_on_7th)
                goto no_queen_on_7th;
            enemy_pawns = state.piece[!color][PAWN];
            seventh_row = ROW_MSK(seventh);
            is_enemy_pawn_on_7th = enemy_pawns & seventh_row;
            enemy_king = state.piece[!color][KING];
            eighth = color == WHITE ? 7 : 0;
            eighth_row = ROW_MSK(eighth);
            is_enemy_king_on_8th = enemy_king & eighth_row;
            if (!is_enemy_pawn_on_7th && !is_enemy_king_on_8th)
                goto no_queen_on_7th;
            rooks = state.piece[color][ROOK];
            is_rook_on_7th = rooks & seventh_row;
            if (!is_rook_on_7th)
                goto no_queen_on_7th;
            sum += sign * value_queen_rook_on_7th;
no_queen_on_7th:

            //
            if (count_64(state.piece[color][PAWN]) > 4)
            {
                int enemy_king_n = FST(state.piece[!color][KING]);
                int enemy_king_x = enemy_king_n & 0x7;
                if (x <= 1 && enemy_king_x >= 5 ||
                    x >= 6 && enemy_king_x <= 2)
                    sum += sign * value_queen_offside;
            }
        }
    }
    return sum;
}

/*----------------------------------------------------------------------------*\
 |                              evaluate_kings()                              |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_kings(int depth) const
{

/// Evaluate king position.

    value_t sign, sum = 0;

    for (int color = WHITE; color <= BLACK; color++)
    {
        sign = color == OFF_MOVE ? 1 : -1;
        int n = FST(state.piece[color][KING]);
        int x = n & 0x7;
        int y = n >> 3;

        // Penalize giving up castling.
        if (state.castle[color][QUEEN_SIDE] == CANT_CASTLE &&
            state.castle[color][ KING_SIDE] == CANT_CASTLE)
            sum += sign * value_king_cant_castle;

        
        // Penalize bad position or reward good position.
        bitboard_t pawns = state.piece[WHITE][PAWN] | state.piece[BLACK][PAWN];
        if (pawns & SQUARES_QUEEN_SIDE && pawns & SQUARES_KING_SIDE)
        {
            // There are pawns on both sides of the board.  The king should be
            // in the middle.
            if (color == WHITE)
              sum += sign * value_position[KING][x][y];
            else
              sum += sign * value_position[KING][7-x][7-y];
        }
        else if (pawns)
        {
            // There are pawns on only one side of the board.  The king should
            // be on that side.
            if (pawns & SQUARES_QUEEN_SIDE)
            {
              sum += sign * value_king_position[x][!color?y:7-y];
            }
            else
            {
              sum += sign * value_king_position[7-x][!color?y:7-y];
            }
        }
    }
    return sum;
}

/*----------------------------------------------------------------------------*\
 |                               precomp_pawn()                               |
\*----------------------------------------------------------------------------*/
void board_heuristic::precomp_pawn()
{
    // A pawn duo is two friendly pawns that are side by side.  In endgame, a
    // pawn duo is powerful because the two pawns can advance together and
    // protect one another.  For each square on the board, pretend that a pawn
    // is on that square and compute the squares on which one friendly pawn must
    // sit in order to form a pawn duo.
    for (int n = 0; n <= 63; n++)
    {
        int x = n & 0x7;
        int y = n >> 3;

        squares_pawn_duo[x][y] = 0;
        if (y == 0 || y == 7)
            // There can never be any pawn (of either color) on rank 1 or rank
            // 7.
            continue;
        squares_pawn_duo[x][y] = squares_adj_cols[x] & ROW_MSK(y);
    }

    for (int color = WHITE; color <= BLACK; color++)
    {
        int sign = color == WHITE ? -1 : 1;
        for (int n = 0; n <= 63; n++)
        {
            int x = n & 0x7;
            int y = n >> 3;

            squares_pawn_potential_attacks[color][x][y] = 0;
            for (int k = y + sign; k >= 1 && k <= 6; k += sign)
                squares_pawn_potential_attacks[color][x][y] |=
                    squares_adj_cols[x] & ROW_MSK(k);
        }
    }
}
