// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Controllers.

#include <string>
#include <boost/asio.hpp>
#include "tyfirc-scmessage.h"
#include "tyfirc-exceptions.h"
#include "tyfirc-controllers.h"

namespace tyfirc {

namespace client {

bool ConnectionController::Connect(boost::asio::ip::address_v4 address,
		unsigned short port) {
	return socket_->Connect(address, port);
}


bool ConnectionController::Login(std::string username, std::string password) {
	if (!socket_->is_connected())
		throw ConnectionFailException();
	ScMessage login_msg{ ScMessageType::LOGIN };
	login_msg.SetProperty("username", username);
	login_msg.SetProperty("password", password);

	std::string str = static_cast<std::string>(login_msg);
	//socket_.Write()
	//boost::asio::write(*socket_,	boost::asio::buffer(str.c_str(), str.size()));

}

}	 // namespace client

}	 // namespace tyfirc