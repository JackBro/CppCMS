#ifndef CACHE_IFACE_H
#define CACHE_IFACE_H

#include <string>
#include <set>
#include "archive.h"
#include "base_cache.h"

namespace cppcms {

using namespace std;

const time_t infty=(sizeof(time_t)==4 ? 0x7FFFFFFF: 0x7FFFFFFFFFFFFFFFULL );

class worker_thread;
class cache_iface {
	worker_thread *cms;
	set<string> triggers;
public:
	void reset() { triggers.clear(); };
	cache_iface(worker_thread *w) : cms (w) {};
	bool fetch_page(string const &key);
	void store_page(string const &key,time_t timeout=infty);
	void rise(string const &trigger);
	void add_trigger(string const &trigger);
	bool fetch_frame(string const &key,string &result);
	void store_frame(string const &key,
			 string const &frame,
			 set<string> const &triggers=set<string>(),
			 time_t timeout=infty);
	bool fetch_data(string const &key,serializable &data);
	void store_data(string const &key,serializable const &data,
			set<string> const &triggers=set<string>(),
			time_t timeout=infty);
	void clear();
	bool stats(unsigned &keys,unsigned &triggers);

};

void deflate(string const &text,ostream &stream,long level,long length);
string deflate(string const &text,long level,long length);

class cache_factory {
public:
	virtual base_cache *get() const { return NULL; };
	virtual void del(base_cache *p) const { };
	virtual ~cache_factory() {};
};


}

#endif
