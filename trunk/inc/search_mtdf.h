/*----------------------------------------------------------------------------*\
 |  search_mtdf.h - MTD(f) move search interface                              |
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

#ifndef SEARCH_MTDF_H
#define SEARCH_MTDF_H

// Extra Gray Matter stuff:
#include "search_base.h"
#include "table.h"
#include "clock.h"
#include "xboard.h"

/// MTD(f) move search.
class search_mtdf : public search_base
{
public:
    search_mtdf(table* t, history* h, chess_clock* c, xboard* x);
    ~search_mtdf();
    search_mtdf& operator=(const search_mtdf& that);
private:
    //the move arrays
    MoveArray* MoveArrays;
    bool iterate(int s);  //false if we used a book move
    //Move mtdf(int depth, value_t guess = 0);
    Move minimax(int depth, value_t alpha = -INFINITY, 
                   value_t beta = +INFINITY, 
                   bool specialCase = true,
                   bool try_null_move = true);
    //value_t quiesce(int shallowness, value_t alpha, value_t beta);
    void useBook(bool yesno); //turn book on or off
};

#endif
