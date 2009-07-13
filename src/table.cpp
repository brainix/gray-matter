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
        if ((slots = mb * MB / sizeof(xpos_slot_t)) == 0)
            throw;
        data = new xpos_slot_t[(size_t)slots];
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
    delete[] data;
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
//inline int history::probe(bool color, Move m) const
//{
//    return data[color][m.x1][m.y1][m.x2][m.y2];
//}

/*----------------------------------------------------------------------------*\
 |                              history::store()                              |
\*----------------------------------------------------------------------------*/
//void history::store(bool color, Move m, int depth)
//{
  /// Gray Matter has searched to the specified depth and determined the specified
  /// move for the specified color to be the best.  Note this.
//    data[color][m.x1][m.y1][m.x2][m.y2] += 1 << depth;
//}

/*----------------------------------------------------------------------------*\
 |                                PawnTable::PawnTable()                                |
\*----------------------------------------------------------------------------*/
PawnTable::PawnTable(int mb)
{
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
 |                               PawnTable::~PawnTable()                                |
\*----------------------------------------------------------------------------*/
PawnTable::~PawnTable()
{
    delete[] data;
}

/*----------------------------------------------------------------------------*\
 |                               pawn::clear()                                |
\*----------------------------------------------------------------------------*/
void PawnTable::clear()
{
    for (uint64_t index = 0; index < slots; index++)
    {
        data[index].hash = 0;
        data[index].value = -INFINITY;
    }
}


