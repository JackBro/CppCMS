#ifndef CPPCMS_SERVICE_IMPL_H
#define CPPCMS_SERVICE_IMPL_H

#include "asio_config.h"
#include "json.h"
#include <memory>

namespace cppcms {
class service;
class applications_pool;
class thread_pool;

namespace impl {
	namespace cgi {
		class acceptor;
	}

	class service : public util::noncopyable {
	public:
		service();
		~service();
		boost::asio::io_service &get_io_service()
		{
			return *io_service_;
		}


	private:
		friend class cppcms::service;
		std::auto_ptr<boost::asio::io_service> io_service_;

		std::auto_ptr<cgi::acceptor> acceptor_;
		std::auto_ptr<json::value> settings_;
		std::auto_ptr<applications_pool> applications_pool_;
		std::auto_ptr<thread_pool> thread_pool_;
		std::auto_ptr<locale::pool> locale_pool_;

#ifdef CPPCMS_WIN32
		typedef SOCKET native_socket_type;
		typedef boost::asio::ip::tcp::socket loopback_socket_type;
#else
		typedef int native_socket_type;
		typedef boost::asio::local::stream_protocol::socket loopback_socket_type;
#endif
		native_socket_type notification_socket_;
		std::auto_ptr<loopback_socket_type> sig_,breaker_;


	};


}
} // cppcms



#endif
