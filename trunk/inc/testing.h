/*----------------------------------------------------------------------------*\
 |	testing.h - correctness and performance tests interface		      |
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

#ifndef TESTING_H
#define TESTING_H

#include <iostream>
#include <vector>
#include <string>
#include <iterator>

#include "board_base.h"
#include "board_heuristic.h"

// Implementation of several correctness and performance tests

class testing
{
public:
	testing(char *test);
	void start();

protected:
	void test_perft_1();

private:
	std::string test_name;
};

#endif
