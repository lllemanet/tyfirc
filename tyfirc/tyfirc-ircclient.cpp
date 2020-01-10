// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building controller and model environment.

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-irclient.h"

namespace tyfirc {

IrcClient::IrcClient()
		: service_{ std::make_shared<boost::asio::io_service>() },
			ctx_(std::make_shared<boost::asio::ssl::context>
					(boost::asio::ssl::context::sslv23)) {
	ctx_->load_verify_file("server.crt");
}

bool IrcClient::Setup() {
	return true;
}

}  // namespace tyfirc