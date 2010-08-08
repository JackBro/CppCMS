//
//  Copyright (c) 2009-2010 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#define BOOST_LOCALE_SOURCE
#include <boost/locale/codepage.hpp>
#include <iconv.h>
#include "conv.h"
#include <assert.h>
#include <iostream>

#ifdef BOOST_MSVC
#  pragma warning(disable : 4244) // loose data 
#endif

#include <vector>
namespace boost {
namespace locale {
namespace conv {
namespace impl {


extern "C" {
    typedef size_t gnu_iconv_type(iconv_t,char const **,size_t *,char **,size_t *);
    typedef size_t std_iconv_type(iconv_t,char **,size_t *,char **,size_t *);
}

namespace {

    template<typename CharType>
    char const *utf_name()
    {
        union {
            char first;
            uint16_t u16;
            uint32_t u32;
        } v;

        if(sizeof(CharType) == 1) {
            return "UTF-8";
        }
        else if(sizeof(CharType) == 2) {
            v.u16 = 1;
            if(v.first == 1) {
                return "UTF-16LE";
            }
            else {
                return "UTF-16BE";
            }
        }
        else if(sizeof(CharType) == 4) {
            v.u32 = 1;
            if(v.first == 1) {
                return "UTF-32LE";
            }
            else {
                return "UTF-32BE";
            }

        }
        else {
            return "Unknown Character Encoding";
        }
    }
} // anon-namespace

class iconverter_base {
public:
    
    iconverter_base() : 
    cvt_((iconv_t)(-1))
    {
    }

    virtual ~iconverter_base()
    {
        close();
    }

    size_t conv(char const **inbuf,size_t *inchar_left,
                char **outbuf,size_t *outchar_left)
    {
        return do_conv(iconv,inbuf,inchar_left,outbuf,outchar_left);
    }

    bool open(char const *to,char const *from)
    {
        close();
        cvt_ = iconv_open(to,from);
        return cvt_ != (iconv_t)(-1);
    }

private:

    void close()
    {
        if(cvt_!=(iconv_t)(-1)) {
            iconv_close(cvt_);
            cvt_ = (iconv_t)(-1);
        }
    }
    

    size_t do_conv(gnu_iconv_type *real_conv,char const **inbuf,size_t *inchar_left,char **outbuf,size_t *outchar_left)
    {
        return real_conv(cvt_,inbuf,inchar_left,outbuf,outchar_left);
    }

    size_t do_conv(std_iconv_type *real_conv,char const **inbuf,size_t *inchar_left,char **outbuf,size_t *outchar_left)
    {
        return real_conv(cvt_,const_cast<char **>(inbuf),inchar_left,outbuf,outchar_left);
    }

    iconv_t cvt_;

};

template<typename CharType>
class iconv_from_utf : public iconverter_base, public converter_from_utf<CharType>
{
public:

    typedef CharType char_type;

    virtual bool open(char const *charset,method_type how)
    {
        if(!iconverter_base::open(charset,utf_name<CharType>()))
            return false;
        how_ = how;
        return true;
    }

    virtual std::string convert(char_type const *ubegin,char_type const *uend)
    {
        std::string sresult;
        std::vector<char> result((uend-ubegin)+10,'\0');
        char *out_start = &result[0];

        char const *begin = reinterpret_cast<char const *>(ubegin);
        char const *end = reinterpret_cast<char const *>(uend);

        for(;;) {

            size_t in_left = end - begin;
            size_t out_left = result.size();
            
            char *out_ptr = out_start;

            size_t res = conv(&begin,&in_left,&out_ptr,&out_left);
            int err = errno;
            sresult.append(&result[0],out_ptr - out_start);

            if(res == (size_t)(-1)) {
                if(err == EILSEQ || err == EINVAL) {
                    if(how_ == stop) {
                        throw conversion_error();
                    }
                    else if(begin != end) {
                        begin+=sizeof(char_type);
                    }
                    else {
                        break;
                    }
                }
                else if (err==E2BIG) {
                    continue;
                }
            }
            else {
                break;
            }
        }
        return sresult;
    }

private:
    method_type how_;

};


template<typename CharType>
class iconv_to_utf : public iconverter_base, public converter_to_utf<CharType>
{
public:

    typedef CharType char_type;
    typedef std::basic_string<char_type> string_type;

    virtual bool open(char const *charset,method_type how)
    {
        if(!iconverter_base::open(utf_name<CharType>(),charset))
            return false;
        how_ = how;
        return true;
    }

    virtual string_type convert(char const *begin,char const *end)
    {
        string_type sresult;
        std::vector<char_type> result((end-begin)+10,char_type());
        char *out_start = reinterpret_cast<char *>(&result[0]);

        for(;;) {

            size_t in_left = end - begin;
            size_t out_left = result.size() * sizeof(char_type);
            
            char *out_ptr = out_start;

            size_t res = conv(&begin,&in_left,&out_ptr,&out_left);
            int err = errno;
            sresult.append(&result[0],(out_ptr - out_start)/sizeof(char_type));

            if(res == (size_t)(-1)) {
                if(err == EILSEQ || err == EINVAL) {
                    if(how_ == stop) {
                        throw conversion_error();
                    }
                    else if(begin != end) {
                        begin++;
                    }
                    else {
                        break;
                    }
                }
                else if (err==E2BIG) {
                    continue;
                }
            }
            else {
                break;
            }
        }
        return sresult;
    }

private:
    method_type how_;
};




} // impl
} // conv
} // locale 
} // boost


int main()
{
    using namespace boost::locale::conv;
    using namespace boost::locale::conv::impl;
    std::string shalom_utf8="שלום";
    std::string shalom_pease_utf8="\xFF\xFFשלום Мир world";
    std::wstring shalom_wchar_t=L"שלום";
    std::string shalom_1255="\xf9\xec\xe5\xed";

    {
        iconv_from_utf<char> utfc;
        iconv_from_utf<wchar_t> wcharc;
        assert(utfc.open("windows-1255",stop));
        assert(wcharc.open("windows-1255",stop));
        assert(utfc.convert(shalom_utf8.c_str(),shalom_utf8.c_str()+shalom_utf8.size()) == shalom_1255);
        assert(wcharc.convert(shalom_wchar_t.c_str(),shalom_wchar_t.c_str()+shalom_wchar_t.size()) == shalom_1255);
        assert(wcharc.open("utf-8",stop));
        assert(wcharc.convert(shalom_wchar_t.c_str(),shalom_wchar_t.c_str()+shalom_wchar_t.size()) == shalom_utf8);
        assert(utfc.open("windows-1255",skip));
        assert(utfc.convert(shalom_pease_utf8.c_str(),shalom_pease_utf8.c_str()+shalom_pease_utf8.size()) == shalom_1255+"  world");
    }

    std::cerr << "From Ok" << std::endl;

    {
        iconv_to_utf<char> utfc;
        iconv_to_utf<wchar_t> wcharc;
        assert(utfc.open("windows-1255",stop));
        assert(wcharc.open("windows-1255",stop));
        assert(utfc.convert(shalom_1255.c_str(),shalom_1255.c_str()+shalom_1255.size())==shalom_utf8);
        assert(wcharc.convert(shalom_1255.c_str(),shalom_1255.c_str()+shalom_1255.size())==shalom_wchar_t);
    }
    
    std::cerr << "To Ok" << std::endl;
}



// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
