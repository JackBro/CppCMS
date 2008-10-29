/* Nothing meanwhile */
#include "cxxmarkdown/markdowncxx.h"
#include "wiki.h"

namespace data {

login_form::login_form(wiki *_w) :
	w(_w),
	username("username",w->gettext("Username")),
	password("password",w->gettext("Password")),
	login("login",w->gettext("Login"))
{
	*this & username & password & login;
	username.set_nonempty();
	password.set_nonempty();
}

bool login_form::validate()
{
	if(!form::validate())
		return false;
	if(w->users.check_login(username.get(),password.get()))
		return true;
	password.not_valid();
	return false;
}

string master::markdown(string s)
{
	string tmp;
	markdown2html(s,tmp);
	return tmp;
}

page_form::page_form(wiki *_w):
	w(_w),
	title("title",w->gettext("Title")),
	content("content",w->gettext("Content")),
	sidebar("sidebar",w->gettext("Sidebar")),
	save("save",w->gettext("Save")),
	save_cont("save_cont",w->gettext("Save and Continue")),
	preview("preview",w->gettext("Preview")),
	users_only("users_only")
{
	*this & title & content & sidebar & save & save_cont & preview & users_only;
	fields<<title<<content<<sidebar;
	buttons<<save<<save_cont<<preview<<users_only;
	users_only.help=w->gettext("Disable editing by visitors");
	users_only.error_msg=w->gettext("Please Login");
	title.set_nonempty();
	content.set_nonempty();
	content.rows=25;
	content.cols=60;
	sidebar.rows=10;
	sidebar.cols=60;
}

bool page_form::validate()
{
	bool res=form::validate();
	if(users_only.get() && !w->users.auth()) {
		users_only.not_valid();
		users_only.set(false);
		return false;
	}
	return res;
}

options_form::options_form(wiki *_w):
	w(_w),
	users_only("uonly",w->gettext("Users Only")),
	contact_mail("contact",w->gettext("Contact e-mail")),
	wiki_title("wtitle",w->gettext("Wiki Title")),
	about("about",w->gettext("About Wiki")),
	copyright("copy",w->gettext("Copyright String")),
	submit("submit",w->gettext("Submit"))
{
	*this & users_only & contact_mail & wiki_title & copyright & about & submit;
	wiki_title.set_nonempty();
	copyright.set_nonempty();
	contact_mail.set_nonempty();
	about.set_nonempty();
	about.rows=10;
	about.cols=40;
	users_only.help=w->gettext("Disable creation of new articles by visitors");
}

new_user_form::new_user_form(wiki *_w):
	w(_w),
	username("username",w->gettext("Username")),
	password1("p1",w->gettext("Password")),
	password2("p2",w->gettext("Confirm")),
	submit("submit",w->gettext("Submit"))
{
	*this & username & password1 & password2 ;
	username.set_nonempty();
	password1.set_nonempty();
	password2.set_equal(password1);

	vector<string> const &quiz=w->app.config.slist("wikipp.quiz_q");
	int i=1;
	for(vector<string>::const_iterator p=quiz.begin(),e=quiz.end();p!=e;++p) {
		this->quiz.push_back(widgets::checkbox((boost::format("%d") % i).str()));
		this->quiz.back().help=*p; 
		*this & this->quiz.back();
		i++;
	}
	*this & submit;
}

bool new_user_form::validate()
{
	if(!form::validate())
		return false;
	vector<long> const &quiz=w->app.config.llist("wikipp.quiz_a");
	list<widgets::checkbox>::iterator qp=this->quiz.begin();
	for(vector<long>::const_iterator p=quiz.begin(),e=quiz.end();p!=e;++p) {
		if(qp==this->quiz.end() || qp->get()!=*p)
			return false;
		++qp;
	}
	if(w->users.user_exists(username.get())) {
		username.error_msg=w->gettext("This user exists");
		username.not_valid();
		return false;
	}
	return true;
}

} // namespac data

