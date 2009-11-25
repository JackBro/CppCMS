//
//  Copyright (c) 2009 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include <locale>
#include <boost/locale/format.hpp>
#include <boost/locale/formatting.hpp>
#include <boost/locale/numeric.hpp>
#include <boost/locale/info.hpp>
#include <boost/locale/message.hpp>
#include <boost/locale/conversion.hpp>
#include <boost/locale/collator.hpp>
#include <boost/locale/generator.hpp>
#include <boost/locale/codepage.hpp>
#include <boost/locale/boundary.hpp>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <fstream>
#include <map>

int main()
{
	try {
	using namespace boost::locale;
	using namespace std;
	
	try {
		std::locale::global(std::locale(""));
	}
	catch(std::exception const &e)
	{
	}

	generator gen;

	//gen.categories(gen.categories() ^ codepage_facet); 
	gen.set_default_messages_domain("test");
	gen.add_messages_path(".");
	gen.add_messages_path("../src");

	std::locale::global(gen.generate("ja_JP.UTF-8"));

	typedef comparator<char,collator_base::primary> comp_type;

	typedef std::map<std::string,std::string,comp_type> phones_map_type;
	phones_map_type phones;

	phones["facade"]="Yes";
	phones["Façade"]="Now";

	for(phones_map_type::const_iterator p=phones.begin();p!=phones.end();++p)
		std::cerr<<p->first<<":"<<p->second<<std::endl;


	std::cout << to_upper<char>("Артем") << std::endl;
	
	stringstream msg,out;

	time_t now=std::time(0);
	
	msg<<translate("#A message#Hello World")<<endl;
	msg<<translate("##Hello World")<<endl;
	for(int i=0;i<15;i++) {
		msg<<format(translate("Passed one day since {2,date=m}","Passed {1,spell} days since {2,date=m}",i)) % i % now<<endl;
		msg<<format(translate("So it would cost you about {1,currency,w=10,>} per {2,ord} day")) % -i % i<<endl;
	}
	std::cout<<msg.str()<<std::flush;

	out<<as::number<<104<<" "<<1234<<" ";
	out<<std::scientific<<1234.56e-10<<std::endl;
	out<<as::currency<<104.12<<" ";
	out<<as::currency_iso<<104.12<<endl;
	out<<as::spellout<<104<<"   "<<1234.56<<std::endl;
	out<<as::ordinal<<104<<endl;
	out<<as::date<<now<<endl;
	out<<as::datetime<<as::date_full<<as::time_full << std::time(0) << endl;

	int v_104 = 0,v_1234 = 0;
	time_t now_a = 0;
	double v_1234_56e_10 = 0,v_1234_56 = 0,v_104_12 = 0;

	
	out.unsetf(std::ios_base::scientific);
	out >> as::date_default >> as::time_default;

	out>> as::number >> v_104 >> v_1234;

	std::cout<<"104 = "<<104<<" 1234 = "<<v_1234<<endl;
	out>>std::scientific>>v_1234_56e_10;
	cout << "v_1234_56e_10="<<v_1234_56e_10 << endl;

	out>>as::currency>>v_104_12;
	cout << "v_104_12 = " << v_104_12 <<endl;
	v_104_12=0;
	out>>as::currency_iso>>v_104_12;
	
	cout << "v_104_12 = " << v_104_12 <<endl;
	v_104=0;
	v_1234_56 = 0;

	out>>as::spellout >> v_104 >> v_1234_56;
	cout << "v_104 = " << v_104 <<endl;
	cout << "v_1034.56 = " << v_1234_56 <<endl;

	v_104=0;
	out >> as::ordinal >> v_104 ;
	cout << "v_104 = " << v_104 <<endl;

	out>>as::date>>now_a;
	cout << now_a <<"="<< now <<endl;
	now_a=0;
	out>>as::datetime>>as::date_full>>as::time_full >> now_a;
	cout << now_a <<"="<< now <<endl;

	std::cout<<out.str();

	out.str("");
	out<<as::ftime("[ '%Y-%m-%d %H:%M:%S' ]") << now;
	std::cout<<out.str();

	std::cout<<std::endl;
	std::string text="10 Hello Windows7 平仮名ひらがなヒラガナ";
	//std::string text="123 Hello World, שָלוֹם Hello-World, Windows7, ウィキペディアはオープンコンテントの百科事典です。基本方針に賛同していただけるなら、誰でも記事を編集したり新しく作成したりできます。";
	

	boundary::mapping<std::string::iterator> indx(boundary::word,text.begin(),text.end());

	boundary::break_iterator<std::string::iterator> p(indx),end,cur;

	while(p!=end) {
		cur=p++;
		if(p!=end) {
			std::cout<<"["<<std::string(*cur,*p)<<"]"<<std::endl;
		}
	}

	boundary::token_iterator<std::string::iterator> words(indx,boundary::letter | boundary::kana | boundary::ideo),wend;
	while(words!=wend)
		std::cout <<"["<<*words++<<"]";
	std::cout<<std::endl;

/*	for(unsigned i=0;i<index.size()-1;i++) {
		std::cout<<"["<<text.substr(index[i].offset,index[i+1].offset-index[i].offset)<<"]"<<endl;
		std::cout<<index[i].next<<std::endl;
	}*/

	}
	catch(std::exception const &e) {
	     std::cerr<<e.what()<<std::endl;
	}
	
}
