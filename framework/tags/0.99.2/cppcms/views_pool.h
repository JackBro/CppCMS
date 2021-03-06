///////////////////////////////////////////////////////////////////////////////
//                                                                             
//  Copyright (C) 2008-2010  Artyom Beilis (Tonkikh) <artyomtnk@yahoo.com>     
//                                                                             
//  This program is free software: you can redistribute it and/or modify       
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef CPPCMS_VIEWS_POOL_H
#define CPPCMS_VIEWS_POOL_H

#include <cppcms/defs.h>
#include <booster/noncopyable.h>
#include <cppcms/base_view.h>

#include <memory>
#include <map>
#include <ostream>

namespace cppcms {

	namespace json { class value; }

	class CPPCMS_API views_pool : public booster::noncopyable {
	public:

		/// \cond INTERNAL

		typedef std::auto_ptr<base_view> (*view_factory_type)(std::ostream &,base_content *c);
		typedef std::map<std::string,view_factory_type> mapping_type;

		template<typename View,typename Content>
		static std::auto_ptr<base_view> view_builder(std::ostream &stream,base_content *c) 
		{
			std::auto_ptr<base_view> p(new View(stream,dynamic_cast<Content &>(*c)));
			return p;
		};

		views_pool();
		views_pool(json::value const &settings);
		~views_pool();
		
		void add_view(std::string skin,mapping_type const &mapping);
		void remove_view(std::string skin);

		static views_pool &static_instance();

		/// \endcond
	
		///
		/// This member function is used to render templates. Generally you should not use
		/// it directly, unless you have very good reasons.
		///
		/// \param skin - the name of the skin that should be used
		/// \param template_name - the name of template (class) that should be rendered.
		/// \param out - the output stream into which the view should be rendered
		/// \param content - the content that should be rendered using this view.
		///
		/// This function is thread safe
		///
		void render(std::string skin,std::string template_name,std::ostream &out,base_content &content);

		///
		/// Get default skin name
		///
		std::string default_skin() const;
		
	private:

		struct _data;
		struct skin;
		booster::hold_ptr<_data> d;
	};

}


#endif
