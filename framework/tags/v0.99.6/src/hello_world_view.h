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
#include <cppcms/view.h>
#include <list>
class my_hello_world;
namespace view {

struct master : public cppcms::base_content {
	std::string title;
	bool ok;
};

struct data {
	std::string name;
	int val;
	data(char const *n="",int v=0) : name(n),val(v){}
};

struct a_form : public cppcms::form {
	cppcms::widgets::text username;
	cppcms::widgets::textarea name;
	cppcms::widgets::email mail;
	cppcms::widgets::password p1;
	cppcms::widgets::password p2;
	cppcms::widgets::numeric<int> integer;
	cppcms::widgets::numeric<double> real;
	cppcms::widgets::checkbox ok;
	//widgets::select fruit;
	//widgets::radio fruit;
	//widgets::select_multiple meat;
	//widgetset my_set;
	a_form() 
	{
		using cppcms::locale::translate;
		username.message(translate("Username"));
		name.message(translate("Real Name"));
		mail.message(translate("E-Mail"));
		p1.message(translate("Password"));
		p2.message(translate("Confirm"));
		integer.message(translate("Integer"));
		real.message(translate("Real"));
		ok.message(translate("Never Save"));
		*this + username + mail + name + p1 + p2 +
			integer + real + ok;
		username.non_empty();
		name.non_empty();
		p2.check_equal(p1);
		p2.help(translate("(Same as above)"));
		p1.non_empty();
		p2.non_empty();
		real.range(-1.0,1.5);
/*		fruit.add("Orange");
		fruit.add("Palm");
		meat.add("Beef");
		meat.add("<<Chicken>>");
		meat.add("Duck");
		meat.set_min(2);
		meat.help=w->gettext("At least two choises");*/
	}
};


struct hello : public master {
	std::string username,realname,password;
	bool ok;
	std::string msg;
	typedef std::list<int> numbers_type;
	numbers_type numbers;
	
	std::list<data> lst;
	a_form form;
};

};

