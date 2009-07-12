/*----------------------------------------------------------------------------*\
 |  table.h - transposition, history, and pawn table interfaces               |
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

#ifndef TABLE_H
#define TABLE_H

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

// Extra Gray Matter stuff:
#include "bitboard.h"
#include "move.h"

// Transposition table entry types:
#define USELESS     0 // Useless.
#define BOOK        1 // Prescribed by the opening book.
#define EXACT       2 // Exact MiniMax value.
#define UPPER       3 // Upper bound.
#define LOWER       4 // Lower bound.
#define ENTRY_TYPES 5

/*----------------------------------------------------------------------------*\
 |                            Transposition Table                             |
\*----------------------------------------------------------------------------*/

/// Transposition table slot.
#pragma pack(1)
typedef struct xpos_slot
{
    bitboard_t hash;                   ///< Zobrist hash key.           64 bits
    uint16_t depth;                    ///< Depth of our search.     +  16 bits
    uint16_t type;                     ///< Upper, exact, or lower.  +  16 bits
    Move move;                       ///< Best move and score.     +  32 bits
} xpos_slot_t;                         //                            = 128 bits
#pragma pack()

/// Transposition table.
class table
{
public:
    table(int mb = XPOS_TABLE_MB);
    ~table();
    inline void clear()
    {
        for (uint64_t index = 0; index < slots; index++)
        {
            data[index].hash = 0;
            data[index].depth = MAX_DEPTH;
            data[index].type = USELESS;
            data[index].move.set_null();
            data[index].move.value = 0;
        }
    }

    inline bool probe(bitboard_t hash, int depth, int type, Move *move_ptr)
    {
      uint64_t index = hash % slots;
          if (data[index].hash == hash)
          {
              *move_ptr = data[index].move;
              //if the data in the table is valid for this depth
              //just return the move
              if (data[index].depth >= depth &&
                  (data[index].type == BOOK  ||
                   data[index].type == EXACT ||
                   data[index].type == type))
              {
                  successful++;
                  total++;
                  return true;
              }
              semi_successful++;
              total++;
              return false;
          }
      move_ptr->set_null();
      move_ptr->value = 0;
      unsuccessful++;
      total++;
      return false;
    }
    inline void store(bitboard_t hash, int depth, int type, Move move)
    {
      uint64_t index = hash % slots;
        if (depth > data[index].depth)
        {
            data[index].hash = hash;
            data[index].depth = depth;
            data[index].type = type;
            data[index].move = move;
        }
    }
private:
    uint64_t slots;      ///< The number of slots.
    xpos_slot_t *data;   ///< The slots themselves.
    int successful;      ///< The number of successful queries.
    int semi_successful; ///< The number of semi-successful queries.
    int unsuccessful;    ///< The number of unsuccessful queries;
    int total;           ///< The total number of queries.
};

/*----------------------------------------------------------------------------*\
 |                               History Table                                |
\*----------------------------------------------------------------------------*/

/// History table.
class history
{
public:
    history();
    ~history();
    void clear();
    int probe(bool color, Move m) const;
    void store(bool color, Move m, int depth);
private:
    int *****data;
};

/*----------------------------------------------------------------------------*\
 |                                 Pawn Table                                 |
\*----------------------------------------------------------------------------*/

/// Pawn table slot.
#pragma pack(1)
typedef struct pawn_slot
{
    bitboard_t hash;                   ///< Zobrist hash key.    64 bits
    value_t value;                     ///< Score.             + 16 bits
} pawn_slot_t;                          //                      = 80 bits
#pragma pack()

/// Pawn table.
class PawnTable
{
public:
    PawnTable(int mb = PAWN_TABLE_MB);
    ~PawnTable();
    void clear();
    inline bool probe(bitboard_t hash, value_t *value_ptr)
    {
       /// Given the pawn structure described in hash, check the pawn table to see if
       /// we've evaluated it before.  If so, then save its previous evaluation to the
       /// memory pointed to by value_ptr and return success.  If not, then return
       /// failure.

       uint64_t index = hash % slots;
       bool found = data[index].hash == hash;
       *value_ptr = found ? data[index].value : 0;
       successful += found ? 1 : 0;
       unsuccessful += found ? 0 : 1;
       total++;
       return found;
    }
    inline void store(bitboard_t hash, value_t value)
    {
      /// We've just evaluated the pawn structure described in hash.  Save its
      /// evaluation in the pawn table for future probes.

      uint64_t index = hash % slots;
      data[index].hash = hash;
      data[index].value = value;
    }
private:
    uint64_t slots;    ///< The number of slots.
    pawn_slot_t *data; ///< The slots themselves.
    int successful;    ///< The number of successful queries.
    int unsuccessful;  ///< The number of unsuccessful queries;
    int total;         ///< The total number of queries.
};

#endif
