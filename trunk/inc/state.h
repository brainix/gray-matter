/*----------------------------------------------------------------------------*\
 |  state.h - state interface and implementation                              |
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

#ifndef STATE_H
#define STATE_H

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "bitboard.h"

/// This structure describes the entire state of the board.  

/// This structure contains the 12 BitBoards needed to represent the state of
/// the board along with castling statuses, en passant vulnerability, the color
/// on move, and the 50 move rule counter.
///
/// Subtle!  In the en passant vulnerability field, we need only store the file
/// of the pawn susceptible to en passant.  Its rank is implied by the color on
/// move (which is also kept in the state).  If white is on move, then the pawn
/// susceptible to en passant must be black and on rank 5.  If black is on move,
/// then the pawn susceptible to en passant must be white and on rank 4.
typedef struct state
{
    bitboard_t piece[COLORS][SHAPES]; ///< 12 BitBoards.
    int castle[COLORS][SIDES];        ///< Castling statuses.
    int en_passant;                   ///< En passant vulnerability.
    bool on_move;                     ///< Color on move.
    int fifty;                        ///< 50 move rule counter.
} state_t;

/// This macro assembles a BitBoard that contains all of a color's pieces.
#define ALL(s, c)   ((s).piece[c][PAWN]   | (s).piece[c][KNIGHT] | \
                     (s).piece[c][BISHOP] | (s).piece[c][ROOK]   | \
                     (s).piece[c][QUEEN]  | (s).piece[c][KING])

#endif
