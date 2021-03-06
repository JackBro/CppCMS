//
//  Copyright (c) 2009-2010 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOSTER_LOCALE_CONV_IMPL_HPP
#define BOOSTER_LOCALE_CONV_IMPL_HPP

#include <booster/config.h>
#include <booster/locale/encoding.h>
namespace booster {
    namespace locale {
        namespace conv {
            namespace impl {

                std::string normalize_encoding(char const *encoding);
                
                inline int compare_encodings(char const *l,char const *r)
                {
                    return normalize_encoding(l).compare(normalize_encoding(r));
                }
            
                #if defined(BOOSTER_WIN_NATIVE)  || defined(__CYGWIN__)
                int encoding_to_windows_codepage(char const *ccharset);
                #endif
            
                class converter_between {
                public:
                    typedef char char_type;

                    typedef std::string string_type;
                    
                    virtual bool open(char const *to_charset,char const *from_charset,method_type how) = 0;
                    
                    virtual std::string convert(char const *begin,char const *end) = 0;
                    
                    virtual ~converter_between()
                    {
                    }
                };

                template<typename CharType>
                class converter_from_utf {
                public:
                    typedef CharType char_type;

                    typedef std::basic_string<char_type> string_type;
                    
                    virtual bool open(char const *charset,method_type how) = 0;
                    
                    virtual std::string convert(CharType const *begin,CharType const *end) = 0;
                    
                    virtual ~converter_from_utf()
                    {
                    }
                };

                template<typename CharType>
                class converter_to_utf {
                public:
                    typedef CharType char_type;

                    typedef std::basic_string<char_type> string_type;

                    virtual bool open(char const *charset,method_type how) = 0;

                    virtual string_type convert(char const *begin,char const *end) = 0;

                    virtual ~converter_to_utf()
                    {
                    }
                };
            }
        }
    }
}

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
#endif
