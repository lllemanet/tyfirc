// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building controller and model environment.
#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-chatsocket.h"
#include "tyfirc-controllers.h"

namespace tyfirc {

namespace client {

// Used to setup controllers. Create and inject into controllers connection
// obect, io_service etc.
//
// Also used to access created controllers.
class IrcClientApp {

public:
	// Initializes service_ and ctx_.
	IrcClientApp() : service_{ std::make_shared<boost::asio::io_service>() } {}

	// Creates controllers with injecting dependencies.
	//
	// SSL ctx_ must be set before Setup call. Otherwise domain_error is
	// throwed.
	//
	// If ctx_ is not appropriate for safe connection, TODO error in 
	// ChatSocket connect.
	bool Setup();

	// Must be set before Setup.
	IrcClientApp& SetCtx(std::shared_ptr<boost::asio::ssl::context> ctx) {
		ctx_ = ctx;
		return *this;
	}

	// Use sslv3 context with specified certificate filename. Throws 
	IrcClientApp& UseDefaultCtx(std::string crt_filename = "server.crt") {
		using boost::asio::ssl::context;
		ctx_ = std::make_shared<context>(context::sslv23);
		ctx_->load_verify_file(crt_filename);
		return *this;
	}


 private:
	std::shared_ptr<boost::asio::io_service> service_;
	std::shared_ptr<boost::asio::ssl::context> ctx_;
	std::shared_ptr<ChatSocket> socket_;
	std::shared_ptr<ConnectionController> connection_controller_;
	std::shared_ptr<ReadController> read_controller_;
	std::shared_ptr<WriteController> write_controller_;

	bool is_crt_loaded; // TODO?
};

} //namespace client
} //namespace tyfirc