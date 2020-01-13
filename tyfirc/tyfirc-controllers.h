// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Controllers.
#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/signals2.hpp>
#include "tyfirc-chatsocket.h"
#include "tyfirc-msgpack.h"

namespace tyfirc {
// This controller responsible for establishing connection and logging in.
class ConnectionController {
 public:
	 ConnectionController(std::shared_ptr<ChatSocket> socket) : socket_{socket} {}

	// Trying to establish safe connection to address::port. Returns true if
	// successfully and false otherwise. Returns false if connection was 
	// established already.
	bool Connect(boost::asio::ip::address_v4 address, unsigned short port);

	// Trying to login to chat if username-password was registred. Returns result
	// of login try. Connection must be established before call (otherwise false
	// is returned).
	bool Login(std::string username, std::string password);

	// Trying to register to chat. If username is specified, returns false. 
	// Connection must be established before call (otherwise false is returned).
	bool Register(std::string username, std::string password);
 private:
	 std::shared_ptr<ChatSocket> socket_;	//this object is shared among all controllers
};


// Used to write messages to chat synchronously.
class WriteController {
 public:
	WriteController(std::shared_ptr<ChatSocket> socket) : socket_{ socket } {}

	// Synchronoulsy writes message to chat if connection is established.
	bool WriteMsg(std::string msg);
 private:
	 std::shared_ptr<ChatSocket> socket_;	//this object is shared among all controllers
};


// This class is used for reading info from chat server. It contains next
// signals:
// -OnMsgPackArrive;
// to which user connect their handlers to process input.
//
// Run() invocation blocks thread by service_.run() (which execute handlers)
// SO Run() call better be done in separate thread.
class ReadController {
	using OnMsgPackArrive = boost::signals2::signal<void(MessagePack)>;

 public:
	ReadController(std::shared_ptr<ChatSocket> socket)
		: socket_{socket} {}

	//Run();

	boost::signals2::connection DoOnMsgPackArrive(const OnMsgPackArrive& handler);

 private:
	std::shared_ptr<ChatSocket> socket_;	//this object is shared among all controllers
};
}  // namespace tyfirc