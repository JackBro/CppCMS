#ifndef BOOST_LOCALE_INFO_HPP_INCLUDED
#define BOOST_LOCALE_INFO_HPP_INCLUDED
#include <locale>
#include <string>
#include <map>
#include <memory>
#include <boost/locale/config.hpp>

namespace boost {
    namespace locale {

        class info_impl;

        class BOOST_LOCALE_DECL info : public std::locale::facet
        {
        public:
            static std::locale::id id;
            
            ///
            /// Creates locale using general locale id that includes encoding
            /// If encoding is not found, default system encoding is taken, if the string is empty
            /// default system locale is used.
            ///
            info(std::string posix_id,size_t refs = 0);
            
            ///
            /// Creates locale using general locale id and cherset encoding
            /// if posix_id is empty default system locale is used.
            ///
            info(std::string posix_id,std::string encoding,size_t refs = 0);


            
            ///
            /// Get language name
            ///
            std::string language() const;
            ///
            /// Get country name
            ///
            std::string country() const;
            ///
            /// Get locale variant
            ///
            std::string variant() const;
            ///
            /// Get encoding
            ///
            std::string encoding() const;

            ///
            /// Is underlying encoding is UTF-8 (for char streams and strings)
            ///
            bool utf8() const
            {
                return utf8_;
            }

            info_impl const *impl() const
            {
                return impl_.get();
            }

        protected:
            
            virtual ~info();

        private:
            std::auto_ptr<info_impl> impl_;
            bool utf8_;
        };

    }
}


#endif

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4