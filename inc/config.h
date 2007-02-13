/*----------------------------------------------------------------------------*\
 |	config.h							      |
 |									      |
 |	Copyright © 2005-2007, The Gray Matter Team, original authors.	      |
 |		All rights reserved.					      |
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



/*
 | This file contains user-tunable settings for Gray Matter.  If you don't know
 | what you're doing, leave this file alone.  The defaults are good enough for
 | me, so they're more than good enough for you.  ;-)
 */



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

/* In an excess of time, the maximum search depth (in plies): */
#define DEPTH	16

/* The default size of the transposition table (in megabytes): */
#define MB	256

/* The opening book file name: */
#define BOOK	"book.pgn"



/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

/* Value of material: */
#define WEIGHT_PAWN		  100
#define WEIGHT_KNIGHT		  300
#define WEIGHT_BISHOP		  300
#define WEIGHT_ROOK		  500
#define WEIGHT_QUEEN		  900
#define WEIGHT_KING		10300

/* Penalties for castling statuses: */
#define WEIGHT_CANT_CASTLE	 -100
#define WEIGHT_CAN_CASTLE	    0
#define WEIGHT_HAS_CASTLED	  100

/* How much we hate our opponent: */
#define CONTEMPT		  900



/*----------------------------------------------------------------------------*\
 |		There are no tunable settings beyond this point!	      |
\*----------------------------------------------------------------------------*/

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

/* Game statuses: */
#define IN_PROGRESS	0
#define STALEMATE	1
#define INSUFFICIENT	2
#define THREE		3
#define FIFTY		4
#define CHECKMATE	5
#define ILLEGAL		6
#define GAME_STATS	7

/* Castling requirements: */
#define UNOCCUPIED	0
#define UNATTACKED	1
#define REQS		2

/* */
#define MAP		0
#define UNMAP		1
#define MAPS		2

/* Rotated bitboard angles: */
#define L45		0
#define ZERO		1
#define R45		2
#define R90		3
#define ANGLES		4

/* */
#define X		0
#define Y		1
#define COORDS		2

/* List positions: */
#define FRONT		0
#define BACK		1
#define POSITIONS	2

/* Transposition table entry replacement policies: */
#define DEEP		0 /* Only replace if same depth or deeper. */
#define FRESH		1 /* Always replace.                       */
#define POLICIES	2

/* Transposition table entry types: */
#define USELESS		0
#define ALPHA		1
#define BETA		2
#define EXACT		3
#define ENTRY_TYPES	4

/* */
#define THINKING	0 /* Thinking (on our own time).         */
#define PONDERING	1 /* Pondering (on our opponent's time). */

/* Convenient bitboards: */
#define CENTER		0x0000001818000000ULL /* The 4 center squares.              */
#define EXPANDED_CENTER	0x00003C3C3C3C0000ULL /* The 16 center squares.             */
#define PRINCIPAL_DIAGS	0x8142241818244281ULL /* The 16 principal diagonal squares. */
#define WHITE_SIDE	0x00000000FFFFFFFFULL /* The 16 white side squares.         */
#define BLACK_SIDE	0xFFFFFFFF00000000ULL /* The 16 black side squares.         */
#define CORNERS		0x8100000000000081ULL /* The 4 corner squares.              */

/* Memory units: */
#define B		1           /* Byte.     */
#define KB		(1024 * B)  /* Kilobyte. */
#define MB		(1024 * KB) /* Megabyte. */
#define GB		(1024 * MB) /* Gigabyte. */
#define TB		(1024 * GB) /* Terabyte. */
