//
//  Copyright (c) 2009 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#define CPPCMS_LOCALE_SOURCE
#include <cppcms/locale_conversion.h>
#include <cppcms/locale_info.h>
#include "utf_iterator.h"
#include <cppcms/config.h>
namespace cppcms {
namespace locale {
namespace impl{
    
    #ifdef CPPCMS_HAVE_STD_WSTRING
    
    std::wstring to_wide(char const *begin,char const *end)
    {
        std::wstring res;
        res.reserve(end-begin);
        while(begin < end) {
            uint32_t point=utf8::next(begin,end,false,true);
            if(point==utf::illegal)
                break;
            if(sizeof(wchar_t)==4) {
                res+=(wchar_t(point));
            }
            else  {
                utf16::seq s=utf16::encode(point);
                res.append(reinterpret_cast<wchar_t *>(s.c),s.len);
            }
        }
        return res;
    }

    std::string from_wide(std::wstring const &w)
    {
        std::string res;
        res.reserve(w.size());
        wchar_t const *begin=w.c_str(),*end=w.c_str()+w.size();
        while(begin < end) {
            uint32_t point;
            if(sizeof(wchar_t)==2)
                point=utf16::next(begin,end);
            else
                point=uint32_t(*begin++); 
            if(point==utf::illegal)
                break;
            utf8::seq s=utf8::encode(point);
            res.append(s.c,s.len);
        }
        return res;
    }

    #endif
   

    std::string convert(conversion_type how,char const *begin,char const *end,std::locale const &l)
    {
        #ifdef CPPCMS_HAVE_STD_WSTRING
        typedef std::ctype<wchar_t> wctype;
        if(begin!=end && std::has_facet<info>(l) && std::use_facet<info>(l).utf8() && std::has_facet<wctype>(l)) {
            std::wstring s = to_wide(begin,end);
            if(s.empty())
                return std::string();
            wctype const &facet=std::use_facet<wctype>(l);
            if(how == lower_case)
                facet.tolower(&s[0],&s[0]+s.size());
            else
                facet.toupper(&s[0],&s[0]+s.size());
            return from_wide(s); 
        }
        #endif
        std::string s(begin,end-begin);
        if(s.empty())
            return s;
        std::ctype<char> const &facet=std::use_facet<std::ctype<char> >(l);
        if(how == lower_case)
            facet.tolower(&s[0],&s[0]+s.size());
        else
            facet.toupper(&s[0],&s[0]+s.size());
        return s;
    }


} // namespace impl
} // locale
} // boost

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
