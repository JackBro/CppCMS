//
// C++ Implementation: views
//
// Description:
//
//
// Author: artik <artik@art-laptop>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "views.h"
#include <cppcms/text_tool.h>
#include <boost/format.hpp>
#include "blog.h"
#include "error.h"

using boost::format;
using boost::str;

using namespace dbixx;

void View_Comment::init(comment_t &com)
{
	Text_Tool tt;
	string author;
	tt.text2html(com.author,author);
	c["username"]=author;
	string url;
	if(com.url.size()!=0){
		tt.text2url(com.url.c_str(),url);
		c["url"]=url;
	}
	string message;
	tt.markdown2html(com.content,message);
	c["content"]=message;
	string date;
	blog->date(com.publish_time,date);
	c["date"]=date;
	if(blog->userid!=-1){
		c["delete_url"]=str(format(blog->fmt.del_comment) % com.id);
	}
}

void View_Post::ini_share(post_t &p)
{
	Text_Tool tt;
	string title;
	tt.text2html(p.title,title);
	c["title"]=title;
	c["subtitle"]=title;
	string date;
	blog->date( p.publish ,date);
	c["date"]=date;
	string author;
	tt.text2html(p.author_name,author);
	c["author"]=author;

	if(blog->userid!=-1){
		c["edit_url"]=str(format(blog->fmt.edit_post) % p.id);
	}
	c["permlink"]=str(format(blog->fmt.post) % p.id);
}

void View_Post::ini_full(post_t &p)
{
	Text_Tool tt;
	ini_share(p);
	string abstract;
	tt.markdown2html(p.abstract,abstract);
	c["abstract"]=abstract;
	if(p.content!="") {
		string content;
		tt.markdown2html(p.content,content);
		c["content"]=content;
		c["has_content"]=true;
	}
	else {
		c["has_content"]=false;
	}

	string post_comment=str(format(blog->fmt.add_comment) % p.id);
	int n=post_comment.size()/2;
	string post_comment_url_2=post_comment.substr(n);
	string post_comment_url_1=post_comment.substr(0,n);
	c["post_comment_url_1"]=post_comment_url_1;
	c["post_comment_url_2"]=post_comment_url_2;

	result rs;
	blog->sql<<
		"SELECT id,author,email,url,publish_time,content "
		"FROM comments "
		"WHERE post_id=? "
		"ORDER BY publish_time",p.id;
	blog->sql.fetch(rs);

	row cur;

	content::list_t &comments_list=c.list("comments");

	while(rs.next(cur))
	{
		comments_list.push_back(content());
		View_Comment com(blog,comments_list.back());

		comment_t comment;
		cur	>> comment.id >>comment.author >> comment.email >> comment.url
			>> comment.publish_time >> comment.content ;

		com.init(comment);
	}
}


void View_Post::ini_short(post_t &p)
{
	Text_Tool tt;
	ini_share(p);
	string abstract;
	tt.markdown2html(p.abstract,abstract);
	c["abstract"]=abstract;
	c["has_content"]=(bool)p.has_content;
}


void View_Post::ini_feed(post_t &p)
{
	Text_Tool tt;
	ini_share(p);
	string abstract;
	string abstract_html;
	tt.markdown2html(p.abstract,abstract_html);
	// For xml feed we need convert html to text
	tt.text2html(abstract,abstract_html);

	c["abstract"]=abstract_html;
	c["has_content"]=(bool)p.has_content;
}

void View_Main_Page::ini_share()
{
	int id;
	string val;
	result rs;
	blog->sql<<"SELECT id,value FROM options";
	blog->sql.fetch(rs);
	row i;
	for(;rs.next(i);){
		i >>id>>val;
		if(id==BLOG_TITLE)
			c["blog_name"]=val;
		else if(id==BLOG_DESCRIPTION)
			c["blog_description"]=val;
	}
	c["media"]=blog->fmt.media;
	c["admin_url"]=blog->fmt.admin;
	c["base_url"]=global_config.sval("blog.script_path");
	c["host"]=global_config.sval("blog.host");
	c["rss_posts"]=blog->fmt.feed;
}

void View_Main_Page::ini_main(int id,bool feed)
{
	ini_share();

	int max_posts=feed ? 10 : 5;

	content::vector_t &latest_posts=c.vector("posts",max_posts);

	result rs;
	if(id!=-1) {
		blog->sql<<
			"SELECT posts.id,users.username,posts.title, "
			"	posts.abstract, posts.content !='', "
			"	posts.publish "
			"FROM	posts "
			"LEFT JOIN "
			"	users ON users.id=posts.author_id "
			"WHERE	posts.publish <= (SELECT publish FROM posts WHERE id=?) "
			"	AND posts.is_open=1 "
			"ORDER BY posts.publish DESC "
			"LIMIT ?",
			id,max_posts+1;
	}
	else {
		blog->sql<<
			"SELECT posts.id,users.username,posts.title, "
			"	posts.abstract, posts.content!='', "
			"	posts.publish "
			"FROM	posts "
			"LEFT JOIN "
			"	users ON users.id=posts.author_id "
			"WHERE	posts.is_open=1 "
			"ORDER BY posts.publish DESC "
			"LIMIT ?",(max_posts+1);
	}
	blog->sql.fetch(rs);

	row r;
	int counter,n;
	for(counter=1,n=0;rs.next(r);counter++) {
		if(counter==max_posts+1) {
			int id;
			r>>id;
			c["next_page_link"]=str(format(blog->fmt.main_from) % id);
			break;
		}
		post_t post;

		r>>post.id;
		r>>post.author_name;
		r>>post.title;
		r>>post.abstract;
		r>>post.has_content;
		r>>post.publish;

		View_Post post_v(blog,latest_posts[n]);
		if(feed){
			post_v.ini_feed(post);
		}
		else {
			post_v.ini_short(post);
		}
		n++;
	}
	if(n<max_posts){
		latest_posts.resize(n);
	}
	c["master_content"]=string("main_page");
}

void View_Main_Page::ini_post(int id,bool preview)
{
	View_Post post_v(blog,c);
	post_t post;
	post.id=-1;
	row r;
	blog->sql<<
		"SELECT posts.id,users.username,posts.title, "
		"	posts.abstract, posts.content, "
		"	posts.publish,posts.is_open "
		"FROM	posts "
		"JOIN	users ON users.id=posts.author_id "
		"WHERE	posts.id=? ",
		id;
	if(!blog->sql.single(r)) {
		throw Error(Error::E404);
	}
	r>>	post.id>>post.author_name>>post.title>>
		post.abstract>>post.content>>
		post.publish>>post.is_open;

	if(post.id==-1 || (!post.is_open && !preview)){
		throw Error(Error::E404);
	}

	ini_share();

	post_v.ini_full(post);
	c["master_content"]=string("post");
}

void View_Main_Page::ini_error(int id)
{
	ini_share();
	c["master_content"]=string("error");
	switch(id){
	case Error::E404:
		c["error_404"]=true;
		c["error_comment"]=false;
		break;
	case Error::COMMENT_FIELDS:
		c["error_404"]=false;
		c["error_comment"]=true;
		break;
	}
}

void View_Admin::ini_share()
{
	row r;
	blog->sql<<
		"SELECT value FROM options WHERE id=?",
		(int)BLOG_TITLE;
	string blog_name;
	if(blog->sql.single(r))
		r>>blog_name;
	c["blog_name"]=blog_name;
	c["media"]=blog->fmt.media;
	c["base_url"]=global_config.sval("blog.script_path");
	c["admin_url"]=blog->fmt.admin;
	c["logout_url"]=blog->fmt.logout;
	c["new_post_url"]=blog->fmt.new_post;
}

void View_Admin::ini_login()
{
	ini_share();
	c["master_content"]=string("admin_login");
	c["login_url"]=blog->fmt.login;
}

void View_Admin::ini_edit(int id)
{
	ini_share();
	c["master_content"]=string("admin_editpost");
	View_Admin_Post post(blog,c);
	post.ini(id);
}

void View_Admin::ini_main()
{
	ini_share();
	View_Admin_Main main(blog,c);
	main.ini();
	c["master_content"]=string("admin_main");
}

void View_Admin_Main::ini()
{
	Text_Tool tt;
	result rs;
	blog->sql<<
		"SELECT id,title "
		"FROM posts "
		"WHERE is_open=0";
	blog->sql.fetch(rs);
	row r;

	content::list_t &unpublished_posts=c.list("posts");

	for(;rs.next(r);) {
		int id;
		string intitle;
		r>>id>>intitle;
		string edit_url=str(format(blog->fmt.edit_post) % id);
		string title;
		tt.text2html(intitle,title);

		unpublished_posts.push_back(content());
		unpublished_posts.back()["edit_url"]=edit_url;
		unpublished_posts.back()["title"]=title;
	}
	blog->sql<<
		"SELECT post_id,author "
		"FROM comments "
		"ORDER BY id DESC "
		"LIMIT 10",rs;

	content::list_t &latest_comments=c.list("comments");

	for(;rs.next(r);) {
		int id;
		string author;
		r>>id>>author;
		string author_html;
		tt.text2html(author,author_html);

		latest_comments.push_back(content());
		latest_comments.back()["post_permlink"]=str(format(blog->fmt.post) % id);
		latest_comments.back()["username"]=author_html;
	}
}

void View_Admin_Post::ini( int id)
{
	Text_Tool tt;
	post_t post_data;
	if(id!=-1) {
		c["post_id"]=str(format("%1%") % id);
		c["submit_post_url"]=str(format(blog->fmt.update_post) % id);
		c["preview_url"]=str(format(blog->fmt.preview) % id);
	}
	else {
		c["submit_post_url"]=blog->fmt.add_post;
		return;
	}
	post_data.id=-1;
	row r;
	blog->sql<<
		"SELECT id,title,abstract,content "
		"FROM posts "
		"WHERE id=?",
		id;
	if(!blog->sql.single(r)){
		throw Error(Error::E404);
	}
	r>>	post_data.id>>post_data.title>>
		post_data.abstract>>post_data.content;

	string title;
	tt.text2html(post_data.title,title);
	c["post_title"]=title;
	string abstract;
	tt.text2html(post_data.abstract,abstract);
	c["abstract"]=abstract;
	string content;
	tt.text2html(post_data.content,content);
	c["content"]=content;
}
