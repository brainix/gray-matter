/*----------------------------------------------------------------------------*\
 |  bitboard.h - BitBoard interface and implementation                        |
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

#ifndef BITBOARD_H
#define BITBOARD_H

#include <iostream>

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"

/// A BitBoard is a brilliant data structure based on this observation: there
/// are 64 bits in a uint64_t integer, and there are 64 squares on a chess
/// board.  Do you see where I'm going with this?  A BitBoard is an unsigned 64-
/// bit integer in which every bit corresponds to a square.
///
/// A single BitBoard can't represent the entire state of the board.  A single
/// bit can only hold a value of 0 or 1 - enough to describe the absence or
/// presence of a piece on a square, but not enough to describe the piece's
/// color or shape.  Therefore, we need 12 BitBoards to represent the entire
/// state of the board:
///
///		· white pawns		· black pawns
///		· white knights		· black knights
///		· white bishops		· black bishops
///		· white rooks		· black rooks
///		· white queens		· black queens
///		· white kings		· black kings
typedef uint64_t bitboard_t;

class bitBoardArray
{
public:
  bitboard_t hashes[MAX_MOVES_PER_GAME];
  unsigned mNumElements;

  inline bitBoardArray()
  {
    mNumElements = 0;
  }

  inline unsigned size() const
  {
    return mNumElements;
  }

  inline void addHash(const bitboard_t& hash)
  {
    hashes[mNumElements] = hash;
    mNumElements++;
  }

  inline void output()
  {
    std::cout << "outputting hashes:" << std::endl;
    for (unsigned i=0;i<mNumElements;++i)
    {
      std::cout << std::hex << hashes[i] << std::endl; 
    }
  }

  inline void clear()
  {
    mNumElements = 0;
  }

  inline void removeLast()
  {
    mNumElements--;
  }

  inline bitBoardArray& operator=(const bitBoardArray& rhs)
  {
    for(unsigned i=0;i<rhs.mNumElements;++i)
    {
      hashes[i] = rhs.hashes[i];
    }
    mNumElements = rhs.mNumElements;
    return *this;
  }
};

#endif
