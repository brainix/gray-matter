/*----------------------------------------------------------------------------*\
 |  testing.cpp - correctness and performance tests              |
 |                                        |
 |  Copyright © 2005-2007, The Gray Matter Team, original authors.        |
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

using namespace std;

// Constructor sets test name
testing::testing(char *test) : test_name(test) {
}

void testing::start() {
// Start the test
  if(test_name == "perft_1") {
	test_perft_1();
  } else {
	cerr << "Unknown test: '" << test_name << "'" << endl;
  }
  exit(EXIT_SUCCESS);
}

/*----------------------------------------------------------------------------*\
 |				 test_perft_1()				      |
\*----------------------------------------------------------------------------*/
// This tests some known perft() output values to what we produce.
// Data is from http://www.albert.nu/programs/sharper/perft.htm
//
void testing::test_perft_1() {
	vector <string> fen;
	vector <vector <unsigned long> > perft_score;

	// Initial position
	fen.push_back("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	unsigned long fen1[] = {20, 400, 8902, 197281, 4865609, 119060324/*,
		3195901860, 84998978956, 2439530234167, 69352859712417*/};
	vector<unsigned long> perft1(fen1, fen1 + sizeof(fen1) / sizeof(unsigned long *));
	perft_score.push_back(perft1);

	// This position is very good because it catches many possible bugs.
	fen.push_back("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	unsigned long fen2[] = {48, 2039, 97862, 4085603, 193690690/*, 8031647685*/};
	vector<unsigned long> perft2(fen2, fen2 + sizeof(fen2) / sizeof(unsigned long *));
	perft_score.push_back(perft2);

	fen.push_back("8/3K4/2p5/p2b2r1/5k2/8/8/1q6 b - - 1 67");
	unsigned long fen3[] = {50, 279};
	vector<unsigned long> perft3(fen3, fen3 + sizeof(fen3) / sizeof(unsigned long *));
	perft_score.push_back(perft3);

	fen.push_back("8/7p/p5pb/4k3/P1pPn3/8/P5PP/1rB2RK1 b - d3 0 28");
	unsigned long fen4[] = {0,0,0,0,0,38633283};
	vector<unsigned long> perft4(fen4, fen4 + sizeof(fen4) / sizeof(unsigned long *));
	perft_score.push_back(perft4);

	fen.push_back("rnbqkb1r/ppppp1pp/7n/4Pp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
	unsigned long fen5[] = {0,0,0,0,11139762};
	vector<unsigned long> perft5(fen5, fen5 + sizeof(fen5) / sizeof(unsigned long *));
	perft_score.push_back(perft5);

	fen.push_back("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
	unsigned long fen6[] = {0,0,0,0,0,11030083,178633661};
	vector<unsigned long> perft6(fen6, fen6 + sizeof(fen6) / sizeof(unsigned long *));
	perft_score.push_back(perft6);

	// Print all test results
	cout << "This performance test compares to the following data:" << endl;
	for(size_t i = 0; i < fen.size(); i++) {
	  cout << "FEN: " << fen[i] << endl << "\t";
	  copy(perft_score[i].begin(), perft_score[i].end()-1, 
		  ostream_iterator<unsigned long>(cout, ", "));
	  cout << perft_score[i].back() << endl;
	}

	// Now run the tests
	bool tests_left = true;
	int depth = 0;
	unsigned long pval;
	board_base *board_ptr = new board_heuristic();
	string move_str, board_str;

	while(tests_left) {
	  tests_left = false;

	  for(size_t feni = 0; feni < fen.size(); feni++) {
		if(depth < static_cast<signed>(perft_score[feni].size()))
		  tests_left = true;

		if(depth < static_cast<signed>(perft_score[feni].size()) && 
		   perft_score[feni][depth] != 0) { // zero means value is unknown

		  if(!board_ptr->set_board_fen(fen[feni])) {
			cout << "Error setting fen." << endl;
			exit(EXIT_FAILURE);
		  }
		  board_str = board_ptr->to_string();
		  pval = board_ptr->perft(depth+1);

		  if(pval != perft_score[feni][depth]) {
			cout << "Problem with fen = '" << fen[feni] << "'" << endl;
			cout << board_str;

			cout << "At depth " << (depth+1) << ":" << endl;
			cout << "  Expected perft = " << perft_score[feni][depth] << endl;
			cout << "  Computed perft = " << pval << endl;
			cout << "Repeating perft() with more details:" << endl;

			list<move_t> l;
			list<move_t>::iterator it;
			board_ptr->generate(l, true);
			for(it = l.begin(); it != l.end(); it++) {
			  board_ptr->coord_to_san(*it, move_str);
			  board_ptr->make(*it);
			  pval = board_ptr->perft(depth);
			  cout << "  "
				   << static_cast<char>(it->x1+'a') << static_cast<char>(it->y1+'1')
				   << static_cast<char>(it->x2+'a') << static_cast<char>(it->y2+'1')
				   << " " << move_str << " : " << pval << endl;
			  board_ptr->unmake();
			}
		  }
		}
	  }

	  depth++;
	}
	exit(EXIT_SUCCESS);
}
