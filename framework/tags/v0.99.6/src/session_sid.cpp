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
#define CPPCMS_SOURCE
#include <cppcms/session_sid.h>
#include <cppcms/session_storage.h>
#include <cppcms/session_interface.h>
#include <fstream>
#include <cppcms/cppcms_error.h>
#include <cppcms/urandom.h>
#include <stdio.h>
#include <time.h>

#include <cppcms/config.h>

namespace cppcms {
namespace sessions {


struct session_sid::_data {};

session_sid::session_sid(booster::shared_ptr<session_storage> st) :
	storage_(st)
{
}

session_sid::~session_sid()
{
}

std::string session_sid::get_new_sid()
{
	unsigned char sid[16];			
	char res[33];
	urandom_device rnd;
	rnd.generate(sid,sizeof(sid));

	for(int i=0;i<16;i++) {
		#ifdef CPPCMS_HAVE_SNPRINTF
		snprintf(res+i*2,3,"%02x",sid[i]);
		#else
		sprintf(res+i*2,"%02x",sid[i]);
		#endif
	}
	return std::string(res);
}

bool session_sid::valid_sid(std::string const &cookie,std::string &id)
{
	if(cookie.size()!=33 || cookie[0]!='I')
		return false;
	for(int i=1;i<33;i++) {
		char c=cookie[i];
		bool is_low_x_digit=('0'<=c && c<='9') || ('a'<=c && c<='f');
		if(!is_low_x_digit)
			return false;
	}
	id=cookie.substr(1,32);
	return true;
}

void session_sid::save(session_interface &session,std::string const &data,time_t timeout,bool new_data,bool unused)
{
	std::string id;
	if(valid_sid(session.get_session_cookie(),id)) {
		if(new_data) {
			storage_->remove(id);
			id = get_new_sid();
		}
	}
	else {
		id = get_new_sid();
	}
	storage_->save(id,timeout,data);
	session.set_session_cookie("I"+id); // Renew cookie or set new one
}

bool session_sid::load(session_interface &session,std::string &data,time_t &timeout)
{
	std::string id;
	if(!valid_sid(session.get_session_cookie(),id))
		return false;
	std::string tmp_data;
	if(!storage_->load(id,timeout,data))
		return false;
	if(time(0) > timeout) {
		storage_->remove(id);
		return false;
	}
	return true;
}

void session_sid::clear(session_interface &session)
{
	std::string id;
	if(valid_sid(session.get_session_cookie(),id))
		storage_->remove(id);
}

bool session_sid::is_blocking()
{
	return storage_->is_blocking();
}


} // sessions
} // namespace cppcms
