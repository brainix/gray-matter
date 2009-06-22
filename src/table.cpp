/*----------------------------------------------------------------------------*\
 |  table.cpp - transposition, history, and pawn table implementations        |
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

#include "gray.h"
#include "table.h"

/*----------------------------------------------------------------------------*\
 |                               table::table()                               |
\*----------------------------------------------------------------------------*/
table::table(int mb)
{

/// Constructor.

    try
    {
        if ((slots = mb * MB / POLICIES / sizeof(xpos_slot_t)) == 0)
            throw;
        data = new xpos_slot_t*[POLICIES];
        for (int policy = DEEP; policy <= FRESH; policy++)
            data[policy] = new xpos_slot_t[(size_t)slots];
    }
    catch (...)
    {
        // XXX: We should probably do something here.
    }
    clear();

    successful = 0;
    semi_successful = 0;
    unsuccessful = 0;
    total = 0;
}

/*----------------------------------------------------------------------------*\
 |                              table::~table()                               |
\*----------------------------------------------------------------------------*/
table::~table()
{

/// Destructor.

    for (int policy = DEEP; policy <= FRESH; policy++)
        delete[] data[policy];
    delete[] data;
}

/*----------------------------------------------------------------------------*\
 |                               table::clear()                               |
\*----------------------------------------------------------------------------*/
void table::clear()
{

/// Clear the transposition table.

    for (int policy = DEEP; policy <= FRESH; policy++)
        for (uint64_t index = 0; index < slots; index++)
        {
            data[policy][index].hash = 0;
            data[policy][index].depth = 0;
            data[policy][index].type = USELESS;
            data[policy][index].move.set_null();
            data[policy][index].move.value = 0;
        }
}

/*----------------------------------------------------------------------------*\
 |                               table::probe()                               |
\*----------------------------------------------------------------------------*/
bool table::probe(bitboard_t hash, int depth, int type, move_t *move_ptr)
{

///

    uint64_t index = hash % slots;
    for (int policy = DEEP; policy <= FRESH; policy++)
        if (data[policy][index].hash == hash)
        {
            *move_ptr = data[policy][index].move;
            if (data[policy][index].depth >= depth &&
                (data[policy][index].type == BOOK  ||
                 data[policy][index].type == EXACT ||
                 data[policy][index].type == type))
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

/*----------------------------------------------------------------------------*\
 |                               table::store()                               |
\*----------------------------------------------------------------------------*/
void table::store(bitboard_t hash, int depth, int type, move_t move)
{

///

    uint64_t index = hash % slots;
    for (int policy = DEEP; policy <= FRESH; policy++)
        if (depth >= data[policy][index].depth || policy == FRESH)
        {
            data[policy][index].hash = hash;
            data[policy][index].depth = depth;
            data[policy][index].type = type;
            data[policy][index].move = move;
        }
}

/*----------------------------------------------------------------------------*\
 |                             history::history()                             |
\*----------------------------------------------------------------------------*/
history::history()
{

/// Constructor.

    try
    {
        data = new int****[COLORS];
        for (int color = WHITE; color <= BLACK; color++)
        {
            data[color] = new int***[8];
            for (int x1 = 0; x1 <= 7; x1++)
            {
                data[color][x1] = new int**[8];
                for (int y1 = 0; y1 <= 7; y1++)
                {
                    data[color][x1][y1] = new int*[8];
                    for (int x2 = 0; x2 <= 7; x2++)
                        data[color][x1][y1][x2] = new int[8];
                }
            }
        }
    }
    catch (...)
    {
        // XXX: We should probably do something here.
    }
    clear();
}

/*----------------------------------------------------------------------------*\
 |                            history::~history()                             |
\*----------------------------------------------------------------------------*/
history::~history()
{

/// Destructor.

    for (int color = WHITE; color <= BLACK; color++)
    {
        for (int x1 = 0; x1 <= 7; x1++)
        {
            for (int y1 = 0; y1 <= 7; y1++)
            {
                for (int x2 = 0; x2 <= 7; x2++)
                    delete[] data[color][x1][y1][x2];
                delete[] data[color][x1][y1];
            }
            delete[] data[color][x1];
        }
        delete[] data[color];
    }
    delete[] data;
}

/*----------------------------------------------------------------------------*\
 |                              history::clear()                              |
\*----------------------------------------------------------------------------*/
void history::clear()
{

/// Clear the history table.

    for (int color = WHITE; color <= BLACK; color++)
        for (int x1 = 0; x1 <= 7; x1++)
            for (int y1 = 0; y1 <= 7; y1++)
                for (int x2 = 0; x2 <= 7; x2++)
                    for (int y2 = 0; y2 <= 7; y2++)
                        data[color][x1][y1][x2][y2] = 0;
}

/*----------------------------------------------------------------------------*\
 |                              history::probe()                              |
\*----------------------------------------------------------------------------*/
int history::probe(bool color, move_t m) const
{

///

    return data[color][m.x1][m.y1][m.x2][m.y2];
}

/*----------------------------------------------------------------------------*\
 |                              history::store()                              |
\*----------------------------------------------------------------------------*/
void history::store(bool color, move_t m, int depth)
{

/// Gray Matter has searched to the specified depth and determined the specified
/// move for the specified color to be the best.  Note this.

    data[color][m.x1][m.y1][m.x2][m.y2] += 1 << depth;
}

/*----------------------------------------------------------------------------*\
 |                                pawn::pawn()                                |
\*----------------------------------------------------------------------------*/
pawn::pawn(int mb)
{

/// Constructor.

    try
    {
        if ((slots = mb * MB / sizeof(pawn_slot_t)) == 0)
            throw;
        data = new pawn_slot_t[(size_t)slots];
    }
    catch (...)
    {
        // XXX: We should probably do something here.
    }
    clear();

    successful = 0;
    unsuccessful = 0;
    total = 0;
}

/*----------------------------------------------------------------------------*\
 |                               pawn::~pawn()                                |
\*----------------------------------------------------------------------------*/
pawn::~pawn()
{

/// Destructor.

    delete[] data;
}

/*----------------------------------------------------------------------------*\
 |                               pawn::clear()                                |
\*----------------------------------------------------------------------------*/
void pawn::clear()
{

/// Clear the pawn table.

    for (uint64_t index = 0; index < slots; index++)
    {
        data[index].hash = 0;
        data[index].value = -INFINITY;
    }
}

/*----------------------------------------------------------------------------*\
 |                               pawn::probe()                                |
\*----------------------------------------------------------------------------*/
bool pawn::probe(bitboard_t hash, value_t *value_ptr)
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

/*----------------------------------------------------------------------------*\
 |                               pawn::store()                                |
\*----------------------------------------------------------------------------*/
void pawn::store(bitboard_t hash, value_t value)
{

/// We've just evaluated the pawn structure described in hash.  Save its
/// evaluation in the pawn table for future probes.

    uint64_t index = hash % slots;
    data[index].hash = hash;
    data[index].value = value;
}
