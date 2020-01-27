// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Responsible for one client session.
#include <iostream>
#include <boost/bind.hpp>
#include "tyfirc-scmessage.h"
#include "tyfirc-authmanager.h"
#include "tyfirc-session.h"

namespace tyfirc {

namespace server {

void Session::Start() {
	con_state_ = internal::ConnectionState::Connected;
	socket_.async_handshake(boost::asio::ssl::stream_base::server,
			[shared_this = shared_from_this()](const boost::system::error_code& e)
			{ shared_this->ReadLoginScMessage(e); });
}

void Session::ReadLoginScMessage(const boost::system::error_code & error) {
	if (!error) {
		boost::asio::async_read_until(
				socket_,
				boost::asio::dynamic_buffer(read_buffer_),
				'\0',
				[shared_this = shared_from_this()]
				(const boost::system::error_code& e, size_t len)
				{ shared_this->HandleAuthScMessage(e, len); });
	}
	else {
		con_state_ = internal::ConnectionState::NotConnected;
		return;	// "delete this"
	}
}

void Session::HandleAuthScMessage(
	const boost::system::error_code & error, size_t) {
	if (error) { 
		con_state_ = internal::ConnectionState::NotConnected;
		return;	// "delete this"
	}

	ScMessage scmsg = ScMessage::Deserialize(read_buffer_);
	std::string answer;
	std::function<void(const boost::system::error_code&, size_t)> handler;
	bool success = false;
	if (scmsg.GetType() == ScMessageType::LOGIN) {
		success = auth_manager_->Login(scmsg.GetProperty("username"),
			scmsg.GetProperty("password"));
	}
	else if (scmsg.GetType() == ScMessageType::REGISTER) {
		success = auth_manager_->Register(scmsg.GetProperty("username"),
				scmsg.GetProperty("password"));
	}
	else {
		// Try again.
		answer = ScMessageTypeToStr(ScMessageType::FORMAT_FAILURE);
		auto buf = boost::asio::buffer(answer.c_str(), answer.size() + 1);
		boost::asio::async_write(socket_, buf,
				[shared_this = shared_from_this()]
				(const boost::system::error_code& e, size_t len)
				{ shared_this->ReadLoginScMessage(e); });
		return;
	}

	if (success) {
		username_ = scmsg.GetProperty("username");
		answer = ScMessageTypeToStr(
				scmsg.GetType() == ScMessageType::LOGIN ? ScMessageType::LOGIN_SUCCESS
												 : ScMessageType::REGISTER_SUCCESS);
		handler = [shared_this = shared_from_this()]
			(const boost::system::error_code& e, size_t len) { 
			shared_this->SetConnectionState(internal::ConnectionState::LoggedIn);
			shared_this->ReadScMessage(e); 
		};
	}
	else {
		answer = scmsg.GetType() == ScMessageType::LOGIN ?
				ScMessageTypeToStr(ScMessageType::LOGIN_FAILURE)
			: ScMessageTypeToStr(ScMessageType::REGISTER_FAILURE);
		handler = [shared_this = shared_from_this()]
							(const boost::system::error_code& e, size_t len)
							{ shared_this->ReadLoginScMessage(e); };
	}
	
	auto buf = boost::asio::buffer(answer.c_str(), answer.size() + 1);
	boost::asio::async_write(socket_, buf, handler);
}

void Session::ReadScMessage(const boost::system::error_code & error) {
	if (error) {
		con_state_ = internal::ConnectionState::NotConnected;
		return;	// "delete this"
	}

	read_buffer_.clear();
	boost::asio::async_read_until(socket_, 
			boost::asio::dynamic_buffer(read_buffer_),
			'\0',
			[shared_this = shared_from_this()]
			(const boost::system::error_code& e, size_t len)
			{ shared_this->HandleScMessage(e); });
}

void Session::HandleScMessage(const boost::system::error_code & error) {
	if (error) {
		con_state_ = internal::ConnectionState::NotConnected;
		return;	// "delete this"
	}

	ScMessage scmsg = ScMessage::Deserialize(read_buffer_);
	Message msg;
	switch (scmsg.GetType()) {
	case ScMessageType::MESSAGE:
		try {
			msg = Message::Deserialize(scmsg.GetProperty("message[0]"));
		}
		catch (std::invalid_argument) {}
		on_message_(msg);
		break;
	case ScMessageType::END:
		break;
	default:
		break;
	}

	read_buffer_.clear();
	boost::asio::async_read_until(
		socket_,
		boost::asio::dynamic_buffer(read_buffer_),
		'\0',
		[shared_this = shared_from_this()]
	(const boost::system::error_code& e, size_t len)
	{ shared_this->HandleScMessage(e); });
}

void Session::SyncWriteMessage(const Message& msg) {
	// ConnectionFailException?
	std::string scmsgstr = GetMessageScMessage(msg).Serialize();
	boost::asio::write(socket_, 
			boost::asio::buffer(scmsgstr.c_str(), scmsgstr.size() + 1));
}

}  // namespace server

}  // namespace tyfirc