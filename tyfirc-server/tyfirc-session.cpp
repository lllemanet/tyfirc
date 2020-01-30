// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Responsible for one client session.
#include <iostream>
#include <chrono>
#include <boost/bind.hpp>
#include "tyfirc-scmessage.h"
#include "tyfirc-authmanager.h"
#include "tyfirc-chatroom.h"
#include "tyfirc-session.h"

namespace tyfirc {

namespace server {

void Session::Start() {
	con_state_ = internal::ConnectionState::Connected;
	auto self = shared_from_this();
	socket_.async_handshake(boost::asio::ssl::stream_base::server,
		[this, self](const boost::system::error_code& e)
	{ ReadLoginScMessage(e); });
}

void Session::WriteScMessage(const ScMessage& scmsg) {
	bool is_writing = !msg_queue_.empty();
	msg_queue_.push_back(scmsg);
	if (!is_writing) {
		WriteScMessageHelper();
	}
}

void Session::ReadLoginScMessage(const boost::system::error_code & error) {
	if (!error) {
		auto self = shared_from_this();
		boost::asio::async_read_until(
			socket_,
			boost::asio::dynamic_buffer(read_buffer_),
			'\0',
			[this, self]
		(const boost::system::error_code& e, size_t len)
		{ HandleAuthScMessage(e, len); });
	}
	else {
		con_state_ = internal::ConnectionState::NotConnected;
		chat_room_.RemoveSession(shared_from_this());
	}
}

void Session::HandleAuthScMessage(
	const boost::system::error_code & error, size_t) {
	if (error) {
		con_state_ = internal::ConnectionState::NotConnected;
		chat_room_.RemoveSession(shared_from_this());
		return;
	}

	auto self = shared_from_this();
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
			[this, self]
		(const boost::system::error_code& e, size_t len)
		{ ReadLoginScMessage(e); });
		return;
	}

	if (success) {
		username_ = scmsg.GetProperty("username");
		answer = ScMessageTypeToStr(
			scmsg.GetType() == ScMessageType::LOGIN ? ScMessageType::LOGIN_SUCCESS
			: ScMessageType::REGISTER_SUCCESS);
		handler = [this, self]
		(const boost::system::error_code& e, size_t len) {
			SetConnectionState(internal::ConnectionState::LoggedIn);
			ReadScMessage(e);
		};
	}
	else {
		answer = scmsg.GetType() == ScMessageType::LOGIN ?
			ScMessageTypeToStr(ScMessageType::LOGIN_FAILURE)
			: ScMessageTypeToStr(ScMessageType::REGISTER_FAILURE);
		handler = [this, self]
		(const boost::system::error_code& e, size_t len)
		{ ReadLoginScMessage(e); };
	}

	auto buf = boost::asio::buffer(answer.c_str(), answer.size() + 1);
	boost::asio::async_write(socket_, buf, handler);
}

void Session::ReadScMessage(const boost::system::error_code & error) {
	if (error) {
		con_state_ = internal::ConnectionState::NotConnected;
		chat_room_.RemoveSession(shared_from_this());
		return;
	}

	read_buffer_.clear();
	auto self = shared_from_this();
	boost::asio::async_read_until(socket_,
		boost::asio::dynamic_buffer(read_buffer_),
		'\0',
		[this, self]
	(boost::system::error_code e, size_t len)
	{ HandleScMessage(e); });
}

void Session::HandleScMessage(const boost::system::error_code & error) {
	if (error) {
		con_state_ = internal::ConnectionState::NotConnected;
		chat_room_.RemoveSession(shared_from_this());
		return;
	}

	ScMessage scmsg = ScMessage::Deserialize(read_buffer_);
	Message msg;
	switch (scmsg.GetType()) {
	case ScMessageType::MESSAGE:
		try {
			msg = Message::Deserialize(scmsg.GetProperty("message[0]"));
			if (msg.username != username_)
				msg.username = username_;
			msg.time = std::chrono::system_clock::now();
			chat_room_.DeliverScMessage(GetMessageScMessage(msg));
		}
		catch (std::invalid_argument) {}
		break;
	case ScMessageType::END:
		break;
	default:
		break;
	}

	read_buffer_.clear();
	auto self{ shared_from_this() };
	boost::asio::async_read_until(
		socket_,
		boost::asio::dynamic_buffer(read_buffer_),
		'\0',
		[this, self]
	(const boost::system::error_code& e, size_t len)
	{ HandleScMessage(e); });
}

void Session::WriteScMessageHelper() {
	std::string scmsg_str = msg_queue_.front().Serialize();
	auto buf = boost::asio::buffer(scmsg_str.c_str(), scmsg_str.size() + 1);
	auto self{ shared_from_this() };
	boost::asio::async_write(socket_, buf,
		[this, self](boost::system::error_code err, size_t len) {
		if (!err) {
			msg_queue_.pop_front();
			if (!msg_queue_.empty()) {
				WriteScMessageHelper();
			}
		}
		else {
			con_state_ = internal::ConnectionState::NotConnected;
			chat_room_.RemoveSession(shared_from_this());
		}
	});
}

void Session::SyncWriteMessage(const Message& msg) {
	// ConnectionFailException?
	std::string scmsgstr = GetMessageScMessage(msg).Serialize();
	boost::asio::write(socket_,
		boost::asio::buffer(scmsgstr.c_str(), scmsgstr.size() + 1));
}

}  // namespace server

}  // namespace tyfirc