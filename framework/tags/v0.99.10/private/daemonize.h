///////////////////////////////////////////////////////////////////////////////
//                                                                             
//  Copyright (C) 2008-2010  Artyom Beilis (Tonkikh) <artyomtnk@yahoo.com>     
//                                                                             
//  This program is free software: you can redistribute it and/or modify       
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCMS_IMPL_DAEMONIZE_H
#define CPPCMS_IMPL_DAEMONIZE_H

#include <cppcms/defs.h>
#include <string>

namespace cppcms {
namespace json { class value; }
namespace impl {
	
#ifdef CPPCMS_WIN32
	class daemonizer {
	public:
		daemonizer(json::value const &/*conf*/) {}
	};

#else
	class daemonizer {
	public:
		daemonizer(json::value const &conf);
		~daemonizer();
		static int global_urandom_fd;
	private:
		int real_pid;
		std::string unlink_file;
		
		void daemonize(json::value const &conf);
		void cleanup();
	};
#endif

} 
}


#endif
