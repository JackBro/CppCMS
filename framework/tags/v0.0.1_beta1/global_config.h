#ifndef _GLOBAL_CONFIG_H
#define _GLOBAL_CONFIG_H

#include <string>
#include <map>
#include "cppcms_error.h"

namespace cppcms {

using namespace std;

class cppcms_config {

	enum { WORD, INT, DOUBLE, STR };

	typedef std::pair<int,string> tocken_t;

	typedef std::pair<string,string> key_t;

	std::map<string,long> long_map;
	std::map<string,double> double_map;
	std::map<string,string> string_map;

	string filename;

	int line_counter;
	bool loaded;

	bool get_tocken(FILE *,tocken_t &T);
public:

	void load(char const *filename);
	void load(int argc,char *argv[],char const *def=NULL);

	cppcms_config() { loaded = false;};
	long lval(string major) const {
		std::map<string,long>::const_iterator it;
		if((it=long_map.find(major))!=long_map.end()) {
			return it->second;
		}
		throw cppcms_error("Undefined configuration "+major);
	};
	long lval(string major,long def) const {
		std::map<string,long>::const_iterator it;
		if((it=long_map.find(major))!=long_map.end()) {
			return it->second;
		}
		return def;
	};
	double dval(string major) const {
		std::map<string,double>::const_iterator it;
		if((it=double_map.find(major))!=double_map.end()) {
			return it->second;
		}
		throw cppcms_error("Undefined configuration "+major);
	};
	double dval(string major,double def) const {
		std::map<string,double>::const_iterator it;
		if((it=double_map.find(major))!=double_map.end()) {
			return it->second;
		}
		return def;
	};
	string const &sval(string major) const {
		std::map<string,string>::const_iterator it;
		if((it=string_map.find(major))!=string_map.end()) {
			return it->second;
		}
		throw cppcms_error("Undefined configuration "+major);
	};
	string sval(string major,string def) const {
		std::map<string,string>::const_iterator it;
		if((it=string_map.find(major))!=string_map.end()) {
			return it->second;
		}
		return def;
	};

};

} // namespace cppcms


#endif /* _GLOBAL_CONFIG_H */
