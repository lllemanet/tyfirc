// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of class that is responsible for connection, reading from and
// writing to server.
#pragma once
#include <vector>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/signals2.hpp>
#include "tyfirc-msgpack.h"
#include "tyfirc-scmessage.h"


namespace tyfirc {

// Synchronously read message from socket.
// If format is not preserved return ScMessage with type END.
// boost::system::system_error thrown on error.
// We assume server use '\0'-terminating symbol to end message.
// Socket must be SyncReadStream.
template <typename SyncReadStream>
ScMessage ReadScMessage(SyncReadStream&);

// Synchronously writes ScMessage to specified stream.
// boost::system::system_error thrown on error.
template <typename SyncReadStream>
void WriteScMessage(SyncReadStream&, const ScMessage&);

namespace client {

// This class is used to establish connection, log in to server and read/write
// ScMessages from/to server. It internaly creates and uses ssl socket.
// After you connected and logged in you can write messages to server. Reading
// is implemented by binding handlers on each arriving message type and then
// executing method StartRead() that blocks thread and direct incoming messages
// to bound handlers.
//
// Object can only be moved.
class ChatRw {
 private:
	enum class ConnectionState {
		NotConnected = 0,
		Connected = 1,
		LoggedIn = 2		// connected and logged in
	};

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
	// ConnectionFailExceptionis thrown). 
	// Throw boost::system::system-error on asio write and ReadScMessage failure.
	bool Login(const std::string& username, const std::string& password);

	// Rules are similar to Login but return true if username-password wasn't
	// registered and false otherwise. Automaticaly login.
	bool Register(const std::string& username, const std::string& password);

	// Synchronously writes message if logged in.
	// There's no guarantee that message will be written. User is acknowledged
	// about successful read if written messages arrives on read.
	// boost::system::system_error thrown on error.
	void WriteMessage(const Message& msg);

	// Bind handler on new msg arrived. Handler is invoked just after
	// message with this type was read from socket.
	boost::signals2::connection BindHandlerOnMessage(void(*handler)(const Message&));

	// Bind handler if message from socket is invalid.
	boost::signals2::connection BindHandlerOnDiscard(
			void(*handler)(const std::string&));

	// Starts reading from socket and invoke corresponding handlers after 
	// message from socket is read. If message is invalid discard it and invoke
	// handler for discarded message.
	// Blocks thread (So better to execute on distinct thread).
	// Throws on errors with connections (connection lost/not established etc)
	// and doesn't catch exceptions in handlers.
	void StartRead();

	// Used to stop reading after StartRead invoked. Stops after last handler
	// completes.
	void StopRead() { is_read_ = false; }

	bool is_connected() { return con_state_ != ConnectionState::NotConnected; }
	bool is_logged_in() { return con_state_ == ConnectionState::LoggedIn; }

	ChatRw(ChatRw&&) = default;
	ChatRw& operator=(ChatRw&&) = default;
 private:
	bool VerifyCertificate(bool preverified,
			boost::asio::ssl::verify_context& ctx);

	// Helper for Login and Register
	bool Auth(ScMessageType type, const std::string& username, 
			const std::string& password);

	using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
	boost::asio::io_service& service_;
	ssl_socket socket_;
	// signals
	boost::signals2::signal<void(const Message&)> message_signal_;
	boost::signals2::signal<void(const std::string&)> discard_signal_;
	// Since there's no way to check if socket is connected in asio we use this
	// state and change it if socket operations cause errors.
	ConnectionState con_state_;
	std::atomic_bool is_read_;	// Switch if started read to finish read.
};

}	 // namespace client

}  // namespace tyfirc