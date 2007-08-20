/*----------------------------------------------------------------------------*\
 |	types.h - data types						      |
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

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include "config.h"

// This structure describes a transposition table slot.
typedef struct xpos_slot
{
	uint64_t hash;                 // Zobrist hash key.           64 bits
	uint16_t depth;                // Depth of our search.     +  16 bits
	uint16_t type;                 // Upper, exact, or lower.  +  16 bits
	move_t move;                   // Best move and score.     +  32 bits
} __attribute__((packed)) xpos_slot_t; //                          = 128 bits

// This structure describes a pawn table slot.
typedef struct pawn_slot
{
	uint64_t hash;                 // Zobrist hash key.    64 bits
	int16_t value;                 // Score.             + 16 bits
} __attribute__((packed)) pawn_slot_t; //                    = 80 bits

#endif
