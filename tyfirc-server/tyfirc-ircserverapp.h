// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building and use server app. Name of file could be 
// changed.
#pragma once
#include <vector>
#include <list>
#include <condition_variable>
#include <mutex>
#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-authmanager.h"
#include "tyfirc-scmessage.h"
#include "tyfirc-session.h"
#include "tyfirc-chatroom.h"

namespace tyfirc {

namespace server {

class IrcServerApp {
 public:
	 // Use default ctx.
	IrcServerApp(unsigned short port, std::shared_ptr<IAuthManager>);
	IrcServerApp(unsigned short port, std::shared_ptr<IAuthManager>,
			boost::asio::ssl::context ctx);

	// Starts accept connections. Blocks thread.
	void StartAccept();
	void HandleAccept(std::shared_ptr<Session>,
			const boost::system::error_code& error);

	// Starts reading and writing
	void Run();
 private:
	std::string GetPassword() const {	return "test"; }

	boost::asio::io_service service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::optional<boost::asio::ssl::context> ctx_;
	std::shared_ptr<IAuthManager> auth_manager_;
	ChatRoom chat_room_;
};

}  // namespace server

}  // namespace tyfirc
