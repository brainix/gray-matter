/*----------------------------------------------------------------------------*\
 |	board_base.cpp - board representation implementation		      |
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
#include "board_base.h"

// This array maps coordinates between rotated BitBoards:
static int coord[MAPS][ANGLES][8][8][COORDS] =
{
	// From 0° to 45° left:
	{{{{0,0},{1,0},{3,0},{6,0},{2,1},{7,1},{5,2},{4,3}},
	  {{2,0},{4,0},{7,0},{3,1},{0,2},{6,2},{5,3},{4,4}},
	  {{5,0},{0,1},{4,1},{1,2},{7,2},{6,3},{5,4},{3,5}},
	  {{1,1},{5,1},{2,2},{0,3},{7,3},{6,4},{4,5},{1,6}},
	  {{6,1},{3,2},{1,3},{0,4},{7,4},{5,5},{2,6},{6,6}},
	  {{4,2},{2,3},{1,4},{0,5},{6,5},{3,6},{7,6},{2,7}},
	  {{3,3},{2,4},{1,5},{7,5},{4,6},{0,7},{3,7},{5,7}},
	  {{3,4},{2,5},{0,6},{5,6},{1,7},{4,7},{6,7},{7,7}}},

	// From 0° to 0°:
	 {{{0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7}},
	  {{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7}},
	  {{2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7}},
	  {{3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7}},
	  {{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{4,7}},
	  {{5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{5,7}},
	  {{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{6,7}},
	  {{7,0},{7,1},{7,2},{7,3},{7,4},{7,5},{7,6},{7,7}}},

	// From 0° to 45° right:
	 {{{4,3},{4,4},{3,5},{1,6},{6,6},{2,7},{5,7},{7,7}},
	  {{5,2},{5,3},{5,4},{4,5},{2,6},{7,6},{3,7},{6,7}},
	  {{7,1},{6,2},{6,3},{6,4},{5,5},{3,6},{0,7},{4,7}},
	  {{2,1},{0,2},{7,2},{7,3},{7,4},{6,5},{4,6},{1,7}},
	  {{6,0},{3,1},{1,2},{0,3},{0,4},{0,5},{7,5},{5,6}},
	  {{3,0},{7,0},{4,1},{2,2},{1,3},{1,4},{1,5},{0,6}},
	  {{1,0},{4,0},{0,1},{5,1},{3,2},{2,3},{2,4},{2,5}},
	  {{0,0},{2,0},{5,0},{1,1},{6,1},{4,2},{3,3},{3,4}}},

	// From 0° to 90° right:
	 {{{7,0},{6,0},{5,0},{4,0},{3,0},{2,0},{1,0},{0,0}},
	  {{7,1},{6,1},{5,1},{4,1},{3,1},{2,1},{1,1},{0,1}},
	  {{7,2},{6,2},{5,2},{4,2},{3,2},{2,2},{1,2},{0,2}},
	  {{7,3},{6,3},{5,3},{4,3},{3,3},{2,3},{1,3},{0,3}},
	  {{7,4},{6,4},{5,4},{4,4},{3,4},{2,4},{1,4},{0,4}},
	  {{7,5},{6,5},{5,5},{4,5},{3,5},{2,5},{1,5},{0,5}},
	  {{7,6},{6,6},{5,6},{4,6},{3,6},{2,6},{1,6},{0,6}},
	  {{7,7},{6,7},{5,7},{4,7},{3,7},{2,7},{1,7},{0,7}}}},

	// From 45° left to 0°:
	{{{{0,0},{2,1},{1,4},{3,3},{4,3},{5,3},{7,2},{6,5}},
	  {{0,1},{3,0},{2,3},{4,2},{5,2},{6,2},{3,7},{7,4}},
	  {{1,0},{0,4},{3,2},{5,1},{6,1},{7,1},{4,6},{5,7}},
	  {{0,2},{1,3},{4,1},{6,0},{7,0},{2,7},{5,5},{6,6}},
	  {{1,1},{2,2},{5,0},{0,7},{1,7},{3,6},{6,4},{7,5}},
	  {{2,0},{3,1},{0,6},{1,6},{2,6},{4,5},{7,3},{6,7}},
	  {{0,3},{4,0},{1,5},{2,5},{3,5},{5,4},{4,7},{7,6}},
	  {{1,2},{0,5},{2,4},{3,4},{4,4},{6,3},{5,6},{7,7}}},

	// From 0° to 0°:
	 {{{0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7}},
	  {{1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7}},
	  {{2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7}},
	  {{3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7}},
	  {{4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{4,7}},
	  {{5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{5,7}},
	  {{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{6,7}},
	  {{7,0},{7,1},{7,2},{7,3},{7,4},{7,5},{7,6},{7,7}}},

	// From 45° right to 0°:
	 {{{7,0},{6,2},{3,1},{4,3},{4,4},{4,5},{5,7},{2,6}},
	  {{6,0},{7,3},{4,2},{5,4},{5,5},{5,6},{0,3},{3,7}},
	  {{7,1},{3,0},{5,3},{6,5},{6,6},{6,7},{1,4},{0,5}},
	  {{5,0},{4,1},{6,4},{7,6},{7,7},{0,2},{2,5},{1,6}},
	  {{6,1},{5,2},{7,5},{0,0},{0,1},{1,3},{3,6},{2,7}},
	  {{7,2},{6,3},{1,0},{1,1},{1,2},{2,4},{4,7},{0,6}},
	  {{4,0},{7,4},{2,1},{2,2},{2,3},{3,5},{0,4},{1,7}},
	  {{5,1},{2,0},{3,2},{3,3},{3,4},{4,6},{1,5},{0,7}}},

	// From 90° right to 0°:
	 {{{0,7},{1,7},{2,7},{3,7},{4,7},{5,7},{6,7},{7,7}},
	  {{0,6},{1,6},{2,6},{3,6},{4,6},{5,6},{6,6},{7,6}},
	  {{0,5},{1,5},{2,5},{3,5},{4,5},{5,5},{6,5},{7,5}},
	  {{0,4},{1,4},{2,4},{3,4},{4,4},{5,4},{6,4},{7,4}},
	  {{0,3},{1,3},{2,3},{3,3},{4,3},{5,3},{6,3},{7,3}},
	  {{0,2},{1,2},{2,2},{3,2},{4,2},{5,2},{6,2},{7,2}},
	  {{0,1},{1,1},{2,1},{3,1},{4,1},{5,1},{6,1},{7,1}},
	  {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0}}}}
};

//
static int diag_index[15] = {0, 1, 3, 6, 10, 15, 21, 28, 36, 43, 49, 54, 58, 61, 63};
static bitrow_t diag_mask[15] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};

// Whether the moves and hash keys have been pre-computed:
bool precomputed_board_base = false;

// Pre-computed moves:
bitboard_t squares_king[8][8];
bitrow_t squares_row[8][256];
bitboard_t squares_knight[8][8];
static bitboard_t squares_castle[COLORS][SIDES][REQS] =
{
	// The squares which must be unoccupied on the queen side in order for
	// the white king to be able to castle:
	{{0x000000000000000EULL,
	// The squares which must be unattacked on the queen side in order for
	// the white king to be able to castle:
	  0x000000000000001CULL},
	// The squares which must be unoccupied on the king side in order for
	// the white king to be able to castle:
	 {0x0000000000000060ULL,
	// The squares which must be unattacked on the king side in order for
	// the white king to be able to castle:
	  0x0000000000000070ULL}},
	// The squares which must be unoccupied on the queen side in order for
	// the black king to be able to castle:
	{{0x0E00000000000000ULL,
	// The squares which must be unattacked on the queen side in order for
	// the black king to be able to castle:
	  0x1C00000000000000ULL},
	// The squares which must be unoccupied on the king side in order for
	// the black king to be able to castle:
	 {0x6000000000000000ULL,
	// The squares which must be unattacked on the king side in order for
	// the black king to be able to castle.
	  0x7000000000000000ULL}}
};
bitboard_t squares_adj_cols[8];
bitboard_t squares_pawn_attacks[COLORS][8][8];

// Zobrist hash keys:
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

// Constructor.  Important!  Seed the random number generator - issue
// srand(time(NULL)); - before instantiating this class!

	if (!precomputed_board_base)
	{
		precomp_king();
		precomp_row();
		precomp_knight();
		precomp_pawn();
		precomp_key();
		precomputed_board_base = true;
	}
	set_board();

	mutex_create(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				 ~board_base()				      |
\*----------------------------------------------------------------------------*/
board_base::~board_base()
{

// Destructor.

	mutex_destroy(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				       =				      |
\*----------------------------------------------------------------------------*/
board_base& board_base::operator=(const board_base& that)
{

// Overloaded assignment operator.

	// It's like this and like that and like this and uh.
	// It's like that and like this and like that and uh.
	// It's like this.  And who gives a f*ck about those?
	// So just chill, 'til the next episode.
	//
	//	Snoop Doggy Dogg and Dr. Dre on overloading the assignment
	//	operator
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

// Set the board to the initial position.

	init_state();
	init_rotation();
	init_hash();
}

/*----------------------------------------------------------------------------*\
 |				set_board_fen()				      |
\*----------------------------------------------------------------------------*/
bool board_base::set_board_fen(string& fen)
{

// Set the board according to the Forsyth-Edwards Notation (FEN) string.  Return
// whether the FEN string represents a legal position.

	size_t index = 0;
	int x = 0, y = 7;

	states.clear();
	for (int color = WHITE; color <= BLACK; color++)
		for (int shape = PAWN; shape <= KING; shape++)
			state.piece[color][shape] = 0;

	// Parse the piece placement data.
	while (fen[index] != ' ')
	{
		if (isalpha(fen[index]))
		{
			int color = isupper(fen[index]) ? WHITE : BLACK;
			int shape = PAWN;
			switch (toupper(fen[index]))
			{
				case 'P' : shape = PAWN;   break;
				case 'N' : shape = KNIGHT; break;
				case 'B' : shape = BISHOP; break;
				case 'R' : shape = ROOK;   break;
				case 'Q' : shape = QUEEN;  break;
				case 'K' : shape = KING;   break;
				default  : goto failure;   break;
			}
			if (x > 7)
				goto failure;
			BIT_SET(state.piece[color][shape], x++, y);
		}
		else if (isdigit(fen[index]))
			if ((x += fen[index] - '0') > 7)
				goto failure;
		else if (fen[index] == '/')
		{
			x = 0;
			if (--y < 0)
				goto failure;
		}
		else
			goto failure;
		if (++index >= fen.length())
			goto failure;
	}
	if (++index >= fen.length())
		goto failure;

	// Parse the active color.
	switch (fen[index])
	{
		case 'w' : state.whose = WHITE; break;
		case 'b' : state.whose = BLACK; break;
		default  : goto failure;        break;
	}
	if (++index >= fen.length())
		goto failure;
	if (fen[index] != ' ')
		goto failure;
	if (++index >= fen.length())
		goto failure;

	// Parse the castling availability.
	for (int color = WHITE; color <= BLACK; color++)
		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			state.castle[color][side] = CANT_CASTLE;
	if (fen[index] != '-')
		while (fen[index] != ' ')
		{
			if (toupper(fen[index]) != 'K' && toupper(fen[index]) != 'Q')
				goto failure;
			int color = isupper(fen[index]) ? WHITE : BLACK;
			int side = toupper(fen[index]) == 'K' ? KING_SIDE : QUEEN_SIDE;
			state.castle[color][side] = CAN_CASTLE;
			if (++index >= fen.length())
				goto failure;
		}
	else if (++index >= fen.length())
		goto failure;
	if (fen[index] != ' ')
		goto failure;
	if (++index >= fen.length())
		goto failure;

	// Parse the en passant target square.
	if (fen[index] == '-')
		state.en_passant = -1;
	else
	{
		if (fen[index] < 'a' || fen[index] > 'h')
			goto failure;
		state.en_passant = fen[index] - 'a';
		if (++index >= fen.length())
			goto failure;
		if (fen[index] < '1' || fen[index] > '8')
			goto failure;
	}
	if (++index >= fen.length())
		goto failure;
	if (fen[index] != ' ')
		goto failure;
	if (++index >= fen.length())
		goto failure;

	// Parse the halfmove clock.
	if (!isdigit(fen[index]))
		goto failure;
	state.fifty = 0;
	while (isdigit(fen[index]))
	{
		state.fifty = state.fifty * 10 + fen[index] - '0';
		if (++index >= fen.length())
			goto failure;
	}
	if (fen[index] != ' ')
		goto failure;
	if (++index >= fen.length())
		goto failure;

	// Sanity check the current state of the board resulting from the FEN
	// string.  For now, just make sure both colors have one king each and
	// the color off move isn't in check.
	for (int color = WHITE; color <= BLACK; color++)
		if (count_64(state.piece[color][KING]) != 1)
			goto failure;
	if (check(state.piece[OFF_MOVE][KING], ON_MOVE))
		goto failure;

	init_rotation();
	init_hash();
	return true;

failure:
	for (int color = WHITE; color <= BLACK; color++)
	{
		for (int shape = PAWN; shape <= KING; shape++)
			state.piece[color][shape] = 0;
		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			state.castle[color][side] = CANT_CASTLE;
	}
	state.en_passant = -1;
	state.whose = WHITE;
	state.fifty = 0;
	init_rotation();
	init_hash();
	return false;
}

/*----------------------------------------------------------------------------*\
 |				     lock()				      |
\*----------------------------------------------------------------------------*/
void board_base::lock()
{

// Wait for the board, then grab the board.

	mutex_lock(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				    unlock()				      |
\*----------------------------------------------------------------------------*/
void board_base::unlock()
{

// Release the board.

	mutex_unlock(&mutex);
}

/*----------------------------------------------------------------------------*\
 |				  get_whose()				      |
\*----------------------------------------------------------------------------*/
bool board_base::get_whose() const
{

// Return the color on move.

	return ON_MOVE;
}

/*----------------------------------------------------------------------------*\
 |				   get_hash()				      |
\*----------------------------------------------------------------------------*/
bitboard_t board_base::get_hash() const
{

// Return the hash key for the current state.

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
 |				get_num_moves()				      |
\*----------------------------------------------------------------------------*/
int board_base::get_num_moves() const
{
	return states.size();
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

// Is the current position zugzwang?
//
// In most positions, there's at least one move the color on move could make to
// improve her lot.  In these normal positions, null-move pruning works.
// However, in certain positions, her best move would be to pass (particularly
// in endgame).  These positions are called "zugzwang" (German for "compelled to
// move").  In these zugzwang positions, null-move pruning doesn't work.
//
// The search class calls this method on a particular position to decide whether
// or not to try null-move pruning.

	for (int color = WHITE; color <= BLACK; color++)
	{
		if (!state.piece[color][KNIGHT] &&
		    !state.piece[color][BISHOP] &&
		    !state.piece[color][ROOK]   &&
		    !state.piece[color][QUEEN])
			// One color only has pawns and a king.
			return true;
		if (!state.piece[color][KING])
			// One color doesn't even have a king.
			return true;
	}
	if (check(state.piece[ON_MOVE][KING], OFF_MOVE))
		// The color on move is in check.
		return true;
	return false;
}

/*----------------------------------------------------------------------------*\
 |				   generate()				      |
\*----------------------------------------------------------------------------*/
void board_base::generate(list<move_t> &l, bool only_legal_moves, bool only_captures)
{
	generate_king(l, only_captures);
	generate_queen(l, only_captures);
	generate_rook(l, only_captures);
	generate_bishop(l, only_captures);
	generate_knight(l, only_captures);
	generate_pawn(l, only_captures);

	if (only_legal_moves)
		for (list<move_t>::iterator it = l.begin(); it != l.end();)
		{
			make(*it);
			if (check(state.piece[OFF_MOVE][KING], ON_MOVE))
				it = l.erase(it);
			else
				it++;
			unmake();
		}
}

/*----------------------------------------------------------------------------*\
 |				     make()				      |
\*----------------------------------------------------------------------------*/
bool board_base::make(move_t m)
{

// Make a move.

	// Save the current state, rotated BitBoards, and hash keys.
	states.push_back(state);
	for (int angle = L45; angle <= R90; angle++)
		for (int color = WHITE; color <= COLORS; color++)
			rotations[angle][color].push_back(rotation[angle][color]);
	hashes.push_back(hash);
	pawn_hashes.push_back(pawn_hash);

	// If we're making a null move, skip a bunch of this nonsense.
	if (IS_NULL_MOVE(m))
		goto end;

	// Move the piece and remove the captured piece.
	for (int shape = PAWN; shape <= KING; shape++)
	{
		// Move the piece.
		if (BIT_GET(state.piece[ON_MOVE][shape], m.x1, m.y1))
		{
			BIT_CLR(state.piece[ON_MOVE][shape], m.x1, m.y1);
			BIT_SET(state.piece[ON_MOVE][shape], m.x2, m.y2);
			for (int angle = L45; angle <= R90; angle++)
			{
				BIT_CLR(rotation[angle][ON_MOVE], coord[MAP][angle][m.x1][m.y1][X], coord[MAP][angle][m.x1][m.y1][Y]);
				BIT_SET(rotation[angle][ON_MOVE], coord[MAP][angle][m.x2][m.y2][X], coord[MAP][angle][m.x2][m.y2][Y]);
			}
			hash ^= key_piece[ON_MOVE][shape][m.x1][m.y1];
			hash ^= key_piece[ON_MOVE][shape][m.x2][m.y2];
			if (shape == PAWN)
			{
				pawn_hash ^= key_piece[ON_MOVE][shape][m.x1][m.y1];
				pawn_hash ^= key_piece[ON_MOVE][shape][m.x2][m.y2];

				// We're moving a pawn.  Reset the 50 move rule
				// counter.
				state.fifty = -1;
			}
		}

		// Remove the captured piece.
		if (BIT_GET(state.piece[OFF_MOVE][shape], m.x2, m.y2))
		{
			BIT_CLR(state.piece[OFF_MOVE][shape], m.x2, m.y2);
			for (int angle = L45; angle <= R90; angle++)
				BIT_CLR(rotation[angle][OFF_MOVE], coord[MAP][angle][m.x2][m.y2][X], coord[MAP][angle][m.x2][m.y2][Y]);
			hash ^= key_piece[OFF_MOVE][shape][m.x2][m.y2];
			if (shape == PAWN)
				pawn_hash ^= key_piece[OFF_MOVE][shape][m.x2][m.y2];

			// We're capturing a piece.  Reset the 50 move rule
			// counter.
			state.fifty = -1;
		}
	}

	// If we're moving a piece from one of our rooks' initial positions,
	// make sure we're no longer marked able to castle on that rook's
	// side.
	if ((m.x1 == 0 || m.x1 == 7) && (m.y1 == (ON_MOVE ? 7 : 0)) && state.castle[ON_MOVE][m.x1 == 7] == CAN_CASTLE)
	{
		state.castle[ON_MOVE][m.x1 == 7] = CANT_CASTLE;
		hash ^= key_castle[ON_MOVE][m.x1 == 7][CANT_CASTLE];
	}

	// If we're moving a piece to one of our opponent's rooks' initial
	// positions, make sure our opponent is no longer marked able to castle
	// on that rook's side.
	if ((m.x2 == 0 || m.x2 == 7) && (m.y2 == (OFF_MOVE ? 7 : 0)) && state.castle[OFF_MOVE][m.x2 == 7] == CAN_CASTLE)
	{
		state.castle[OFF_MOVE][m.x2 == 7] = CANT_CASTLE;
		hash ^= key_castle[OFF_MOVE][m.x1 == 7][CANT_CASTLE];
	}

	// If we're moving the king:
	if (BIT_GET(state.piece[ON_MOVE][KING], m.x2, m.y2))
	{
		// If we're castling, move the rook and mark us having castled
		// on this side.
		if (abs((int) m.x1 - (int) m.x2) == 2)
		{
			BIT_CLR(state.piece[ON_MOVE][ROOK], m.x2 == 6 ? 7 : 0, ON_MOVE ? 7 : 0);
			BIT_SET(state.piece[ON_MOVE][ROOK], m.x2 == 6 ? 5 : 3, ON_MOVE ? 7 : 0);
			for (int angle = L45; angle <= R90; angle++)
			{
				BIT_CLR(rotation[angle][ON_MOVE], coord[MAP][angle][m.x2 == 6 ? 7 : 0][ON_MOVE ? 7 : 0][X], coord[MAP][angle][m.x2 == 6 ? 7 : 0][ON_MOVE ? 7 : 0][Y]);
				BIT_SET(rotation[angle][ON_MOVE], coord[MAP][angle][m.x2 == 6 ? 5 : 3][ON_MOVE ? 7 : 0][X], coord[MAP][angle][m.x2 == 6 ? 5 : 3][ON_MOVE ? 7 : 0][Y]);
			}
			hash ^= key_piece[ON_MOVE][ROOK][m.x2 == 6 ? 7 : 0][ON_MOVE ? 7 : 0];
			hash ^= key_piece[ON_MOVE][ROOK][m.x2 == 6 ? 5 : 3][ON_MOVE ? 7 : 0];
			state.castle[ON_MOVE][m.x2 == 6] = HAS_CASTLED;
			hash ^= key_castle[ON_MOVE][m.x2 == 6][HAS_CASTLED];
		}

		// At this point, we've moved the king.  Make sure we're no
		// longer marked able to castle on either side.
		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			if (state.castle[ON_MOVE][side] == CAN_CASTLE)
			{
				state.castle[ON_MOVE][side] = CANT_CASTLE;
				hash ^= key_castle[ON_MOVE][side][CANT_CASTLE];
			}
	}

	// If we're moving a pawn:
	hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];
	pawn_hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];
	if (BIT_GET(state.piece[ON_MOVE][PAWN], m.x2, m.y2))
	{
		// If we're promoting a pawn, replace it with the promotion
		// piece.
		if (m.promo)
		{
			BIT_CLR(state.piece[ON_MOVE][PAWN], m.x2, m.y2);
			BIT_SET(state.piece[ON_MOVE][m.promo], m.x2, m.y2);
			hash ^= key_piece[ON_MOVE][PAWN][m.x2][m.y2];
			hash ^= key_piece[ON_MOVE][m.promo][m.x2][m.y2];
			pawn_hash ^= key_piece[ON_MOVE][PAWN][m.x2][m.y2];
		}

		// If we're performing an en passant, remove the captured
		// pawn.
		if ((int) m.x2 == state.en_passant && m.y2 == (ON_MOVE ? 2 : 5))
		{
			BIT_CLR(state.piece[OFF_MOVE][PAWN], m.x2, m.y1);
			for (int angle = L45; angle <= R90; angle++)
				BIT_CLR(rotation[angle][OFF_MOVE], coord[MAP][angle][m.x2][m.y1][X], coord[MAP][angle][m.x2][m.y1][Y]);
			hash ^= key_piece[OFF_MOVE][PAWN][m.x2][m.y1];
			pawn_hash ^= key_piece[OFF_MOVE][PAWN][m.x2][m.y1];
		}

		// If we're advancing a pawn two squares, mark it vulnerable to
		// en passant.
		state.en_passant = abs((int) m.y1 - (int) m.y2) == 2 ? (int) m.x1 : -1;
	}
	else
		// Oops.  We're not moving a pawn.  Mark no pawn vulnerable to
		// en passant.
		state.en_passant = -1;
	hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];
	pawn_hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];

end:
	// Set the other color on move.
	state.whose = !state.whose;
	hash ^= key_whose;

	// Increment the 50 move rule counter.
	state.fifty++;

	// Update the rotated BitBoards.
	for (int angle = L45; angle <= R90; angle++)
		rotation[angle][COLORS] = rotation[angle][WHITE] | rotation[angle][BLACK];

	return true;
}

/*----------------------------------------------------------------------------*\
 |				    unmake()				      |
\*----------------------------------------------------------------------------*/
bool board_base::unmake()
{

// Take back the last move.

	if (states.empty())
		return false;

	// Restore the previous state, rotated BitBoards, and hash keys.
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
 |				 san_to_coord()				      |
\*----------------------------------------------------------------------------*/
move_t board_base::san_to_coord(string& san)
{

// Convert a move from Standard Algebraic Notation (SAN) to coordinate notation.
// In the current position, if the SAN string doesn't represent a legal move,
// return the null move.

	size_t index = 0;
	int shape = -1, x1 = -1, y1 = -1, x2 = -1, y2 = -1, promo = -1;
	bool capture = false;
	move_t m;

	SET_NULL_MOVE(m);
	m.value = 0;

	// Check for special cases.  O-O-O means we're castling queen side and
	// O-O means we're castling king side.
	if (san == "O-O-O" || san == "O-O")
	{
		int side = san == "O-O-O" ? QUEEN_SIDE : KING_SIDE;
		if (state.castle[ON_MOVE][side] != CAN_CASTLE                          ||
		    squares_castle[ON_MOVE][side][UNOCCUPIED] & rotation[ZERO][COLORS] ||
		    check(squares_castle[ON_MOVE][side][UNATTACKED], OFF_MOVE))
			return m;
		m.x2 = (m.x1 = 4) + (side ? 2 : -2);
		m.y2 = m.y1 = ON_MOVE ? 7 : 0;
		m.promo = 0;
		return m;
	}

	// If the SAN string begins with the letter 'K', 'Q', 'R', 'B', 'N', or
	// 'P', we're moving a king, queen, rook, bishop, knight, or pawn
	// respectively.  If it begins with none of these letters, we're to
	// assume we're moving a pawn.
	switch (san[index])
	{
		case 'K' :          shape = KING;   break;
		case 'Q' :          shape = QUEEN;  break;
		case 'R' :          shape = ROOK;   break;
		case 'B' :          shape = BISHOP; break;
		case 'N' :          shape = KNIGHT; break;
		case 'P' :          shape = PAWN;   break;
		default  : index--; shape = PAWN;   break;
	}
	if (++index >= san.length())
		return m;

	// If there's an 'x' here, it means the move is a capture.  Note this
	// (to verify it's a capture later).
	if (san[index] == 'x')
	{
		if (++index >= san.length())
			return m;
		capture = true;
	}

	// If there's a letter between 'a' and 'h' here, assume it specifies the
	// destination file.
	if (san[index] >= 'a' && san[index] <= 'h')
	{
		x2 = san[index] - 'a';
		if (++index >= san.length())
			return m;
	}

	// If there's an 'x' here, it means the move is a capture.  Note this
	// (to verify it's a capture later).
	if (san[index] == 'x')
	{
		if (++index >= san.length())
			return m;
		capture = true;
	}

	// If there's a number between 1 and 8 here, assume it specifies the
	// destination rank.
	if (san[index] >= '1' && san[index] <= '8')
		y2 = san[index++] - '1';

	// If there's an 'x' here, it means the move is a capture.  Note this
	// (to verify it's a capture later).
	if (index < san.length() && san[index] == 'x')
	{
		if (++index >= san.length())
			return m;
		capture = true;
	}

	// If there's a letter between 'a' and 'h' here, it must be followed by
	// a number between 1 and 8.  And it means our earlier assumption was
	// wrong - the previous letter and/or number actually specified the
	// source file and/or rank, and the current letter and number specify
	// the destination file and rank.
	if (index < san.length() && san[index] >= 'a' && san[index] <= 'h')
	{
		x1 = x2;
		y1 = y2;
		x2 = san[index] - 'a';
		if (++index >= san.length())
			return m;
		y2 = san[index++] - '1';
	}

	// If there's an = sign here, it must be followed by the letter 'Q',
	// 'R', 'B', or 'N'.  The = sign means we're promoting a pawn and the
	// following letter represents the piece we're promoting to: a queen,
	// rook, bishop, or knight respectively.  If there's no = sign here, it
	// means we're not promoting a pawn.
	if (index < san.length() && san[index] == '=')
	{
		if (++index >= san.length())
			return m;
		switch (san[index])
		{
			case 'Q' : index++; promo = QUEEN;  break;
			case 'R' : index++; promo = ROOK;   break;
			case 'B' : index++; promo = BISHOP; break;
			case 'N' : index++; promo = KNIGHT; break;
			default  :          return m;       break;
		}
	}
	else
		promo = 0;

	// At this point, we're supposed to have filled in all the info except
	// possibly the source file and rank.  But if we haven't filled in the
	// source file and rank, the rest of the info (the piece and the
	// destination file and rank) is supposed to be sufficient to uniquely
	// identify the move and derive the source file and rank.
	//
	// If we haven't already filled in the source file and rank, derive them
	// and fill them in now.
	if (x1 < 0 || y1 < 0)
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
			if ((x1 < 0 || x1 == (int) it->x1) &&
			    (y1 < 0 || y1 == (int) it->y1) &&
			               x2 == (int) it->x2  &&
			               y2 == (int) it->y2)
			{
				x1 = it->x1;
				y1 = it->y1;
				break;
			}
	}

	// At this point, we're supposed to have filled in all the info.  If we
	// haven't, the SAN string must've been badly formed.
	if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 || promo < 0)
		return m;

	// Verify the piece we're moving is actually sitting on the source file
	// and rank.
	if (!BIT_GET(state.piece[ON_MOVE][shape], x1, y1))
		return m;

	// If the SAN string didn't indicate a capture, verify the move actually
	// isn't a capture.  Conversely, if the SAN string did indicate a
	// capture, verify the move actually is a capture.
	if (capture != (bool) BIT_GET(rotation[ZERO][OFF_MOVE], x2, y2))
		return m;

	// OK, we appear to have a valid move.
	m.x1 = x1;
	m.y1 = y1;
	m.x2 = x2;
	m.y2 = y2;
	m.promo = promo;

	// The only thing that can go wrong now is for the move to leave us in
	// check.  Make sure this isn't the case.
	make(m);
	if (check(state.piece[OFF_MOVE][KING], ON_MOVE))
		SET_NULL_MOVE(m); // Oops, the move leaves us in check.
	unmake();
	return m;
}

/*----------------------------------------------------------------------------*\
 |				 coord_to_san()				      |
\*----------------------------------------------------------------------------*/
void board_base::coord_to_san(move_t m, string& san)
{

// Convert a move from coordinate notation to Standard Algebraic Notation (SAN).
// In the current position, if the move in coordinate notation doesn't represent
// a legal move, san is set to an empty string (but do not rely on this).

	int shape;
	ostringstream sanstr;

	// What shape is being moved?
	for (shape = PAWN; shape <= KING; shape++)
		if (BIT_GET(state.piece[ON_MOVE][shape], m.x1, m.y1))
		  break;

	if (shape == KING && ABS(m.x2 - m.x1) == 2) {
		// Must be a castling move
		int side = m.x2 > m.x1 ? KING_SIDE : QUEEN_SIDE;
		if (state.castle[ON_MOVE][side] == CAN_CASTLE &&
			!(squares_castle[ON_MOVE][side][UNOCCUPIED] & rotation[ZERO][COLORS]) &&
			!check(squares_castle[ON_MOVE][side][UNATTACKED], OFF_MOVE))
			sanstr << (side == QUEEN_SIDE ? "O-O-O" : "O-O");

	} else if (shape <= KING) {
		// If shape > KING, the move was illegal.
		// We're not castling, so start with the piece name.
		switch(shape) {
			case KING:		sanstr << "K"; break;
			case QUEEN:		sanstr << "Q"; break;
			case ROOK:		sanstr << "R"; break;
			case BISHOP:	sanstr << "B"; break;
			case KNIGHT:	sanstr << "N"; break;
			case PAWN:		break; // piece name omitted
			default:		break;
		}

		// In case of a pawn capture, insert from rank
		if (shape == PAWN && m.x1 != m.x2)
		  sanstr << static_cast<char>(m.x1 + 'a');

		// Check whether another piece of the same shape can reach the to square
		// If possible first try to distinguish the two by adding from file
		// If from files are the same, then use from rank
		list<move_t> l;
		bool add_rank = false, add_file = false;
		switch (shape)
		{
			case QUEEN  : generate_queen(l);  break;
			case ROOK   : generate_rook(l);   break;
			case BISHOP : generate_bishop(l); break;
			case KNIGHT : generate_knight(l); break;
			case PAWN   : generate_pawn(l);   break;
		}
		for (list<move_t>::iterator it = l.begin(); it != l.end(); it++) {
			if ((it->x1 != m.x1 || it->y1 != m.y1) &&
				it->x2 == m.x2 && it->y2 == m.y2) {
			  // We found another 'shape' that can move to the 'to' square
			  if(it->x1 != m.x1) {
				// It suffices to add rank of 'from' square
				add_rank = true;
			  } else {
				// Files must differ, so adding file of 'from' square
				add_file = true;
			  }
			}
		}
		if(add_rank)
		  sanstr << static_cast<char>(m.x1 + 'a');
		if(add_file)
		  sanstr << static_cast<char>(m.y1 + '1');

		// Add 'x' for captures.
		// Drunk pawns are assumed to be captures (possibly en passant).
		if (BIT_GET(ALL(state, OFF_MOVE), m.x2, m.y2) ||
			(shape == PAWN && m.x1 != m.x2))
		  sanstr << "x";

		// Add target square to notation
		sanstr << static_cast<char>(m.x2 + 'a') << static_cast<char>(m.y2 + '1');

		// Add promotion piece
		switch(m.promo) {
			case QUEEN:		sanstr << "=Q"; break;
			case ROOK:		sanstr << "=R"; break;
			case BISHOP:	sanstr << "=B"; break;
			case KNIGHT:	sanstr << "=N"; break;
			default:		break;
		}

		// If it's a pawn capture and the target square is empty,
		// indicate the move is en passant
		if (shape == PAWN && m.x1 != m.x2 &&
			!BIT_GET(ALL(state, OFF_MOVE), m.x2, m.y2)) {
		  sanstr << " e.p.";
		}

		// Check for check '+' or checkmate '#'
		make(m);
		if (get_status(true) == CHECKMATE)
		  sanstr << "#";
		else if(check(state.piece[ON_MOVE][KING], OFF_MOVE))
		  sanstr << "+";
		unmake();
	}

	// Replace san
	san.replace(0, san.length(), sanstr.str());
}

/*----------------------------------------------------------------------------*\
 |				    perft()				      |
\*----------------------------------------------------------------------------*/
int board_base::perft(int depth)
{

// From the current position, grow the move tree to the given depth and count
// the leaf nodes.  This may sound useless, but it makes for a good unit test
// and benchmark for the move generator.

	list<move_t> l;
	list<move_t>::iterator it;
	int nodes = 0;

	// Base case.
	if (depth == 0)
		return 1;

	// Recursive case.
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

// Initialize the state.

	// Clear the previous states.
	states.clear();

	// Initialize the current state.
	for (int color = WHITE; color <= BLACK; color++)
	{
		// Clear the board.
		for (int shape = PAWN; shape <= KING; shape++)
			state.piece[color][shape] = 0;

		// Place the pawns.
		ROW_SET(state.piece[color][PAWN], !color ? 1 : 6, 0xFF);

		// Place the other pieces.
		BIT_SET(state.piece[color][ROOK],   0, !color ? 0 : 7);
		BIT_SET(state.piece[color][KNIGHT], 1, !color ? 0 : 7);
		BIT_SET(state.piece[color][BISHOP], 2, !color ? 0 : 7);
		BIT_SET(state.piece[color][QUEEN],  3, !color ? 0 : 7);
		BIT_SET(state.piece[color][KING],   4, !color ? 0 : 7);
		BIT_SET(state.piece[color][BISHOP], 5, !color ? 0 : 7);
		BIT_SET(state.piece[color][KNIGHT], 6, !color ? 0 : 7);
		BIT_SET(state.piece[color][ROOK],   7, !color ? 0 : 7);

		// Mark both colors able to castle on both sides.
		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			state.castle[color][side] = CAN_CASTLE;
	}

	// Mark no pawn vulnerable to en passant, set white on move, and
	// initialize the 50 move rule counter.
	state.en_passant = -1;
	state.whose = WHITE;
	state.fifty = 0;
}

/*----------------------------------------------------------------------------*\
 |				init_rotation()				      |
\*----------------------------------------------------------------------------*/
void board_base::init_rotation()
{

// Initialize the rotated BitBoards.

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

// Initialize the Zobrist hash.

	// Clear the previous Zobrist hashes.
	hashes.clear();
	hash = 0;
	pawn_hashes.clear();
	pawn_hash = 0;

	// Initialize the current Zobrist hash.
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

	hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];
	pawn_hash ^= state.en_passant == -1 ? key_no_en_passant : key_en_passant[state.en_passant];

	if (state.whose == WHITE)
		hash ^= key_whose;
}

/*----------------------------------------------------------------------------*\
 |				 precomp_key()				      |
\*----------------------------------------------------------------------------*/
void board_base::precomp_key() const
{

// Pre-compute the Zobrist hash keys.

	for (int color = WHITE; color <= BLACK; color++)
	{
		for (int shape = PAWN; shape <= KING; shape++)
			for (int y = 0; y <= 7; y++)
				for (int x = 0; x <= 7; x++)
					key_piece[color][shape][x][y] = rand_64();

		for (int side = QUEEN_SIDE; side <= KING_SIDE; side++)
			for (int stat = CAN_CASTLE; stat <= HAS_CASTLED; stat++)
				key_castle[color][side][stat] = rand_64();
	}

	key_no_en_passant = rand_64();
	for (int x = 0; x <= 8; x++)
		key_en_passant[x] = rand_64();

	key_whose = rand_64();
}

/*----------------------------------------------------------------------------*\
 |				generate_king()				      |
\*----------------------------------------------------------------------------*/
void board_base::generate_king(list<move_t> &l, bool only_captures) const
{

// Generate the king moves.

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
		m.x2 = (m.x1 = 4) + (side ? 2 : -2);
		m.y2 = m.y1 = ON_MOVE ? 7 : 0;
		m.value = m.promo = 0;
		l.push_front(m);
	}
}

/*----------------------------------------------------------------------------*\
 |				generate_queen()			      |
\*----------------------------------------------------------------------------*/
void board_base::generate_queen(list<move_t> &l, bool only_captures) const
{

// Generate the queen moves.

	bitboard_t from = state.piece[ON_MOVE][QUEEN];

	for (int n, x, y; (n = FST(from)) != -1; BIT_CLR(from, x, y))
	{
		x = n & 0x7;
		y = n >> 3;

		// Generate the horizontal and vertical moves.
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

		// Generate the diagonal moves.
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

// Generate the rook moves.

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

// Generate the bishop moves.

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

// Generate the knight moves.

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

// Generate the pawn moves.

	bitboard_t b;
	move_t m;
	m.value = m.promo = 0;

	// For its first move, a pawn can advance two squares.
	if (!only_captures)
	{
		b = state.piece[ON_MOVE][PAWN] & ROW_MSK(ON_MOVE ? 6 : 1);
		for (int y = 1; y <= 2; y++)
		{
			b <<= ON_MOVE ? 0 : 8;
			b >>= ON_MOVE ? 8 : 0;
			b &= ~rotation[ZERO][COLORS];
		}
		for (int n; (n = FST(b)) != -1; BIT_CLR(b, m.x2, m.y2))
		{
			m.x1 = m.x2 = n & 0x7;
			m.y1 = (m.y2 = n >> 3) + (ON_MOVE ? 2 : -2);
			l.push_back(m);
		}
	}

	// If our pawn is on our fifth row, and our opponent's pawn is beside
	// our pawn, and, as her last move, our opponent advanced her pawn two
	// squares, then we can perform an en passant.
	if (state.en_passant != -1)
	{
		m.promo = 0;
		m.y2 = ON_MOVE ? 2 : 5;
		m.x2 = state.en_passant;
		if (state.en_passant != 0 && BIT_GET(state.piece[ON_MOVE][PAWN], m.x1 = state.en_passant - 1, m.y1 = ON_MOVE ? 3 : 4))
			l.push_front(m);
		if (state.en_passant != 7 && BIT_GET(state.piece[ON_MOVE][PAWN], m.x1 = state.en_passant + 1, m.y1 = ON_MOVE ? 3 : 4))
			l.push_front(m);
	}

	// A pawn can advance one square.
	b = state.piece[ON_MOVE][PAWN];
	b <<= ON_MOVE ? 0 : 8;
	b >>= ON_MOVE ? 8 : 0;
	b &= ~rotation[ZERO][COLORS];
	for (int n; (n = FST(b)) != -1; BIT_CLR(b, m.x2, m.y2))
	{
		m.x1 = m.x2 = n & 0x7;
		m.y1 = (m.y2 = n >> 3) + (ON_MOVE ? 1 : -1);
		if (m.y2 != (ON_MOVE ? 0 : 7))
		{
			if (!only_captures)
				l.push_back(m);
			continue;
		}
		for (m.promo = KNIGHT; m.promo <= QUEEN; m.promo++)
			l.push_front(m);
		m.promo = 0;
	}

	// A pawn can capture diagonally.
	for (int x = -1; x <= 1; x += 2)
	{
		b = state.piece[ON_MOVE][PAWN];
		b <<= ON_MOVE ? 0 : x == -1 ? 7 : 9;
		b >>= ON_MOVE ? x == -1 ? 9 : 7 : 0;
		COL_CLR(b, x == -1 ? 7 : 0);
		b &= rotation[ZERO][OFF_MOVE];
		for (int n; (n = FST(b)) != -1; BIT_CLR(b, m.x2, m.y2))
		{
			m.x1 = (m.x2 = n & 0x7) - x;
			m.y1 = (m.y2 = n >> 3) + (ON_MOVE ? 1 : -1);
			if (m.y2 != (ON_MOVE ? 0 : 7))
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

// Pre-compute the king moves.

	// Imagine an empty board except for a king at (x, y).  Mark the king's
	// legal moves in the BitBoard squares_king[x][y].
	for (int n = 0; n <= 63; n++)
	{
		int x = n & 0x7;
		int y = n >> 3;

		squares_king[x][y] = 0;
		for (int k = -1; k <= 1; k++)
			for (int j = -1; j <= 1; j++)
			{
				if (!j && !k)
					// Oops.  The king can't stand still.
					continue;
				if (x + j < 0 || x + j > 7 ||
				    y + k < 0 || y + k > 7)
					// Oops.  The king can't step off the
					// board.
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

// Pre-compute the sliding piece moves.

	// Imagine a sliding piece on square x.  For each possible occupancy
	// (combination) of enemy pieces along the sliding piece's row, mark the
	// sliding piece's legal moves in the bitrow squares_row[x][occ].
	for (int x = 0; x <= 7; x++)
		for (int occ = 0; occ <= 0xFF; occ++)
		{
			squares_row[x][occ] = 0;
			for (int dir = -1; dir <= 1; dir += 2)
				for (int j = x + dir; j >= 0 && j <= 7; j += dir)
				{
					BIT_SET(squares_row[x][occ], j, 0);
					if (BIT_GET(occ, j, 0))
						// Oops.  The sliding piece
						// can't slide through an enemy
						// piece.
						break;
				}
		}
}

/*----------------------------------------------------------------------------*\
 |				precomp_knight()			      |
\*----------------------------------------------------------------------------*/
void board_base::precomp_knight() const
{

// Pre-compute the knight moves.

	// Imagine an empty board except for a knight at (x, y).  Mark the
	// knight's legal moves in the BitBoard squares_knight[x][y].
	for (int n = 0; n <= 63; n++)
	{
		int x = n & 0x7;
		int y = n >> 3;

		squares_knight[x][y] = 0;
		for (int k = -2; k <= 2; k++)
			for (int j = -2; j <= 2; j++)
			{
				if (abs(j) == abs(k) || !j || !k)
					// Oops.  The knight can only jump two
					// squares in one direction and one
					// square in a perpendicular direction.
					continue;
				if (x + j < 0 || x + j > 7 ||
				    y + k < 0 || y + k > 7)
					// Oops.  The knight can't jump off the
					// board.
					continue;
				BIT_SET(squares_knight[x][y], x + j, y + k);
			}
	}
}

/*----------------------------------------------------------------------------*\
 |				 precomp_pawn()				      |
\*----------------------------------------------------------------------------*/
void board_base::precomp_pawn() const
{
	// For every column, pre-compute its adjacent columns.
	for (int x = 0; x <= 7; x++)
	{
		squares_adj_cols[x] = 0;
		for (int j = x == 0 ? 1 : -1; j <= (x == 7 ? -1 : 1); j += 2)
			squares_adj_cols[x] |= COL_MSK(x + j);
	}

	// For both colors and every square on the board, pre-compute the
	// squares from which pawns of the color can attack.  In other words,
	// squares_pawn_attacks[color][x][y] is the BitBoard representing the
	// "from" squares and (x, y) are the coordinates representing the "to"
	// square.
	for (int color = WHITE; color <= BLACK; color++)
		for (int n = 0; n <= 63; n++)
		{
			int x = n & 0x7;
			int y = n >> 3;

			squares_pawn_attacks[color][x][y] = 0;
			if (color == WHITE && (y == 0 || y == 1) ||
			    color == BLACK && (y == 6 || y == 7))
				// A white pawn can never attack a square in
				// rank 1 or 2.  Similarly, a black pawn can
				// never attack a square in rank 6 or 7.
				continue;
			bitboard_t row = ROW_MSK(y + (!color ? -1 : 1));
			bitboard_t cols = squares_adj_cols[x];
			squares_pawn_attacks[color][x][y] =  row & cols;
		}
}

/*----------------------------------------------------------------------------*\
 |				     mate()				      |
\*----------------------------------------------------------------------------*/
int board_base::mate()
{

// Is the game over due to stalemate or checkmate?  We test for both conditions
// in the same method because they're so similar.  During both, the color on
// move doesn't have a legal move.  The only difference: during stalemate, her
// king isn't attacked; during checkmate, her king is attacked.

	list<move_t> l;
	list<move_t>::iterator it;
	bool escape = false;

	// Look for a legal move.
	generate(l);
	for (it = l.begin(); it != l.end(); it++)
	{
		make(*it);
		if (!check(state.piece[OFF_MOVE][KING], ON_MOVE))
			escape = true;
		unmake();
		if (escape)
			// The color on move has a legal move; the game isn't
			// over.
			return IN_PROGRESS;
	}

	// The color on move doesn't have a legal move; the game is over.  If
	// the king isn't attacked, the game is over due to stalemate.
	// Otherwise, the game is over due to checkmate.
	if (!check(state.piece[ON_MOVE][KING], OFF_MOVE))
		return STALEMATE;
	return CHECKMATE;
}

/*----------------------------------------------------------------------------*\
 |				    check()				      |
\*----------------------------------------------------------------------------*/
bool board_base::check(bitboard_t b1, bool color) const
{

// Is any of the specified squares being attacked by the specified color?  Check
// for check.  ;-)

	for (int n, x, y; (n = FST(b1)) != -1; BIT_CLR(b1, x, y))
	{
		x = n & 0x7;
		y = n >> 3;

		// Look for a king attack.
		if (squares_king[x][y] & state.piece[color][KING])
			return true;

		// Look for a horizontal or vertical queen or rook attack.  The
		// logic here is interesting.  Pretend our king were a rook.
		// Would it be able to capture a rook?  If so, we're in check.
		// If not, we're not in check, at least not by a rook.
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

		// Look for a diagonal queen or bishop attack.  The logic here
		// is interesting.  Pretend our king were a bishop.  Would it be
		// able to capture a bishop?  If so, we're in check.  If not,
		// we're not in check, at least not by a bishop.
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

		// Look for a knight attack.  The logic here is interesting.
		// Pretend our king were a knight.  Would it be able to capture
		// a knight?  If so, we're in check.  If not, we're not in
		// check, at least not by a knight.
		if (squares_knight[x][y] & state.piece[color][KNIGHT])
			return true;

		// Look for a pawn attack.  The logic here is the same as for
		// the previous pieces, but things are a bit easier because a
		// pawn can attack at most 2 squares and, conversely, a square
		// can be attacked by at most 2 pawns.  We simply mark the
		// squares in the (at most) 2 columns from which a pawn could
		// attack and find the intersection between those squares and
		// the squares in the (at most) 1 row from which a pawn could
		// attack.  This results in 0, 1, or 2 marked squares from which
		// a pawn could attack.  Then, we simply check whether an
		// opposing pawn sits on any of our marked squares.  If so,
		// we're in check.  If not, we're not in check, at least not by
		// a pawn.  Easy, breezy, beautiful.
		if (squares_pawn_attacks[color][x][y] & state.piece[color][PAWN])
			return true;
	}
	return false;
}

/*----------------------------------------------------------------------------*\
 |				 insufficient()				      |
\*----------------------------------------------------------------------------*/
bool board_base::insufficient() const
{

// Is the game drawn due to insufficient material?

	int n_count = 0, b_count = 0, b_array[COLORS][COLORS];

	for (int color = WHITE; color <= BLACK; color++)
		if (state.piece[color][PAWN] ||
		    state.piece[color][ROOK] ||
		    state.piece[color][QUEEN])
			// Oops.  There's a pawn, rook, or queen on the board.
			// Someone could mate.
			return false;

	for (int color = WHITE; color <= BLACK; color++)
	{
		n_count += count_64(state.piece[color][KNIGHT]);
		b_count += count_64(state.piece[color][BISHOP]);
		b_array[color][WHITE] = count_64(state.piece[color][BISHOP] & SQUARES_WHITE);
		b_array[color][BLACK] = count_64(state.piece[color][BISHOP] & SQUARES_BLACK);
	}
	return n_count + b_count <= 1 ||
	       n_count == 0 && b_count == 2 && b_array[WHITE][WHITE] == b_array[BLACK][WHITE] && b_array[WHITE][BLACK] == b_array[BLACK][BLACK];
}

/*----------------------------------------------------------------------------*\
 |				    three()				      |
\*----------------------------------------------------------------------------*/
bool board_base::three() const
{

// Is the game drawn by threefold repetition?

	list<bitboard_t>::const_reverse_iterator it;
	int sum = 1;

	for (it = hashes.rbegin(); it != hashes.rend(); it++)
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

// Is the game drawn by the fifty move rule?

	return state.fifty >= 50;
}

/*----------------------------------------------------------------------------*\
 |				    rotate()				      |
\*----------------------------------------------------------------------------*/
bitboard_t board_base::rotate(bitboard_t b1, int map, int angle) const
{

// Rotate a BitBoard.

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
 |				    insert()				      |
\*----------------------------------------------------------------------------*/
void board_base::insert(int x, int y, bitboard_t b, int angle, list<move_t> &l, bool pos) const
{

// Prepend or append a piece's possible moves to a list.

	move_t m;
	m.x1 = x;
	m.y1 = y;
	m.value = m.promo = 0;

	for (int n; (n = FST(b)) != -1; BIT_CLR(b, x, y))
	{
		x = n & 0x7;
		y = n >> 3;
		m.x2 = coord[UNMAP][angle][x][y][X];
		m.y2 = coord[UNMAP][angle][x][y][Y];
		if (pos == FRONT)
			l.push_front(m);
		else
			l.push_back(m);
	}
}
