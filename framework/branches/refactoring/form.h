#ifndef CPPCMS_FORM_H
#define CPPCMS_FORM_H

#include "defs.h"
#include "noncopyable.h"

#include <string>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <stack>
#include <ostream>
#include <sstream>
#include "http_context.h"
#include "http_request.h"
#include "http_response.h"
#include "copy_ptr.h"
#include "cppcms_error.h"
#include "util.h"
#include "regex.h"
#include "localization.h"

namespace cppcms {

	namespace widgets {
		class base_widget;
	}

	struct form_flags {
		typedef enum {
			as_html = 0,	///< render form/widget as ordinary HTML
			as_xhtml= 1,	///< render form/widget as XHTML
		} html_type;

		typedef enum {
			as_p	= 0 , ///< Render each widget using paragraphs
			as_table= 1 , ///< Render each widget using table
			as_ul 	= 2 , ///< Render each widget using unordered list
			as_dl	= 3 , ///< Render each widget using definitions list
			as_space= 4   ///< Render each widget using simple blank space separators

			// to be extended
		} html_list_type;

		typedef enum {
			first_part  = 0, ///< Render part 1; HTML attributes can be inserted after it
			second_part = 1  ///< Render part 2 -- compete part 1.
		} widget_part_type;
	};

	class form_context : public form_flags
	{
	public:
		form_context();
		form_context(form_context const &other);
		form_context const &operator = (form_context const &other);
		form_context(	std::ostream &output,
				html_type ht = form_flags::as_html,
				html_list_type hlt=form_flags::as_p);
		~form_context();
		
		void html(html_type t);
		void html_list(html_list_type t);
		void widget_part(widget_part_type t);
		void out(std::ostream &out);
		
		html_type html() const;
		html_list_type html_list() const;
		widget_part_type widget_part() const;
		std::ostream &out() const;

	private:
		uint32_t html_type_;
		uint32_t html_list_type_;
		uint32_t widget_part_type_;
		std::ostream *output_;
		uint32_t reserved_1;
		uint32_t reserved_2;
		struct data;
		util::hold_ptr<data> d;

	};

	
	///
	/// \brief This class is base class of any form or form-widget used in CppCMS.
	///
	/// It provides abstract basic operations that every widget or form should implement
	///

	class CPPCMS_API base_form : public form_flags {
	public:

		///
		/// Render the widget to std::ostream \a output with control flags \a flags.
		/// Usually this function is called directly by template rendering functions
		///

		virtual void render(form_context &context) = 0;


		///
		/// Load the information of form from the provided http::context \a context.
		/// User calls this function for loading all information from the raw POST/GET
		/// form to internal widget representation.
		///

		virtual void load(http::context &context) = 0;

		///
		/// Validate the form according to defined rules. If all checks are OK
		/// true is returned. If some widget or form fails, false is returned.
		///

		virtual bool validate() = 0;

		///
		/// Clear the form from all user provided data.
		///
		virtual void clear() = 0;

		///
		/// Set parent of this form. Used internaly, should not be used
		///
		
		virtual void parent(base_form *subform) = 0;

		///
		/// Get parent of this form. If this is topmost form, NULL is returned
		///
		virtual base_form *parent() = 0;

		base_form();
		virtual ~base_form();
	};

	///
	/// \brief The \a form is a container that used to collect other widgets and forms to single unit
	///
	/// Generally various widgets and forms are combined into single form in order to simplify rendering
	/// and validation of forms that include more then one widget
	///

	class CPPCMS_API form :	public util::noncopyable,
				public base_form
	{
	public:


		form();
		virtual ~form();


		///
		/// Render all widgets and sub-forms to \a output, using
		/// base_form \a flags
		///

		virtual void render(form_context &context);


		///
		/// Load all information from widgets from http::context \a cont
		///
		virtual void load(http::context &cont);

		///
		/// validate all subforms and widgets. If at least one of them fails,
		/// false is returned. Otherwise, true is returned.
		///

		virtual bool validate();

		///
		/// Clear all subforms and widgets for all loaded data.
		///
		virtual void clear();

		///
		/// adds \a subform to form, the ownership is not transferred to
		/// to the parent
		///

		void add(form &subform);

		///
		/// add \a subform to form, the ownership is transferred to
		/// the parent and subform will be destroyed together with
		/// the parent
		///

		void attach(form *subform);

		///
		/// adds \a widget to form, the ownership is not transferred to
		/// to the parent
		///

		void add(widgets::base_widget &widget);

		///
		/// add \a widget to form, the ownership is transferred to
		/// the parent the widget will be destroyed together with
		/// the parent form
		///

		void attach(widgets::base_widget *widget);

		///
		/// Shortcut to \a add
		///
		inline form &operator + (form &f)
		{
			add(f);
			return *this;
		}
		
		///
		/// Shortcut to \a add
		///
		inline form &operator + (widgets::base_widget &f)
		{
			add(f);
			return *this;
		}
		///
		/// Set parent of this form. Used internaly, should not be used. It is called
		/// when the form is added or attached to other form.
		///
		
		virtual void parent(base_form *subform);

		///
		/// Get parent of this form. If this is topmost form, NULL is returned
		/// It is assumed that the parent is always form.
		///
		virtual form *parent();
		
		///
		/// \brief Input iterator that is used to iterate over all widgets of the form
		///
		/// This class is mainly used by templates framework for widgets rendering. It
		/// walks on all widgets and subforms recursively.
		///
		/// Note: it walks over widgets only:
		///
		/// \code
		/// iterator p=f.begin();
		/// if(p!=f.end())
		///   if p!=f.end() --> *p is derived from widgets::base_widget.
		/// \endcode
		///

		class CPPCMS_API iterator : public std::iterator<std::input_iterator_tag,widgets::base_widget>
		{
		public:
			///
			/// End iterator
			///

			iterator();

			///
			/// Create widgets iterator
			///
			iterator(form &);

			~iterator();
			iterator(iterator const &other);
			iterator const &operator = (iterator const &other);

			widgets::base_widget *operator->() const
			{
				return get();
			}
			widgets::base_widget &operator*() const
			{
				return *get();
			}

			bool operator==(iterator const &other) const
			{
				return equal(other);
			}
			bool operator!=(iterator const &other) const
			{
				return !equal(other);
			}

			iterator operator++(int unused)
			{
				iterator tmp(*this);
				next();
				return tmp;
			}
			iterator &operator++()
			{
				next();
				return *this;
			}

		private:

			friend class form;

			bool equal(iterator const &other) const;
			void zero();
			void next();
			widgets::base_widget *get() const;

			std::stack<unsigned> return_positions_;
			form *current_;
			unsigned offset_;
			struct data;
			util::copy_ptr<data> d;

		};

		///
		/// Returns an iterator to the first widget.
		///
		iterator begin();

		///
		/// Returns the end-iterator for walking over all widgets.
		///
		iterator end();


	private:
		friend class iterator;

		struct data;
		// Widget and ownership true mine
		typedef std::pair<base_form *,bool> widget_type;
		std::vector<widget_type> elements_;
		form *parent_;
		util::hold_ptr<data> d;
	};



	namespace widgets {

		///
		/// \brief this class is the base class of all renderable widgets that can be
		/// used together with forms
		///
		/// All cppcms widgets are derived from this class. User, who want to create 
		/// its own custom widgets must derive them from this class
		///

		class CPPCMS_API base_widget : 	
			public base_form,
			public util::noncopyable
		{
		public:

			///
			/// Default constructor
			///
			base_widget();

			virtual ~base_widget();
			
			/// 
			/// Check if a value was assigned to widget. Usually becomes true
			/// when user assignees value to widget or the widget is loaded.
			/// 
			bool set();

			///
			/// After executing validation, each widget can be tested for validity
			///
			bool valid();

			///
			/// Get html id attribute
			///
			std::string id();

			///
			/// Get html name attribute
			/// 
			std::string name();

			///
			/// Get short message that would be displayed near the widget
			/// 
			locale::message message();

			///
			/// Check if message is set
			///
			bool has_message();

			///
			/// Get associated error message that would be displayed near the widget
			/// if widget validation failed.
			/// 
			locale::message error_message();

			///
			/// Check if error message is set
			///

			bool has_error_message();

			///
			/// Get long description for specific widget
			///

			locale::message help();

			///
			/// Check if help message is set
			///
			
			bool has_help();

			///
			/// Get disabled html attribute
			///

			bool disabled();

			///
			/// Set/Unset disabled html attribute
			///

			void disabled(bool);

			///
			/// Get the general user defined attributes string that can be added to widget
			/// 
			std::string attributes_string();

			///
			/// Set the existence of content for widget. By default the widget is not set.
			/// Any value fetch from "unset" widget by convention should throw an exception
			/// Calling set with true -- changes state to "set" and with false to "unset"
			///
			
			void set(bool);

			///
			/// Set validity state of widget. By default the widget is valid. When it
			/// passes validation its validity state is changed by calling this function
			///
			/// Note: widget maybe not-set but still valid and it may be set but not-valid
			///

			void valid(bool);

			///
			/// Set html id attribute of the widget
			///
			void id(std::string);

			///
			/// Set html name attribute of the widget. Note: if this attribute
			/// is not set, the widget would not be able to be loaded from POST/GET
			/// data.
			///
			
			void name(std::string);

			///
			/// Set short description for the widget. Generally it is good idea to
			/// define this value.
			///
			/// Short message can be also set using base_widget constructor
			///
			void message(std::string);

			///
			/// Set short translatable description for the widget. Generally it is good idea to
			/// define this value.
			///
			/// Short message can be also set using base_widget constructor
			///
			void message(locale::message const &);

			///
			/// Set error message that is displayed for invalid widgets.
			///
			/// If it is not set, simple "*" is shown
			///
			void error_message(std::string);

			///
			/// Set translatable error message that is displayed for invalid widgets.
			///
			/// If it is not set, simple "*" is shown
			///
			void error_message(locale::message const &);

			///
			/// Set longer help message that describes this widget
			///
			void help(std::string);
			
			///
			/// Set translatable help message that describes this widget
			///
			void help(locale::message const &msg);

			///
			/// Set general html attributes that are not supported
			/// directly. For example:
			///
			/// \code
			///  my_widget.attributes_string("style='direction:rtl' onclick='return foo()'");
			/// \endcode
			///
			/// This string is inserted as-is just behind render_input_start
			/// 
			void attributes_string(std::string v);


			///
			/// Render full widget with error messages and decorations as paragraphs
			/// or table elements to \a output
			///

			virtual void render(form_context &context);

			///
			/// This is a virtual member function that should be implemented by each widget
			/// It executes actual rendering of the input HTML form 
			///
			virtual void render_input(form_context &context) = 0;

			///
			/// Clean the form. Calls set(false) as well
			///
			virtual void clear();

			///
			/// Validate form. If not overridden it sets widget to valid
			///
			virtual bool validate();

			///
			/// Render standard common attributes like id, name, disabled etc.
			///

			virtual void render_attributes(form_context &context);
			
			///
			/// Set parent of this widget. Used internaly, should not be used. It is called
			/// when the form is added or attached to other form.
			///
			
			virtual void parent(base_form *subform);

			///
			/// Get parent of this form. If this is topmost form, NULL is returned
			/// Note widget is assumed to be assigned to forms only
			///
			virtual form *parent();

		protected:
			///
			/// This function should be called by overloaded load/render methods
			/// before rendering/loading starts
			///
			void auto_generate(form_context *context = 0);
		private:

			void generate(int position,form_context *context = 0);

			std::string id_;
			std::string name_;
			locale::message message_;
			locale::message error_message_;
			locale::message help_;
			std::string attr_;
			form *parent_;

			uint32_t is_valid_  : 1;
			uint32_t is_set_ : 1;
			uint32_t is_disabled_ : 1;
			uint32_t is_generation_done_ : 1;
			uint32_t has_message_ : 1;
			uint32_t has_error_ : 1;
			uint32_t has_help_ : 1;
			uint32_t reserverd_ : 25;

			struct data;
			util::hold_ptr<data> d;
		};

		///
		/// \brief this is the widget that is used as base for text input field representation
		///
		/// This widget is used as base class for other widgets that are used for
		/// text input like: text, textarea, etc.
		///
		/// This widget does much more then reading simple filed data from the POST
		/// or GET form, it performs charset validation and if required conversion
		/// to and from Unicode charset to locale charset.
		///


		class CPPCMS_API base_text : virtual public base_widget {
		public:
			
			base_text();
			virtual ~base_text();

			///
			/// Get the string that contains input value of the widget.
			///
			
			
			std::string value();
			
			///
			/// Set the widget content before rendering, the value \a v 
			///
			
			void value(std::string v);
			
			///
			/// Acknowledge the validator that this text widget should contain some text.
			/// similar to limits(1,-1)
			///
			void non_empty();
			///
			/// Set minimum and maximum limits for text size. Note max == -1 indicates that there
			/// is no maximal limit, min==0 indicates that there is no minimal limit.
			///
			/// Note: these numbers represent the length in Unicode code points (even if the encoding
			/// is not Unicode). If character set validation is disabled, then these number represent
			/// the number of octets in the string.
			///
			void limits(int min,int max);

			///
			/// Get minimal and maximal size limits, 
			///
			
			std::pair<int,int> limits();

			///
			/// Acknowledge the validator if it should not check the validity of the charset.
			/// Default -- enabled
			///
			/// Generally you should not use this option unless you want to load some raw data as
			/// form input, or the character set is different from the defined in locale.
			///
			void validate_charset(bool );
			
			///
			/// Returns true if charset validation is enabled.
			///

			bool validate_charset();

			///
			/// Validate the widget content according to rules and charset encoding.
			///
			/// Notes:
			/// 
			/// -  The charset validation is very efficient for variable length UTF-8 encoding, 
			///    and most popular fixed length ISO-8859-*, windows-125* and koi8* encodings, for other
			///    encodings iconv conversion is used for actual validation.
			/// -  Special characters (that not allowed in HTML) are assumed as forbidden, even if they are
			///    valid code points (like NUL = 0 or DEL=127).
			/// 
			virtual bool validate();

			///
			/// Load the widget for http::context. It used the locale given in the context for
			/// validation of text.
			///
			virtual void load(http::context &);
		private:
			std::string value_;
			int low_;
			int high_;
			bool validate_charset_;
			size_t code_points_;
			struct data;
			util::hold_ptr<data> d;
		};

		class base_html_input : virtual public base_widget {
		public:
			base_html_input(std::string const &type);
			virtual ~base_html_input();
			virtual void render_input(form_context &context);

		protected:
			virtual void render_value(form_context &context) = 0;
		private:
			struct data;
			util::hold_ptr<data> d;
			std::string type_;
		};

		///
		/// \brief This class represents html input of type text
		/// 
		
		class CPPCMS_API text : public base_html_input, public base_text
		{
		public:
			///
			/// Create text field widget
			///
			text();
			
			///
			/// This constructor is provided for use by derived classes where it is required
			/// to change the type of widget, like text, password, etc. 
			///
			text(std::string const &type);

			~text();

			///
			/// Set html attribute size of the widget
			///

			void size(int n);

			///
			/// Get html attribute size of the widget, -1 undefined
			///

			int size();


			virtual void render_attributes(form_context &context);
			virtual void render_value(form_context &context);
		private:
			int size_;
			struct data;
			util::hold_ptr<data> d;
		};

		class CPPCMS_API textarea : public base_text 
		{
		public:
			textarea();
			~textarea();

			///
			/// Get number of rows in textarea -- default -1 -- undefined
			///
			int rows();
			///
			/// Get number of columns in textarea -- default -1 -- undefined
			///
			int cols();

			///
			/// Set number of rows in textarea
			///
			void rows(int n);
			///
			/// Set number of columns in textarea
			///
			void cols(int n);

			virtual void render_input(form_context &context);
		private:
			int rows_,cols_;

			struct data;
			util::hold_ptr<data> d;
		};


		///
		/// \brief Widget for number input. It is template class that assumes that T is number
		///

		template<typename T>
		class numeric: public base_html_input {
		public:
			numeric() :
				base_html_input("text"),
				check_low_(false),
				check_high_(false),
				non_empty_(false)
			{
			}

			///
			/// Defines that this widget should have some value
			///
			void non_empty()
			{
				non_empty_=true;
			}


			///
			/// Get loaded widget value
			///
			T value()
			{
				if(!set())
					throw cppcms_error("Value not loaded");
				return value_;
			}

			///
			/// Set widget value
			///
			void value(T v)
			{
				set(true);
				value_=v;
			}

			/// 
			/// Set minimal input number value
			///
			void low(T a)
			{
				min_=a;
				check_low_=true;
				non_empty();
			}

			/// 
			/// Set maximal input number value
			///

			void high(T b)
			{
				max_=b;
				check_high_=true;
				non_empty();
			}

			///
			/// Same as low(a); high(b);
			///			
			void range(T a,T b)
			{
				low(a);
				high(b);
			}

			///
			/// Render first part of widget
			///

			virtual void render_value(form_context &context)
			{
				if(set())
					context.out()<<"value=\""<<value_<<"\" ";
				else
					context.out()<<"value=\""<<util::escape(loaded_string_)<<"\" ";
			}

			virtual void clear()
			{
				base_html_input::clear();
				loaded_string_.clear();
			}

			///
			/// Load widget data
			///
			
			virtual void load(http::context &context)
			{
				auto_generate();

				loaded_string_.clear();

				set(false);
				valid(true);

				http::request::form_type::const_iterator p;
				http::request::form_type const &request=context.request().post_or_get();
				p=request.find(name());
				if(p==request.end()) {
					return;
				}
				else {
					loaded_string_=p->second;
					if(loaded_string_.empty())
						return;

					std::istringstream ss(loaded_string_);
					ss.imbue(context.locale());
					ss>>value_;
					if(ss.fail() || !ss.eof())
						valid(false);
					else
						set(true);
				}
			}
			
			///
			/// Validate widget
			///
			virtual bool validate()
			{
				if(!valid())
					return false;
				if(!set()) {
					if(non_empty_) {
						valid(false);
						return false;
					}
					return true;
				}
				if(check_low_ && value_ <min_) {
					valid(false);
					return false;
				}
				if(check_high_ && value_ > max_) {
					valid(false);
					return false;
				}
				return true;
			}

		private:

			T min_,max_,value_;

			bool check_low_;
			bool check_high_;
			bool non_empty_;
			std::string loaded_string_;
		};

		/// 
		/// \brief The password widget is a simple text widget with some different
		//  
		
		class CPPCMS_API password: public text {
		public:
			password();

			~password();

			///
			/// Set equality constraint to password widget -- this password should be
			/// equal to other one \a p2. Usefull for creation of new passwords -- if passwords
			/// are not equal, validation would fail
			///
			void check_equal(password &p2);
			virtual bool validate();
		private:
			struct data;
			util::hold_ptr<data> d;
			password *password_to_check_;
		};

		
		///
		/// \brief This class is extinction of text widget that validates it using additional regular expression
		///

		class CPPCMS_API regex_field : public text {
		public:
			regex_field();
			///
			/// Create widget using regular expression \a e
			///
			regex_field(util::regex const &e);

			///
			/// Set regular expression
			///
			void regex(util::regex const &e);

			~regex_field();
			
			virtual bool validate();
		private:
			util::regex const *expression_;
			struct data;
			util::hold_ptr<data> d;
		};

		///
		/// \brief widget that checks that input is valid e-mail
		///
		
		class CPPCMS_API email : public regex_field {
		public:

			email();
			~email();

		private:
			static util::regex const email_expression_;
			struct data;
			util::hold_ptr<data> d;
		};

		class checkbox: public base_html_input {
		public:
			
			checkbox();
			~checkbox();
			

			bool value();
			void value(bool is_set);

			virtual void render_value(form_context &context);
			virtual void load(http::context &context);
		};
/*
		class select_multiple : public base_widget {
			int min;
		public:
			int size;
			select_multiple(string name="",int s=0,string msg="") : base_widget(name,msg),min(-1),size(s) {};
			set<string> chosen;
			map<string,string> available;
			void add(string val,string opt,bool selected=false);
			void add(int val,string opt,bool selected=false);
			void add(string v,bool s=false) { add(v,v,s); }
			void add(int v,bool s=false) {
				ostringstream ss;
				ss<<v;
				add(v,ss.str(),s);
			}
			set<string> &get() { return chosen; };
			set<int> geti();
			void set_min(int n) { min=n; };
			virtual string render_input(int how);
			virtual bool validate();
			virtual void load(cgicc::Cgicc const &cgi);
			virtual void clear();
		};

		class select_base : public base_widget {
		public:
			string value;
			struct option {
				string value;
				string option;
			};
			list<option> select_list;
			select_base(string name="",string msg="") : base_widget(name,msg){};
			void add(string value,string option);
			void add(string v) { add(v,v); }
			void add(int value,string option);
			void add(int v) {
				ostringstream ss;
				ss<<v;
				add(v,ss.str());
			}
			void set(string value);
			void set(int value);
			string get();
			int geti();
			virtual bool validate();
			virtual void load(cgicc::Cgicc const &cgi);
		};

		class select : public select_base {
			int size;
		public:
			select(string n="",string m="") : select_base(n,m),size(-1) {};
			void set_size(int n) { size=n;}
			virtual string render_input(int how);
		};

		class radio : public select_base {
			bool add_br;
		public:
			radio(string name="",string msg="") : select_base(name,msg),add_br(false) {}
			void set_vertical() { add_br=true; }
			virtual string render_input(int how);
		};

		class hidden : public text {
		public:
			hidden(string n="",string msg="") : text(n,msg) { set_nonempty(); };
			virtual string render(int how);
		};

		class submit : public base_widget {
		public:
			string value;
			bool pressed;
			submit(string name="",string button="",string msg="") : base_widget(name,msg), value(button),pressed(false) {};
			virtual string render_input(int);
			virtual void load(cgicc::Cgicc const &cgi);
		};  */

	} // widgets


} //cppcms

#endif // CPPCMS_FORM_H
