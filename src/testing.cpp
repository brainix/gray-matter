/*----------------------------------------------------------------------------*\
 |	testing.cpp - correctness and performance tests implementation	      |
 |									      |
 |	Copyright © 2005-2008, The Gray Matter Team, original authors.	      |
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

#include "testing.h"
#include <time.h>

using namespace std;

// Constructor sets test name
testing::testing(char *test) : test_name(test) 
{
}

void testing::start() 
{
  if(test_name == "perft0") test_perft(0);
  else if(test_name == "perft1") test_perft(1);
  else if(test_name == "perft2") test_perft(2);
  else if(test_name == "perft3") test_perft(3);
  else if(test_name == "perft4") test_perft(4);
  else if(test_name == "perft5") test_perft(5);
  else if(test_name == "perft6") test_perft(6);
  else if(test_name == "perft7") test_perft(7);
  else if(test_name == "perft8") test_perft(8);
  else if(test_name == "perft9") test_perft(9);
  else if(test_name == "perft10") test_perft(10);
  else 
  {
	  cerr << "Unknown test: '" << test_name << "'" << endl;
    cerr << "try \"-perft1\" or \"-perft2\", etc. . ." << endl;
    cerr << "Note: we are currently only supporting up to depth 10." << endl;
  }
  exit(EXIT_SUCCESS);
}

/*----------------------------------------------------------------------------*\
 |				 test_perft_1()				      |
\*----------------------------------------------------------------------------*/
// This tests some known perft() output values to what we produce.
// Data is from http://www.albert.nu/programs/sharper/perft.htm
//
void testing::test_perft(int depth) 
{
	vector <string> fen;
	vector <vector <uint64_t> > perft_score;

  if (depth == 0) return;

	// Initial position
	fen.push_back("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  uint64_t fen1[] = {20, 400, 8902, 197281, 4865609, 119060324,
    3195901860, 84998978956, 2439530234167, 69352859712417};
	vector<uint64_t> perft1(fen1, fen1 + sizeof(fen1) / sizeof(uint64_t));
	perft_score.push_back(perft1);

  // This position is very good because it catches many possible bugs.
	fen.push_back("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  uint64_t fen2[] = {48, 2039, 97862, 4085603, 193690690, 8031647685};
	vector<uint64_t> perft2(fen2, fen2 + sizeof(fen2) / sizeof(uint64_t));
	perft_score.push_back(perft2);

	fen.push_back("8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - - 1 67");
	uint64_t fen3[] = {50, 279};
	vector<uint64_t> perft3(fen3, fen3 + sizeof(fen3) / sizeof(uint64_t));
	perft_score.push_back(perft3);

	fen.push_back("8/7p/p5pb/4k3/P1pPn3/8/P5PP/1rB2RK1 b - d3 0 28");
	uint64_t fen4[] = {0,0,0,0,0,38633283};
	vector<uint64_t> perft4(fen4, fen4 + sizeof(fen4) / sizeof(uint64_t));
	perft_score.push_back(perft4);

	fen.push_back("rnbqkb1r/ppppp1pp/7n/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
	uint64_t fen5[] = {0,0,0,0,11139762};
	vector<uint64_t> perft5(fen5, fen5 + sizeof(fen5) / sizeof(uint64_t));
	perft_score.push_back(perft5);

	fen.push_back("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
	uint64_t fen6[] = {0,0,0,0,0,11030083,178633661};
	vector<uint64_t> perft6(fen6, fen6 + sizeof(fen6) / sizeof(uint64_t));
	perft_score.push_back(perft6);


	// Print all test results
	cout << "This performance test compares to the following data:" << endl;
	for(size_t i = 0; i < fen.size(); i++) 
  {
	  cout << "FEN[" << i << "]: " << fen[i] << endl << "\t";
	  copy(perft_score[i].begin(), perft_score[i].end()-1, 
		  ostream_iterator<uint64_t>(cout, ", "));
	  cout << perft_score[i].back() << endl;
	}

	// Now run the tests
	uint64_t pval;
	board_base *board_ptr = new board_heuristic();
	string move_str, board_str;

  double timeStart = clock();

  for(int currDepth = 0;currDepth<depth;++currDepth)
  {
    for(size_t feni = 0; feni < fen.size(); feni++) 
    {
	    //if(currDepth < static_cast<signed>(perft_score[feni].size()))
	    //  tests_left = true;

	    if(currDepth < static_cast<signed>(perft_score[feni].size()) && 
	       perft_score[feni][currDepth] != 0)  // zero means value is unknown
      {
	      cout << "Testing FEN[" << feni << "] at depth <" 
             << (currDepth+1) << "> ";
	      if(!board_ptr->set_board_fen(fen[feni])) 
        {
		      cout << "Error setting fen." << endl;
		      exit(EXIT_FAILURE);
	      }
	      board_str = board_ptr->to_string();
	      pval = board_ptr->perft(currDepth+1);

	      if(pval != perft_score[feni][currDepth])
        {
		      cout << "Problem with fen = '" << fen[feni] << "'" << endl;
		      cout << board_str;
		      cout << "At depth " << (currDepth+1) << ":" << endl;
		      cout << "  Expected perft = " << perft_score[feni][currDepth] << endl;
		      cout << "  Computed perft = " << pval << endl;
		      cout << "Repeating perft() with more details:" << endl;

          moveArray l(MAX_MOVES_PER_TURN);
		      board_ptr->generate(l, true);
          for (unsigned i=0;i<l.numElements;++i)
          {
		        board_ptr->coord_to_san(l.theArray[i], move_str);
		        board_ptr->make(l.theArray[i]);
		        pval = board_ptr->perft(currDepth);
		        cout << move_str << " : " << pval << ", ";
		        board_ptr->unmake();
			    }
		      cout << endl << endl;
	      }
        else  //the score was correct.
        {
          cout << " Correct!" << endl;
        }
	    }
    }
    //finished the level . . how long did it take?
    double timeEnd = clock();
    cout << "......................................" 
         << (timeEnd - timeStart)/CLOCKS_PER_SEC << " seconds." << endl;
  }
	exit(EXIT_SUCCESS);
}
