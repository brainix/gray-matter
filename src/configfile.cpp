/*----------------------------------------------------------------------------*\
 |  configfile.cpp - user tunable configuration file                    |
 |                                        |
 |  Copyright © 2005-2008, The Gray Matter Team, original authors.        |
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

#include "configfile.h"

/// Trim a string on both sides,
/// @returns The trimmed string.
string trim(string const& source, char const* delims = " \t\r\n'\"") {
  string result(source);
  // Search the last character we want to be in the string
  string::size_type index = result.find_last_not_of(delims);
  // Chop anything beyond
  if(index != string::npos)
    result.erase(++index);

  // Find the first character we want to be in the string
  index = result.find_first_not_of(delims);
  // And chop anything before this character
  if(index != string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}

/// Constructor
/** Class constructor checks the existence of a configuration file at
 *  the platform-dependent default location. If it exists it also parses
 *  this file and stores the result in a map named items.
 *  Before parsing, some default values are set. */
ConfigFile::ConfigFile() {

#if defined(LINUX)
  ifstream file("/home/jonne/.graymatter");
#else
  ifstream file("graymatter.conf");
#endif

  // Setting default values
  items["engine"] = SEARCH_ENGINE;
  items["xpos_table_mb"] = XPOS_TABLE_MB;
  items["book_name"] = BOOK_NAME;
  items["book_moves"] = STRINGIFY(BOOK_MOVES); // in plies
  items["overhead"] = STRINGIFY(OVERHEAD); // in centiseconds

  items["xml_dump"] = "false";
  items["xml_dir"] = "/tmp";
  items["xml_depth"] = "3";

  string line;
  while(getline(file, line)) {

	line = trim(line);
	// skip emtpy lines
    if(!line.length()) continue;
	// skip lines starting with # or ;
    if(line[0] == '#') continue;
    if(line[0] == ';') continue;

    int posEqual = line.find('=');
    string key = trim(line.substr(0, posEqual));
    string value = trim(line.substr(posEqual + 1));

	// store configuration item (as a string)
    items[key] = value;
  }
}

/// Search and validate configuration items
string ConfigFile::get(string key) const {
  if(items.find(key) != items.end()) {
	// configuration item exists
	string string_val = items.find(key)->second;
	// TODO: maybe do some validation here? when setting defaults
	// we can also set value ranges. maybe we should also specify the
	// variable type.
	return string_val;
  } else {
	// configuration item not found
	throw "Requested configuration item '" + key + "' does not exist";
  }
}

/// Retrieve integer configuration items
int ConfigFile::getInt(string key) const {
  string string_val = get(key);
  istringstream buffer(string_val);
  int int_val;
  buffer >> int_val;
  return int_val;
}

/// Retrieve string configuration items
string ConfigFile::getString(string key) const {
  string string_val = get(key);
  return string_val;
}

/// Add or modify a string configuration item
void ConfigFile::set(string key, string value) {
  items[key] = value;
}

/// Add or modify a numeric configuration item
void ConfigFile::set(string key, int value) {
  ostringstream ostr;
  ostr << value;
  items[key] = ostr.str();
}

/// Dump configuration items
void ConfigFile::dump(ostream &stream) const {
  stream << "Dump of configuration items:" << endl;
  for(map<string,string>::const_iterator iter = items.begin();
	  iter != items.end(); ++iter) {
    stream << iter->first << " = '" << iter->second << "'"<< endl;
  }
}
