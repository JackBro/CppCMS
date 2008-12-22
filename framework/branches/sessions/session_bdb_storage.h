#ifndef SESSION_BDB_STORAGE_H
#define SESSION_BDB_STORAGE_H

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "session_storage.h"
#include "session_backend_factory.h"

namespace cppcms {
class manager;

namespace storage {
class bdb;
} // storage

class session_bdb_storage : public session_server_storage {
	boost::shared_ptr<storage::bdb> db;
public:
	static session_backend_factory factory(manager &);
	session_bdb_storage(boost::shared_ptr<storage::bdb> );
	virtual void save(std::string const &sid,time_t timeout,std::string const &in);
	virtual bool load(std::string const &sid,time_t *timeout,std::string &out);
	virtual void remove(std::string const &sid) ;
};

} // cppcms


#endif
