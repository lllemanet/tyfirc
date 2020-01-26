// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Responsible for one client session.
#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-authmanager.h"
#include "tyfirc-misc.h"

namespace {
using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
} // namespace

namespace tyfirc {

namespace server {

// Because Session can be shared and we need to preserve
// pointer to this in async calls (so handlers won't meet dangling pointer),
// we use std::enable_shared_from_this mechanism.
class Session : public std::enable_shared_from_this<Session> {
 public:
	Session(boost::asio::io_service& service, boost::asio::ssl::context& ctx,
					std::shared_ptr<IAuthManager> auth_manager)
		: socket_{ service, ctx }, auth_manager_{ auth_manager } {}

	//std::shared_ptr<Session> get_sptr() { return shared_from_this(); }

	void Start();

	void HandleHandshake(const boost::system::error_code& error);
	void HandleScMessageRead(const boost::system::error_code& error);

	ssl_socket::lowest_layer_type& socket() { return socket_.lowest_layer(); }
 private:
	// private so object cannot be allocated on stack
	/*Session(boost::asio::io_service& service, boost::asio::ssl::context& ctx)
		 : socket_{ service, ctx } {}*/

	ssl_socket socket_;
	std::string read_buffer_;
	std::shared_ptr<IAuthManager> auth_manager_;
	internal::ConnectionState con_state_ = internal::ConnectionState::NotConnected;
};

}  // namespace server

}  // namespace tyfirc