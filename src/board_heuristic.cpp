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

// We define a piece's tempo as the minimum number of moves required to move it
// from its starting position to its current position on an otherwise empty
// chess board.  Tempo is a measure of development.  Good chess players maximize
// their tempo and minimize their opponents' tempo.
//
// Given a white piece and its current position, this array gives its tempo.
// Since the white pieces' starting positions are reflections of the black
// pieces' starting positions, with this info, it's trivial to compute any black
// piece's tempo.
static int tempo[SHAPES][8][8] =
{
	       // Pawn:
	/* A */ {{  0,   0,   1,   1,   2,   3,   4,   0},
	/* B */  {  0,   0,   1,   1,   2,   3,   4,   0},
	/* C */  {  0,   0,   1,   1,   2,   3,   4,   0},
	/* D */  {  0,   0,   1,   1,   2,   3,   4,   0},
	/* E */  {  0,   0,   1,   1,   2,   3,   4,   0},
	/* F */  {  0,   0,   1,   1,   2,   3,   4,   0},
	/* G */  {  0,   0,   1,   1,   2,   3,   4,   0},
	/* H */  {  0,   0,   1,   1,   2,   3,   4,   0}},
	       //   1    2    3    4    5    6    7    8

	       // Knight:
	/* A */ {{  3,   2,   1,   2,   3,   4,   3,   4},
	/* B */  {  0,   3,   2,   3,   2,   3,   4,   4},
	/* C */  {  2,   2,   1,   2,   3,   3,   3,   4},
	/* D */  {  2,   1,   3,   2,   2,   3,   3,   4},
	/* E */  {  2,   1,   3,   2,   2,   3,   3,   4},
	/* F */  {  2,   2,   1,   2,   3,   3,   3,   4},
	/* G */  {  0,   3,   2,   3,   2,   3,   4,   4},
	/* H */  {  3,   2,   1,   2,   3,   4,   3,   4}},
	       //   1    2    3    4    5    6    7    8

	       // Bishop:
	/* A */ {{  2,   2,   1,   2,   2,   1,   2,   2},
	/* B */  {  2,   1,   2,   2,   1,   2,   2,   2},
	/* C */  {  0,   2,   2,   1,   2,   2,   2,   2},
	/* D */  {  2,   1,   1,   2,   2,   2,   2,   2},
	/* E */  {  2,   1,   1,   2,   2,   2,   2,   2},
	/* F */  {  0,   2,   2,   1,   2,   2,   2,   2},
	/* G */  {  2,   1,   2,   2,   1,   2,   2,   2},
	/* H */  {  2,   2,   1,   2,   2,   1,   2,   2}},
	       //   1    2    3    4    5    6    7    8

	       // Rook:
	/* A */ {{  0,   1,   1,   1,   1,   1,   1,   1},
	/* B */  {  1,   2,   2,   2,   2,   2,   2,   2},
	/* C */  {  1,   2,   2,   2,   2,   2,   2,   2},
	/* D */  {  1,   2,   2,   2,   2,   2,   2,   2},
	/* E */  {  1,   2,   2,   2,   2,   2,   2,   2},
	/* F */  {  1,   2,   2,   2,   2,   2,   2,   2},
	/* G */  {  1,   2,   2,   2,   2,   2,   2,   2},
	/* H */  {  0,   1,   1,   1,   1,   1,   1,   1}},
	       //   1    2    3    4    5    6    7    8

	       // Queen:
	/* A */ {{  1,   2,   2,   1,   2,   2,   2,   2},
	/* B */  {  1,   2,   1,   2,   2,   2,   2,   2},
	/* C */  {  1,   1,   2,   2,   2,   2,   2,   2},
	/* D */  {  0,   1,   1,   1,   1,   1,   1,   1},
	/* E */  {  1,   1,   2,   2,   2,   2,   2,   2},
	/* F */  {  1,   2,   1,   2,   2,   2,   2,   2},
	/* G */  {  1,   2,   2,   1,   2,   2,   2,   2},
	/* H */  {  1,   2,   2,   2,   1,   2,   2,   2}},
	       //   1    2    3    4    5    6    7    8

	       // King:
	/* A */ {{  3,   3,   3,   4,   4,   5,   6,   7},
	/* B */  {  2,   2,   3,   3,   4,   5,   6,   7},
	/* C */  {  1,   2,   2,   3,   4,   5,   6,   7},
	/* D */  {  1,   1,   2,   3,   4,   5,   6,   7},
	/* E */  {  0,   1,   2,   3,   4,   5,   6,   7},
	/* F */  {  1,   1,   2,   3,   4,   5,   6,   7},
	/* G */  {  1,   2,   2,   3,   4,   5,   6,   7},
	/* H */  {  2,   2,   3,   3,   4,   5,   6,   7}}
	       //   1    2    3    4    5    6    7    8
};

//
static int position[SHAPES][8][8] =
{
	       // Pawn:
	/* A */ {{  0,   0,   1,   3,   6,  10,  40,   0},
	/* B */  {  0,   0,   1,   3,   6,  10,  40,   0},
	/* C */  {  0,   0,   1,   3,   6,  10,  40,   0},
	/* D */  {  0, -10,  10,  13,  16,  30,  40,   0},
	/* E */  {  0, -10,  10,  13,  16,  30,  40,   0},
	/* F */  {  0,   0,   1,   3,   6,  10,  40,   0},
	/* G */  {  0,   0,   1,   3,   6,  10,  40,   0},
	/* H */  {  0,   0,   1,   3,   6,  10,  40,   0}},
	       //   1    2    3    4    5    6    7    8

	       // Knight:
	/* A */ {{-60, -30, -30, -30, -30, -30, -30, -60},
	/* B */  {-30, -24,  -6,  -6,  -6,  -6, -24, -30},
	/* C */  {-30, -10,  -6,  -2,  -2,  -6, -10, -30},
	/* D */  {-30, -10,  -6,   0,   0,  -6, -10, -30},
	/* E */  {-30, -10,  -6,   0,   0,  -6, -10, -30},
	/* F */  {-30, -10,  -6,  -2,  -2,  -6, -10, -30},
	/* G */  {-30, -24,  -6,  -6,  -6,  -6, -24, -30},
	/* H */  {-60, -30, -30, -30, -30, -30, -30, -60}},
	       //   1    2    3    4    5    6    7    8

	       // Bishop:
	/* A */ {{-20, -20, -20, -20, -20, -20, -20, -20},
	/* B */  {-20,   6,   6,   3,   3,   6,   6, -20},
	/* C */  {-20,   6,   8,   6,   6,   8,   6, -20},
	/* D */  {-20,   3,   6,  10,  10,   6,   3, -20},
	/* E */  {-20,   3,   6,  10,  10,   6,   3, -20},
	/* F */  {-20,   6,   8,   6,   6,   8,   6, -20},
	/* G */  {-20,   6,   6,   3,   3,   6,   6, -20},
	/* H */  {-20, -20, -20, -20, -20, -20, -20, -20}},
	       //   1    2    3    4    5    6    7    8

	       // Rook:
	/* A */ {{-10, -10, -10, -10, -10, -10, -10, -10},
	/* B */  { -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6},
	/* C */  { -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2},
	/* D */  {  2,   2,   2,   2,   2,   2,   2,   2},
	/* E */  {  2,   2,   2,   2,   2,   2,   2,   2},
	/* F */  { -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2},
	/* G */  { -6,  -6,  -6,  -6,  -6,  -6,  -6,  -6},
	/* H */  {-10, -10, -10, -10, -10, -10, -10, -10}},
	       //   1    2    3    4    5    6    7    8

	       // Queen:
	/* A */ {{-20, -20,   0,   0,   0,   0, -20, -20},
	/* B */  {-20,   0,   8,   8,   8,   8,   0, -20},
	/* C */  {  0,   8,   8,  12,  12,   8,   8,   0},
	/* D */  {  0,   8,  12,  16,  16,  12,   8,   0},
	/* E */  {  0,   8,  12,  16,  16,  12,   8,   0},
	/* F */  {  0,   8,   8,  12,  12,   8,   8,   0},
	/* G */  {-20,   0,   8,   8,   8,   8,   0, -20},
	/* H */  {-20, -20,   0,   0,   0,   0, -20, -20}},
	       //   1    2    3    4    5    6    7    8

	       // King:
	/* A */ {{  0,   0,   0,   0,   0,   0,   0,   0},
	/* B */  {  0,   0,   0,   0,   0,   0,   0,   0},
	/* C */  {  0,   0,   0,   0,   0,   0,   0,   0},
	/* D */  {  0,   0,   0,   0,   0,   0,   0,   0},
	/* E */  {  0,   0,   0,   0,   0,   0,   0,   0},
	/* F */  {  0,   0,   0,   0,   0,   0,   0,   0},
	/* G */  {  0,   0,   0,   0,   0,   0,   0,   0},
	/* H */  {  0,   0,   0,   0,   0,   0,   0,   0}}
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

	sum += evaluate_material();
	return sum;
}

/*----------------------------------------------------------------------------*\
 |			      evaluate_material()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_material() const
{

// Evaluate material.

	static const int weight[] = {WEIGHT_PAWN, WEIGHT_KNIGHT, WEIGHT_BISHOP,
	                             WEIGHT_ROOK, WEIGHT_QUEEN,  WEIGHT_KING};
	int sign, coef, sum = 0;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		for (int shape = PAWN; shape <= QUEEN; shape++)
		{
			coef = count_64(state.piece[color][shape]);
			sum += sign * coef * weight[shape];
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_tempo()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_tempo() const
{

// Evaluate tempo.  I'm not sure this is perfect, but it at least seems to
// prevent The Happy King Dance (TM).

	int sign, coef, sum = 0;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		coef = 0;
		for (int shape = PAWN; shape <= QUEEN; shape++)
		{
			bitboard_t b = state.piece[color][shape];
			for (int n, x, y; (n = FST(b)) != -1; BIT_CLR(b, x, y))
			{
				x = n & 0x7;
				y = n >> 3;
				coef += tempo[shape][x][color == WHITE ? y : 7 - y];
			}
		}
		sum += sign * coef * WEIGHT_TEMPO;
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |			      evaluate_position()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_position() const
{

// Evaluate position.

	int sign, sum = 0;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		for (int shape = PAWN; shape <= QUEEN; shape++)
		{
			bitboard_t b = state.piece[color][shape];
			for (int n, x, y; (n = FST(b)) != -1; BIT_CLR(b, x, y))
			{
				x = n & 0x7;
				y = n >> 3;
				sum += sign * position[shape][x][color == WHITE ? y : 7 - y];
			}
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_pawn()				      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_pawn() const
{

// Evaluate pawn structure.

	int sign, coef, sum;
	bitboard_t pawns, adj_files, adj_pawns, ranks;

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
				adj_files |= COL_MSK(j);
			adj_pawns = state.piece[color][PAWN] & adj_files;

			// Penalize isolated pawns.
			if (!adj_pawns)
				sum += sign * coef * WEIGHT_ISOLATED;

			// Penalize doubled pawns.
			sum += sign * (coef - 1) * WEIGHT_DOUBLED;

			for (int n, x, y; (n = FST(pawns)) != -1; BIT_CLR(pawns, x, y))
			{
				x = n & 0x7;
				y = n >> 3;

				// Penalize backward pawns.
				ranks = ROW_MSK(y) | ROW_MSK(y - sign);
				if (!(state.piece[color][PAWN] & adj_files & ranks))
					sum += sign * WEIGHT_BACKWARD;

				// Reward passed pawns.
				ranks = 0;
				for (int k = y + sign; k < 7 && k > 0; k += sign)
					ranks |= ROW_MSK(k);
				if (!(state.piece[!color][PAWN] & adj_files & ranks))
					sum += sign * WEIGHT_PASSED;
			}
		}
	}

	pawn_table.store(pawn_hash, sum);
end:
	sign = OFF_MOVE == WHITE ? 1 : -1;
	return sign * sum;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_king()				      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_king() const
{

// Evaluate king position.

	static const int weight[] = {WEIGHT_CAN_CASTLE,
	                             WEIGHT_CANT_CASTLE,
	                             WEIGHT_HAS_CASTLED};
	int sign, sum = 0;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			sum += sign * weight[state.castle[color][side]];
	}
	return sum;
}
