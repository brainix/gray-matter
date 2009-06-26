/*----------------------------------------------------------------------------*\
 |  xboard.cpp - Chess Engine Communication Protocol implementation           |
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
#include "xboard.h"

using namespace std;

/*----------------------------------------------------------------------------*\
 |                                  xboard()                                  |
\*----------------------------------------------------------------------------*/
xboard::xboard()
{

/// Constructor.
    // Turn I/O buffering off.
    setvbuf(stdout, NULL, _IONBF, 1024);
    setvbuf(stdin, NULL, _IONBF, 1024);

    // Initialize the variables.
    analyze = false;
    ponder = true;
    force = false;
    draw = false;
    sync = true;

    // Initialize TestSuite statistics
    ts_mode = false;
    ts_erroneous = 0;
    ts_success = 0;
    ts_failure = 0;
}

/*----------------------------------------------------------------------------*\
 |                                 ~xboard()                                  |
\*----------------------------------------------------------------------------*/
xboard::~xboard()
{

/// Destructor.

}

/*----------------------------------------------------------------------------*\
 |                                  vomit()                                   |
\*----------------------------------------------------------------------------*/
void xboard::vomit(char *message) const
{

/// Houston, we have a problem...

    printf("tellusererror %s\n", message);
    exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------------*\
 |                                   loop()                                   |
\*----------------------------------------------------------------------------*/
void xboard::loop(search_base *s, chess_clock *c, book *o)
{

/// This is the event loop.

    board_ptr = new board_heuristic();
    search_ptr = s;
    clock_ptr = c;
    book_ptr = o;

    do
    {
        memset(buffer, '\0', BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        if (!strncmp(buffer, "xboard", 6))
            do_xboard();
        else if (!strncmp(buffer, "protover", 8))
            do_protover();
        else if (!strncmp(buffer, "accepted", 8))
            do_accepted();
        else if (!strncmp(buffer, "rejected", 8))
            do_rejected();
        else if (!strncmp(buffer, "new", 3))
            do_new();
        else if (!strncmp(buffer, "quit", 4))
            do_quit();
        else if (!strncmp(buffer, "force", 5))
            do_force();
        else if (!strncmp(buffer, "go", 2))
            do_go();
        else if (!strncmp(buffer, "playother", 9))
            do_playother();
        else if (!strncmp(buffer, "level", 5))
            do_level();
        else if (!strncmp(buffer, "st", 2))
            do_st();
        else if (!strncmp(buffer, "sd", 2))
            do_sd();
        else if (!strncmp(buffer, "time", 4))
            do_time();
        else if (!strncmp(buffer, "otim", 4))
            do_otim();
        else if (!strncmp(buffer, "usermove", 8))
            do_usermove();
        else if (!strncmp(buffer, "?", 1))
            do_question();
        else if (!strncmp(buffer, "ping", 4))
            do_ping();
        else if (!strncmp(buffer, "draw", 4))
            do_draw();
        else if (!strncmp(buffer, "setboard", 8))
            do_setboard();
        else if (!strncmp(buffer, "hint", 4))
            do_hint();
        else if (!strncmp(buffer, "undo", 4))
            do_undo();
        else if (!strncmp(buffer, "remove", 6))
            do_remove();
        else if (!strncmp(buffer, "hard", 4))
            do_hard();
        else if (!strncmp(buffer, "easy", 4))
            do_easy();
        else if (!strncmp(buffer, "post", 4))
            do_post();
        else if (!strncmp(buffer, "nopost", 6))
            do_nopost();
        else if (!strncmp(buffer, "analyze", 7))
            do_analyze();
        else if (!strncmp(buffer, "exit", 4))
            do_exit();
        // These commands are not part of the Chess Engine Interface Protocol,
        // but they come in handy to us.
        else if (!strncmp(buffer, "display", 7))
            do_display();
        else if (!strncmp(buffer, "test", 4))
            do_test();
        else
            do_unknown();
    } while (strncmp(buffer, "quit", 4));
}

/*----------------------------------------------------------------------------*\
 |                               print_output()                               |
\*----------------------------------------------------------------------------*/
void xboard::print_output(int ply, int value, int time, int nodes, list<move_t>& pv) const
{

/// Print thinking output.

  
    printf("%d %d %d %d", ply, value, time, nodes);
    for (list<move_t>::iterator it = pv.begin(); it != pv.end(); it++)
    {
        printf(" ");
        print_move(*it, true);
        board_ptr->make(*it);
    }
    for (list<move_t>::iterator it = pv.begin(); it != pv.end(); it++)
        board_ptr->unmake();
    printf("\n");
}

/*----------------------------------------------------------------------------*\
 |                               print_result()                               |
\*----------------------------------------------------------------------------*/
void xboard::print_result(move_t m)
{

/// We've just finished thinking.  If we came up with a move, update the board
/// and clock, inform XBoard of the move we're making, and change the move
/// search engine's mode.

    // Did we come up with a move?
    if (m.is_null())
    {
        // No.  :'(  Give up.
        print_resignation();
        return;
    }

    // Update the board and clock.
    board_ptr->make(m);
    clock_ptr->dec_remaining_moves(!board_ptr->get_whose());

    // Inform XBoard of the move we're making.
    printf("move ");
    print_move(m);
    printf("\n");

    // Change the move search engine's mode.
    int status = game_over();
    int mode = ponder && status == IN_PROGRESS ? PONDERING : IDLING;
    search_ptr->change(mode, *board_ptr);

    // In TestSuite mode, continue processing TestSuite
    if (ts_mode)
        test_suite_next(m);
}

/*----------------------------------------------------------------------------*\
 |                            print_resignation()                             |
\*----------------------------------------------------------------------------*/
void xboard::print_resignation()
{

/// We've determined our situation to be hopeless.  If our opponent had offered
/// a draw before, accept it now.  Otherwise, resign.

    printf("%s\n", draw ? "offer draw" : "resign");

    // In TestSuite mode, continue processing TestSuite
    if (ts_mode)
    {
        // This code should not be reached ??!!
        move_t m;
        m.set_null();
        test_suite_next(m);
    }
}

/*----------------------------------------------------------------------------*\
 |                                print_move()                                |
\*----------------------------------------------------------------------------*/
void xboard::print_move(move_t m, bool san) const
{
    if (san)
    {
        // Standard Algebraic Notation.
        string str;
        board_ptr->coord_to_san(m, str);
        printf("%s", str.c_str());
    }
    else
    {
        // Coordinate notation.
        printf("%c%c", m.x1 + 'a', m.y1 + '1');
        printf("%c%c", m.x2 + 'a', m.y2 + '1');
        switch (m.promo)
        {
            case KNIGHT : printf("n"); break;
            case BISHOP : printf("b"); break;
            case ROOK   : printf("r"); break;
            case QUEEN  : printf("q"); break;
        }
    }
}

/*----------------------------------------------------------------------------*\
 |                                do_xboard()                                 |
\*----------------------------------------------------------------------------*/
void xboard::do_xboard() const
{
}

/*----------------------------------------------------------------------------*\
 |                               do_protover()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_protover() const
{
    printf("feature ping=1\n");
    printf("feature setboard=1\n");
    printf("feature playother=1\n");
    printf("feature usermove=1\n");
    printf("feature sigint=0\n");

#ifdef _MSDEV_WINDOWS
    printf("feature myname=\"graySVN1535\"\n");
#else
  #ifdef SVN_REV
    printf("feature myname=\"Gray Matter rev %s\"\n", SVN_REV);
  #else
    printf("feature myname=\"Gray Matter 0.0\"\n");
  #endif
#endif
    printf("feature variants=\"normal\"\n");
    printf("feature colors=0\n");
    printf("feature done=1\n");

}

/*----------------------------------------------------------------------------*\
 |                               do_accepted()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_accepted() const
{
}

/*----------------------------------------------------------------------------*\
 |                               do_rejected()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_rejected() const
{
    exit(EXIT_FAILURE);
}

/*----------------------------------------------------------------------------*\
 |                                  do_new()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_new()
{
    ponder = true;
    force = false;
    draw = false;
    sync = true;
    board_ptr->set_board();
    search_ptr->change(IDLING, *board_ptr);
    search_ptr->set_depth(MAX_DEPTH);
    book_ptr->read();
    if (analyze)
        search_ptr->change(ANALYZING, *board_ptr);
}

/*----------------------------------------------------------------------------*\
 |                                 do_quit()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_quit() const
{
    thread_t thread = search_ptr->get_thread(); 
    search_ptr->change(QUITTING, *board_ptr);
    thread_wait(&thread);
}

/*----------------------------------------------------------------------------*\
 |                                 do_force()                                 |
\*----------------------------------------------------------------------------*/
void xboard::do_force()
{
    force = true;
    search_ptr->change(IDLING, *board_ptr);
}

/*----------------------------------------------------------------------------*\
 |                                  do_go()                                   |
\*----------------------------------------------------------------------------*/
void xboard::do_go()
{
    force = false;
    search_ptr->change(THINKING, *board_ptr);
}

/*----------------------------------------------------------------------------*\
 |                               do_playother()                               |
\*----------------------------------------------------------------------------*/
void xboard::do_playother()
{
    force = false;
    search_ptr->change(ponder ? PONDERING : IDLING, *board_ptr);
}

/*----------------------------------------------------------------------------*\
 |                                 do_level()                                 |
\*----------------------------------------------------------------------------*/
void xboard::do_level() const
{
    const char *p = &buffer[6];
    int moves = str_to_num(p);
    while (*p++ != ' ')
        ;
    int secs = str_to_secs(p);
    while (*p++ != ' ')
        ;
    int inc = str_to_num(p);

    clock_ptr->set_mode(WHITE, moves, secs * 100, inc * 100);
    clock_ptr->set_mode(BLACK, moves, secs * 100, inc * 100);
}

/*----------------------------------------------------------------------------*\
 |                                  do_st()                                   |
\*----------------------------------------------------------------------------*/
void xboard::do_st()
{

/// Set the maximum search time.

    sync = false;
    clock_ptr->set_mode(WHITE, 1, str_to_num(&buffer[3]) * 100, 0);
    clock_ptr->set_mode(BLACK, 1, str_to_num(&buffer[3]) * 100, 0);
}

/*----------------------------------------------------------------------------*\
 |                                  do_sd()                                   |
\*----------------------------------------------------------------------------*/
void xboard::do_sd() const
{

/// Set the maximum search depth.

    search_ptr->set_depth(str_to_num(&buffer[3]));
}

/*----------------------------------------------------------------------------*\
 |                                 do_time()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_time() const
{
    if (sync)
        clock_ptr->update_remaining_csecs(!board_ptr->get_whose(),
                                          str_to_num(&buffer[5]));
}

/*----------------------------------------------------------------------------*\
 |                                 do_otim()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_otim() const
{
    if (sync)
        clock_ptr->update_remaining_csecs(board_ptr->get_whose(),
                                          str_to_num(&buffer[5]));
}

/*----------------------------------------------------------------------------*\
 |                               do_usermove()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_usermove()
{

/// Our opponent has moved.  If the move was legal, and it didn't just end the
/// game, and we're not in force mode, then formulate a response.

    move_t m;

    // Was the move legal?
    m.x1 = buffer[ 9] - 'a'; m.y1 = buffer[10] - '1';
    m.x2 = buffer[11] - 'a'; m.y2 = buffer[12] - '1';
    m.promo = char_to_shape(buffer[13]);
    if (!test_move(m))
    {
        // No!  The gypsy was trying to pull a fast one on us!
        printf("Illegal move: ");
        print_move(m);
        printf("\n");
        return;
    }

    board_ptr->make(m);
    search_ptr->verify_prediction(m);
    clock_ptr->dec_remaining_moves(!board_ptr->get_whose());

    // Alright, so the move was legal.  Did it just end the game?
    if (game_over())
    {
        // Yes.  We're not to respond.
        search_ptr->change(IDLING, *board_ptr);
        return;
    }

    // Alright, so the move was legal, and it didn't just end the game.  Are we
    // in force mode?
    if (force)
        // Yes.  We're not to respond.
        return;

    // Alright, so the move was legal, and it didn't just end the game, and
    // we're not in force mode.  Formulate a response.
    search_ptr->change(analyze ? ANALYZING : THINKING, *board_ptr);
}

/*----------------------------------------------------------------------------*\
 |                               do_question()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_question() const
{

/// Our opponent has grown impatient.  Move now, even if there's still thinking
/// time left.

    search_ptr->move_now();
}

/*----------------------------------------------------------------------------*\
 |                                 do_ping()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_ping()
{

/// XBoard has sent a ping request (to make sure we're not on drugs).  Send a
/// pong reply.

    buffer[1] = 'o';
    printf("%s\n", buffer);
}

/*----------------------------------------------------------------------------*\
 |                                 do_draw()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_draw()
{

/// Our opponent has offered a draw.  Just note this for now.

    draw = true;
}

/*----------------------------------------------------------------------------*\
 |                               do_setboard()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_setboard(string argfen) const
{
    if (argfen.size()) {
        if (!board_ptr->set_board_fen(argfen))
            printf("tellusererror Illegal position\n");
    } else {
        string fen(buffer + 9);
        if (!board_ptr->set_board_fen(fen))
            printf("tellusererror Illegal position\n");
    }
}

/*----------------------------------------------------------------------------*\
 |                                 do_hint()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_hint() const
{

/// Give our opponent a hint.

    move_t m = search_ptr->get_hint();
    if (m.is_null())
        return;
    printf("Hint: ");
    print_move(m);
    printf("\n");
}

/*----------------------------------------------------------------------------*\
 |                                 do_undo()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_undo()
{

/// Take back one ply.

    board_ptr->unmake();
    clock_ptr->inc_remaining_moves(board_ptr->get_whose());
    if (analyze)
        search_ptr->change(ANALYZING, *board_ptr);
}

/*----------------------------------------------------------------------------*\
 |                                do_remove()                                 |
\*----------------------------------------------------------------------------*/
void xboard::do_remove()
{

/// Take back two plies.

    board_ptr->unmake();
    board_ptr->unmake();
    clock_ptr->inc_remaining_moves(WHITE);
    clock_ptr->inc_remaining_moves(BLACK);
    search_ptr->change(ponder ? PONDERING : IDLING, *board_ptr);
}

/*----------------------------------------------------------------------------*\
 |                                 do_hard()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_hard()
{

/// Turn on pondering.

    ponder = true;
}

/*----------------------------------------------------------------------------*\
 |                                 do_easy()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_easy()
{

/// Turn off pondering.

    ponder = false;
}

/*----------------------------------------------------------------------------*\
 |                                 do_post()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_post() const
{

/// Turn on thinking output.

    search_ptr->set_output(true);
}

/*----------------------------------------------------------------------------*\
 |                                do_nopost()                                 |
\*----------------------------------------------------------------------------*/
void xboard::do_nopost() const
{

/// Turn off thinking output.

    search_ptr->set_output(false);
}

/*----------------------------------------------------------------------------*\
 |                                do_analyze()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_analyze()
{

/// Enter analyze mode.

    analyze = true;
    search_ptr->change(ANALYZING, *board_ptr);
}

/*----------------------------------------------------------------------------*\
 |                                 do_exit()                                  |
\*----------------------------------------------------------------------------*/
void xboard::do_exit()
{

/// Exit analyze mode.

    analyze = false;
}

/*----------------------------------------------------------------------------*\
 |                                do_display()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_display() const
{
    cout << board_ptr->to_string();
}

/*----------------------------------------------------------------------------*\
 |                                   test()                                   |
\*----------------------------------------------------------------------------*/
void xboard::do_test() {
	string testfile(buffer + 5);
	string::size_type pos = testfile.find_last_not_of("\n \t");
	if (pos != string::npos)
		testfile = testfile.substr(0, pos+1);

	int i = 0;
	string line;
	string TESTFILE = testfile;
	ifstream inputfile(testfile.c_str());

	transform(TESTFILE.begin(), TESTFILE.end(),
		TESTFILE.begin(), (int(*)(int))toupper);

	// Parse the test suite
	if(!inputfile) {
		cerr << "Cannot open '" << testfile << "'" << endl;
	} else if (TESTFILE.find(".EPD") != string::npos) {
		// Extended Positinal Diagram, format is:
		// FEN bm (best move) am (avoid move) 
		//   pm (predicted move) pv (predicted variation)
		do {
			getline(inputfile, line);
			string::size_type idx;
			if((idx = line.find("bm")) != string::npos) {
				// Store FEN
				ts_fen.push_back(line.substr(0, idx-1));
				// Store description
				ostringstream ostr;
				ostr << testfile << " " << ++i;
				ts_desc.push_back(ostr.str());
				// Store solution
				string::size_type idx2 = line.find(";", idx);
				ts_sol.push_back(line.substr(idx+3, idx2 == string::npos ? 0 : idx2-idx-3));
			} else if ((idx = line.find("1.")) != string::npos) {
				// Store FEN
				string::size_type idx_colon = line.find(";");
				ts_fen.push_back(line.substr(0, idx_colon-1));
				// Store description
				ostringstream ostr;
				ostr << testfile << " " << ++i;
				ts_desc.push_back(ostr.str());
				// Store solution
				while(idx < line.size() && line.at(idx) == '.') idx++;
				idx_colon = line.find(";", idx);
				ts_sol.push_back(line.substr(idx, 
					idx_colon == string::npos ? string::npos : idx_colon-idx));
			} else if (line.size()){
				ts_erroneous++;
			}
		} while (inputfile.good());
	} else if (TESTFILE.find(".PGN") != string::npos) {
		// Portable Game Notation
		int status = 0;
		string::size_type idx, pos;
		do {
			getline(inputfile, line);
			switch (status) {
				case 0: // In tag pair section
					if (line.find("[") != string::npos)
						status = 1;
					// continue with next case
				case 1: // Tag opened "[..."
					if (line.find("]") != string::npos)
						status = 0;
					if ((idx = line.find("FEN")) != string::npos) {
						// Parse FEN position
						string fen = line.substr(idx+3);
						// Strip from the end
						pos = fen.find_last_not_of("\r\n \t\"]");
						if (pos != string::npos)
							fen = fen.substr(0, pos+1);
						// Strip from the start
						pos = fen.find_first_not_of("\r\n \t\"");
						if (pos != string::npos)
							fen = fen.substr(pos);
						// Store FEN and description
						ts_fen.push_back(fen);
						ostringstream ostr;
						ostr << testfile << " " << ++i;
						ts_desc.push_back(ostr.str());
					}
					idx = line.find("key:");
					if(idx == string::npos)
						idx = line.find("Key:");
					if (idx != string::npos) {
						// Key: best-move
						string solution = line.substr(idx+4);
						// Strip from the end
						pos = solution.find_last_not_of("\r\n \t\"]");
						if (pos != string::npos)
							solution = solution.substr(0, pos+1);
						// Strip from the start
						pos = solution.find_first_not_of("\r\n \t\"");
						if (pos != string::npos)
							solution = solution.substr(pos);
						// Strip subsequent moves, if any
					 	pos = solution.find_first_of(" \t");
						if (pos != string::npos)
							solution = solution.substr(0, pos);
						// Store solution
						ts_sol.push_back(solution);
					}
					break;
				case 2: // Movetext section, SAN moves
					if (line.find("*")   != string::npos ||
						line.find("1-0") != string::npos ||
						line.find("0-1") != string::npos ||
						line.find("1/2-1/2") != string::npos) {
						// Game termination marker found
						status = 0;
					}
					break;
				default:
					break;
			}
		} while (inputfile.good());
	} else {
		cerr << "Unknown file type '" << testfile << "'" << endl;
	}

	if (ts_fen.size() != ts_sol.size()) {
	}

	if (ts_fen.size()) {
		ts_mode = true;
		do_easy();
		string fen = ts_fen.front();
		ts_fen.erase(ts_fen.begin());
		do_setboard(fen);
		clock_ptr->set_mode(board_ptr->get_whose(), 40, 5 * 60 * 100, 0);
		do_go();
	}
}

void xboard::test_suite_next(move_t m) {

	// Check whether we did the right thing
	if (ts_sol.size()) {
		string solution = ts_sol.front();
		ts_sol.erase(ts_sol.begin());
		string desc = ts_desc.front();
		ts_desc.erase(ts_desc.begin());

		// Remove several characters from solution, from the end:
		string::size_type pos = solution.find_last_not_of("\r\n \t\":!?");
		if (pos != string::npos)
			solution = solution.substr(0, pos+1);
		// Remove from the beginning:
		pos = solution.find_first_not_of("\r\n \t1.");
		if (pos != string::npos)
			solution = solution.substr(pos);

		// Convert our move to san
		string str;
		board_ptr->unmake();
		board_ptr->coord_to_san(m, str);

		// Compare result
		if (str == "null") {
			cerr << desc << " : error!" << endl;
			ts_erroneous++;
		} else if (solution == str) {
			cerr << desc << " : '" << solution << "' == '" << str << "'" << endl;
			ts_success++;
		} else {
			cerr << desc << " : '" << solution << "' != '" << str << "'" << endl;
			ts_failure++;
		}
	}

	if (ts_fen.size()) {
		// Go on to the next test position
		string fen = ts_fen.front();
		ts_fen.erase(ts_fen.begin());
		do_setboard(fen);
		clock_ptr->set_mode(board_ptr->get_whose(), 40, 5 * 60 * 100, 0);
		// cout << "Clock: " << clock_ptr->to_string(board_ptr->get_whose()) << endl;
		do_go();
	} else {
		// Test suite finished
		ts_mode = false;
		// Output statistics
		cout << "Test Suite statistics" << endl
			 << "Total successes:          " << ts_success << endl
			 << "Total failures:           " << ts_failure << endl
			 << "Total errors:             " << ts_erroneous << endl;
	}
}

/*----------------------------------------------------------------------------*\
 |                                do_unknown()                                |
\*----------------------------------------------------------------------------*/
void xboard::do_unknown() const
{
    printf("Error (unknown command): %s\n", buffer);
}

/*----------------------------------------------------------------------------*\
 |                                game_over()                                 |
\*----------------------------------------------------------------------------*/
int xboard::game_over()
{
    int status = board_ptr->get_status(true);

    switch (status)
    {
        case STALEMATE:    printf("1/2-1/2 {Stalemate}\n");             break;
        case INSUFFICIENT: printf("1/2-1/2 {Insufficient material}\n"); break;
        case THREE:        printf("1/2-1/2 {Threefold repetition}\n");  break;
        case FIFTY:        printf("1/2-1/2 {Fifty move rule}\n");       break;
        case CHECKMATE:
            if (!board_ptr->get_whose() == WHITE)
            {
                printf("1-0 {White mates}\n");
                break;
            }
            else
            {
                printf("0-1 {Black mates}\n");
                break;
            }
    }
    return status;
}

/*----------------------------------------------------------------------------*\
 |                                str_to_num()                                |
\*----------------------------------------------------------------------------*/
int xboard::str_to_num(const char *p) const
{
    int n = 0;

    while (*p >= '0' && *p <= '9')
        n = n * 10 + *p++ - '0';
    return n;
}

/*----------------------------------------------------------------------------*\
 |                               str_to_secs()                                |
\*----------------------------------------------------------------------------*/
int xboard::str_to_secs(const char *p) const
{
    int mins = 0, secs = 0;

    mins = str_to_num(p);
    while (*p != ' ' && *p != ':')
        p++;
    secs = *p++ == ' ' ? 0 : str_to_num(p);
    return mins * 60 + secs;
}

/*----------------------------------------------------------------------------*\
 |                              char_to_shape()                               |
\*----------------------------------------------------------------------------*/
int xboard::char_to_shape(char c) const
{
    switch (c)
    {
        case 'n' : return KNIGHT;
        case 'b' : return BISHOP;
        case 'r' : return ROOK;
        case 'q' : return QUEEN;
        default  : return PAWN;
    }
}

/*----------------------------------------------------------------------------*\
 |                                test_move()                                 |
\*----------------------------------------------------------------------------*/
bool xboard::test_move(move_t m)
{

/// In the current position, is the specified move legal?

    vector<move_t> l;

    board_ptr->generate(l, true);
    for (vector<move_t>::iterator it = l.begin(); it != l.end(); it++)
        if (*it == m)
            return true;
    return false;
}
