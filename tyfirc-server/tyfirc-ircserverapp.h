// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building and use server app. Name of file could be 
// changed.
#pragma once
#include <vector>
#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-authmanager.h"
#include "tyfirc-session.h"

namespace tyfirc {

namespace server {

class IrcServerApp {
 public:
	IrcServerApp(unsigned short port, std::shared_ptr<IAuthManager>);	// Use default ctx
	IrcServerApp(unsigned short port, std::shared_ptr<IAuthManager>,
			boost::asio::ssl::context ctx);

	// Starts accept connections. Blocks thread.
	void StartAccept();
	void HandleAccept(std::shared_ptr<Session>,
			const boost::system::error_code& error);

	void Run() { service_.run(); }
 private:
	std::string GetPassword() const {	return "test"; }

	boost::asio::io_service service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::optional<boost::asio::ssl::context> ctx_;
	std::shared_ptr<IAuthManager> auth_manager_;
	std::vector<std::shared_ptr<Session>> sessions_;
};

}  // namespace server

}  // namespace tyfirc
