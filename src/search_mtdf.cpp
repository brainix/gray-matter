/*----------------------------------------------------------------------------*\
 |  search_mtdf.cpp - MTD(f) move search implementation                       |
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
#include "board_base.h"
#include "search_mtdf.h"

/*----------------------------------------------------------------------------*\
 |                               search_mtdf()                                |
\*----------------------------------------------------------------------------*/
search_mtdf::search_mtdf(table* t, history* h, chess_clock* c, xboard* x) :
search_base(t, h, c, x)
{
  MoveArrays = new MoveArray[MAX_DEPTH];

/// Constructor.

}

/*----------------------------------------------------------------------------*\
 |                               ~search_mtdf()                               |
\*----------------------------------------------------------------------------*/
search_mtdf::~search_mtdf()
{
  delete MoveArrays;

/// Destructor.

}

/*----------------------------------------------------------------------------*\
 |                                     =                                      |
\*----------------------------------------------------------------------------*/
search_mtdf& search_mtdf::operator=(const search_mtdf& that)
{

/// Overloaded assignment operator.

    if (this != &that)
        search_base::operator=(that);
    return *this;
}

void search_mtdf::useBook(bool yesno)
{
  inBook = yesno;
}

/*----------------------------------------------------------------------------*\
 |                                 iterate()                                  |
\*----------------------------------------------------------------------------*/
bool search_mtdf::iterate(int state)
{

/// Perform iterative deepening.  This method handles analyzing (thinking
/// indefinitely), thinking (on our own time), and pondering (on our opponent's
/// time) since they're so similar.

    Move guess[2], m;
    bool strong_pondering = false;

    // Wait for the board, then grab the board.
    board_ptr->lock();

    // If we're to think:  For the current position, does the opening book
    // recommend a move? 
    if ((inBook) && (state == THINKING))
    {
        if (table_ptr->probe(board_ptr->get_hash(), MAX_DEPTH, BOOK, &m))
        {
            // Yes.  Make the move.
            extract_pv();
            extract_hint(THINKING);
            board_ptr->unlock();
            xboard_ptr->print_result(m);
            return false;
        }
        else
          useBook(false);  //we're out of book
    }

    //if there is only one legal move, make it
    MoveArray l;
    board_ptr->generate(l, true);  //legal moves only
    if (l.mNumElements == 1)
    {
      extract_pv();
      extract_hint(THINKING);
      m = l.theArray[0];
      board_ptr->unlock();
      xboard_ptr->print_result(m);
      return false;
    }


    // Note the start time.  If we're to think, then set the alarm.  (If we're
    // to analyze or ponder, then there's no need to set the alarm.  We analyze
    // or ponder indefinitely until our opponent has moved.)
    clock_ptr->note_time();
    if (state == THINKING)
        clock_ptr->set_alarm(board_ptr->get_whose());

    // If we're to ponder, then pretend that our opponent has made the move that
    // we think that she'll make, then think about our best response.
    if (state == PONDERING && !hint.is_null())
    {
        strong_pondering = true;
        board_ptr->make(hint);
    }

    // Initialize the number of nodes searched.
    nodes = 0;
    for (int depth = 0; depth <= 1; depth++)
    {
        guess[depth].set_null();
        guess[depth].value = 0;
    }

    // Perform iterative deepening until the alarm has sounded (if we're
    // thinking), our opponent has moved (if we're analyzing or pondering), or
    // we've reached the maximum depth (in any case).
    for (int depth = SPECIAL_SEARCH_DEPTH+1; depth <= max_depth; depth++)
    {
        DEBUG_SEARCH_INIT(1, "");
        //guess[depth & 1] = mtdf(depth, guess[depth & 1].value);
        guess[depth & 1] = minimax(depth);

        if ((timeout_flag) || (guess[depth & 1].is_null()))
            // Oops.  Either the alarm has interrupted this iteration (and the
            // results are incomplete and unreliable), or there's no legal move
            // in this position (and the game must've ended).
            break;
        m = guess[depth & 1];

        //extract_pv();
        pv.addMove(m);
        if (output)
        {
            if (strong_pondering)
                pv.addMove(hint);
            //please try to always score from "our" perspective
            xboard_ptr->print_output(depth,
                board_ptr->get_whose()? -m.value: m.value,
                clock_ptr->get_elapsed(), nodes, pv);
            if (strong_pondering)
              pv.removeLast();
        }
        if (ABS(m.value) >= VALUE_KING)
            // Oops.  The game will be over at this depth.  There's no point in
            // searching deeper.  Eyes on the prize.
            break;
    }

    // If we've just finished thinking, then cancel the alarm.
    if (state == THINKING)
    {
        clock_ptr->cancel_alarm();
        extract_hint(THINKING);
    }

    // If we've just finished pondering, then take back the move that we thought
    // that our opponent would've made.
    if (strong_pondering)
        board_ptr->unmake();

    // Release the board.
    board_ptr->unlock();

    // If we've just finished thinking, then inform XBoard of our favorite move.
    if (state == THINKING && search_status != QUITTING)
        xboard_ptr->print_result(m);

    return true;
}

/*----------------------------------------------------------------------------*\
 |                                   mtdf()                                   |
\*----------------------------------------------------------------------------*/
/*
Move search_mtdf::mtdf(int depth, value_t guess)
{

/// From the current position, search for the best move.  This method implements
/// the MTD(f) algorithm.

    Move m;
    m.set_null();
    m.value = guess;
    value_t upper = +INFINITY, lower = -INFINITY, beta;

    while (upper > lower && !timeout_flag)
    {
        beta = m.value + (m.value == lower);
        m = minimax(depth, 0, beta - 1, beta, true);
        upper = m.value < beta ? m.value : upper;
        lower = m.value < beta ? lower : m.value;
    }
    return m;
}
*/

/*----------------------------------------------------------------------------*\
 |                                 minimax()                                  |
\*----------------------------------------------------------------------------*/
Move search_mtdf::minimax(int depth, value_t alpha, value_t beta, 
                            bool specialCase, bool try_null_move)
{

/// From the current position, search for the best move.  This method implements
/// the MiniMax algorithm.
///
/// On top of MiniMax, this method implements NegaMax.  NegaMax produces the
/// same results as MiniMax but is simpler to code.  Instead of juggling around
/// two players, Max and Min, NegaMax treats both players as Max and negates the
/// scores (and negates and swaps the lower and upper bounds - more on that in
/// the next paragraph) on each recursive call.  In other words, NegaMax always
/// views the color on move as Max and the color off move as Min.
///
/// On top of NegaMax, this method implements AlphaBeta.  AlphaBeta produces the
/// same results as NegaMax but far more efficiently.
//
/// On top of AlphaBeta, this method implements FailSoft.  FailSoft returns more
/// information than AlphaBeta.  If the exact score falls outside of the window,
/// then AlphaBeta returns either alpha (to represent that the exact score is
/// lower than the window) or beta (to represent that the exact score is higher
/// than the window).  On the other hand, FailSoft returns either an upper bound
/// (<= alpha) or a lower bound (>= beta) on the exact score.
///
/// This method also implements null move pruning.

    // Local variables that pertain to the current position:
    bool whose = board_ptr->get_whose();     // The color on move.
    bitboard_t hash = board_ptr->get_hash(); // This position's hash.
    int status = board_ptr->get_status(false);   // Whether the game is over.
    value_t saved_alpha = alpha;             // Saved lower bound on score.
    value_t saved_beta = beta;               // Saved upper bound on score.
    Move null_move;                        // The all-important null move.
    Move m;                                // The best move and score.

    //set the special flag for deeper searches (captures, etc.)
    bool specialFlag = (specialCase && (depth <= SPECIAL_SEARCH_DEPTH))?true:false;

    // Increment the number of positions searched.
    nodes++;

    // If this position is terminal (the end of the game), then there's no legal
    // move - all we have to do is determine if the game is drawn or lost.
    // (Subtle!  We couldn't have just won because our opponent moved last.)
    if (status != IN_PROGRESS)
    {
        m.set_null();
        switch (status)
        {
            case STALEMATE    : m.value = +VALUE_CONTEMPT;     break;
            case INSUFFICIENT : m.value = +VALUE_CONTEMPT;     break;
            case THREE        : m.value = +VALUE_CONTEMPT;     break;
            case FIFTY        : m.value = +VALUE_CONTEMPT;     break;
            case CHECKMATE    : m.value = -VALUE_KING;         break;
            case ILLEGAL      : m.value = -VALUE_ILLEGAL;      break;
        }
                      
        //table_ptr->store(hash, depth, EXACT, m);  //mark this spot in hash table

#ifndef _MSDEV_WINDOWS
        DEBUG_SEARCH_PRINT("terminal state %d.", status);
#endif
        return m;
    }

    // If we've already sufficiently examined this position, then return the
    // best move from our previous search.  Otherwise, if we can, reduce the
    // size of our AlphaBeta window.
    //if (table_ptr->probe(hash, depth, EXACT, &m))
      //  return m;
    
//  if (table_ptr->probe(hash, depth, UPPER, &m))
//  {
//      if (m.value <= alpha)
//          return m;
//      // When doing MTD(f) zero-window searches, our window should never be
//      // resized here.  I've only accounted for this in the interest of
//      // robustness.
//      beta = LESSER(beta, m.value);
//  }
// if (table_ptr->probe(hash, depth, LOWER, &m))
//  {
//      if (m.value >= beta)
//          return m;
//      // When doing MTD(f) zero-window searches, our window should never be
//      // resized here.  I've only accounted for this in the interest of
//      // robustness.
//     alpha = GREATER(alpha, m.value);
//}


    // If we've reached the maximum search depth, then this node is a leaf - all
    // we have to do is apply the static evaluator.
    //numbers less than SPECIAL_SEARCH_DEPTH indicate special conditions, don't store the hash
    //because we're not doing sufficient analysis
    if ((depth <= SPECIAL_SEARCH_DEPTH) && (!specialFlag))  //uninteresting leaf node
    {
        m.set_null();
        m.value = -board_ptr->evaluate();
        //if (depth == SPECIAL_SEARCH_DEPTH)
          //table_ptr->store(hash, depth, EXACT, m);
#ifndef _MSDEV_WINDOWS
        DEBUG_SEARCH_PRINT("evaluate() says %d.", board_ptr->get_whose() ? -m.value : m.value);
#endif
        return m;
    }

    if (depth <= 0) //leaf node in any case
    {
        m.set_null();
        m.value = -board_ptr->evaluate();
        //if (depth == 0)
          //table_ptr->store(hash, depth, EXACT, m);
#ifndef _MSDEV_WINDOWS
        DEBUG_SEARCH_PRINT("evaluate() says %d.", board_ptr->get_whose() ? -m.value : m.value);
#endif
        return m;
    }

    /*  //currently just makes things worse
    // Perform null move pruning.
    if (try_null_move && !board_ptr->zugzwang())
      {
      null_move.set_null();
      DEBUG_SEARCH_ADD_MOVE(null_move);
      board_ptr->make(null_move);
      null_move = minimax(depth - R, -beta, -alpha, false, false);
      DEBUG_SEARCH_DEL_MOVE(null_move);
      board_ptr->unmake();
      if (-null_move.value >= beta)
      {
          null_move.value = beta;
          return null_move;
      }
    }
    */

    // Generate the move list.
    if (!board_ptr->generate(MoveArrays[depth], false))  //include illegal moves
    {
        // There's a move in the list that captures the opponent's king, which
        // means that we're in an illegal position.
        m.set_null();
        m.value = VALUE_ILLEGAL;
#ifndef _MSDEV_WINDOWS
        DEBUG_SEARCH_PRINT("Opponent's king can be captured - illegal position.");
#endif
        return m;
    }

    // If according to the transposition table, a previous search from this
    // position determined this move to be best, then in this search, this
    // move could be good too - score this move highly to force it to the
    // front of the list to score it first to hopefully cause an earlier
    // cutoff.  Otherwise, score this move according to the history
    // heuristic.
    for (unsigned i=0;i<MoveArrays[depth].mNumElements;++i)
    {
      MoveArrays[depth].theArray[i].value = 
        MoveArrays[depth].theArray[i] == m ? VALUE_KING : 
        history_ptr->probe(whose, MoveArrays[depth].theArray[i]);
    }

    // sort the move list.
    Move tmpMove;
    for(unsigned i=0;i<MoveArrays[depth].mNumElements;++i)
    {
      unsigned bestIndex = i;
      for (unsigned j=i;j<MoveArrays[depth].mNumElements;++j)
      {
        if (MoveArrays[depth].theArray[j].value > MoveArrays[depth].theArray[bestIndex].value)
        {
          bestIndex = j;
        }
      }

      //now we have i'th best move, put it in its place if it's not there
      if (bestIndex != i)
      {
        tmpMove = MoveArrays[depth].theArray[i];
        MoveArrays[depth].theArray[i] = MoveArrays[depth].theArray[bestIndex];
        MoveArrays[depth].theArray[bestIndex] = tmpMove;
      }
    }

    // Score each move in the list.
    m.set_null();
    m.value = -VALUE_ILLEGAL;
    for(unsigned i=0;i<MoveArrays[depth].mNumElements;++i)
    {
        DEBUG_SEARCH_ADD_MOVE(MoveArrays[depth].theArray[i]);
        bool capture = board_ptr->make(MoveArrays[depth].theArray[i]);
        bool check = board_ptr->check(false);
        MoveArrays[depth].theArray[i].value = -minimax(depth - 1, -beta, -alpha, (capture||check)).value;
        DEBUG_SEARCH_DEL_MOVE(MoveArrays[depth].theArray[i]);
        board_ptr->unmake();
        if (ABS(MoveArrays[depth].theArray[i].value) == VALUE_ILLEGAL)
            continue;
        if (MoveArrays[depth].theArray[i].value > m.value)
        {
          m = MoveArrays[depth].theArray[i];
          if (m.value > alpha) 
            alpha = m.value;
        }
        if ((beta <= alpha) || (timeout_flag))
            break;
    }

    // if we didn't find any legal moves
    if (m.value == -VALUE_ILLEGAL)
    {
        // Nope, there was no legal move in the list.  There are three
        // possibilities: the current position is illegal, a draw, or a
        // checkmate.  How can we tell which?  If our opponent is in check, then
        // the position is illegal.  If we're not in check, then we're drawn.
        // If we're in check, then we're checkmated.
        m.set_null();
        if (board_ptr->check(true))
            // Our opponent is in check; the position is illegal; we've already
            // won.
            m.value = VALUE_ILLEGAL;
        else if (!board_ptr->check())
            // We're not in check; the position is a draw.
            m.value = VALUE_CONTEMPT;
        else
            // We're in check; the position is a checkmate; we've lost.
            m.value = -(VALUE_KING);
        //if (!timeout_flag)
          //  table_ptr->store(hash, depth, EXACT, m);
#ifndef _MSDEV_WINDOWS
        DEBUG_SEARCH_PRINT("%s.", m.value == VALUE_ILLEGAL ? "Illegal position" : m.value == VALUE_CONTEMPT ? "Stalemated" : "Checkmated");
#endif
        return m;
    }

    // Was the search interrupted?
    if (!timeout_flag)
    {
        // Nope, the results are complete and reliable.  Save them for progeny.
        //if (m.value > saved_alpha && m.value < saved_beta)
            // When doing MTD(f) zero-window searches, our move search should
            // never return an exact score.  I've only accounted for this in the
            // interest of robustness.
          //  table_ptr->store(hash, depth, EXACT, m);
        //else if (m.value <= saved_alpha)
          //  table_ptr->store(hash, depth, UPPER, m);
        //else // m.value >= saved_beta
          //  table_ptr->store(hash, depth, LOWER, m);
        history_ptr->store(whose, m, depth);
    }
#ifndef _MSDEV_WINDOWS
    DEBUG_SEARCH_PRINTM(m, "max of %d children: %d.", MoveArrays[depth].mNumElements, m.value);
#endif
    return m;
}

/*----------------------------------------------------------------------------*\
 |                                 quiesce()                                  |
\*----------------------------------------------------------------------------*/
/*
value_t search_mtdf::quiesce(int shallowness, value_t alpha, value_t beta)
{
    // Local variables that pertain to the current position:
    value_t value_stand_pat;       //
    vector<Move> l;                // The move list.
    //list<Move>::iterator it;     // The move list's iterator.

    // Increment the number of positions searched.
    nodes++;

    if (shallowness >= MAX_DEPTH)
        return beta;

    value_stand_pat = board_ptr->evaluate();
    if (value_stand_pat > alpha)
        alpha = value_stand_pat;
    if (value_stand_pat >= beta)
        return value_stand_pat;

    // Generate the move list.
    board_ptr->generate(MoveArrays[shallowness], false, true);

    // Score each move in the list.
    for (unsigned i=0;i<MoveArrays[shallowness].mNumElements;++i)
    {
        board_ptr->make(l[i]);
        MoveArrays[shallowness].theArray[i].value = -quiesce(shallowness + 1, -beta, -alpha);
        board_ptr->unmake();
        if (MoveArrays[shallowness].theArray[i].value > alpha)
            alpha = MoveArrays[shallowness].theArray[i].value;
        if (MoveArrays[shallowness].theArray[i].value >= beta)
            return MoveArrays[shallowness].theArray[i].value;
        if (timeout_flag)
            return beta;
    }
    return alpha;
}
*/
