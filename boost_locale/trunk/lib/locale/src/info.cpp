#include <boost/locale/info.hpp>
#include <unicode/locid.h>
#include <unicode/ucnv.h>

#include "info_impl.hpp"

namespace boost {
    namespace locale {
        
        std::locale::id info::id;

        info::~info()
        {
        }

        info::info(std::string posix_id,size_t refs) : 
            std::locale::facet(refs)
        {
            impl_.reset(new info_impl());

            if(posix_id.empty()) {
                impl_->encoding = ucnv_getDefaultName();
            }
            else {
                impl_->locale = icu::Locale::createCanonical(posix_id.c_str());
                size_t n = posix_id.find('.');
                if(n!=std::string::npos) {
                    size_t e = posix_id.find('@',n);
                    if(e == std::string::npos)
                        impl_->encoding = posix_id.substr(n+1);
                    else
                        impl_->encoding = posix_id.substr(n+1,e-n-1);
                }
                else
                    impl_->encoding = ucnv_getDefaultName();
            }

            if(ucnv_compareNames(impl_->encoding.c_str(),"UTF8")==0)
                utf8_=true;
            else
                utf8_=false;
        }

        info::info(std::string posix_id,std::string encoding,size_t refs) : 
            std::locale::facet(refs)
        {
            impl_.reset(new info_impl());
            impl_->encoding = encoding;

            if(!posix_id.empty()) {
                impl_->locale = icu::Locale::createCanonical(posix_id.c_str());
            }
            
            if(ucnv_compareNames(impl_->encoding.c_str(),"UTF8")==0)
                utf8_=true;
            else
                utf8_=false;
        }

        std::string info::encoding() const
        {
            return impl_->encoding;
        }
        std::string info::language() const
        {
            return impl_->locale.getLanguage();
        }
        std::string info::country() const
        {
            return impl_->locale.getCountry();
        }
        std::string info::variant() const
        {
            return impl_->locale.getVariant();
        }
    }
}
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
