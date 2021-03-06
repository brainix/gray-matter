/*----------------------------------------------------------------------------*\
 |  move.h - move interface and implementation                                |
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

#ifndef MOVE_H
#define MOVE_H

// Default Gray Matter stuff:
#include "config.h"
#include "library.h"  //int16_t

typedef int16_t value_t;

/// This class contains the from and to coordinates, the pawn promotion information, 
/// and the MiniMax score.  We use a BitField to tightly pack this information 
/// into 32 bits because some of our methods return this structure (rather 
/// than a pointer to this structure or other similar ugliness).
#pragma pack(1)

class Move
{
public:
    unsigned x1      : 3; ///< From x coordinate.              3 bits
    unsigned y1      : 3; ///< From y coordinate.           +  3 bits
    unsigned x2      : 3; ///< To x coordinate.             +  3 bits
    unsigned y2      : 3; ///< To y coordinate.             +  3 bits
    unsigned promo   : 3; ///< Pawn promotion information.  +  3 bits
    unsigned padding : 1; ///< The Evil Bit (TM).           +  1 bit
    value_t  value;       ///< MiniMax score.               + 16 bits
                          //                                = 32 bits
    inline Move()
    {
        x1 = y1 = x2 = y2 = promo = padding = value = 0;
    }
    inline bool operator==(const Move& that) const
    {
      return this->x1 == that.x1 && this->y1 == that.y1 &&
             this->x2 == that.x2 && this->y2 == that.y2 &&
             this->promo == that.promo;
    }
    inline bool operator!=(const Move& that) const
    {
      return this->x1 != that.x1 || this->y1 != that.y1 ||
             this->x2 != that.x2 || this->y2 != that.y2 ||
             this->promo != that.promo;
    }
    inline Move& operator=(const Move& that)
    {
        x1 = that.x1;
        y1 = that.y1;
        x2 = that.x2;
        y2 = that.y2;
        promo = that.promo;
        value = that.value;
        return *this;
    }
    inline bool is_null() const
    {
      return !x1 && !y1 && !x2 && !y2 && !promo;
    }
    inline void set_null()
    {
      x1 = y1 = x2 = y2 = promo = 0;
    }
    inline bool operator<(const Move& rhs) const
    {
      return (value < rhs.value);
    }

    inline bool operator>(const Move& rhs) const
    {
      return (value > rhs.value);
    }
};

#pragma pack()

class MoveArray
{
public:
  Move* theArray;
  size_t mArraySize;
  size_t mNumElements;

  inline MoveArray()
  {
    mArraySize = MAX_MOVES_PER_TURN;
    theArray = new Move[mArraySize];
    mNumElements = 0;
  }

  inline ~MoveArray()
  {
    delete theArray;
  }

  inline MoveArray(size_t size)
  {
    mArraySize = size;
    theArray = new Move[mArraySize];
    mNumElements = 0;
  }

  inline MoveArray& operator=(const MoveArray& that)
  {
     mArraySize = that.mArraySize;
     mNumElements = that.mNumElements;
     for(unsigned i = 0;i<that.mNumElements;i++)
       theArray[i] = that.theArray[i];
     return *this;
  }

  inline size_t size()
  {
    return mNumElements;
  }
  inline void addMove(Move& m)
  {
    //WARNING!  This will cull all moves more than the array size
    theArray[mNumElements] = m;
    if (mNumElements < (mArraySize-1))
      mNumElements++;
  }

  inline void removeLast()
  {
    mNumElements--;
  }

  inline void clear()
  {
    mNumElements = 0;
  }

  inline void reset()
  {
    mNumElements = 0;
  }
};

#endif
