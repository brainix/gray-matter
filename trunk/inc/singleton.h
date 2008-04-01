/*----------------------------------------------------------------------------*\
 |	singleton.h - singleton interface and implementation		      |
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

#ifndef SINGLETON_H
#define SINGLETON_H

template<typename t>
class singleton
{
public:
	static t& instance()
	{
		static t single_instance;
		return single_instance;
	}
};

#endif
