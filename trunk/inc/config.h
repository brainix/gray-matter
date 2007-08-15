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

/* Windows insists on feeling included by being included first. */
#if defined(WINDOWS)
#include <windows.h>
#endif



/*
 | This file contains user-tunable settings for Gray Matter.  If you don't know
 | what you're doing, leave this file alone.  The defaults are good enough for
 | me, so they're more than good enough for you.  ;-)
 */



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

#define BOOK_NAME	"book.pgn" // The opening book file name.
#define BOOK_MOVES	40         // The number of moves to read per game in the opening book.
#define SEARCH_ENGINE	"MTD(f)"   // The move search engine.

/* All of the values in this section must be >= 1. */
#define XPOS_TABLE_MB	64         // The transposition table size (in MB).
#define PAWN_TABLE_MB	 1         // The pawn table size (in MB).
#define OVERHEAD	 1         // The move search overhead (in centiseconds).
#define MIN_DEPTH	 2         // The minimum search depth (in plies).
#define MAX_DEPTH	16         // The maximum search depth (in plies).



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

/* Values of material (in centipawns): */
#define WEIGHT_PAWN		  100
#define WEIGHT_KNIGHT		  300
#define WEIGHT_BISHOP		  300
#define WEIGHT_ROOK		  500
#define WEIGHT_QUEEN		  900
#define WEIGHT_KING		10000
#define WEIGHT_ILLEGAL		20000

/* Values of pawn formations: */
#define WEIGHT_ISOLATED		  -10
#define WEIGHT_DOUBLED		  -10
#define WEIGHT_BACKWARD		  -10
#define WEIGHT_PASSED		   10

/* Values of castling statuses: */
#define WEIGHT_CANT_CASTLE	  -20
#define WEIGHT_CAN_CASTLE	    0
#define WEIGHT_HAS_CASTLED	   40

/* Value of tempo: */
#define WEIGHT_TEMPO		   33

/* How much we hate our opponent: */
#define WEIGHT_CONTEMPT		  100



/*----------------------------------------------------------------------------*\
 |		There are no tunable settings beyond this point!	      |
\*----------------------------------------------------------------------------*/

/*
 | The following preprocessor directives are used to ensure that the user has
 | chosen sane values for the preceeding settings.
 */

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

/* Piece colors: */
#define WHITE		0
#define BLACK		1
#define COLORS		2

/* Piece shapes: */
#define PAWN		0
#define KNIGHT		1
#define BISHOP		2
#define ROOK		3
#define QUEEN		4
#define KING		5
#define SHAPES		6

/* Castling sides: */
#define QUEEN_SIDE	0
#define KING_SIDE	1
#define SIDES		2

/* Castling statuses: */
#define CAN_CASTLE	0
#define CANT_CASTLE	1
#define HAS_CASTLED	2
#define CASTLE_STATS	3

/* Castling requirements: */
#define UNOCCUPIED	0 // Squares which mustn't be occupied.
#define UNATTACKED	1 // Squares which mustn't be attacked.
#define REQS		2

/* Game statuses: */
#define IN_PROGRESS	0 // Still in progress.
#define STALEMATE	1 // Drawn by stalemate.
#define INSUFFICIENT	2 // Drawn by insufficient material.
#define THREE		3 // Drawn by threefold repetition.
#define FIFTY		4 // Drawn by fifty move rule.
#define CHECKMATE	5 // Checkmated.
#define ILLEGAL		6 // Post-checkmated (king captured).
#define GAME_STATS	7

/* Game phases: */
#define OPENING		0
#define MIDGAME		1
#define ENDGAME		2
#define PHASES		3

/* Rotated bitboard maps: */
#define MAP		0
#define UNMAP		1
#define MAPS		2

/* Rotated bitboard angles: */
#define L45		0
#define ZERO		1
#define R45		2
#define R90		3
#define ANGLES		4

/* Board coordinates: */
#define X		0 // x-coordinate (file).
#define Y		1 // y-coordinate (rank).
#define COORDS		2

/* List positions: */
#define FRONT		0
#define BACK		1
#define POSITIONS	2

/* Transposition table entry replacement policies: */
#define DEEP		0 // Replace if same depth or deeper.
#define FRESH		1 // Replace always.
#define POLICIES	2

/* Transposition table entry types: */
#define USELESS		0 // Useless.
#define UPPER		1 // Upper bound.
#define LOWER		2 // Lower bound.
#define EXACT		3 // Exact MiniMax value.
#define BOOK		4 // Prescribed by the opening book.
#define ENTRY_TYPES	5

/* Search statuses: */
#define IDLING		0 // Masturbating.
#define THINKING	1 // Thinking (on our own time).
#define PONDERING	2 // Pondering (on our opponent's time).
#define QUITTING	3 // Terminating search thread.
#define SEARCH_STATS	4

/* Convenient bitboards: */
#define SQUARES_CENTER		0x0000001818000000ULL //  4 center squares.
#define SQUARES_EXPANDED_CENTER	0x00003C3C3C3C0000ULL // 16 center squares.
#define SQUARES_PRINCIPAL_DIAG	0x8142241818244281ULL // 16 principal diagonal squares.
#define SQUARES_WHITE_SIDE	0x00000000FFFFFFFFULL // 32 white side squares.
#define SQUARES_BLACK_SIDE	0xFFFFFFFF00000000ULL // 32 black side squares.
#define SQUARES_WHITE		0x55AA55AA55AA55AAULL // 32 white squares.
#define SQUARES_BLACK		0xAA55AA55AA55AA55ULL // 32 black squares.
#define SQUARES_QUEEN_SIDE	0x0F0F0F0F0F0F0F0FULL // 32 queen side squares.
#define SQUARES_KING_SIDE	0xF0F0F0F0F0F0F0F0ULL // 32 king side squares.
#define SQUARES_CORNER		0x8100000000000081ULL //  4 corner squares.

/* Time control styles: */
#define CONVENTIONAL	0
#define INCREMENTAL	1
#define SECS_PER_MOVE	2

/* Memory units: */
#define  B		1           // Byte.
#define KB		(1024 * B)  // Kilobyte.
#define MB		(1024 * KB) // Megabyte.
#define GB		(1024 * MB) // Gigabyte.
#define TB		(1024 * GB) // Terabyte.

#define INFINITY	32767 // Close enough.  ;-)
