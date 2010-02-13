//
//  Copyright (c) 2009 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#define CPPCMS_LOCALE_SOURCE
#include "locale_generator.h"
#include "locale_numeric.h"
#include "locale_collator.h"
#include "locale_info.h"
#include "locale_message.h"
#include "locale_codepage.h"
namespace cppcms {
    namespace locale {
        struct generator::data {
            typedef std::pair<std::string,std::string> locale_id_type;

            struct keycomp { 
                bool operator()(locale_id_type const &left,locale_id_type const &right) const
                {
                    if(left.first == right.first)
                        return left.second < right.second;
                    return left.first < right.first;
                }
            };
            
            typedef std::map<locale_id_type,std::locale,keycomp> cached_type;
            cached_type cached;

            std::string encoding;

            std::vector<std::string> paths;
            std::set<std::string> domains;
            std::string default_domain;
        };

        generator::generator() :
            d(new generator::data())
        {
        }
        generator::~generator()
        {
        }

        void generator::octet_encoding(std::string const &enc)
        {
            d->encoding=enc;
        }
        std::string generator::octet_encoding() const
        {
            return d->encoding;
        }
        
        void generator::add_messages_domain(std::string const &domain)
        {
            if(d->default_domain.empty())
                d->default_domain=domain;
            d->domains.insert(domain);
        }
        void generator::set_default_messages_domain(std::string const &domain)
        {
            add_messages_domain(domain);
            d->default_domain=domain;
        }
        void generator::clear_domains()
        {
            d->default_domain.clear();
            d->domains.clear();
        }
        void generator::add_messages_path(std::string const &path)
        {
            d->paths.push_back(path);
        }
        void generator::clear_paths()
        {
            d->paths.clear();
        }
        void generator::clear_cache()
        {
            d->cached.empty();
        }


        std::locale generator::generate(std::string const &id,std::string encoding) const
        {
        }

        std::locale generator::get_base(std::string id,std::string encoding)
        {
        }

        info *generator::get_info(std::locale const &loc,std::string id,std::string encoding)
        {
            std::string const name = loc.name();
            size_t pos = name.find('.');
            if(pos==std::string::npos)
        }

        std::locale generator::generate(std::string const &id) const
        {
            std::locale base(d->encoding.empty() ? id.c_str() : (id + "," + d->encoding).c_str());
            info *tmp = d->encoding.empty() ? new info(id) : new info(id,d->encoding);
            std::locale result=std::locale(base,tmp);
            return complete_generation(result);
        }

        std::locale generator::generate(std::locale const &base,std::string const &id) const
        {
            info *tmp = d->encoding.empty() ? new info(id) : new info(id,d->encoding);
            std::locale result=std::locale(base,tmp);
            return complete_generation(result);
        }

        std::locale generator::generate(std::string const &id,std::string const &encoding) const
        {
            std::locale base;
            std::locale result=std::locale(base,new info(id,encoding));
            return complete_generation(result);
        }
        std::locale generator::generate(std::locale const &base,std::string const &id,std::string const &encoding) const
        {
            std::locale result=std::locale(base,new info(id,encoding));
            return complete_generation(result);
        }
        void generator::preload(std::string const &id) 
        {
            d->cached[data::locale_id_type(id,"")]=generate(id);
        }
        void generator::preload(std::string const &id,std::string const &encoding)
        {
            d->cached[data::locale_id_type(id,encoding)]=generate(id,encoding);
        }
        void generator::preload(std::locale const &base,std::string const &id)
        {
            d->cached[data::locale_id_type(id,"")]=generate(base,id);
        }
        void generator::preload(std::locale const &base,std::string const &id,std::string const &encoding)
        {
            d->cached[data::locale_id_type(id,encoding)]=generate(base,id,encoding);
        }
        
        template<typename CharType>
        std::locale generator::generate_for(std::locale const &source) const
        {
            std::locale result=source;
            info const &inf=std::use_facet<info>(source);
            if(d->cats & collation_facet)
                result=std::locale(result,collator<CharType>::create(inf));
            if(d->cats & formatting_facet) {
                result=std::locale(result,new num_format<CharType>());
                result=std::locale(result,new num_parse<CharType>());
            }
            if(d->cats & message_facet) {
                if(!d->default_domain.empty() && !d->paths.empty()) {
                    std::vector<std::string> domains;
                    domains.push_back(d->default_domain);
                    for(std::set<std::string>::const_iterator p=d->domains.begin(),e=d->domains.end();p!=e;++p) {
                        if(*p!=d->default_domain) {
                            domains.push_back(*p);
                        }
                    }
                    result=std::locale(result,message_format<CharType>::create(inf,domains,d->paths));
                }
            }
            if(d->cats & codepage_facet) {
                result=std::locale(result,code_converter<CharType>::create(inf));
            }
            return result;
        }
        
        std::locale generator::complete_generation(std::locale const &source) const
        {
            std::locale result=source;
            if(d->chars & char_facet)
                result=generate_for<char>(result);
            return result;
        }
        std::locale generator::get(std::string const &id) const
        {
            data::cached_type::const_iterator p=d->cached.find(data::locale_id_type(id,""));            
            if(p==d->cached.end())
                return generate(id);
            else
                return p->second;
        }
        std::locale generator::get(std::string const &id,std::string const &encoding) const
        {
            data::cached_type::const_iterator p=d->cached.find(data::locale_id_type(id,encoding));
            if(p==d->cached.end())
                return generate(id,encoding);
            else
                return p->second;
        }
    }
}
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 