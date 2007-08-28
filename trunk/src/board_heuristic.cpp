/*----------------------------------------------------------------------------*\
 |	board_heuristic.cpp - heuristic evaluation implementation	      |
 |									      |
 |	Copyright � 2005-2007, The Gray Matter Team, original authors.	      |
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
static int position[SHAPES][8][8] =
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

static int king_position[SIDES][8][8]
{
	       // White king:
	/* A */ {{-20,   0,  20,  40,  40,  40,  40, -20},
	/* B */  {-20,   0,  20,  40,  60,  60,  40, -20},
	/* C */  {-20,   0,  20,  40,  60,  60,  40, -20},
	/* D */  {-20,   0,  20,  40,  60,  60,  40, -20},
	/* E */  {-20,   0,  20,  20,  20,  20,  20, -20},
	/* F */  {-20, -20, -20, -20, -20, -20, -20, -20},
	/* G */  {-40, -40, -40, -40, -40, -40, -40, -40},
	/* H */  {-60, -60, -60, -60, -60, -60, -60, -60}},
	       //   1    2    3    4    5    6    7    8

	       // White king:
	/* A */ {{-60, -60, -60, -60, -60, -60, -60, -60},
	/* B */  {-40, -40, -40, -40, -40, -40, -40, -40},
	/* C */  {-20, -20, -20, -20, -20, -20, -20, -20},
	/* D */  {-20,   0,  20,  20,  20,  20,  20, -20},
	/* E */  {-20,   0,  20,  40,  60,  60,  40, -20},
	/* F */  {-20,   0,  20,  40,  60,  60,  40, -20},
	/* G */  {-20,   0,  20,  40,  60,  60,  40, -20},
	/* H */  {-20,   0,  20,  40,  40,  40,  40, -20}}
	       //   1    2    3    4    5    6    7    8
};

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
	bitboard_t pawns, adj_files, adj_pawns, ranks;
	int num_isolated = 0, num_isolated_open_file = 0;

	static const int weight_isolated[] = {80, 80, 80, 70, 60, 40, 20, 8, 0, -8, -20, -40, -60, -70, -80, -80, -80};
	static const int weight_isolated_open_file[] = {24, 24, 24, 24, 24, 16, 10, 4, 0, -4, -10, -16, -24, -24, -24, -24, -24};
	static const int weight_doubled[] = {0, 0, 4, 7, 10, 10, 10, 10, 10};
	static const int weight_passed[] = {0, 12, 20, 48, 72, 120, 150, 0};

	// If we've already evaluated this pawn structure, return our previous
	// evaluation.
	if (pawn_table.probe(pawn_hash, &sum))
		goto end;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == WHITE ? 1 : -1;
		for (int file = 0; file <= 7; file++)
		{
			pawns = state.piece[color][PAWN] & COL_MSK(file);
			if (!(coef = count_64(pawns)))
				// The current color has no pawn on the current
				// file.  Move on to the next file.
				continue;
			adj_files = 0;
			for (int j = file == 0 ? 1 : -1; j <= (file == 7 ? -1 : 1); j += 2)
				adj_files |= COL_MSK(file + j);
			adj_pawns = state.piece[color][PAWN] & adj_files;

			// Count isolated pawns and isolated pawns on open
			// files.
			if (!adj_pawns)
			{
				num_isolated += sign * coef;
				if (!(state.piece[!color][PAWN] & COL_MSK(file)))
					num_isolated_open_file += sign * coef;
			}

			// Penalize doubled pawns.
			sum += sign * coef * weight_doubled[coef];

			for (int n, x, y; (n = FST(pawns)) != -1; BIT_CLR(pawns, x, y))
			{
				x = n & 0x7;
				y = n >> 3;

				// Reward pawn duos.
				for (int j = x == 0 ? 1 : -1; j <= (x == 7 ? -1 : 1); j += 2)
					if (BIT_GET(state.piece[color][PAWN], x + j, y))
					{
						sum += sign * WEIGHT_DUO;
						break;
					}

				// Reward passed pawns.
				ranks = 0;
				for (int k = y + sign; k < 7 && k > 0; k += sign)
					ranks |= ROW_MSK(k);
				if (!(state.piece[!color][PAWN] & adj_files & ranks))
					sum += sign * weight_passed[color == WHITE ? y : 7 - y];

				// TODO: Reward hidden passed pawns.

				// Reward position and material.
				sum += sign * position[KNIGHT][x][color == WHITE ? y : 7 - y];
				sum += sign * WEIGHT_PAWN;
			}
		}
	}

	// Penalize isolated pawns and isolated pawns on open files.
	sum += weight_isolated[num_isolated + 9];
	sum += weight_isolated_open_file[num_isolated_open_file + 9];

	pawn_table.store(pawn_hash, sum);
end:
	sign = OFF_MOVE == WHITE ? 1 : -1;
	return sign * sum;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_knights()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_knights() const
{
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
			sum += sign * WEIGHT_KNIGHT;

			// Reward position.
			sum += sign * position[KNIGHT][x][y];
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_bishops()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_bishops() const
{
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
			sum += sign * WEIGHT_BISHOP;

			// Reward position.
			sum += sign * position[BISHOP][x][y];
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_rooks()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_rooks() const
{
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
			sum += sign * WEIGHT_ROOK;

			// Reward position.
			sum += sign * position[ROOK][x][y];
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_queens()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_queens() const
{
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
			sum += sign * WEIGHT_QUEEN;

			// Reward position.
			sum += sign * position[QUEEN][x][y];
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

		// Penalize not castling.
		if (state.castle[color][QUEEN_SIDE] == CANT_CASTLE &&
		    state.castle[color][KING_SIDE]  == CANT_CASTLE)
			sum += sign * WEIGHT_CANT_CASTLE;

		// Reward position.
		if (pawns & SQUARES_QUEEN_SIDE && pawns & SQUARES_KING_SIDE)
			sum += sign * position[KING][x][color == WHITE ? y : 7 - y];
		else if (pawns & SQUARES_QUEEN_SIDE)
			sum += sign * king_position[QUEEN_SIDE][x][color == WHITE ? y : 7 - y];
		else if (pawns)
			sum += sign * king_position[KING_SIDE][x][color == WHITE ? y : 7 - y];
	}
	return sum;
}
