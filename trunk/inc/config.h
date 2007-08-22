/*----------------------------------------------------------------------------*\
 |	config.h - user-tunable settings				      |
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

// Windows insists on feeling included by being included first.
#if defined(WINDOWS)
#include <windows.h>
#endif



// This file contains user-tunable settings for Gray Matter.  If you don't know
// what you're doing, leave this file alone.  The defaults are good enough for
// me, so they're more than good enough for you.  ;-)



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

#define BOOK_NAME	"book.pgn" // The opening book file name.
#define BOOK_MOVES	40         // The number of moves to read per game from the opening book.
#define SEARCH_ENGINE	"MTD(f)"   // The move search engine.

// All of the values in this section must be >= 1.
#define XPOS_TABLE_MB	64         // The transposition table size (in MB).
#define PAWN_TABLE_MB	 1         // The pawn table size (in MB).
#define OVERHEAD	 1         // The move search overhead (in centiseconds).
#define MIN_DEPTH	 2         // The minimum search depth (in plies).
#define MAX_DEPTH	16         // The maximum search depth (in plies).



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

// Values of material (in centipawns):
#define WEIGHT_PAWN		  100
#define WEIGHT_KNIGHT		  300
#define WEIGHT_BISHOP		  300
#define WEIGHT_ROOK		  500
#define WEIGHT_QUEEN		  900
#define WEIGHT_KING		10000
#define WEIGHT_ILLEGAL		20000

// Values of pawn formations:
#define WEIGHT_ISOLATED		  -10
#define WEIGHT_DOUBLED		  -10
#define WEIGHT_BACKWARD		  -10
#define WEIGHT_PASSED		   10

// Values of castling statuses:
#define WEIGHT_CANT_CASTLE	  -20
#define WEIGHT_CAN_CASTLE	    0
#define WEIGHT_HAS_CASTLED	   40

// Value of tempo:
#define WEIGHT_TEMPO		   33

// How much we hate our opponent:
#define WEIGHT_CONTEMPT		  100



/*----------------------------------------------------------------------------*\
 |		There are no tunable settings beyond this point!	      |
\*----------------------------------------------------------------------------*/

// The following preprocessor directives are used to ensure that the user has
// chosen sane values for the preceeding settings.

#if BOOK_MOVES < 0
#error "In inc/config.h, BOOK_MOVES must be >= 0."
#endif

#if XPOS_TABLE_MB < 1
#error "In inc/config.h, XPOS_TABLE_MB must be >= 1."
#endif

#if PAWN_TABLE_MB < 1
#error "In inc/config.h, PAWN_TABLE_MB must be >= 1."
#endif

#if MIN_DEPTH < 1
#error "In inc/config.h, MIN_DEPTH must be >= 1."
#endif

#if MIN_DEPTH >= MAX_DEPTH
#error "In inc/config.h, MIN_DEPTH must be < MAX_DEPTH."
#endif

#if OVERHEAD < 1
#error "In inc/config.h, OVERHEAD must be >= 1."
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
