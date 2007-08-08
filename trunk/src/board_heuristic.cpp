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

static uint8_t tempo[SHAPES][8][8] =
{
	/* Pawn: */
	{{0, 0, 1, 1, 2, 3, 4, 0},
	 {0, 0, 1, 1, 2, 3, 4, 0},
	 {0, 0, 1, 1, 2, 3, 4, 0},
	 {0, 0, 1, 1, 2, 3, 4, 0},
	 {0, 0, 1, 1, 2, 3, 4, 0},
	 {0, 0, 1, 1, 2, 3, 4, 0},
	 {0, 0, 1, 1, 2, 3, 4, 0},
	 {0, 0, 1, 1, 2, 3, 4, 0}},

	/* Knight: */
	{{3, 2, 1, 2, 3, 4, 3, 4},
	 {0, 3, 2, 3, 2, 3, 4, 4},
	 {2, 2, 1, 2, 3, 3, 3, 4},
	 {2, 1, 3, 2, 2, 3, 3, 4},
	 {2, 1, 3, 2, 2, 3, 3, 4},
	 {2, 2, 1, 2, 3, 3, 3, 4},
	 {0, 3, 2, 3, 2, 3, 4, 4},
	 {3, 2, 1, 2, 3, 4, 3, 4}},

	/* Bishop: */
	{{2, 2, 1, 2, 2, 1, 2, 2},
	 {2, 1, 2, 2, 1, 2, 2, 2},
	 {0, 2, 2, 1, 2, 2, 2, 2},
	 {2, 1, 1, 2, 2, 2, 2, 2},
	 {2, 1, 1, 2, 2, 2, 2, 2},
	 {0, 2, 2, 1, 2, 2, 2, 2},
	 {2, 1, 2, 2, 1, 2, 2, 2},
	 {2, 2, 1, 2, 2, 1, 2, 2}},

	/* Rook: */
	{{0, 1, 1, 1, 1, 1, 1, 1},
	 {1, 2, 2, 2, 2, 2, 2, 2},
	 {1, 2, 2, 2, 2, 2, 2, 2},
	 {1, 2, 2, 2, 2, 2, 2, 2},
	 {1, 2, 2, 2, 2, 2, 2, 2},
	 {1, 2, 2, 2, 2, 2, 2, 2},
	 {1, 2, 2, 2, 2, 2, 2, 2},
	 {0, 1, 1, 1, 1, 1, 1, 1}},

	/* Queen: */
	{{1, 2, 2, 1, 2, 2, 2, 2},
	 {1, 2, 1, 2, 2, 2, 2, 2},
	 {1, 1, 2, 2, 2, 2, 2, 2},
	 {0, 1, 1, 1, 1, 1, 1, 1},
	 {1, 1, 2, 2, 2, 2, 2, 2},
	 {1, 2, 1, 2, 2, 2, 2, 2},
	 {1, 2, 2, 1, 2, 2, 2, 2},
	 {1, 2, 2, 2, 1, 2, 2, 2}},

	/* King: */
	{{3, 3, 3, 4, 4, 5, 6, 7},
	 {2, 2, 3, 3, 4, 5, 6, 7},
	 {1, 2, 2, 3, 4, 5, 6, 7},
	 {1, 1, 2, 3, 4, 5, 6, 7},
	 {0, 1, 2, 3, 4, 5, 6, 7},
	 {1, 1, 2, 3, 4, 5, 6, 7},
	 {1, 2, 2, 3, 4, 5, 6, 7},
	 {2, 2, 3, 3, 4, 5, 6, 7}}
};

pawn pawn_table;

/*----------------------------------------------------------------------------*\
 |			       board_heuristic()			      |
\*----------------------------------------------------------------------------*/
board_heuristic::board_heuristic() : board_base()
{

/* Constructor. */

}

/*----------------------------------------------------------------------------*\
 |			       ~board_heuristic()			      |
\*----------------------------------------------------------------------------*/
board_heuristic::~board_heuristic()
{

/* Destructor. */

}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
board_heuristic& board_heuristic::operator=(const board_heuristic& that)
{

/* Overloaded assignment operator. */

	if (this != &that)
		board_base::operator=(that);
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				   evaluate()				      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate() const
{

/*
 | Evaluate the current state.  For simplicity's sake, evaluate from the
 | perspective of the player who's just moved (the color that's off move).
 */

	if (!state.piece[ON_MOVE][KING])
		return WEIGHT_ILLEGAL;

	return evaluate_material() +
	       evaluate_tempo()    +
	       evaluate_pawn()     +
	       evaluate_knight()   +
	       evaluate_bishop()   +
	       evaluate_rook()     +
	       evaluate_queen()    +
	       evaluate_king();
}

/*----------------------------------------------------------------------------*\
 |			      evaluate_material()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_material() const
{

/* Evaluate material. */

	static const int weight[] = {WEIGHT_PAWN, WEIGHT_KNIGHT, WEIGHT_BISHOP,
	                             WEIGHT_ROOK, WEIGHT_QUEEN,  WEIGHT_KING};
	int sign, coef, sum = 0;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		for (int shape = PAWN; shape <= QUEEN; shape++)
		{
			coef = count(state.piece[color][shape]);
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

/* Evaluate tempo. */

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
 |				evaluate_pawn()				      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_pawn() const
{

/* Evaluate pawn formation. */

	int sign, coef, sum;
	bitboard_t pawns, adj_files, adj_pawns, ranks;

	/*
	 | If we've already evaluated this pawn structure, return our previous
	 | evaluation.
	 */
	if (pawn_table.probe(pawn_hash, &sum))
		goto end;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == WHITE ? 1 : -1;
		for (int file = 0; file <= 7; file++)
		{
			pawns = state.piece[color][PAWN] & COL_MSK(file);
			if (!(coef = count(pawns)))
				/*
				 | The current color has no pawn on the current
				 | file.  Move to the next file.
				 */
				continue;
			adj_files = 0;
			for (int j = file == 0 ? 1 : -1; j <= (file == 7 ? -1 : 1); j += 2)
				adj_files |= COL_MSK(j);
			adj_pawns = state.piece[color][PAWN] & adj_files;

			/* Penalize isolated pawns. */
			if (!adj_pawns)
				sum += sign * coef * WEIGHT_ISOLATED;

			/* Penalize doubled pawns. */
			sum += sign * (coef - 1) * WEIGHT_DOUBLED;

			for (int n, x, y; (n = FST(pawns)) != -1; BIT_CLR(pawns, x, y))
			{
				x = n & 0x7;
				y = n >> 3;

				/* Penalize backward pawns. */
				ranks = ROW_MSK(y) | ROW_MSK(y - sign);
				if (!(state.piece[color][PAWN] & adj_files & ranks))
					sum += sign * WEIGHT_BACKWARD;

				/* Reward passed pawns. */
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
 |			       evaluate_knight()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_knight() const
{

/* Evaluate knight position. */

	return 0;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_bishop()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_bishop() const
{

/* Evaluate bishop position. */

	return 0;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_rook()				      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_rook() const
{

/* Evaluate rook position. */

	return 0;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_queen()			      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_queen() const
{

/* Evaluate queen position. */

	return 0;
}

/*----------------------------------------------------------------------------*\
 |				evaluate_king()				      |
\*----------------------------------------------------------------------------*/
int board_heuristic::evaluate_king() const
{

/* Evaluate king position. */

	static const int weight[] = {WEIGHT_CAN_CASTLE,
	                             WEIGHT_CANT_CASTLE,
	                             WEIGHT_HAS_CASTLED};
	int sign, weight, sum = 0;

	for (int color = WHITE; color <= BLACK; color++)
	{
		sign = color == OFF_MOVE ? 1 : -1;
		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
		{
			weight = weight[state.castle[color][side]];
			sum += sign * weight;
		}
	}
	return sum;
}
