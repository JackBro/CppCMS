//
// C++ Interface: blog
//
// Description:
//
//
// Author: artik <artik@art-laptop>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BLOG_H
#define BLOG_H
#include <cppcms/worker_thread.h>
#include <cppcms/url.h>
#include "views.h"
#include "data.h"


struct links_t {
	string media;
	string main;
	string main_from;
	string post;
	string admin;
	string new_post;
	string edit_post;
	string edit_comment;
	string add_comment;
	string add_post;
	string approve;
};

struct post_content_t {
	bool has_content;
	string title;
	string permlink;
	string author;
	string published;
	string abstract;
	string content;
};

class Blog : public Worker_Thread {
	URL_Parser url;
// Member functions:
	void main_page(string s);
	void post(string s);
	void add_comment(string &postid);

public:
	links_t fmt;
	virtual void init();
	virtual void main();
	void date(time_t time,string &s);
	Blog() : url(this) {};
};
#endif
