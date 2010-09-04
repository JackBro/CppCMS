//
//  Copyright (c) 2009-2010 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_LOCALE_UTIL_DEFAULT_LOCALE_HPP
#define BOOST_LOCALE_UTIL_DEFAULT_LOCALE_HPP
#include <string>

namespace boost {
    namespace locale {
        namespace util {
            std::string get_system_locale(bool use_utf8_on_windows = false);
        }
    }
}
#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
