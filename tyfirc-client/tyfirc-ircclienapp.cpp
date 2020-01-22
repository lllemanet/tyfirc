// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building controller and model environment.

#include <chrono>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-msgpack.h"
#include "tyfirc-chatrw.h"
#include "tyfirc-irclientapp.h"

namespace tyfirc {

namespace client {

IrcClientApp::IrcClientApp() : service_{} {
	using boost::asio::ssl::context;
	ctx_ = context(context::sslv23);
	ctx_->load_verify_file("server.crt");
	chat_rw_.emplace(service_, *ctx_);
}

IrcClientApp::IrcClientApp(boost::asio::ssl::context ctx) 
		: service_{}, ctx_{ std::move(ctx) } {
	chat_rw_.emplace(service_, *ctx_);
}

bool IrcClientApp::Login(std::string username, std::string password) {
	bool res = chat_rw_->Login(username, password);
	if (res)
		username_ = username;
	return res;
}

bool IrcClientApp::Register(std::string username, std::string password) {
	bool res = chat_rw_->Login(username, password);
	if (res)
		username_ = username;
	return res;
}

void  IrcClientApp::WriteMessage(std::string msg) {
	if (msg.find('\x001f') != std::string::npos)
		throw std::invalid_argument("Message text should not contain \x001f symbol");

	Message res_msg;
	res_msg.username = username_;
	res_msg.time = std::chrono::system_clock::now();
	res_msg.text = msg;
	chat_rw_->WriteMessage(res_msg);
}

}	 // namespace client

}  // namespace tyfirc