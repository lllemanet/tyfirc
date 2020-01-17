// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of ChatSocket.
#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/buffer.hpp>

namespace {
using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
}	 // namespace

namespace tyfirc {

// Behaves like simple ssl socket that require connection+authentication for
// read/write.
class ChatSocket {
 public:
	// Ctor requirements same as for ssl_socket
	ChatSocket(boost::asio::io_service& service,
						 boost::asio::ssl::context& ctx)
		: socket_{ service, ctx } {}

	// Trying to connect synchronously. Returns result of connection try.
	bool Connect(boost::asio::ip::address_v4 address, unsigned short port);

	// Tries to write synchronously. Throw ConnectionFailException if not 
	// connected. Returns written length.
	size_t Write(const boost::asio::const_buffer& buff);
	//TODO WriteAsync

	//TODO Read
	void AsyncRead(boost::asio::mutable_buffer buf,
			void (*handler)(boost::system::error_code, std::size_t));



	void SetLoggedIn(bool value) { is_logged_in_ = value; }
	bool is_connected() { return is_connected_; }
	bool is_logged_in() { return is_logged_in_; }
 private:
	bool VerifyCertificate(bool preverified,
			boost::asio::ssl::verify_context& ctx);

	ssl_socket socket_;
	bool is_connected_ = false;
	bool is_logged_in_ = false;
};

}  // namespace tyfirc