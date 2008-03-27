/*----------------------------------------------------------------------------*\
 |	board_heuristic.h - heuristic evaluation interface		      |
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

#ifndef BOARD_HEURISTIC_H
#define BOARD_HEURISTIC_H

using namespace std;

// Default Gray Matter stuff:
#include "const.h"
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "board_base.h"
#include "table.h"

// Forward declarations:
class pawn_table;

/// Heuristic evaluation.
class board_heuristic : public board_base
{
public:
	board_heuristic();
	~board_heuristic();
	board_heuristic &operator=(const board_heuristic &that);
	value_t evaluate(int depth) const;

private:
	// The values of the pieces:
	static const value_t value_material[SHAPES] = {VALUE_PAWN, VALUE_KNIGHT, VALUE_BISHOP, VALUE_ROOK, VALUE_QUEEN, VALUE_KING};

	// The values of having different pieces on different squares:
	static const value_t value_position[SHAPES][8][8] =
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
		       //   1    2    3    4    5    6    7    8

		       // Knights:
		/* A */ {{-60, -30, -30, -30, -30, -30, -30, -60},
		/* B */  {-30, -24,  -6,  -6,  -6,  -6, -24, -30},
		/* C */  {-30, -10,  -6,  -2,  -2,  -6, -10, -30},
		/* D */  {-30, -10,  -6,   0,   0,  -6, -10, -30},
		/* E */  {-30, -10,  -6,   0,   0,  -6, -10, -30},
		/* F */  {-30, -10,  -6,  -2,  -2,  -6, -10, -30},
		/* G */  {-30, -24,  -6,  -6,  -6,  -6, -24, -30},
		/* H */  {-60, -30, -30, -30, -30, -30, -30, -60}},
		       //   1    2    3    4    5    6    7    8

		       // Bishops:
		/* A */ {{-20, -20, -20, -20, -20, -20, -20, -20},
		/* B */  {-20,   6,   6,   3,   3,   6,   6, -20},
		/* C */  {-20,   6,   8,   6,   6,   8,   6, -20},
		/* D */  {-20,   3,   6,  10,  10,   6,   3, -20},
		/* E */  {-20,   3,   6,  10,  10,   6,   3, -20},
		/* F */  {-20,   6,   8,   6,   6,   8,   6, -20},
		/* G */  {-20,   6,   6,   3,   3,   6,   6, -20},
		/* H */  {-20, -20, -20, -20, -20, -20, -20, -20}},
		       //   1    2    3    4    5    6    7    8

		       // Rooks:
		/* A */ {{-10, -10, -10, -10, -10, -10, -10, -10},
		/* B */  { -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6},
		/* C */  { -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2},
		/* D */  {  2,   2,   2,   2,   2,   2,   2,   2},
		/* E */  {  2,   2,   2,   2,   2,   2,   2,   2},
		/* F */  { -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2},
		/* G */  { -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6},
		/* H */  {-10, -10, -10, -10, -10, -10, -10, -10}},
		       //   1    2    3    4    5    6    7    8

		       // Queens:
		/* A */ {{-20, -20,   0,   0,   0,   0, -20, -20},
		/* B */  {-20,   0,   8,   8,   8,   8,   0, -20},
		/* C */  {  0,   8,   8,  12,  12,   8,   8,   0},
		/* D */  {  0,   8,  12,  16,  16,  12,   8,   0},
		/* E */  {  0,   8,  12,  16,  16,  12,   8,   0},
		/* F */  {  0,   8,   8,  12,  12,   8,   8,   0},
		/* G */  {-20,   0,   8,   8,   8,   8,   0, -20},
		/* H */  {-20, -20,   0,   0,   0,   0, -20, -20}},
		       //   1    2    3    4    5    6    7    8

		       // White king:
		/* A */ {{-40, -20, -20, -20, -20, -20, -20, -40},
		/* B */  {-20,   0,  20,  20,  30,  30,  30, -20},
		/* C */  {-20,   0,  20,  40,  60,  60,  40, -20},
		/* D */  {-20,   0,  20,  40,  60,  60,  40, -20},
		/* E */  {-20,   0,  20,  40,  60,  60,  40, -20},
		/* F */  {-20,   0,  20,  40,  60,  60,  40, -20},
		/* G */  {-20,   0,  20,  20,  30,  30,  30, -20},
		/* H */  {-40, -20, -20, -20, -20, -20, -20, -40}}
		       //   1    2    3    4    5    6    7    8
	};

	// The values of having the white king on different squares during an endgame
	// with pawns (of both colors) only on the queen side:
	static const value_t value_king_position[8][8] =
	{
		/* A */ {-20,   0,  20,  40,  40,  40,  40, -20},
		/* B */ {-20,   0,  20,  40,  60,  60,  40, -20},
		/* C */ {-20,   0,  20,  40,  60,  60,  40, -20},
		/* D */ {-20,   0,  20,  40,  60,  60,  40, -20},
		/* E */ {-20,   0,  20,  20,  20,  20,  20, -20},
		/* F */ {-20, -20, -20, -20, -20, -20, -20, -20},
		/* G */ {-40, -40, -40, -40, -40, -40, -40, -40},
		/* H */ {-60, -60, -60, -60, -60, -60, -60, -60}
		       //  1    2    3    4    5    6    7    8
	};

	// The values of various pawn structure features:
	static const value_t value_pawn_passed[8]           = {0, 12,  20,  48,  72, 120, 150,   0};
	static const value_t value_pawn_doubled[9]          = {0,  0,  -4,  -7, -10, -10, -10, -10, -10};
	static const value_t value_pawn_isolated[9]         = {0, -8, -20, -40, -60, -70, -80, -80, -80};
	static const value_t value_pawn_doubled_isolated[9] = {0, -5, -10, -15, -15, -15, -15, -15, -15};
	static const value_t value_pawn_duo = 2;

	//
	static const value_t value_knight_outpost[8][8] =
	{
		/* A */ {  0,   0,   0,   0,   0,   0,   0,   0},
		/* B */ {  0,   0,   0,   5,   5,   0,   0,   0},
		/* C */ {  0,   0,   0,  10,  10,  10,   0,   0},
		/* D */ {  0,   0,   0,  20,  24,  24,   0,   0},
		/* E */ {  0,   0,   0,  20,  24,  24,   0,   0},
		/* F */ {  0,   0,   0,  10,  10,  10,   0,   0},
		/* G */ {  0,   0,   0,   5,   5,   0,   0,   0},
		/* H */ {  0,   0,   0,   0,   0,   0,   0,   0}
		       //  1    2    3    4    5    6    7    8
	};

	//
	static const value_t value_bishop_over_knight = 36;
	static const value_t value_bishop_trapped = -174;

	//
	static const value_t value_rook_on_7th = 24;
	static const value_t value_rooks_on_7th = 10;

	//
	static const value_t value_queen_rook_on_7th = 50;
	static const value_t value_queen_offside = -30;

	// The penalty for giving up castling:
	static const value_t value_king_cant_castle = -20;

	//
	bool precomputed_board_heuristic = false;
	extern bitboard_t squares_adj_cols[];
	bitboard_t squares_pawn_duo[8][8];
	bitboard_t squares_pawn_potential_attacks[COLORS][8][8];
	bitboard_t squares_pawn_defenses[COLORS][8][8];

	// Since pawn structure remains relatively static, we maintain a hash
	// table of previous pawn structure evaluations.  According to my tests,
	// this hash table sustains a hit rate of around 97%.  This enables us
	// to perform sophisticated pawn structure analysis almost for free.
	static pawn pawn_table;

	value_t evaluate_pawns() const;
	value_t evaluate_knights() const;
	value_t evaluate_bishops() const;
	value_t evaluate_rooks() const;
	value_t evaluate_queens() const;
	value_t evaluate_kings(int depth) const;
	void precomp_pawn() const;
};

#endif
