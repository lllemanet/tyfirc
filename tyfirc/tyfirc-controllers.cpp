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
	bool res = socket_->Connect(address, port);
	// Signal
	return res;
}


bool ConnectionController::Login(std::string username, std::string password) {
	if (!socket_->is_connected())
		throw ConnectionFailException();
	

	std::string str = AuthScMessage(ScMessageType::LOGIN, username, password).
			ToString();
	size_t read_len = socket_->Write(
			boost::asio::buffer(str.c_str(), str.size() + 1));
	//boost::asio::write(*socket_,	boost::asio::buffer(str.c_str(), str.size()));
	return true;
}

}	 // namespace client

}	 // namespace tyfirc