#ifndef BOOST_SRC_ICU_UTIL_HPP
#define BOOST_SRC_ICU_UTIL_HPP
#include <unicode/utypes.h>
#include <stdexcept>

namespace boost {
namespace locale {
namespace impl {
	
    inline void throw_icu_error(UErrorCode err)
    {
        throw std::runtime_error(u_errorName(err));
    }

    inline void check_and_throw_icu_error(UErrorCode err)
    {
        if(U_FAILURE(err))
            throw_icu_error(err);
    }
} // impl
} // locale
} // boost

#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 