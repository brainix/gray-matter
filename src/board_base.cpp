/*----------------------------------------------------------------------------*\
 |	board_base.cpp - board representation implementation		      |
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
#include "board_base.h"

/* This array maps coordinates between rotated BitBoards. */
static int coord[MAPS][ANGLES][8][8][COORDS] =
{
	/* From 0� to 45� left: */
	{{{{0,0},{1,0},{3,0},{6,0},{2,1},{7,1},{5,2},{4,3}},
	  {{2,0},{4,0},{7,0},{3,1},{0,2},{6,2},{5,3},{4,4}},
	  {{5,0},{0,1},{4,1},{1,2},{7,2},{6,3},{5,4},{3,5}},
	  {{1,1},{5,1},{2,2},{0,3},{7,3},{6,4},{4,5},{1,6}},
	  {{6,1},{3,2},{1,3},{0,4},{7,4},{5,5},{2,6},{6,6}},
	  {{4,2},{2,3},{1,4},{0,5},{6,5},{3,6},{7,6},{2,7}},
	  {{3,3},{2,4},{1,5},{7,5},{4,6},{0,7},{3,7},{5,7}},
	  {{3,4},{2,5},{0,6},{5,6},{1,7},{4,7},{6,7},{7,7}}},

	/* From 0� to 0�: */
	 {{{0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7}},
	  {{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7}},
	  {{2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7}},
	  {{3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7}},
	  {{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{4,7}},
	  {{5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{5,7}},
	  {{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{6,7}},
	  {{7,0},{7,1},{7,2},{7,3},{7,4},{7,5},{7,6},{7,7}}},

	/* From 0� to 45� right: */
	 {{{4,3},{4,4},{3,5},{1,6},{6,6},{2,7},{5,7},{7,7}},
	  {{5,2},{5,3},{5,4},{4,5},{2,6},{7,6},{3,7},{6,7}},
	  {{7,1},{6,2},{6,3},{6,4},{5,5},{3,6},{0,7},{4,7}},
	  {{2,1},{0,2},{7,2},{7,3},{7,4},{6,5},{4,6},{1,7}},
	  {{6,0},{3,1},{1,2},{0,3},{0,4},{0,5},{7,5},{5,6}},
	  {{3,0},{7,0},{4,1},{2,2},{1,3},{1,4},{1,5},{0,6}},
	  {{1,0},{4,0},{0,1},{5,1},{3,2},{2,3},{2,4},{2,5}},
	  {{0,0},{2,0},{5,0},{1,1},{6,1},{4,2},{3,3},{3,4}}},

	/* From 0� to 90� right: */
	 {{{7,0},{6,0},{5,0},{4,0},{3,0},{2,0},{1,0},{0,0}},
	  {{7,1},{6,1},{5,1},{4,1},{3,1},{2,1},{1,1},{0,1}},
	  {{7,2},{6,2},{5,2},{4,2},{3,2},{2,2},{1,2},{0,2}},
	  {{7,3},{6,3},{5,3},{4,3},{3,3},{2,3},{1,3},{0,3}},
	  {{7,4},{6,4},{5,4},{4,4},{3,4},{2,4},{1,4},{0,4}},
	  {{7,5},{6,5},{5,5},{4,5},{3,5},{2,5},{1,5},{0,5}},
	  {{7,6},{6,6},{5,6},{4,6},{3,6},{2,6},{1,6},{0,6}},
	  {{7,7},{6,7},{5,7},{4,7},{3,7},{2,7},{1,7},{0,7}}}},

	/* From 45� left to 0�: */
	{{{{0,0},{2,1},{1,4},{3,3},{4,3},{5,3},{7,2},{6,5}},
	  {{0,1},{3,0},{2,3},{4,2},{5,2},{6,2},{3,7},{7,4}},
	  {{1,0},{0,4},{3,2},{5,1},{6,1},{7,1},{4,6},{5,7}},
	  {{0,2},{1,3},{4,1},{6,0},{7,0},{2,7},{5,5},{6,6}},
	  {{1,1},{2,2},{5,0},{0,7},{1,7},{3,6},{6,4},{7,5}},
	  {{2,0},{3,1},{0,6},{1,6},{2,6},{4,5},{7,3},{6,7}},
	  {{0,3},{4,0},{1,5},{2,5},{3,5},{5,4},{4,7},{7,6}},
	  {{1,2},{0,5},{2,4},{3,4},{4,4},{6,3},{5,6},{7,7}}},

	/* From 0� to 0�: */
	 {{{0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7}},
	  {{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7}},
	  {{2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7}},
	  {{3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7}},
	  {{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{4,7}},
	  {{5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{5,7}},
	  {{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{6,7}},
	  {{7,0},{7,1},{7,2},{7,3},{7,4},{7,5},{7,6},{7,7}}},

	/* From 45� right to 0�: */
	 {{{7,0},{6,2},{3,1},{4,3},{4,4},{4,5},{5,7},{2,6}},
	  {{6,0},{7,3},{4,2},{5,4},{5,5},{5,6},{0,3},{3,7}},
	  {{7,1},{3,0},{5,3},{6,5},{6,6},{6,7},{1,4},{0,5}},
	  {{5,0},{4,1},{6,4},{7,6},{7,7},{0,2},{2,5},{1,6}},
	  {{6,1},{5,2},{7,5},{0,0},{0,1},{1,3},{3,6},{2,7}},
	  {{7,2},{6,3},{1,0},{1,1},{1,2},{2,4},{4,7},{0,6}},
	  {{4,0},{7,4},{2,1},{2,2},{2,3},{3,5},{0,4},{1,7}},
	  {{5,1},{2,0},{3,2},{3,3},{3,4},{4,6},{1,5},{0,7}}},

	/* From 90� right to 0�: */
	 {{{0,7},{1,7},{2,7},{3,7},{4,7},{5,7},{6,7},{7,7}},
	  {{0,6},{1,6},{2,6},{3,6},{4,6},{5,6},{6,6},{7,6}},
	  {{0,5},{1,5},{2,5},{3,5},{4,5},{5,5},{6,5},{7,5}},
	  {{0,4},{1,4},{2,4},{3,4},{4,4},{5,4},{6,4},{7,4}},
	  {{0,3},{1,3},{2,3},{3,3},{4,3},{5,3},{6,3},{7,3}},
	  {{0,2},{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{7,2}},
	  {{0,1},{1,1},{2,1},{3,1},{4,1},{5,1},{6,1},{7,1}},
	  {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0}}}}
};

static int diag_index[15] = {0, 1, 3, 6, 10, 15, 21, 28, 36, 43, 49, 54, 58, 61, 63};
static bitrow_t diag_mask[15] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};

/* Whether the moves and hash keys have been pre-computed: */
bool precomputed = false;

/* Pre-computed moves: */
bitboard_t squares_king[8][8];
bitrow_t squares_row[8][256];
bitboard_t squares_knight[8][8];

/* Zobrist hash keys: */
bitboard_t key_piece[COLORS][SHAPES][8][8];
bitboard_t key_castle[COLORS][SIDES][CASTLE_STATS];
bitboard_t key_no_en_passant;
bitboard_t key_en_passant[8];
bitboard_t key_whose;

/*----------------------------------------------------------------------------*\
 |				  board_base()				      |
\*----------------------------------------------------------------------------*/
board_base::board_base()
{

/*
 | Constructor.  Important!  Seed the random number generator - issue
 | srand(time(NULL)); - before instantiating this class!
 */

	if (!precomputed)
	{
		precomp_king();
		precomp_row();
		precomp_knight();
		precomp_key();
		precomputed = true;
	}
	set_board();

	mutex_create(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				 ~board_base()				      |
\*----------------------------------------------------------------------------*/
board_base::~board_base()
{

/* Destructor. */

	mutex_destroy(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
board_base& board_base::operator=(const board_base& that)
{

/* Overloaded assignment operator. */

	/*
	 | It's like this and like that and like this and uh.
	 | It's like that and like this and like that and uh.
	 | It's like this.  And who gives a f*ck about those?
	 | So just chill, 'til the next episode.
	 |
	 |	Snoop Doggy Dogg and Dr. Dre on overloading the assignment
	 |	operator
	 */
	if (this == &that)
		return *this;

	states = that.states;
	state = that.state;
	for (int angle = L45; angle <= R90; angle++)
		for (int color = WHITE; color <= COLORS; color++)
		{
			rotations[angle][color] = that.rotations[angle][color];
			rotation[angle][color] = that.rotation[angle][color];
		}
	hashes = that.hashes;
	hash = that.hash;
	pawn_hashes = that.pawn_hashes;
	pawn_hash = that.pawn_hash;
	return *this;
}

/*----------------------------------------------------------------------------*\
 |				  set_board()				      |
\*----------------------------------------------------------------------------*/
void board_base::set_board()
{

/* Set the board. */

	init_state();
	init_rotation();
	init_hash();
}

/*----------------------------------------------------------------------------*\
 |				     lock()				      |
\*----------------------------------------------------------------------------*/
void board_base::lock()
{
	mutex_lock(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				    unlock()				      |
\*----------------------------------------------------------------------------*/
void board_base::unlock()
{
	mutex_unlock(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				  get_whose()				      |
\*----------------------------------------------------------------------------*/
bool board_base::get_whose() const
{

/* Return the color on move. */

	return ON_MOVE;
}

/*----------------------------------------------------------------------------*\
 |				   get_hash()				      |
\*----------------------------------------------------------------------------*/
bitboard_t board_base::get_hash() const
{

/* Return the hash key for the current state. */

	return hash;
}

/*----------------------------------------------------------------------------*\
 |				  get_status()				      |
\*----------------------------------------------------------------------------*/
int board_base::get_status(bool mate_test)
{
	int type;

	if (!state.piece[WHITE][KING] || !state.piece[BLACK][KING])
		return ILLEGAL;
	if (mate_test)
		if ((type = mate()) != IN_PROGRESS)
			return type;
	if (insufficient())
		return INSUFFICIENT;
	if (three())
		return THREE;
	if (fifty())
		return FIFTY;
	return IN_PROGRESS;
}

/*----------------------------------------------------------------------------*\
 |				    check()				      |
\*----------------------------------------------------------------------------*/
bool board_base::check() const
{
	return check(state.piece[ON_MOVE][KING], OFF_MOVE);
}

/*----------------------------------------------------------------------------*\
 |				   zugzwang()				      |
\*----------------------------------------------------------------------------*/
bool board_base::zugzwang() const
{

/*
 | Is the current position zugzwang?
 |
 | In most positions, there's at least one move that the color that's on move
 | could make to improve her lot.  In these normal positions, null-move pruning
 | works.  However, in certain positions, her best move would be to pass
 | (particularly in endgame).  These positions are called "zugzwang" (German for
 | "compelled to move").  In these zugzwang positions, null-move pruning doesn't
 | work.
 |
 | The search class calls this method on a particular position to decide whether
 | or not to try null-move pruning.
*/

	if (check(state.piece[ON_MOVE][KING], OFF_MOVE))
		/* The color that's on move is in check. */
		return true;
	for (int color = WHITE; color <= BLACK; color++)
	{
		if (!count(state.piece[color][KNIGHT] | state.piece[color][BISHOP] | state.piece[color][ROOK] | state.piece[color][QUEEN]))
			/* One color only has pawns and a king. */
			return true;
		if (!state.piece[color][KING])
			/* One color doesn't even have a king. */
			return true;
	}
	return false;
}

/*----------------------------------------------------------------------------*\
 |				   generate()				      |
\*----------------------------------------------------------------------------*/
void board_base::generate(list<move_t> &l, bool only_legal_moves, bool only_captures)
{
	list<move_t> tmp;
	list<move_t>::iterator it;

	if (!only_legal_moves)
	{
		generate_king(l, only_captures);
		generate_queen(l, only_captures);
		generate_rook(l, only_captures);
		generate_bishop(l, only_captures);
		generate_knight(l, only_captures);
		generate_pawn(l, only_captures);
	}
	else
	{
		generate_king(tmp, only_captures);
		generate_queen(tmp, only_captures);
		generate_rook(tmp, only_captures);
		generate_bishop(tmp, only_captures);
		generate_knight(tmp, only_captures);
		generate_pawn(tmp, only_captures);

		for (it = tmp.begin(); it != tmp.end(); it++)
		{
			make(*it);
			if (!check(state.piece[OFF_MOVE][KING], ON_MOVE))
				l.push_back(*it);
			unmake();
		}
	}
}

/*----------------------------------------------------------------------------*\
 |				     make()				      |
\*----------------------------------------------------------------------------*/
bool board_base::make(move_t m)
{

/* Make a move. */

	/* Save the current state, rotated BitBoards, and hash keys. */
	states.push_back(state);
	for (int angle = L45; angle <= R90; angle++)
		for (int color = WHITE; color <= COLORS; color++)
			rotations[angle][color].push_back(rotation[angle][color]);
	hashes.push_back(hash);
	pawn_hashes.push_back(pawn_hash);

	/* If we're making a null move, skip a bunch of this nonsense. */
	if (IS_NULL_MOVE(m))
		goto end;

	/* Move the piece and remove the captured piece. */
	for (int shape = PAWN; shape <= KING; shape++)
	{
		if (BIT_GET(state.piece[ON_MOVE][shape], m.old_x, m.old_y))
		{
			BIT_CLR(state.piece[ON_MOVE][shape], m.old_x, m.old_y);
			BIT_SET(state.piece[ON_MOVE][shape], m.new_x, m.new_y);
			for (int angle = L45; angle <= R90; angle++)
			{
				BIT_CLR(rotation[angle][ON_MOVE], coord[MAP][angle][m.old_x][m.old_y][X], coord[MAP][angle][m.old_x][m.old_y][Y]);
				BIT_SET(rotation[angle][ON_MOVE], coord[MAP][angle][m.new_x][m.new_y][X], coord[MAP][angle][m.new_x][m.new_y][Y]);
			}
			hash ^= key_piece[ON_MOVE][shape][m.old_x][m.old_y];
			hash ^= key_piece[ON_MOVE][shape][m.new_x][m.new_y];
			if (shape == PAWN)
			{
				pawn_hash ^= key_piece[ON_MOVE][shape][m.old_x][m.old_y];
				pawn_hash ^= key_piece[ON_MOVE][shape][m.new_x][m.new_y];
			}
		}
		if (BIT_GET(state.piece[OFF_MOVE][shape], m.new_x, m.new_y))
		{
			BIT_CLR(state.piece[OFF_MOVE][shape], m.new_x, m.new_y);
			for (int angle = L45; angle <= R90; angle++)
				BIT_CLR(rotation[angle][OFF_MOVE], coord[MAP][angle][m.new_x][m.new_y][X], coord[MAP][angle][m.new_x][m.new_y][Y]);
			hash ^= key_piece[OFF_MOVE][shape][m.new_x][m.new_y];
			if (shape == PAWN)
				pawn_hash ^= key_piece[OFF_MOVE][shape][m.new_x][m.new_y];
		}
	}

	/*
	 | If we're moving a piece from one of our rooks' initial positions,
	 | make sure we're no longer marked able to castle on that rook's
	 | side.
	 */
	if ((m.old_x == 0 || m.old_x == 7) && (m.old_y == (ON_MOVE ? 7 : 0)) && state.castle[ON_MOVE][m.old_x == 7] == CAN_CASTLE)
	{
		state.castle[ON_MOVE][m.old_x == 7] = CANT_CASTLE;
		hash ^= key_castle[ON_MOVE][m.old_x == 7][CANT_CASTLE];
	}

	/*
	 | If we're moving a piece to one of our opponent's rooks' initial
	 | positions, make sure our opponent is no longer marked able to castle
	 | on that rook's side.
	 */
	if ((m.new_x == 0 || m.new_x == 7) && (m.new_y == (OFF_MOVE ? 7 : 0)) && state.castle[OFF_MOVE][m.new_x == 7] == CAN_CASTLE)
	{
		state.castle[OFF_MOVE][m.new_x == 7] = CANT_CASTLE;
		hash ^= key_castle[OFF_MOVE][m.old_x == 7][CANT_CASTLE];
	}

	/* If we're moving the king: */
	if (BIT_GET(state.piece[ON_MOVE][KING], m.new_x, m.new_y))
	{
		/*
		 | If we're castling, move the rook and mark us having castled
		 | on this side.
		 */
		if (abs((int) m.old_x - (int) m.new_x) == 2)
		{
			BIT_CLR(state.piece[ON_MOVE][ROOK], m.new_x == 6 ? 7 : 0, ON_MOVE ? 7 : 0);
			BIT_SET(state.piece[ON_MOVE][ROOK], m.new_x == 6 ? 5 : 3, ON_MOVE ? 7 : 0);
			for (int angle = L45; angle <= R90; angle++)
			{
				BIT_CLR(rotation[angle][ON_MOVE], coord[MAP][angle][m.new_x == 6 ? 7 : 0][ON_MOVE ? 7 : 0][X], coord[MAP][angle][m.new_x == 6 ? 7 : 0][ON_MOVE ? 7 : 0][Y]);
				BIT_SET(rotation[angle][ON_MOVE], coord[MAP][angle][m.new_x == 6 ? 5 : 3][ON_MOVE ? 7 : 0][X], coord[MAP][angle][m.new_x == 6 ? 5 : 3][ON_MOVE ? 7 : 0][Y]);
			}
			hash ^= key_piece[ON_MOVE][ROOK][m.new_x == 6 ? 7 : 0][ON_MOVE ? 7 : 0];
			hash ^= key_piece[ON_MOVE][ROOK][m.new_x == 6 ? 5 : 3][ON_MOVE ? 7 : 0];
			state.castle[ON_MOVE][m.new_x == 6] = HAS_CASTLED;
			hash ^= key_castle[ON_MOVE][m.new_x == 6][HAS_CASTLED];
		}

		/*
		 | At this point, we've moved the king.  Make sure we're no
		 | longer marked able to castle on either side.
		 */
		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			if (state.castle[ON_MOVE][side] == CAN_CASTLE)
			{
				state.castle[ON_MOVE][side] = CANT_CASTLE;
				hash ^= key_castle[ON_MOVE][side][CANT_CASTLE];
			}
	}

	/* If we're moving a pawn: */
	hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];
	pawn_hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];
	if (BIT_GET(state.piece[ON_MOVE][PAWN], m.new_x, m.new_y))
	{
		/*
		 | If we're promoting a pawn, replace it with the promotion
		 | piece.
		 */
		if (m.promo)
		{
			BIT_CLR(state.piece[ON_MOVE][PAWN], m.new_x, m.new_y);
			BIT_SET(state.piece[ON_MOVE][m.promo], m.new_x, m.new_y);
			hash ^= key_piece[ON_MOVE][PAWN][m.new_x][m.new_y];
			hash ^= key_piece[ON_MOVE][m.promo][m.new_x][m.new_y];
			pawn_hash ^= key_piece[ON_MOVE][PAWN][m.new_x][m.new_y];
		}

		/*
		 | If we're performing an en passant, remove the captured
		 | pawn.
		 */
		if ((int) m.new_x == state.en_passant && m.new_y == (ON_MOVE ? 2 : 5))
		{
			BIT_CLR(state.piece[OFF_MOVE][PAWN], m.new_x, m.old_y);
			for (int angle = L45; angle <= R90; angle++)
				BIT_CLR(rotation[angle][OFF_MOVE], coord[MAP][angle][m.new_x][m.old_y][X], coord[MAP][angle][m.new_x][m.old_y][Y]);
			hash ^= key_piece[OFF_MOVE][PAWN][m.new_x][m.old_y];
			pawn_hash ^= key_piece[OFF_MOVE][PAWN][m.new_x][m.old_y];
		}

		/*
		 | If we're advancing a pawn two squares, mark it vulnerable to
		 | en passant.
		 */
		state.en_passant = abs((int) m.old_y - (int) m.new_y) == 2 ? (int) m.old_x : -1;
	}
	else
		/*
		 | Oops.  We're not moving a pawn.  Mark no pawn vulnerable to
		 | en passant.
		 */
		state.en_passant = -1;
	hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];
	pawn_hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];

end:
	/* Set the other color on move. */
	state.whose = !state.whose;
	hash ^= key_whose;

	/* Update the rotated BitBoards. */
	for (int angle = L45; angle <= R90; angle++)
		rotation[angle][COLORS] = rotation[angle][WHITE] | rotation[angle][BLACK];

	return true;
}

/*----------------------------------------------------------------------------*\
 |				    unmake()				      |
\*----------------------------------------------------------------------------*/
bool board_base::unmake()
{

/* Take back the last move. */

	if (states.empty())
		return false;

	/* Restore the previous state, rotated BitBoards, and hash keys. */
	state = states.back();
	states.pop_back();
	for (int angle = R90; angle >= L45; angle--)
		for (int color = COLORS; color >= WHITE; color--)
		{
			rotation[angle][color] = rotations[angle][color].back();
			rotations[angle][color].pop_back();
		}
	hash = hashes.back();
	hashes.pop_back();
	pawn_hash = pawn_hashes.back();
	pawn_hashes.pop_back();
	return true;
}

/*----------------------------------------------------------------------------*\
 |				     make()				      |
\*----------------------------------------------------------------------------*/
bool board_base::make(char *p)
{

/*
 | Convert a move from standard algebraic notation to coordinate notation, then
 | make the move.
 */

	int shape = -1, old_x = -1, old_y = -1, new_x = -1, new_y = -1, promo = -1;
	move_t m;

	if (!strncmp(p, "O-O-O", 5) || !strncmp(p, "O-O", 3))
	{
		m.new_x = (m.old_x = 4) + !strncmp(p, "O-O-O", 5) ? -2 : 2;
		m.new_y = m.old_y = ON_MOVE ? 7 : 0;
		m.promo = 0;
		return make(m);
	}

	switch (*p)
	{
		case 'K' : p++; shape = KING;   break;
		case 'Q' : p++; shape = QUEEN;  break;
		case 'R' : p++; shape = ROOK;   break;
		case 'B' : p++; shape = BISHOP; break;
		case 'N' : p++; shape = KNIGHT; break;
		case 'P' : p++; shape = PAWN;   break;
		default  :      shape = PAWN;   break;
	}

	if (*p == 'x')
		p++;

	if (*p >= 'a' && *p <= 'h')
		new_x = *p++ - 'a';

	if (*p == 'x')
		p++;

	if (*p >= '1' && *p <= '8')
		new_y = *p++ - '1';

	if (*p == 'x')
		p++;

	if (*p >= 'a' && *p <= 'h')
	{
		old_x = new_x;
		old_y = new_y;
		new_x = *p++ - 'a';
		new_y = *p++ - '1';
	}

	if (*p == '=')
		switch (*++p)
		{
			case 'Q' : p++; promo = QUEEN;  break;
			case 'R' : p++; promo = ROOK;   break;
			case 'B' : p++; promo = BISHOP; break;
			case 'N' : p++; promo = KNIGHT; break;
		}
	else
		promo = 0;

	if (old_x == -1 || old_y == -1)
	{
		list<move_t> l;
		switch (shape)
		{
			case KING   : generate_king(l);   break;
			case QUEEN  : generate_queen(l);  break;
			case ROOK   : generate_rook(l);   break;
			case BISHOP : generate_bishop(l); break;
			case KNIGHT : generate_knight(l); break;
			case PAWN   : generate_pawn(l);   break;
		}
		for (list<move_t>::iterator it = l.begin(); it != l.end(); it++)
			if ((old_x == -1 || old_x == (int) it->old_x) && (old_y == -1 || old_y == (int) it->old_y) && new_x == (int) it->new_x && new_y == (int) it->new_y)
			{
				old_x = it->old_x;
				old_y = it->old_y;
				break;
			}
	}

	if (old_x == -1 || old_y == -1 || new_x == -1 || new_y == -1 || promo == -1)
		return false;

	m.old_x = old_x;
	m.old_y = old_y;
	m.new_x = new_x;
	m.new_y = new_y;
	m.promo = promo;
	return make(m);
}

/*----------------------------------------------------------------------------*\
 |				    perft()				      |
\*----------------------------------------------------------------------------*/
int board_base::perft(int depth)
{
	list<move_t> l;
	list<move_t>::iterator it;
	int nodes = 0;

	/* Base case. */
	if (depth == 0)
		return 1;

	/* Recursive case. */
	generate(l, true);
	for (it = l.begin(); it != l.end(); it++)
	{
		make(*it);
		nodes += perft(depth - 1);
		unmake();
	}
	return nodes;
}

/*----------------------------------------------------------------------------*\
 |				  init_state()				      |
\*----------------------------------------------------------------------------*/
void board_base::init_state()
{

/* Initialize the state. */

	/* Clear the previous states. */
	states.clear();

	/* Initialize the current state. */
	for (int color = WHITE; color <= BLACK; color++)
	{
		/* Clear the board. */
		for (int shape = PAWN; shape <= KING; shape++)
			state.piece[color][shape] = 0;

		/* Place the pawns. */
		ROW_SET(state.piece[color][PAWN], color ? 6 : 1, 0xFF);

		/* Place the other pieces. */
		BIT_SET(state.piece[color][ROOK],   0, color ? 7 : 0);
		BIT_SET(state.piece[color][KNIGHT], 1, color ? 7 : 0);
		BIT_SET(state.piece[color][BISHOP], 2, color ? 7 : 0);
		BIT_SET(state.piece[color][QUEEN],  3, color ? 7 : 0);
		BIT_SET(state.piece[color][KING],   4, color ? 7 : 0);
		BIT_SET(state.piece[color][BISHOP], 5, color ? 7 : 0);
		BIT_SET(state.piece[color][KNIGHT], 6, color ? 7 : 0);
		BIT_SET(state.piece[color][ROOK],   7, color ? 7 : 0);

		/* Mark both colors able to castle on both sides. */
		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			state.castle[color][side] = CAN_CASTLE;
	}

	/* Mark no pawn vulnerable to en passant and set white on move. */
	state.en_passant = -1;
	state.whose = WHITE;
}

/*----------------------------------------------------------------------------*\
 |				init_rotation()				      |
\*----------------------------------------------------------------------------*/
void board_base::init_rotation()
{

/* Initialize the rotated BitBoards. */

	for (int angle = L45; angle <= R90; angle++)
		for (int color = WHITE; color <= COLORS; color++)
		{
			rotations[angle][color].clear();
			rotation[angle][color] = color != COLORS ? rotate(ALL(state, color), MAP, angle) : rotation[angle][WHITE] | rotation[angle][BLACK];
		}
}

/*----------------------------------------------------------------------------*\
 |				  init_hash()				      |
\*----------------------------------------------------------------------------*/
void board_base::init_hash()
{

/* Initialize the Zobrist hash. */

	/* Clear the previous Zobrist hashes. */
	hashes.clear();
	hash = 0;
	pawn_hashes.clear();
	pawn_hash = 0;

	/* Initialize the current Zobrist hash. */
	for (int color = WHITE; color <= BLACK; color++)
	{
		for (int shape = PAWN; shape <= KING; shape++)
		{
			bitboard_t b = state.piece[color][shape];
			for (int n, x, y; (n = FST(b)) != -1; BIT_CLR(b, x, y))
			{
				x = n & 0x7;
				y = n >> 3;
				hash ^= key_piece[color][shape][x][y];
				if (shape == PAWN)
					pawn_hash ^= key_piece[color][shape][x][y];
			}
		}

		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			hash ^= key_castle[color][side][state.castle[color][side]];
	}

	hash ^= key_no_en_passant;
	pawn_hash ^= key_no_en_passant;

	hash ^= key_whose;
}

/*----------------------------------------------------------------------------*\
 |				 precomp_key()				      |
\*----------------------------------------------------------------------------*/
void board_base::precomp_key() const
{

/* Pre-compute the Zobrist hash keys. */

	for (int color = WHITE; color <= BLACK; color++)
	{
		for (int shape = PAWN; shape <= KING; shape++)
			for (int y = 0; y <= 7; y++)
				for (int x = 0; x <= 7; x++)
					key_piece[color][shape][x][y] = randomize();

		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			for (int stat = CAN_CASTLE; stat <= HAS_CASTLED; stat++)
				key_castle[color][side][stat] = randomize();
	}

	key_no_en_passant = randomize();
	for (int x = 0; x <= 8; x++)
		key_en_passant[x] = randomize();

	key_whose = randomize();
}

/*----------------------------------------------------------------------------*\
 |				generate_king()				      |
\*----------------------------------------------------------------------------*/
void board_base::generate_king(list<move_t> &l, bool only_captures) const
{

/* Generate the king moves. */

	static const bitboard_t squares_castle[COLORS][SIDES][REQS] =
		{{{0x000000000000000EULL,    // The squares that must be unoccupied on the queen side in order for the white king to be able to castle.
		   0x000000000000001CULL},   // The squares that must be unattacked on the queen side in order for the white king to be able to castle.
		  {0x0000000000000060ULL,    // The squares that must be unoccupied on the king side in order for the white king to be able to castle.
		   0x0000000000000070ULL}},  // The squares that must be unattacked on the king side in order for the white king to be able to castle.
		 {{0x0E00000000000000ULL,    // The squares that must be unoccupied on the queen side in order for the black king to be able to castle.
		   0x1C00000000000000ULL},   // The squares that must be unattacked on the queen side in order for the black king to be able to castle.
		  {0x6000000000000000ULL,    // The squares that must be unoccupied on the king side in order for the black king to be able to castle.
		   0x7000000000000000ULL}}}; // The squares that must be unattacked on the king side in order for the black king to be able to castle.

	int n = FST(state.piece[ON_MOVE][KING]), x = n & 0x7, y = n >> 3;
	bitboard_t takes = squares_king[x][y] & rotation[ZERO][OFF_MOVE];
	insert(x, y, takes, ZERO, l, FRONT);
	if (!only_captures)
	{
		bitboard_t moves = squares_king[x][y] & ~rotation[ZERO][COLORS];
		insert(x, y, moves, ZERO, l, BACK);
	}

	for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
	{
		if (state.castle[ON_MOVE][side] != CAN_CASTLE)
			continue; // The king or rook has already moved.
		if (squares_castle[ON_MOVE][side][UNOCCUPIED] & rotation[ZERO][COLORS])
			continue; // There's a piece in the way.
		if (check(squares_castle[ON_MOVE][side][UNATTACKED], OFF_MOVE))
			continue; // One of the squares is being attacked.

		move_t m;
		m.new_x = (m.old_x = 4) + (side ? 2 : -2);
		m.new_y = m.old_y = ON_MOVE ? 7 : 0;
		m.value = m.promo = 0;
		l.push_front(m);
	}
}

/*----------------------------------------------------------------------------*\
 |				generate_queen()			      |
\*----------------------------------------------------------------------------*/
void board_base::generate_queen(list<move_t> &l, bool only_captures) const
{

/* Generate the queen moves. */

	bitboard_t from = state.piece[ON_MOVE][QUEEN];

	for (int n, x, y; (n = FST(from)) != -1; BIT_CLR(from, x, y))
	{
		x = n & 0x7;
		y = n >> 3;

		/* Generate the horizontal and vertical moves. */
		for (int angle = ZERO; angle == ZERO || angle == R90; angle += R90 - ZERO)
		{
			int loc = ROW_LOC(x, y, angle);
			int num = ROW_NUM(x, y, angle);
			bitrow_t occ = ROW_GET(rotation[angle][COLORS], num);
			bitrow_t r = squares_row[loc][occ];
			bitboard_t b = 0;
			ROW_SET(b, num, r);
			bitboard_t takes = b & rotation[angle][OFF_MOVE];
			insert(x, y, takes, angle, l, FRONT);
			if (!only_captures)
			{
				bitboard_t moves = b & ~rotation[angle][COLORS];
				insert(x, y, moves, angle, l, BACK);
			}
		}

		/* Generate the diagonal moves. */
		for (int angle = L45; angle == L45 || angle == R45; angle += R45 - L45)
		{
			int loc = DIAG_LOC(x, y, angle);
			int num = DIAG_NUM(x, y, angle);
			bitrow_t occ = DIAG_GET(rotation[angle][COLORS], num);
			bitrow_t msk = diag_mask[num];
			bitrow_t d = squares_row[loc][occ] & msk;
			bitboard_t b = 0;
			DIAG_SET(b, num, d);
			bitboard_t takes = b & rotation[angle][OFF_MOVE];
			insert(x, y, takes, angle, l, FRONT);
			if (!only_captures)
			{
				bitboard_t moves = b & ~rotation[angle][COLORS];
				insert(x, y, moves, angle, l, BACK);
			}
		}
	}
}

/*----------------------------------------------------------------------------*\
 |				generate_rook()				      |
\*----------------------------------------------------------------------------*/
void board_base::generate_rook(list<move_t> &l, bool only_captures) const
{

/* Generate the rook moves. */

	bitboard_t from = state.piece[ON_MOVE][ROOK];

	for (int n, x, y; (n = FST(from)) != -1; BIT_CLR(from, x, y))
	{
		x = n & 0x7;
		y = n >> 3;
		for (int angle = ZERO; angle == ZERO || angle == R90; angle += R90 - ZERO)
		{
			int loc = ROW_LOC(x, y, angle);
			int num = ROW_NUM(x, y, angle);
			bitrow_t occ = ROW_GET(rotation[angle][COLORS], num);
			bitrow_t r = squares_row[loc][occ];
			bitboard_t b = 0;
			ROW_SET(b, num, r);
			bitboard_t takes = b & rotation[angle][OFF_MOVE];
			insert(x, y, takes, angle, l, FRONT);
			if (!only_captures)
			{
				bitboard_t moves = b & ~rotation[angle][COLORS];
				insert(x, y, moves, angle, l, BACK);
			}
		}
	}
}

/*----------------------------------------------------------------------------*\
 |			       generate_bishop()			      |
\*----------------------------------------------------------------------------*/
void board_base::generate_bishop(list<move_t> &l, bool only_captures) const
{

/* Generate the bishop moves. */

	bitboard_t from = state.piece[ON_MOVE][BISHOP];

	for (int n, x, y; (n = FST(from)) != -1; BIT_CLR(from, x, y))
	{
		x = n & 0x7;
		y = n >> 3;
		for (int angle = L45; angle == L45 || angle == R45; angle += R45 - L45)
		{
			int loc = DIAG_LOC(x, y, angle);
			int num = DIAG_NUM(x, y, angle);
			bitrow_t occ = DIAG_GET(rotation[angle][COLORS], num);
			bitrow_t msk = diag_mask[num];
			bitrow_t d = squares_row[loc][occ] & msk;
			bitboard_t b = 0;
			DIAG_SET(b, num, d);
			bitboard_t takes = b & rotation[angle][OFF_MOVE];
			insert(x, y, takes, angle, l, FRONT);
			if (!only_captures)
			{
				bitboard_t moves = b & ~rotation[angle][COLORS];
				insert(x, y, moves, angle, l, BACK);
			}
		}
	}
}

/*----------------------------------------------------------------------------*\
 |			       generate_knight()			      |
\*----------------------------------------------------------------------------*/
void board_base::generate_knight(list<move_t> &l, bool only_captures) const
{

/* Generate the knight moves. */

	bitboard_t from = state.piece[ON_MOVE][KNIGHT];

	for (int n, x, y; (n = FST(from)) != -1; BIT_CLR(from, x, y))
	{
		x = n & 0x7;
		y = n >> 3;
		bitboard_t takes = squares_knight[x][y] & rotation[ZERO][OFF_MOVE];
		insert(x, y, takes, ZERO, l, FRONT);
		if (!only_captures)
		{
			bitboard_t moves = squares_knight[x][y] & ~rotation[ZERO][COLORS];
			insert(x, y, moves, ZERO, l, BACK);
		}
	}
}

/*----------------------------------------------------------------------------*\
 |				generate_pawn()				      |
\*----------------------------------------------------------------------------*/
void board_base::generate_pawn(list<move_t> &l, bool only_captures) const
{

/* Generate the pawn moves. */

	bitboard_t b;
	move_t m;
	m.value = m.promo = 0;

	/* For its first move, a pawn can advance two squares. */
	if (!only_captures)
	{
		b = state.piece[ON_MOVE][PAWN] & ROW_MSK(ON_MOVE ? 6 : 1);
		for (int y = 1; y <= 2; y++)
		{
			b <<= ON_MOVE ? 0 : 8;
			b >>= ON_MOVE ? 8 : 0;
			b &= ~rotation[ZERO][COLORS];
		}
		for (int n; (n = FST(b)) != -1; BIT_CLR(b, m.new_x, m.new_y))
		{
			m.old_x = m.new_x = n & 0x7;
			m.old_y = (m.new_y = n >> 3) + (ON_MOVE ? 2 : -2);
			l.push_back(m);
		}
	}

	/*
	 | If our pawn is on our fifth row, and our opponent's pawn is beside
	 | our pawn, and, as her last move, our opponent advanced her pawn two
	 | squares, then we can perform an en passant.
	 */
	if (state.en_passant != -1)
	{
		m.promo = 0;
		m.new_y = ON_MOVE ? 2 : 5;
		m.new_x = state.en_passant;
		if (state.en_passant != 0 && BIT_GET(state.piece[ON_MOVE][PAWN], m.old_x = state.en_passant - 1, m.old_y = ON_MOVE ? 3 : 4))
			l.push_front(m);
		if (state.en_passant != 7 && BIT_GET(state.piece[ON_MOVE][PAWN], m.old_x = state.en_passant + 1, m.old_y = ON_MOVE ? 3 : 4))
			l.push_front(m);
	}

	/* A pawn can advance one square. */
	b = state.piece[ON_MOVE][PAWN];
	b <<= ON_MOVE ? 0 : 8;
	b >>= ON_MOVE ? 8 : 0;
	b &= ~rotation[ZERO][COLORS];
	for (int n; (n = FST(b)) != -1; BIT_CLR(b, m.new_x, m.new_y))
	{
		m.old_x = m.new_x = n & 0x7;
		m.old_y = (m.new_y = n >> 3) + (ON_MOVE ? 1 : -1);
		if (m.new_y != (ON_MOVE ? 0 : 7))
		{
			if (!only_captures)
				l.push_back(m);
			continue;
		}
		for (m.promo = KNIGHT; m.promo <= QUEEN; m.promo++)
			l.push_front(m);
		m.promo = 0;
	}

	/* A pawn can capture diagonally. */
	for (int x = -1; x <= 1; x += 2)
	{
		b = state.piece[ON_MOVE][PAWN];
		b <<= ON_MOVE ? 0 : x == -1 ? 7 : 9;
		b >>= ON_MOVE ? x == -1 ? 9 : 7 : 0;
		COL_CLR(b, x == -1 ? 7 : 0);
		b &= rotation[ZERO][OFF_MOVE];
		for (int n; (n = FST(b)) != -1; BIT_CLR(b, m.new_x, m.new_y))
		{
			m.old_x = (m.new_x = n & 0x7) - x;
			m.old_y = (m.new_y = n >> 3) + (ON_MOVE ? 1 : -1);
			if (m.new_y != (ON_MOVE ? 0 : 7))
			{
				l.push_front(m);
				continue;
			}
			for (m.promo = KNIGHT; m.promo <= QUEEN; m.promo++)
				l.push_front(m);
			m.promo = 0;
		}
	}
}

/*----------------------------------------------------------------------------*\
 |				 precomp_king()				      |
\*----------------------------------------------------------------------------*/
void board_base::precomp_king() const
{

/* Pre-compute the king moves. */

	/*
	 | Imagine an empty board except for a king at (x, y).  Mark the king's
	 | legal moves in the BitBoard squares_king[x][y].
	 */
	for (int y = 0; y <= 7; y++)
		for (int x = 0; x <= 7; x++)
		{
			squares_king[x][y] = 0;
			for (int k = -1; k <= 1; k++)
				for (int j = -1; j <= 1; j++)
				{
					if (!j && !k)
						/*
						 | Oops.  The king can't stand
						 | still.
						 */
						continue;
					if (x + j < 0 || x + j > 7 ||
					    y + k < 0 || y + k > 7)
						/*
						 | Oops.  The king can't step
						 | off the board.
						 */
						continue;
					BIT_SET(squares_king[x][y], x + j, y + k);
				}
		}
}

/*----------------------------------------------------------------------------*\
 |				 precomp_row()				      |
\*----------------------------------------------------------------------------*/
void board_base::precomp_row() const
{

/* Pre-compute the sliding piece moves. */

	/*
	 | Imagine a sliding piece on square x.  For each possible occupancy
	 | (combination) of enemy pieces along the sliding piece's row, mark the
	 | sliding piece's legal moves in the bitrow squares_row[x][occ].
	 */
	for (int x = 0; x <= 7; x++)
		for (int occ = 0; occ <= 0xFF; occ++)
		{
			squares_row[x][occ] = 0;
			for (int dir = -1; dir <= 1; dir += 2)
				for (int j = x + dir; j >= 0 && j <= 7; j += dir)
				{
					BIT_SET(squares_row[x][occ], j, 0);
					if (BIT_GET(occ, j, 0))
						/*
						 | Oops.  The sliding piece
						 | can't slide through an enemy
						 | piece.
						 */
						break;
				}
		}
}

/*----------------------------------------------------------------------------*\
 |				precomp_knight()			      |
\*----------------------------------------------------------------------------*/
void board_base::precomp_knight() const
{

/* Pre-compute the knight moves. */

	/*
	 | Imagine an empty board except for a knight at (x, y).  Mark the
	 | knight's legal moves in the BitBoard squares_knight[x][y].
	 */
	for (int y = 0; y <= 7; y++)
		for (int x = 0; x <= 7; x++)
		{
			squares_knight[x][y] = 0;
			for (int k = -2; k <= 2; k++)
				for (int j = -2; j <= 2; j++)
				{
					if (abs(j) == abs(k) || !j || !k)
						/*
						 | Oops.  The knight can only
						 | jump two squares in one
						 | direction and one square in a
						 | perpendicular direction.
						 */
						continue;
					if (x + j < 0 || x + j > 7 ||
					    y + k < 0 || y + k > 7)
						/*
						 | Oops.  The knight can't jump
						 | off the board.
						 */
						continue;
					BIT_SET(squares_knight[x][y], x + j, y + k);
				}
		}
}

/*----------------------------------------------------------------------------*\
 |				     mate()				      |
\*----------------------------------------------------------------------------*/
int board_base::mate()
{

/*
 | Is the game over due to stalemate or checkmate?  We test for both conditions
 | in the same function because they're so similar.  During both, the color on
 | move doesn't have a legal move.  The only difference: during stalemate, her
 | king isn't attacked; during checkmate, her king is attacked.
 */

	list<move_t> l;
	bool escape = false;

	/* Look for a legal move. */
	generate(l);
	for (list<move_t>::iterator it = l.begin(); it != l.end() && !escape; it++)
	{
		make(*it);
		if (!check(state.piece[OFF_MOVE][KING], ON_MOVE))
			escape = true;
		unmake();
	}

	/*
	 | If there's a legal move, the game isn't over.  Otherwise, if the king
	 | isn't attacked, the game is over due to stalemate.  Otherwise, the
	 | game is over due to checkmate.
	 */
	return escape ? IN_PROGRESS : !check(state.piece[ON_MOVE][KING], OFF_MOVE) ? STALEMATE : CHECKMATE;
}

/*----------------------------------------------------------------------------*\
 |				    check()				      |
\*----------------------------------------------------------------------------*/
bool board_base::check(bitboard_t b1, bool color) const
{

/*
 | Is any of the specified squares being attacked by the specified color?  Check
 | for check.  ;-)
 */

	for (int n, x, y; (n = FST(b1)) != -1; BIT_CLR(b1, x, y))
	{
		x = n & 0x7;
		y = n >> 3;

		/* Look for a king attack. */
		if (squares_king[x][y] & state.piece[color][KING])
			return true;

		/*
		 | Look for a horizontal or vertical queen or rook attack.  The
		 | logic here is interesting.  Pretend our king were a rook.
		 | Would it be able to capture a rook?  If so, we're in check.
		 | If not, we're not in check, at least not by a rook.
		 */
		for (int angle = ZERO; angle == ZERO || angle == R90; angle += R90 - ZERO)
		{
			int loc = ROW_LOC(x, y, angle);
			int num = ROW_NUM(x, y, angle);
			bitrow_t occ = ROW_GET(rotation[angle][COLORS], num);
			bitrow_t r = squares_row[loc][occ];
			bitboard_t b2 = 0;
			ROW_SET(b2, num, r);
			b2 &= rotation[angle][color];
			b2 = rotate(b2, UNMAP, angle);
			if (b2 & (state.piece[color][QUEEN] | state.piece[color][ROOK]))
				return true;
		}

		/* Look for a diagonal queen or bishop attack. */
		for (int angle = L45; angle == L45 || angle == R45; angle += R45 - L45)
		{
			int loc = DIAG_LOC(x, y, angle);
			int num = DIAG_NUM(x, y, angle);
			bitrow_t occ = DIAG_GET(rotation[angle][COLORS], num);
			bitrow_t msk = diag_mask[num];
			bitrow_t d = squares_row[loc][occ] & msk;
			bitboard_t b2 = 0;
			DIAG_SET(b2, num, d);
			b2 &= rotation[angle][color];
			b2 = rotate(b2, UNMAP, angle);
			if (b2 & (state.piece[color][QUEEN] | state.piece[color][BISHOP]))
				return true;
		}

		/* Look for a knight attack. */
		if (squares_knight[x][y] & state.piece[color][KNIGHT])
			return true;

		/*
		 | Look for a pawn attack.  The logic here is the same as for
		 | the previous pieces, but things are a bit easier because a
		 | pawn can attack at most 2 squares and, conversely, a square
		 | can be attacked by at most 2 pawns.  We simply mark the
		 | squares in the (at most) 2 columns from which a pawn could
		 | attack and find the intersection between those squares and
		 | the squares in the (at most) 1 row from which a pawn could
		 | attack.  This results in 0, 1, or 2 marked squares from which
		 | a pawn could attack.  Then, we simply check whether an
		 | opposing pawn sits on any of our marked squares.  If so,
		 | we're in check.  If not, we're not in check, at least not by
		 | a pawn.  Easy, breezy, beautiful.
		 */
		bitboard_t b2 = 0;
		for (int j = x == 0 ? 1 : -1; j <= (x == 7 ? -1 : 1); j += 2)
			b2 |= COL_MSK(x + j);
		b2 &= (color && y >= 6 || !color && y <= 1) ? 0 : ROW_MSK(y + (color ? 1 : -1));
		if (b2 & state.piece[color][PAWN])
			return true;
	}
	return false;
}

/*----------------------------------------------------------------------------*\
 |				 insufficient()				      |
\*----------------------------------------------------------------------------*/
bool board_base::insufficient() const
{

/* Is the game drawn due to insufficient material? */

	int n_count = 0, b_count = 0, b_array[COLORS][COLORS];

	for (int color = WHITE; color <= BLACK; color++)
		if (state.piece[color][PAWN] ||
		    state.piece[color][ROOK] ||
		    state.piece[color][QUEEN])
			/*
			 | Oops.  There's a pawn, rook, or queen on the board.
			 | Someone could mate.
			 */
			return false;

	for (int color = WHITE; color <= BLACK; color++)
	{
		n_count += count(state.piece[color][KNIGHT]);
		b_count += count(state.piece[color][BISHOP]);
		b_array[color][WHITE] = count(state.piece[color][BISHOP] & SQUARES_WHITE);
		b_array[color][BLACK] = count(state.piece[color][BISHOP] & SQUARES_BLACK);
	}
	return n_count + b_count <= 1 ||
	       n_count == 0 && b_count == 2 && b_array[WHITE][WHITE] == b_array[BLACK][WHITE] && b_array[WHITE][BLACK] == b_array[BLACK][BLACK];
}

/*----------------------------------------------------------------------------*\
 |				    three()				      |
\*----------------------------------------------------------------------------*/
bool board_base::three() const
{

/* Is the game drawn by threefold repetition? */

	int sum = 1;

	for (list<bitboard_t>::const_reverse_iterator it = hashes.rbegin(); it != hashes.rend(); it++)
		if (*it == hash)
			if (++sum == 3)
				return true;
	return false;
}

/*----------------------------------------------------------------------------*\
 |				    fifty()				      |
\*----------------------------------------------------------------------------*/
bool board_base::fifty() const
{

/* Is the game drawn by the fifty move rule? */

	int num[COLORS] = {count(rotation[ZERO][WHITE]), count(rotation[ZERO][BLACK])}, sum = 0;

	for (list<state_t>::const_reverse_iterator it = states.rbegin(); it != states.rend(); it++)
	{
		if (it->piece[!it->whose][PAWN] != state.piece[!it->whose][PAWN] || count(ALL(*it, it->whose)) != num[it->whose])
			return false;
		if (++sum == 50)
			return true;
	}
	return false;
}

/*----------------------------------------------------------------------------*\
 |				    count()				      |
\*----------------------------------------------------------------------------*/
int board_base::count(bitboard_t b) const
{

/* Count the number of pieces in a BitBoard. */

	static const int table[] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
	int sum = 0;

	for (; b; b >>= 4)
		sum += table[b & 0xF];
	return sum;
}

/*
 | These next two methods, we shamelessly yoinked from the GNU C Library,
 | version 2.5, copyright � 1991-1998, the Free Software Foundation, originally
 | written by Torbjorn Granlund <tege@sics.se>.
 */

/*----------------------------------------------------------------------------*\
 |				   find_64()				      |
\*----------------------------------------------------------------------------*/
int board_base::find_64(int64_t n) const
{

/*
 | Find the first (least significant) set bit in a 64-bit integer.  The return
 | value ranges from 0 (for no bits set) to 64 (for only the most significant
 | bit set).
 */

#if defined(OS_X) || defined(WINDOWS)
	n &= -n;
	int shift = (uint64_t) n <= 0xFFFFFFFFULL ? 0 : 32;
#endif

#if defined(LINUX)
	return ffsll(n);
#elif defined(OS_X)
	return ffs(n >> shift) + shift;
#elif defined(WINDOWS)
	return find_32(n >> shift) + shift;
#endif
}

/*----------------------------------------------------------------------------*\
 |				   find_32()				      |
\*----------------------------------------------------------------------------*/
int board_base::find_32(int32_t n) const
{

/*
 | Find the first (least significant) set bit in a 32-bit integer.  The return
 | value ranges from 0 (for no bits set) to 32 (for only the most significant
 | bit set).
 */

#if defined(LINUX) || defined(OS_X)
	return ffs(n);
#elif defined(WINDOWS)
	static const uint8_t table[] =
	{
		0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
		8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
	};
	n &= -n;
	int shift = n <= 0xFFFF ? (n <= 0xFF ? 0 : 8) : (n <= 0xFFFFFF ?  16 : 24);
	return table[n >> shift] + shift;
#endif
}

/*----------------------------------------------------------------------------*\
 |				    rotate()				      |
\*----------------------------------------------------------------------------*/
bitboard_t board_base::rotate(bitboard_t b1, int map, int angle) const
{

/* Rotate a BitBoard. */

	bitboard_t b2 = 0;

	for (int n, x, y; (n = FST(b1)) != -1; BIT_CLR(b1, x, y))
	{
		x = n & 0x7;
		y = n >> 3;
		BIT_SET(b2, coord[map][angle][x][y][X], coord[map][angle][x][y][Y]);
	}
	return b2;
}

/*----------------------------------------------------------------------------*\
 |				  randomize()				      |
\*----------------------------------------------------------------------------*/
uint64_t board_base::randomize() const
{

/* Generate a 64-bit pseudo-random number. */

#if defined(LINUX)
	return (uint64_t) rand() << 32 | rand();
#elif defined(OS_X)
	return (uint64_t) arc4random() << 32 | arc4random();
#elif defined(WINDOWS)
	return (uint64_t) rand() << 32 | rand();
#endif
}

/*----------------------------------------------------------------------------*\
 |				    insert()				      |
\*----------------------------------------------------------------------------*/
void board_base::insert(int x, int y, bitboard_t b, int angle, list<move_t> &l, bool pos) const
{

/* Prepend or append a piece's possible moves to a list. */

	move_t m;
	m.old_x = x;
	m.old_y = y;
	m.value = m.promo = 0;

	for (int n; (n = FST(b)) != -1; BIT_CLR(b, x, y))
	{
		x = n & 0x7;
		y = n >> 3;
		m.new_x = coord[UNMAP][angle][x][y][X];
		m.new_y = coord[UNMAP][angle][x][y][Y];
		if (pos == FRONT)
			l.push_front(m);
		else
			l.push_back(m);
	}
}