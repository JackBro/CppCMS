#include "worker_thread.h"
#include "global_config.h"
#include "thread_cache.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include "manager.h"

using namespace cgicc;
namespace cppcms {

void worker_thread::main()
{
	out="<h1>Hello World</h2>\n";
}


void worker_thread::run(cgicc_connection &cgi_conn)
{
	cgi=&cgi_conn.cgi();
	env=&(cgi->getEnvironment());
	ostream &cout=cgi_conn.cout();

	other_headers.clear();
	out.clear();
	out.reserve(app.config.lval("server.buffer_reserve",16000));
	cache.reset();

	set_header(new HTTPHTMLHeader);

	gzip=gzip_done=false;
	string encoding;

	if((encoding=cgi_conn.env("HTTP_ACCEPT_ENCODING"))!="") {
		if(strstr(encoding.c_str(),"gzip")!=NULL) {
			gzip=app.config.lval("gzip.enable",0);
		}
	}

	try {
		/**********/
		main();
		/**********/
		if(response_header.get() == NULL) {
			throw cppcms_error("Looks like a bug");
		}
	}
	catch(std::exception const &e) {
		string msg=e.what();
		set_header(new HTTPStatusHeader(500,msg));
		out="<html><body><p>"+msg+"</p><body></html>";
		gzip=gzip_done=false;
		other_headers.clear();
	}

	if(app.config.lval("server.disable_xpowered_by",0)==0) {
		add_header("X-Powered-By: CppCMS/0.0alpha1");
	}

	for(list<string>::iterator h=other_headers.begin();h!=other_headers.end();h++) {
		cout<<*h<<"\n";
	}

	if(gzip) {
		if(out.size()>0) {
			if(gzip_done){
				cout<<"Content-Length: "<<out.size()<<"\n";
			}
			cout<<"Content-Encoding: gzip\n";
			cout<<*response_header;
			if(gzip_done) {
				cout<<out;
			}
			else{
				long level=app.config.lval("gzip.level",-1);
				long length=app.config.lval("gzip.buffer",-1);
				deflate(out,cout,level,length);
			}
		}
		else {
			cout<<*response_header;
		}
	}
	else {
		cout<<"Content-Length: "<<out.size()<<"\n";
		cout<<*response_header;
		cout<<out;
	}

	// Clean Up
	out.clear();
	other_headers.clear();
	response_header.reset();
	cgi=NULL;
}

void worker_thread::init_internal()
{
	caching_module=app.cache->get();
}

worker_thread::~worker_thread()
{
	app.cache->del(caching_module);
}



}



