/*----------------------------------------------------------------------------*\
 |	board_heuristic.cpp - heuristic evaluation implementation	      |
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

#include "gray.h"
#include "board_heuristic.h"

/*----------------------------------------------------------------------------*\
 |			       board_heuristic()			      |
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
 |			       ~board_heuristic()			      |
\*----------------------------------------------------------------------------*/
board_heuristic::~board_heuristic()
{

/// Destructor.

}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
board_heuristic &board_heuristic::operator=(const board_heuristic &that)
{

/// Overloaded assignment operator.

	if (this != &that)
		board_base::operator=(that);
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				   evaluate()				      |
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
 |				evaluate_pawns()			      |
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
			sum += sign * value_position[PAWN][x][y];

			//
			bitboard_t pawns = state.piece[color][PAWN];
			bitboard_t pawns_on_col = pawns & COL_MSK(y);
			int num_on_col = count_64(pawns_on_col);

			if (!(pawns & squares_adj_cols[x]))
			{
				// Count isolated pawns and penalize isolated
				// doubled pawns.
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
 |			       evaluate_knights()			      |
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
					squares_pawn_defenses[color][x][y] &
					state.piece[color][PAWN];
				if (pawn_defenses)
				{
					int tmp_y = color == WHITE ? y : 7 - y;
					value_t value_outpost = value_knight_outpost[x][tmp_y];
					sum += sign * value_outpost;
				}
			}

			// TODO: Reward blocking center pawns.
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_bishops()			      |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_bishops() const
{

///

	value_t sign, sum = 0;
	bitboard_t b;

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

			// Reward bishops (over knights) during endgames with
			// pawns on both sides of the board.
			int friendly_piece_count = count_64(ALL(state, color));
			bool endgame = friendly_piece_count < 7;
			if (endgame)
			{	
				bool enemy_bishop_present = state.piece[!color][BISHOP];
				if (!enemy_bishop_present)
				{
					bitboard_t all_pawns = state.piece[WHITE][PAWN] |
					                       state.piece[BLACK][PAWN];
					bitboard_t squares_both_sides =
						COL_MSK(0) | COL_MSK(1) | COL_MSK(2) |
						COL_MSK(5) | COL_MSK(6) | COL_MSK(7);
					bool pawns_both_sides = all_pawns & squares_both_sides;
					if (pawns_both_sides)
						sum += sign * value_bishop_over_knight;
				}
			}

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
 |				evaluate_rooks()			      |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_rooks() const
{

///

	value_t sign, sum = 0;
	bitboard_t b;

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
			if (is_rook_on_7th)
			{
				bitboard_t enemy_pawns = state.piece[!color][PAWN];
				bitboard_t seventh_row = ROW_MSK(seventh);
				bool is_enemy_pawn_on_7th = enemy_pawns & seventh_row;
				bitboard_t enemy_king = state.piece[!color][KING];
				int eighth = color == WHITE ? 7 : 0;
				bitboard_t eighth_row = ROW_MSK(eighth);
				bool is_enemy_king_on_8th = enemy_king & eighth_row;
				if (is_enemy_pawn_on_7th || is_enemy_king_on_8th)
				{
					sum += sign * value_rook_on_7th;
					bitboard_t rooks = state.piece[color][ROOK];
					bitboard_t rooks_on_7th = rooks & seventh_row;
					int num_rooks_on_7th = count_64(rooks_on_7th);
					if (num_rooks_on_7th >= 2)
						sum += sign * value_rooks_on_7th;
				}
			}
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |			       evaluate_queens()			      |
\*----------------------------------------------------------------------------*/
value_t board_heuristic::evaluate_queens() const
{

///

	value_t sign, sum = 0;
	bitboard_t b;

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
			int seventh = color == WHITE ? 6 : 1;
			bool is_queen_on_7th = y == seventh;
			if (is_queen_on_7th)
			{
				bitboard_t enemy_pawns = state.piece[!color][PAWN];
				bitboard_t seventh_row = ROW_MSK(seventh);
				bool is_enemy_pawn_on_7th = enemy_pawns & seventh_row;
				bitboard_t enemy_king = state.piece[!color][KING];
				int eighth = color == WHITE ? 7 : 0;
				bitboard_t eighth_row = ROW_MSK(eighth);
				bool is_enemy_king_on_8th = enemy_king & eighth_row;
				if (is_enemy_pawn_on_7th || is_enemy_king_on_8th)
				{
					bitboard_t rooks = state.piece[color][ROOK];
					bool is_rook_on_7th = rooks & seventh_row;
					if (is_rook_on_7th)
						sum += sign * value_queen_rook_on_7th;
				}
			}

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
 |				evaluate_kings()			      |
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
		bitboard_t pawns = state.piece[WHITE][PAWN] |
		                   state.piece[BLACK][PAWN];
		if (pawns & SQUARES_QUEEN_SIDE && pawns & SQUARES_KING_SIDE)
		{
			// There are pawns on both sides of the board.  The king
			// should be in the middle.
			int j = x;
			int k = !color ? y : 7 - y;
			sum += sign * value_position[KING][j][k];
		}
		else if (pawns)
		{
			// There are pawns on only one side of the board.  The
			// king should be on that side.
			int j = pawns & SQUARES_QUEEN_SIDE ? x : 7 - x;
			int k = !color ? y : 7 - y;
			sum += sign * value_king_position[j][k];
		}
	}
	return sum;
}

/*----------------------------------------------------------------------------*\
 |				 precomp_pawn()				      |
\*----------------------------------------------------------------------------*/
void board_heuristic::precomp_pawn() const
{
	// A pawn duo is two friendly pawns that are side by side.  In endgame,
	// a pawn duo is powerful because the two pawns can advance together and
	// protect one another.  For each square on the board, pretend that a
	// pawn is on that square and compute the squares on which one friendly
	// pawn must sit in order to form a pawn duo.
	for (int n = 0; n <= 63; n++)
	{
		int x = n & 0x7;
		int y = n >> 3;

		squares_pawn_duo[x][y] = 0;
		if (y == 0 || y == 7)
			// There can never be any pawn (of either color) on rank
			// 1 or rank 7.
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
					ROW_MSK(k) & squares_adj_cols[x];

			if (color == WHITE && (y == 0 || y == 1) ||
			    color == BLACK && (y == 6 || y == 7))
				// A white pawn can never defend a white piece
				// in rank 1 or 2.  Similarly, a black pawn can
				// never defend a black piece in rank 6 or 7.
				squares_pawn_defenses[color][x][y] = 0;
			else
				squares_pawn_defenses[color][x][y] =
					ROW_MSK(y + sign) & squares_adj_cols[x];
		}
	}
}
