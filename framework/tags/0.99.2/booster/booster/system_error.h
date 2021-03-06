//
//  Copyright (c) 2010 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOSTER_SYSTEM_ERROR_H
#define BOOSTER_SYSTEM_ERROR_H

#include <string>
#include <stdexcept>
#include <functional>

#include <booster/config.h>

namespace booster {

///
/// \brief this namespace includes partial implementation of std::tr1's/boost's system_error, error_code
/// classes 
///
namespace system {
	class error_category {
	public:
		virtual ~error_category()
		{
		}
		virtual char const *name() const = 0;
		virtual std::string message(int ev) const = 0;
		bool operator==(error_category const &other) const
		{
			return this==&other;
		}
		bool operator!=(error_category const &other) const
		{
			return this!=&other;
		}
		bool operator<(error_category const &other) const
		{
			return std::less<error_category const *>()(this,&other);
		}

	};

	BOOSTER_API error_category const &get_system_category();
	static const error_category &system_category = get_system_category();
	
	#ifdef BOOSTER_WIN32
	BOOSTER_API error_category const &get_windows_category();
	static const error_category &windows_category = get_system_category();
	#endif

	#ifdef BOOSTER_POSIX
	BOOSTER_API error_category const &get_posix_category();
	static const error_category &posix_category = get_system_category();
	#endif


	class error_code {
	public:
		error_code() : 
			value_(0),
			category_(&system_category)
		{
		}
		error_code(int val,error_category const &cat) :
			value_(val),
			category_(&cat)
		{
		}
		int value() const
		{
			return value_;
		}
		const error_category &category() const
		{
			return *category_;
		}
		std::string message() const
		{
			return std::string(category_->name()) + ": " + category_->message(value_);
		}
		operator bool () const
		{
			return value() != 0;
		}
	private:
		int value_;
		error_category const *category_;
	};

	inline bool operator==(error_code const &left,error_code const &right)
	{
		return left.value() == right.value() && left.category() == right.category(); 
	}
	inline bool operator!=(error_code const &left,error_code const &right)
	{
		return !(left==right);
	}

	class system_error : public std::runtime_error {
	public:
		system_error(error_code const &e) :
			std::runtime_error(e.message()),
			error_(e)
		{
		}
		system_error(error_code const &e,std::string const &message) :
			std::runtime_error(e.message()+": " + message),
			error_(e)
		{
		}
		error_code const &code() const
		{
			return error_;
		}
	private:
		error_code error_;
	};

} // system
} // booster


#endif
