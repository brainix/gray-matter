/*----------------------------------------------------------------------------*\
 |      configfile.h - user-tunable settings                                  |
 |                                                                            |
 |      Copyright � 2005-2008, The Gray Matter Team, original authors.        |
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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include "config.h"

using namespace std;

/// User tunable settings in a configuration file
/** This class reads in a configuration file provided at a platform-
 *  dependent default location. Values set in this file can be overridden
 *  by the command-line options tnat Gray Matter supports. */
class ConfigFile {

public:
  ConfigFile();

  int getInt(string key) const;
  string getString(string key) const;

  void set(string key, string value);
  void set(string key, int value);

  void dump(ostream &stream) const;

private:
  string get(string key) const;

  map<string, string> items;
};

#endif
