#define CPPCMS_SOURCE
#include "locale_pool.h"
#include "cppcms_error.h"
#include "locale_gettext.h"
#include "locale_info.h"
#include "global_config.h"
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace cppcms { namespace locale {

struct pool::data {
	std::vector<std::string> domains;
	typedef std::map<std::string,boost::shared_ptr<std::locale> > locales_type;
	locales_type locales;
	std::string path;
	std::locale fallback;

	data() : fallback(std::locale::classic()) {}
};

pool::pool(cppcms_config const &settings) :
	d(new data)
{
	d->path=settings.str("locale.gettext_path","/usr/local/locale");
	d->domains=settings.slist("locale.gettext_domains");
	std::string default_domain;
	if(!d->domains.empty())
		default_domain=d->domains.front();
	default_domain=settings.str("locale.default_gettext_domain",default_domain);
	std::vector<std::string> const 	&locales=settings.slist("locale.locales");

	for(unsigned i=0;i<locales.size();i++) {


		std::string name=locales[i];
		std::auto_ptr<gettext> gt(new gettext());

		for(unsigned i=0;i < d->domains.size();i++) {
			gt->load(name,d->path,d->domains[i]);
		}
		gt->set_default_domain(default_domain);

		boost::shared_ptr<std::locale> base;

		try {
			base.reset(new std::locale(name.c_str()));
		}
		catch(std::runtime_error const &e) {
			base.reset(new std::locale("C"));
		}

		boost::shared_ptr<std::locale> combined(new std::locale(*base,gt.release()));
		base=combined;
		combined.reset(new std::locale(*base,new info(name)));
		d->locales[name]=combined;
	}
}

std::locale const &pool::get(std::string const &name) const
{
	data::locales_type::const_iterator p;
	if((p=d->locales.find(name))==d->locales.end()) {
		return d->fallback;
	}
	return *p->second;
}


pool::~pool()
{
}




} } // cppcms::locale