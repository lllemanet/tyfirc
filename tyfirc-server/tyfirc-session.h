// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Responsible for one client session.
#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <deque>
#include <boost/signals2.hpp>
#include "tyfirc-authmanager.h"
#include "tyfirc-chatroom.h"
#include "tyfirc-scmessage.h"
#include "tyfirc-msgpack.h"
#include "tyfirc-misc.h"

namespace {
using ssl_socket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
} // namespace

namespace tyfirc {

namespace server {
class ChatRoom;
// Because Session can be shared and we need to preserve
// pointer to this in async calls (so handlers won't meet dangling pointer),
// we use std::enable_shared_from_this mechanism.
class Session : public std::enable_shared_from_this<Session> {
public:
	// We assume service, ctx and chatroom outlives this object.
	Session(boost::asio::io_service& service, boost::asio::ssl::context& ctx,
		ChatRoom& chat_room, std::shared_ptr<IAuthManager> auth_manager)
		: socket_{ service, ctx }, chat_room_{ chat_room },
		auth_manager_{ auth_manager } {}

	// Start of session. Execute async handshake.
	void Start();

	// To be deleted and replaced by WriteMessage
	void SyncWriteMessage(const Message&);
	// Put message on queue and write if needed.
	void WriteScMessage(const ScMessage&);

	boost::signals2::connection	BindOnMessage(
		std::function< void(const Message&)> f) {
		return on_message_.connect(f);
	}

	void SetConnectionState(internal::ConnectionState s) { con_state_ = s; }
	internal::ConnectionState connection_state() { return con_state_; }
	bool is_connected() {
		return con_state_ != internal::ConnectionState::NotConnected;
	}
	bool is_logged_in() { return con_state_ == internal::ConnectionState::LoggedIn; }
	std::string username() { return username_; }

	ssl_socket::lowest_layer_type& socket() { return socket_.lowest_layer(); }
private:
	// After successful handshake we are reading login info.
	void ReadLoginScMessage(const boost::system::error_code& error);
	// Expects read_buffer_ contains string scmessage with auth info.
	// ReadLoginScMessage again on failure and ReadScMessage on success.
	void HandleAuthScMessage(const boost::system::error_code& error, size_t);
	void ReadScMessage(const boost::system::error_code& error);
	void HandleScMessage(const boost::system::error_code& error);

	void WriteScMessageHelper();

	ssl_socket socket_;
	std::string read_buffer_;
	ChatRoom& chat_room_;
	std::shared_ptr<IAuthManager> auth_manager_;
	// Must be set to LoggedIn ONLY after success message sent to client. 
	// If state is LoggedIn, session guarantees not to write to socket.
	internal::ConnectionState con_state_ = internal::ConnectionState::NotConnected;
	std::string username_;

	boost::signals2::signal<void(const Message&)> on_message_;
	std::deque<ScMessage>	msg_queue_;
	bool is_writing_;
};

}  // namespace server

}  // namespace tyfirc