//
//  Copyright (c) 2009-2011 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOSTER_LOCALE_ENCODING_H_INCLUDED
#define BOOSTER_LOCALE_ENCODING_H_INCLUDED

#include <booster/config.h>
#ifdef BOOSTER_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4275 4251 4231 4660)
#endif
#include <booster/locale/info.h>
#include <booster/cstdint.h>
#include <booster/backtrace.h>



namespace booster {
    namespace locale {

        ///
        /// \brief Namespace that contains all functions related to character set conversion
        ///
        namespace conv {
            ///
            /// \defgroup codepage Character conversion functions
            ///
            /// @{

            ///
            /// \brief The excepton that is thrown in case of conversion error
            ///
            class BOOSTER_SYMBOL_VISIBLE conversion_error : public booster::runtime_error {
            public:
                conversion_error() : booster::runtime_error("Conversion failed") {}
            };
            
            ///
            /// \brief This exception is thrown in case of use of unsupported
            /// or invalid character set
            ///
            class BOOSTER_SYMBOL_VISIBLE invalid_charset_error : public booster::runtime_error {
            public:

                /// Create an error for charset \a charset
                invalid_charset_error(std::string charset) : 
                    booster::runtime_error("Invalid or unsupported charset:" + charset)
                {
                }
            };
            

            ///
            /// enum that defines conversion policy
            ///
            typedef enum {
                skip            = 0,    ///< Skip illegal/unconvertable characters
                stop            = 1,    ///< Stop conversion and throw conversion_error
                default_method  = skip  ///< Default method - skip
            } method_type;

            ///
            /// convert string to UTF string from text in range [begin,end) encoded with \a charset according to policy \a how
            ///
            template<typename CharType>
            std::basic_string<CharType> to_utf(char const *begin,char const *end,std::string const &charset,method_type how=default_method);

            ///
            /// convert UTF text in range [begin,end) to a text encoded with \a charset according to policy \a how
            ///
            template<typename CharType>
            std::string from_utf(CharType const *begin,CharType const *end,std::string const &charset,method_type how=default_method);

            ///
            /// convert string to UTF string from text in range [begin,end) encoded according to locale \a loc according to policy \a how
            ///
            /// \note throws std::bad_cast if the loc does not have \ref info facet installed
            /// 
            template<typename CharType>
            std::basic_string<CharType> to_utf(char const *begin,char const *end,std::locale const &loc,method_type how=default_method)
            {
                return to_utf<CharType>(begin,end,std::use_facet<info>(loc).encoding(),how);
            }

            ///
            /// convert UTF text in range [begin,end) to a text encoded according to locale \a loc according to policy \a how
            ///
            /// \note throws std::bad_cast if the loc does not have \ref info facet installed
            /// 
            template<typename CharType>
            std::string from_utf(CharType const *begin,CharType const *end,std::locale const &loc,method_type how=default_method)
            {
                return from_utf(begin,end,std::use_facet<info>(loc).encoding(),how);
            }

            ///
            /// convert a string \a text encoded with \a charset to UTF string
            ///
            
            template<typename CharType>
            std::basic_string<CharType> to_utf(std::string const &text,std::string const &charset,method_type how=default_method)
            {
                return to_utf<CharType>(text.c_str(),text.c_str()+text.size(),charset,how);
            }

            ///
            /// Convert a \a text from \a charset to UTF string
            ///
            template<typename CharType>
            std::string from_utf(std::basic_string<CharType> const &text,std::string const &charset,method_type how=default_method)
            {
                return from_utf(text.c_str(),text.c_str()+text.size(),charset,how);
            }

            ///
            /// Convert a \a text from \a charset to UTF string
            ///
            template<typename CharType>
            std::basic_string<CharType> to_utf(char const *text,std::string const &charset,method_type how=default_method)
            {
                char const *text_end = text;
                while(*text_end) 
                    text_end++;
                return to_utf<CharType>(text,text_end,charset,how);
            }

            ///
            /// Convert a \a text from UTF to \a charset
            ///
            template<typename CharType>
            std::string from_utf(CharType const *text,std::string const &charset,method_type how=default_method)
            {
                CharType const *text_end = text;
                while(*text_end) 
                    text_end++;
                return from_utf(text,text_end,charset,how);
            }

            ///
            /// Convert a \a text in locale encoding given by \a loc to UTF
            ///
            /// \note throws std::bad_cast if the loc does not have \ref info facet installed
            /// 
            template<typename CharType>
            std::basic_string<CharType> to_utf(std::string const &text,std::locale const &loc,method_type how=default_method)
            {
                return to_utf<CharType>(text.c_str(),text.c_str()+text.size(),loc,how);
            }

            ///
            /// Convert a \a text in UTF to locale encoding given by \a loc
            ///
            /// \note throws std::bad_cast if the loc does not have \ref info facet installed
            /// 
            template<typename CharType>
            std::string from_utf(std::basic_string<CharType> const &text,std::locale const &loc,method_type how=default_method)
            {
                return from_utf(text.c_str(),text.c_str()+text.size(),loc,how);
            }

            ///
            /// Convert a \a text in locale encoding given by \a loc to UTF
            ///
            /// \note throws std::bad_cast if the loc does not have \ref info facet installed
            /// 
            template<typename CharType>
            std::basic_string<CharType> to_utf(char const *text,std::locale const &loc,method_type how=default_method)
            {
                char const *text_end = text;
                while(*text_end) 
                    text_end++;
                return to_utf<CharType>(text,text_end,loc,how);
            }

            ///
            /// Convert a \a text in UTF to locale encoding given by \a loc
            ///
            /// \note throws std::bad_cast if the loc does not have \ref info facet installed
            /// 
            template<typename CharType>
            std::string from_utf(CharType const *text,std::locale const &loc,method_type how=default_method)
            {
                CharType const *text_end = text;
                while(*text_end) 
                    text_end++;
                return from_utf(text,text_end,loc,how);
            }


            ///
            /// Convert a text in range [begin,end) to \a to_encoding from \a from_encoding
            ///
            
            BOOSTER_API
            std::string between(char const *begin,
                                char const *end,
                                std::string const &to_encoding,
                                std::string const &from_encoding,
                                method_type how=default_method);

            ///
            /// Convert a \a text to \a to_encoding from \a from_encoding
            ///
            
            inline
            std::string between(char const *text,
                                std::string const &to_encoding,
                                std::string const &from_encoding,
                                method_type how=default_method)
            {
                char const *end=text;
                while(*end)
                    end++;
                return booster::locale::conv::between(text,end,to_encoding,from_encoding,how);
            }

            ///
            /// Convert a \a text to \a to_encoding from \a from_encoding
            ///
            inline
            std::string between(std::string const &text,
                                std::string const &to_encoding,
                                std::string const &from_encoding,
                                method_type how=default_method)
            {
                return booster::locale::conv::between(text.c_str(),text.c_str()+text.size(),to_encoding,from_encoding,how);
            }
          
            /// \cond INTERNAL

            template<>
            BOOSTER_API std::basic_string<char> to_utf(char const *begin,char const *end,std::string const &charset,method_type how);

            template<>
            BOOSTER_API std::string from_utf(char const *begin,char const *end,std::string const &charset,method_type how);

            template<>
            BOOSTER_API std::basic_string<wchar_t> to_utf(char const *begin,char const *end,std::string const &charset,method_type how);

            template<>
            BOOSTER_API std::string from_utf(wchar_t const *begin,wchar_t const *end,std::string const &charset,method_type how);

            #ifdef BOOSTER_HAS_CHAR16_T
            template<>
            BOOSTER_API std::basic_string<char16_t> to_utf(char const *begin,char const *end,std::string const &charset,method_type how);

            template<>
            BOOSTER_API std::string from_utf(char16_t const *begin,char16_t const *end,std::string const &charset,method_type how);
            #endif

            #ifdef BOOSTER_HAS_CHAR32_T
            template<>
            BOOSTER_API std::basic_string<char32_t> to_utf(char const *begin,char const *end,std::string const &charset,method_type how);

            template<>
            BOOSTER_API std::string from_utf(char32_t const *begin,char32_t const *end,std::string const &charset,method_type how);
            #endif

            namespace details {

                template<typename CharOut,typename CharIn>
                struct utf_to_utf_traits {
                    static std::basic_string<CharOut>
                    convert(CharIn const *begin,CharIn const *end,method_type how)
                    {
                        // Make more efficient in fututre - UTF-16/UTF-32 should be quite
                        // simple and fast
                        return to_utf<CharOut>(from_utf(begin,end,"UTF-8",how),"UTF-8",how);
                    }
                };
                template<typename CharOut>
                struct utf_to_utf_traits<CharOut,char> {
                    static std::basic_string<CharOut>
                    convert(char const *begin,char const *end,method_type how)
                    {
                        return to_utf<CharOut>(begin,end,"UTF-8",how);
                    }
                };
                template<typename CharIn>
                struct utf_to_utf_traits<char,CharIn> {
                    static std::string
                    convert(CharIn const *begin,CharIn const *end,method_type how)
                    {
                        return from_utf(begin,end,"UTF-8",how);
                    }
                };
                template<>
                struct utf_to_utf_traits<char,char> { // just test valid
                    static std::string
                    convert(char const *begin,char const *end,method_type how)
                    {
                        return from_utf(begin,end,"UTF-8",how);
                    }
                };
            }

            /// \endcond
           
            ///
            /// Convert a Unicode text in range [begin,end) to other Unicode encoding
            ///
            template<typename CharOut,typename CharIn>
            std::basic_string<CharOut>
            utf_to_utf(CharIn const *begin,CharIn const *end,method_type how = default_method)
            {
                return details::utf_to_utf_traits<CharOut,CharIn>::convert(begin,end,how);
            }

            ///
            /// Convert a Unicode NUL terminated string \a str other Unicode encoding
            ///
            template<typename CharOut,typename CharIn>
            std::basic_string<CharOut>
            utf_to_utf(CharIn const *str,method_type how = default_method)
            {
                CharIn const *end = str;
                while(*end)
                    end++;
                return utf_to_utf<CharOut,CharIn>(str,end,how);
            }


            ///
            /// Convert a Unicode string \a str other Unicode encoding
            ///
            template<typename CharOut,typename CharIn>
            std::basic_string<CharOut>
            utf_to_utf(std::basic_string<CharIn> const &str,method_type how = default_method)
            {
                return utf_to_utf<CharOut,CharIn>(str.c_str(),str.c_str()+str.size(),how);
            }


            /// @}

        } // conv

    } // locale
} // boost

#ifdef BOOSTER_MSVC
#pragma warning(pop)
#endif

#endif

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

