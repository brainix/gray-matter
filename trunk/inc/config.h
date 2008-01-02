/*----------------------------------------------------------------------------*\
 |	config.h - user-tunable settings				      |
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

// Windows insists on feeling included by being included first.
#if defined(WINDOWS)
#include <windows.h>
#endif



// This file contains user-tunable settings for Gray Matter.  If you don't know
// what you're doing, leave this file alone.  The defaults are good enough for
// me, so they're more than good enough for you.  ;-)



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

// Default move search engine - must be "MTD(f)" or "NegaScout":
#define SEARCH_ENGINE	"MTD(f)"

// Default opening book file name:
#define BOOK_NAME	"book.pgn"

// All of the default values in this section must be >= 1.
#define XPOS_TABLE_MB	64 // Transposition table size (in MB).
#define PAWN_TABLE_MB	 1 // Pawn table size (in MB).
#define BOOK_MOVES	40 // Num moves to read per game in book (in plies).
#define OVERHEAD	 1 // Move search overhead (in centiseconds).
#define MAX_DEPTH	16 // Maximum search depth (in plies).
#define R		 2 // Null move pruning depth reduction factor (in plies).



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

// Values of material (in centipawns):
#define VALUE_PAWN		  100
#define VALUE_KNIGHT		  300
#define VALUE_BISHOP		  300
#define VALUE_ROOK		  500
#define VALUE_QUEEN		  900
#define VALUE_KING		10000
#define VALUE_ILLEGAL		20000

//
#define VALUE_CONTEMPT		    0 // How much we hate our opponent.



/*----------------------------------------------------------------------------*\
 |		There are no tunable settings beyond this point!	      |
\*----------------------------------------------------------------------------*/

// The following preprocessor directives are used to ensure that the user has
// chosen sane values for the preceeding settings.

#if XPOS_TABLE_MB < 1
#error "In inc/config.h, XPOS_TABLE_MB must be >= 1."
#endif

#if PAWN_TABLE_MB < 1
#error "In inc/config.h, PAWN_TABLE_MB must be >= 1."
#endif

#if BOOK_MOVES < 1
#error "In inc/config.h, BOOK_MOVES must be >= 1."
#endif

#if OVERHEAD < 1
#error "In inc/config.h, OVERHEAD must be >= 1."
#endif

#if MAX_DEPTH < 1
#error "In inc/config.h, MAX_DEPTH must be >= 1."
#endif

// Piece colors:
#define WHITE		0
#define BLACK		1
#define COLORS		2

// Piece shapes:
#define PAWN		0
#define KNIGHT		1
#define BISHOP		2
#define ROOK		3
#define QUEEN		4
#define KING		5
#define SHAPES		6

// Castling sides:
#define QUEEN_SIDE	0
#define KING_SIDE	1
#define SIDES		2

// Memory units:
#define  B		1           // Byte.
#define KB		(1024 * B)  // Kilobyte.
#define MB		(1024 * KB) // Megabyte.
#define GB		(1024 * MB) // Gigabyte.
#define TB		(1024 * GB) // Terabyte.

// Infinity:
#define INFINITY	32767 // Well, close enough.  ;-)
