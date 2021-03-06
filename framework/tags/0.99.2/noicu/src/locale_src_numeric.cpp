//
//  Copyright (c) 2009-2010 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#define CPPCMS_LOCALE_SOURCE
#include <cppcms/locale_numeric.h>
#include <booster/regex.h>
#include <booster/posix_time.h>

namespace cppcms {
namespace locale {

bool num_format::use_parent(std::ios_base &ios) const
{
	return (ext_flags(ios) & flags::display_flags_mask) == flags::number;
}

num_format::iter_type num_format::put_value(num_format::iter_type out,std::ios_base &ios,char fill,long double value) const
{
	uint64_t flag = ext_flags(ios) & flags::display_flags_mask;
	if(flags::date<=flag && flag <=flags::strftime) {
		std::string format;
		switch(flag) {
		case flags::date: format="%x"; break;
		case flags::time: format="%X"; break;
		case flags::datetime: format="%c"; break;
		case flags::strftime: format=ext_pattern<char>(ios,flags::datetime_pattern); break;
		}
		std::string timezone = ext_pattern<char>(ios,flags::time_zone_id); 

		std::tm tm;
		time_t time=static_cast<time_t>(value);
		if(!timezone.empty()) {
			booster::cmatch m;
			static booster::regex r("^([Gg][Mm][Tt]|[Uu][Tt][Cc])([\\+\\-]?(\\d+))(:(\\d+))?$");
			int gmtoff = 0;
			if(booster::regex_match(timezone.c_str(),m,r)) {
				gmtoff= 3600 * atoi(std::string(m[2]).c_str()) 
					+ 60 * atoi(std::string(m[5]).c_str());
			}
			time+=gmtoff;
			tm=booster::ptime::universal_time(booster::ptime(time));

			#ifdef HAVE_BSD_TM 
			// Set correct timezone name and offset where possible
			// Available under Linux, BSD, Mac OS X, 
			tm.tm_zone=timezone.c_str();
			tm.tm_gmtoff = gmtoff;
			#endif
		}
		else {
			tm=booster::ptime::local_time(booster::ptime(time));
		}
		return std::use_facet<std::time_put<char> >(ios.getloc()).put(out,ios,fill,&tm,format.data(),format.data()+format.size());
	}
	else if(flag == flags::currency) {
		bool intl = (ext_flags(ios) & flags::currency_flags_mask)==flags::currency_iso;
		int digits;
		std::locale loc(ios.getloc());
		if(intl)
			digits=std::use_facet<std::moneypunct<char,true> >(loc).frac_digits();
		else
			digits=std::use_facet<std::moneypunct<char,false> >(loc).frac_digits();
		while(digits > 0) {
			value*=10;
			digits --;
		}
		std::ios_base::fmtflags f=ios.flags();
		ios.flags(f | std::ios_base::showbase);
		out = std::use_facet<std::money_put<char> >(loc).put(out,intl,ios,fill,value);
		ios.flags(f);
		return out;
	}
	return out;
}


} // locale
} // cppcms
