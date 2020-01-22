// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building and use client app.
#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-chatrw.h"

namespace tyfirc {

namespace client {

// Used to setup controllers. Create and inject into controllers connection
// object, io_service etc.
//
// Also used to access created controllers.
// Can be only moved.
class IrcClientApp {

public:
	// Use default ctx
	IrcClientApp();

	IrcClientApp(boost::asio::ssl::context ctx);

	// Trying to establish safe connection to address::port. Returns true if
	// successfully and false otherwise. Returns false if connection was 
	// established already.
	bool Connect(boost::asio::ip::address_v4 address, unsigned short port) {
		return chat_rw_->Connect(address, port);
	}

	// Trying to login to chat if username-password was registred. Returns result
	// of login try. Connection must be established before call (otherwise false
	// is returned).
	// TODO throws
	bool Login(std::string username, std::string password);

	// Trying to register to chat. If username is specified, returns false. 
	// Connection must be established before call (otherwise false is returned).
	// TODO throws
	bool Register(std::string username, std::string password);

	// Synchronoulsy writes message to chat if logged in.
	// Message should NOT contain \x001f or 31th symbol.
	// TODO throws
	void WriteMessage(std::string msg);

	boost::signals2::connection BindHandlerOnMessage(void(*h)(const Message&)) {
		return chat_rw_->BindHandlerOnMessage(h);
	}

	boost::signals2::connection BindHandlerOnDiscard(void(*h)(const std::string&)){
		return chat_rw_->BindHandlerOnDiscard(h);
	}

	// Read ChatRw::StartRead comment for more info.
	void StartRead() { chat_rw_->StartRead();	}

	// Read ChatRw::StopRead comment for more info.
	void StopRead() {	chat_rw_->StopRead(); }

	IrcClientApp(IrcClientApp&&) = default;
	IrcClientApp& operator=(IrcClientApp&&) = default;

 private:
	boost::asio::io_service service_;
	// boost::none on object creation. Must be set before Setup.
	boost::optional<boost::asio::ssl::context> ctx_;
	boost::optional<ChatRw> chat_rw_;	//boost::none on creation
	
	std::string username_;
};

} //namespace client
} //namespace tyfirc