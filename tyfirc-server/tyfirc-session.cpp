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
			{ shared_this->HandleHandshake(e); });
}

void Session::HandleHandshake(const boost::system::error_code & error) {
	if (!error) {
		boost::asio::async_read_until(
				socket_,
				boost::asio::dynamic_buffer(read_buffer_),
				'\0',
				[shared_this = shared_from_this()]
				(const boost::system::error_code& e, size_t len)
				{ shared_this->HandleScMessageRead(e); });
	}
	else {
		// "delete this"
	}
}


void Session::HandleScMessageRead(const boost::system::error_code & error) {
	ScMessage scmsg = ScMessage::Deserialize(read_buffer_);
	switch (scmsg.GetType()) {
	case ScMessageType::LOGIN:	//HandleLogin method
		if (auth_manager_->Login(scmsg.GetProperty("username"),
				scmsg.GetProperty("password"))) {
			con_state_ = internal::ConnectionState::LoggedIn;
			std::string answer = ScMessageTypeToStr(ScMessageType::LOGIN_SUCCESS);
			boost::asio::async_write(socket_, boost::asio::buffer(answer, answer.size()),
					[](const boost::system::error_code&, size_t) {
				/*if error*/});
		}
		else {
			std::string answer = ScMessageTypeToStr(ScMessageType::LOGIN_FAILURE);
			boost::asio::async_write(socket_, boost::asio::buffer(answer, answer.size()),
					[](const boost::system::error_code&, size_t) {});

		}

		break;
	case ScMessageType::END:
		break;
	default:
		break;
	}
	std::cout << read_buffer_ << std::endl;
}

}  // namespace server

}  // namespace tyfirc