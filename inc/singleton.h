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

/// This template container class implements the Singleton design pattern using
/// the Curiously Recurring Template Pattern.  This implementation is known as
/// the Meyers Singleton.  While this implementation isn't thread-safe in
/// general, some compilers (like GCC) supposedly generate thread-safe code in
/// this case.
///
///	#include "singleton.h"
///
///	class the_one_ring : public singleton<the_one_ring>
///	{
///	public:
///		std::string get_inscription() const
///		{
///			return inscription;
///		}
///	private:
///		friend class singleton<the_one_ring>;
///		std::string inscription;
///	protected:
///		the_one_ring() : inscription("One ring to rule them all...")
///		{
///		}
///	};
///
/// For more information, see:
///	http://en.wikipedia.org/wiki/Singleton_pattern#C.2B.2B
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
