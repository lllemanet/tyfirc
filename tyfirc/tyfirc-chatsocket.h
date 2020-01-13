// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of ChatSocket.
#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace {
using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
}	 // namespace

namespace tyfirc {

// Behaves like simple ssl socket that require connection+logging in for
// read/write.
class ChatSocket {
public:
	// Ctor requirements same as for ssl_socket
	ChatSocket(boost::asio::io_service& service,
						 boost::asio::ssl::context& ctx)
		: socket_{ service, ctx }, is_connected_{ false } {}

	// Trying to connect. 
	bool Connect(boost::asio::ip::address_v4 address, unsigned short port);

	bool Login(std::string username, std::string password);

	bool Register(std::string username, std::string password);

	bool is_connected() { return is_connected_; }
private:
	bool VerifyCertificate(bool preverified,
			boost::asio::ssl::verify_context& ctx);


	ssl_socket socket_;
	bool is_connected_;
};

}  // namespace tyfirc