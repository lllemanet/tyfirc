// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building controller and model environment.
#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-controllers.h"

namespace tyfirc {

using LinkageControllerSptr = std::shared_ptr<LinkageController>;
using WriteControllerSptr = std::shared_ptr<WriteController>;
using ReadControllerSptr = std::shared_ptr<ReadController>;

// Used to setup controllers. Create and inject into controllers connection
// obect, io_service etc.
//
// Also used to access created controllers.
class IrcClient {

 public:
	// Initializes service_ and ctx_.
	IrcClient();

	// Creates controllers with injecting dependencies.
	bool Setup();

	// Must be set before Setup. Otherwise ctor-specified ctx_ is used.
	IrcClient& SetCtx(ssl_context_sptr ctx) {
		ctx_ = ctx;
		return *this;
	}

 private:
	io_service_sptr service_;
	ssl_context_sptr ctx_;
	ssl_socket_sptr socket_;

	LinkageControllerSptr linkage_controller_;
	ReadControllerSptr read_controller_;
	WriteControllerSptr write_controller_;
};

} //namespace tyfirc