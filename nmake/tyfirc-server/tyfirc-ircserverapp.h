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
#include "tyfirc-sessionlist.h"

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

	// Synchronously writes messages from scmsg_queue to sessions. Since it
	// blocks thread must be invoked in separate thread. 
	// To notify thread about new_msg arrived use new_msg_cv_ condition var.
	// We stop tracking session if sesion is not connected or write is 
	// unsuccessful.
	void StartWrite();

	// Adds message to msg_queue and notifies Write thread about new message.
	void AddMessage(const Message& new_msg);
 private:
	std::string GetPassword() const {	return "test"; }

	boost::asio::io_service service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::optional<boost::asio::ssl::context> ctx_;
	std::shared_ptr<IAuthManager> auth_manager_;
	SessionList sessions_;

	// Contains sequence of messages to be written to sessions with logged in 
	// clients.
	std::list<Message> msg_queue_;
	// Used to notify Write thread about new msg.
	std::unique_ptr<std::thread> write_msg_thread_;
	std::condition_variable new_msg_cv_;
	std::mutex new_msg_mutex_;	// mutex for new_msg_cv_

};

}  // namespace server

}  // namespace tyfirc
