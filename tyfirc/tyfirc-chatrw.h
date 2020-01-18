// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of class that is responsible for connection, reading from and
// writing to server.
#pragma once
#include <vector>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include "tyfirc-scmessage.h"


namespace tyfirc {

namespace client {

// This class is used to establish connection, log in to server and read/write
// ScMessages from/to server. It internaly creates and uses ssl socket.
// After you connected and logged in you can write messages to server. Reading
// is implemented by binding handlers on each arriving message type and then
// executing method Run() that blocks thread and direct incoming messages
// to bound handlers.
//
// Object can only be moved.
class ChatRw {
 public:
	// Ctor requirements same as for ssl_socket (assumes service and ctx outlive 
	// this object). 
	ChatRw(boost::asio::io_service& service, boost::asio::ssl::context& ctx)
			: service_{service}, socket_{ service, ctx } {}

	// Trying to connect synchronously. Returns result of connection try.
	// Returns false if connection is established. Internally catch
	// all exceptions (that's bad).
	bool Connect(boost::asio::ip::address_v4 address, unsigned short port);

	// Trying to login to chat synchronously. Returns true if username-password
	// was registered, false otherwise. 
	// Connection must be established before call (otherwise 
	// ConnectionFailExceptionis thrown). May throw on unsuccessful calls to
	// socket write methods.
	bool Login(std::string username, std::string password);

	// Rules are similar to Login but return true if username-password wasn't
	// registered and false otherwise. Automaticaly login.
	bool Register(std::string username, std::string password);

	// Bind handler with specified ScMessage type. Handler is invoked just after
	// message with this type was read from socket.
	void BindHandler(ScMessageType type, void(*handler)(ScMessage));

	// Bind handler if message from socket is invalid.
	void BindHandlerOnDiscard(void(*handler)(std::string));

	// Starts reading from ChatSocket and invoke handlers after message from 
	// socket is read. If message is invalid discard it.
	// Blocks thread.
	// If any error happens returns(connection lost/not established etc).
	void Run();

	bool is_connected() { return is_connected_; }
	bool is_logged_in() { return is_logged_in_; }

	ChatRw(ChatRw&&) = default;
	ChatRw& operator=(ChatRw&&) = default;
 private:
	bool VerifyCertificate(bool preverified,
			boost::asio::ssl::verify_context& ctx);

	using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
	
	boost::asio::io_service& service_;
	ssl_socket socket_;

	std::array<boost::signals2::signal<void(ScMessage)>, sc_types_size> read_signals_;
	boost::signals2::signal<void(std::string)> discard_signal_;

	bool is_connected_ = false;
	bool is_logged_in_ = false;
};

}	 // namespace client

}  // namespace tyfirc