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
#ifndef CPPCMS_HTTP_FILE_H
#define CPPCMS_HTTP_FILE_H

#include <cppcms/defs.h>
#include <cppcms/cstdint.h>
#include <booster/hold_ptr.h>
#include <booster/nowide/fstream.h>
#include <booster/noncopyable.h>
#include <sstream>
#include <fstream>

namespace cppcms { namespace http {

	class request;

	class CPPCMS_API file : public booster::noncopyable {
	public:
		std::string name() const;
		std::string mime() const;
		std::string filename() const;
		void name(std::string const &);
		void mime(std::string const &);
		void filename(std::string const &);
		std::istream &data();
		std::ostream &write_data();

		void save_to(std::string const &filename);
		void set_memory_limit(size_t size);
		void set_temporary_directory(std::string const &dir);

		long long size();
		file();
		~file();
	private:
		std::string name_;
		std::string mime_;
		std::string filename_;
		size_t size_limit_;

		booster::nowide::fstream file_;
		std::stringstream file_data_;
		std::string tmp_file_name_;
		std::string temporary_dir_;

		void move_to_file();
		void save_by_copy(std::string const &file_name,std::istream &in);
		void copy_stream(std::istream &in,std::ostream &out);

		
		uint32_t saved_in_file_ : 1;
		uint32_t removed_ : 1 ;
		uint32_t reserverd_ : 30;

		struct impl_data; // for future use
		booster::hold_ptr<impl_data> d;
		friend class request;
	};


} } //::cppcms::http


#endif
