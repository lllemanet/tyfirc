// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of class that reads and saves ScMessages blocking thread.
#pragma once
#include <vector>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include "tyfirc-scmessage.h"
#include "tyfirc-chatsocket.h"


namespace tyfirc {

namespace client {

// This class is used to read/write ScMessages from/to server. You can bind handler
// on each arriving message type.
// Class's method Run() blocks thread and invoke asio io_service run method
// so it requires distinct thread.
class ChatRw {
 public:
	ChatRw(std::shared_ptr<boost::asio::io_service> service, 
						 std::shared_ptr<ChatSocket> socket) 
			: service_{service}, socket_{socket} {}

	// Bind handler with specified type. Handler is invoked just after
	// ScMessage with this type was read from ChatSocket.
	void BindHandler(ScMessageType type, void(*handler)(ScMessage));

	// Bind handler if message from socket is invalid.
	void BindHandlerOnDiscard(void(*handler)(std::string));

	// Starts reading from ChatSocket and invoke handlers after message from 
	// socket is read. If message is invalid discard it.
	// Blocks thread.
	// If any error happens returns(connection lost/not established etc).
	void Run();
 private:
	std::shared_ptr<boost::asio::io_service> service_;
	std::shared_ptr<ChatSocket> socket_;

	std::array<boost::signals2::signal<void(ScMessage)>, sc_types_size> signals_;
	boost::signals2::signal<void(std::string)> discard_signal_;
};

}	 // namespace client

}  // namespace tyfirc