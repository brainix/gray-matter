/*----------------------------------------------------------------------------*\
 |	config.h - user-tunable settings				      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 2 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
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

/* All of the values in this section must be >= 1. */
#define NUM_CPUS	 1 // The number of CPUs.
#define XPOS_TABLE_MB	64 // The transposition table size (in MB).
#define PAWN_TABLE_MB	 1 // The pawn table size (in MB).
#define DEPTH		16 // The maximum search depth (in plies).



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

/* Values of material (in centipawns): */
#define WEIGHT_PAWN		100
#define WEIGHT_KNIGHT		300
#define WEIGHT_BISHOP		300
#define WEIGHT_ROOK		500
#define WEIGHT_QUEEN		900
#define WEIGHT_KING		(9 * WEIGHT_QUEEN + 2 * WEIGHT_ROOK + 2 * WEIGHT_BISHOP + 2 * WEIGHT_KNIGHT)
#define WEIGHT_ILLEGAL		(-WEIGHT_KING - WEIGHT_PAWN)

/* Values of pawn formations: */
#define WEIGHT_ISOLATED		-10
#define WEIGHT_DOUBLED		-10
#define WEIGHT_BACKWARD		-10
#define WEIGHT_PASSED		 10

/* Values of castling statuses: */
#define WEIGHT_CANT_CASTLE	-20
#define WEIGHT_CAN_CASTLE	  0
#define WEIGHT_HAS_CASTLED	 40

/* How much we hate our opponent: */
#define WEIGHT_CONTEMPT		900

/*
 | The absolute value of the difference between the closest two of all of the
 | preceeding WEIGHT_* values - the increment by which MTD(f) shifts the alpha-
 | beta window:
 */
#define WEIGHT_INCREMENT	 10



/*----------------------------------------------------------------------------*\
 |		There are no tunable settings beyond this point!	      |
\*----------------------------------------------------------------------------*/

/*
 | The following preprocessor directives are used to ensure that the user has
 | chosen sane values for the preceeding settings.
 */

#if NUM_CPUS < 1
#error "In inc/config.h, NUM_CPUS must be >= 1."
#endif

#if XPOS_TABLE_MB < 1
#error "In inc/config.h, XPOS_TABLE_MB must be >= 1."
#endif

#if PAWN_TABLE_MB < 1
#error "In inc/config.h, PAWN_TABLE_MB must be >= 1."
#endif

#if DEPTH < 1
#error "In inc/config.h, DEPTH must be >= 1."
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
#define IN_PROGRESS	0
#define STALEMATE	1 // Drawn by stalemate.
#define INSUFFICIENT	2 // Drawn by insufficient material.
#define THREE		3 // Drawn by threefold repetition.
#define FIFTY		4 // Drawn by the fifty move rule.
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

/* Transposition table entry types: */
#define USELESS		0 // The score is useless.
#define UPPER		1 // The score represents an upper bound.
#define EXACT		2 // The score represents an exact MiniMax value.
#define LOWER		3 // The score represents a lower bound.
#define ENTRY_TYPES	4

/* Search statuses: */
#define IDLING		0 // Masturbating.
#define THINKING	1 // Thinking (on our own time).
#define PONDERING	2 // Pondering (on our opponent's time).
#define QUITTING	3 // Terminating search thread.
#define SEARCH_STATS	4

/* Convenient bitboards: */
#define CENTER		0x0000001818000000ULL //  4 center squares.
#define EXPANDED_CENTER	0x00003C3C3C3C0000ULL // 16 center squares.
#define PRINCIPAL_DIAGS	0x8142241818244281ULL // 16 principal diagonal squares.
#define WHITE_SIDE	0x00000000FFFFFFFFULL // 32 white side squares.
#define BLACK_SIDE	0xFFFFFFFF00000000ULL // 32 black side squares.
#define WHITE_SQUARES	0x55AA55AA55AA55AAULL // 32 white squares.
#define BLACK_SQUARES	0xAA55AA55AA55AA55ULL // 32 black squares.
#define CORNERS		0x8100000000000081ULL //  4 corner squares.

/* Memory units: */
#define  B		1           // Byte.
#define KB		(1024 * B)  // Kilobyte.
#define MB		(1024 * KB) // Megabyte.
#define GB		(1024 * MB) // Gigabyte.
#define TB		(1024 * GB) // Terabyte.

#define INFINITY	32767 // Close enough.  ;-)

/* Time control styles: */
#define CONVENTIONAL	0 //
#define INCREMENTAL	1 //
#define SECS_PER_MOVE	2 //
