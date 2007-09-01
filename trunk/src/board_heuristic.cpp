/*----------------------------------------------------------------------------*\
 |	board_heuristic.cpp - heuristic evaluation implementation	      |
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

#include "gray.h"
#include "board_heuristic.h"

//
extern bitboard_t adj_files[8];
extern bitboard_t pawn_attacks[COLORS][8][8];
extern bitboard_t potential_pawn_attacks[COLORS][8][8];
extern bitboard_t pawn_duo[8][8];

//
static int weight_material[SHAPES] = {WEIGHT_PAWN, WEIGHT_KNIGHT, WEIGHT_BISHOP,
                                      WEIGHT_ROOK, WEIGHT_QUEEN,  WEIGHT_KING};

//
static int weight_position[SHAPES][8][8] =
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

//
static int weight_king_position[8][8] =
{
	       // White king:
	/* A */ {-20,   0,  20,  40,  40,  40,  40, -20},
	/* B */ {-20,   0,  20,  40,  60,  60,  40, -20},
	/* C */ {-20,   0,  20,  40,  60,  60,  40, -20},
	/* D */ {-20,   0,  20,  40,  60,  60,  40, -20},
	/* E */ {-20,   0,  20,  20,  20,  20,  20, -20},
	/* F */ {-20, -20, -20, -20, -20, -20, -20, -20},
	/* G */ {-40, -40, -40, -40, -40, -40, -40, -40},
	/* H */ {-60, -60, -60, -60, -60, -60, -60, -60}
	       //   1    2    3    4    5    6    7    8
};

//
static const int weight_pawn_isolated[9] = {0, -8, -20, -40, -60, -70, -80, -80, -80};
static const int weight_pawn_isolated_doubled[9] = {0, -5, -10, -15, -15, -15, -15, -15, -15};
static const int weight_pawn_isolated_open_file[9] = {0, -4, -10, -16, -24, -24, -24, -24, -24};
static const int weight_pawn_weak[2] = {12, 20};
static const int weight_pawn_doubled[9] = {0, 0, -4, -7, -10, -10, -10, -10, -10};
static const int weight_pawn_duo = 2;
static const int weight_pawn_passed[8] = {0, 12, 20, 48, 72, 120, 150, 0};
static const int weight_pawn_hidden_passed[8] = {0, 0, 0, 0, 20, 40, 0, 0};

//
static const int weight_knight_outpost[8][8] =
{
	/* A */  {  0,   0,   0,   0,   0,   0,   0,   0},
	/* B */  {  0,   0,   0,   5,   5,   0,   0,   0},
	/* C */  {  0,   0,   0,  10,  10,  10,   0,   0},
	/* D */  {  0,   0,   0,  20,  24,  24,   0,   0},
	/* E */  {  0,   0,   0,  20,  24,  24,   0,   0},
	/* F */  {  0,   0,   0,  10,  10,  10,   0,   0},
	/* G */  {  0,   0,   0,   5,   5,   0,   0,   0},
	/* H */  {  0,   0,   0,   0,   0,   0,   0,   0}
	       //   1    2    3    4    5    6    7    8
};

// The penalty for giving up castling:
static const int weight_cant_castle = -20;

// Since pawn structure remains relatively static, we maintain a hash table of
// previous pawn structure evaluations.  According to my tests, this hash table
// sustains a hit rate of around 97%.  This enables us to perform sophisticated
// pawn structure analysis pretty much for free.
pawn pawn_table;

/*----------------------------------------------------------------------------*\
 |			       board_heuristic()			      |
\*----------------------------------------------------------------------------*/
board_heuristic::board_heuristic() : board_base()
{

// Constructor.

}

/*----------------------------------------------------------------------------*\
 |			       ~board_heuristic()			      |
\*----------------------------------------------------------------------------*/
board_heuristic::~board_heuristic()
{

// Destructor.

}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
board_heuristic& board_heuristic::operator=(const board_heuristic& that)
{

// Overloaded assignment operator.

	if (this != &that)
		board_base::operator=(that);
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				   evaluate()				      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate() const
{

// Evaluate the current state.  For simplicity's sake, evaluate from the
// perspective of the player who's just moved (the color that's off move).

	int sum = 0;

	if (!state.piece[ON_MOVE][KING])
		return WEIGHT_ILLEGAL;

	sum += evaluate_pawns();
	sum += evaluate_knights();
	sum += evaluate_bishops();
	sum += evaluate_rooks();
	sum += evaluate_queens();
	sum += evaluate_kings();
	return sum;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_pawns()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_pawns() const
{

// Evaluate pawn structure.

	int sign, coef, sum;
	bitboard_t pawns, adj_pawns;
	int num_isolated[COLORS] = {0, 0}, num_isolated_open_file[COLORS] = {0, 0};

	// If we've already evaluated this pawn structure, return our previous
	// evaluation.
	if (pawn_table.probe(pawn_hash, &sum))
		goto end;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = !color ? 1 : -1;
		for (int file = 0; file <= 7; file++)
		{
			pawns = state.piece[color][PAWN] & COL_MSK(file);
			if (!(coef = count_64(pawns)))
				// The current color has no pawn on the current
				// file.  Move on to the next file.
				continue;
			adj_pawns = state.piece[color][PAWN] & adj_files[file];

			if (!adj_pawns)
			{
				// Count isolated pawns, penalize isolated
				// doubled pawns, and count isolated pawns on
				// open files.
				num_isolated[color] += coef;
				if (coef > 1)
					sum += sign * coef * weight_pawn_isolated_doubled[coef];
				if (!(state.piece[!color][PAWN] & COL_MSK(file)))
					num_isolated_open_file[color] += coef;
			}
			else
			{
				// TODO: Penalize weak pawns.

				// Penalize doubled pawns.
				if (coef > 1)
					sum += sign * coef * weight_pawn_doubled[coef];
			}

			for (int n, x, y; (n = FST(pawns)) != -1; BIT_CLR(pawns, x, y))
			{
				x = n & 0x7;
				y = n >> 3;

				// Reward pawn duos.
				if (pawn_duo[x][y] & state.piece[color][PAWN])
					sum += sign * weight_pawn_duo;

				// Reward passed pawns.
				if (!(potential_pawn_attacks[!color][x][y] & state.piece[!color][PAWN]))
					sum += sign * weight_pawn_passed[!color ? y : 7 - y];

				// TODO: Reward hidden passed pawns.

				// Penalize bad position or reward good
				// position.
				sum += sign * weight_position[KNIGHT][x][!color ? y : 7 - y];

				// Reward material.
				sum += sign * weight_material[PAWN];
			}
		}
	}

	// Penalize isolated pawns and isolated pawns on open files.
	sum += weight_pawn_isolated[num_isolated[WHITE]] - weight_pawn_isolated[num_isolated[BLACK]];
	sum += weight_pawn_isolated_open_file[num_isolated_open_file[WHITE]] - weight_pawn_isolated_open_file[num_isolated_open_file[BLACK]];

	pawn_table.store(pawn_hash, sum);
end:
	sign = !OFF_MOVE ? 1 : -1;
	return sign * sum;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_knights()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_knights() const
{

//

	int sign, sum = 0;
	bitboard_t knights;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		knights = state.piece[color][KNIGHT];
		for (int n, x, y; (n = FST(knights)) != -1; BIT_CLR(knights, x, y))
		{
			x = n & 0x7;
			y = n >> 3;

			// Reward material.
			sum += sign * weight_material[KNIGHT];

			// Penalize bad position or reward good position.
			sum += sign * weight_position[KNIGHT][x][y];

			//
			if (pawn_attacks[color][x][y] & state.piece[color][PAWN] && !(potential_pawn_attacks[!color][x][y] & state.piece[!color][PAWN]))
				sum += weight_knight_outpost[x][!color ? y : 7 - y];
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_bishops()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_bishops() const
{

//

	int sign, sum = 0;
	bitboard_t bishops;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		bishops = state.piece[color][BISHOP];
		for (int n, x, y; (n = FST(bishops)) != -1; BIT_CLR(bishops, x, y))
		{
			x = n & 0x7;
			y = n >> 3;

			// Reward material.
			sum += sign * weight_material[BISHOP];

			// Penalize bad position or reward good position.
			sum += sign * weight_position[BISHOP][x][y];
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_rooks()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_rooks() const
{

//

	int sign, sum = 0;
	bitboard_t rooks;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		rooks = state.piece[color][ROOK];
		for (int n, x, y; (n = FST(rooks)) != -1; BIT_CLR(rooks, x, y))
		{
			x = n & 0x7;
			y = n >> 3;

			// Reward material.
			sum += sign * weight_material[ROOK];

			// Penalize bad position or reward good position.
			sum += sign * weight_position[ROOK][x][y];
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_queens()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_queens() const
{

//

	static const int queen_rook_on_7th = 50;
	static const int offside = -30;

	int sign, sum = 0;
	bitboard_t queens;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		queens = state.piece[color][QUEEN];
		for (int n, x, y; (n = FST(queens)) != -1; BIT_CLR(queens, x, y))
		{
			x = n & 0x7;
			y = n >> 3;

			// Reward material.
			sum += sign * weight_material[QUEEN];

			// Penalize bad position or reward good position.
			sum += sign * weight_position[QUEEN][x][y];

			//
			if (y == (!color ? 6 : 1) && state.piece[color][ROOK] & ROW_MSK(!color ? 6 : 1) && state.piece[!color][KING] & ROW_MSK(!color ? 7 : 0))
				sum += queen_rook_on_7th;

			//
			if (x <= 1 && FST(state.piece[!color][KING]) & 0x7 >= 5 || x >= 6 && FST(state.piece[!color][KING]) & 0x7 <= 2)
				sum += offside;
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_kings()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_kings() const
{

// Evaluate king position.

	int sign, sum = 0;
	bitboard_t pawns = state.piece[WHITE][PAWN] | state.piece[BLACK][PAWN];

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		int n = FST(state.piece[color][KING]);
		int x = n & 0x7;
		int y = n >> 3;

		// Penalize giving up castling.
		if (state.castle[color][QUEEN_SIDE] == CANT_CASTLE && state.castle[color][KING_SIDE] == CANT_CASTLE)
			sum += sign * weight_cant_castle;

		// Penalize bad position or reward good position.
		if (pawns & SQUARES_QUEEN_SIDE && pawns & SQUARES_KING_SIDE)
			sum += sign * weight_position[KING][x][!color ? y : 7 - y];
		else if (pawns)
			sum += sign * weight_king_position[pawns & SQUARES_QUEEN_SIDE ? x : 7 - x][!color ? y : 7 - y];
	}
	return sum;
}
