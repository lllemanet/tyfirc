// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Defines class methods for building and use server app. Name of file could be 
// changed.
#include <vector>
#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-ircserverapp.h"
#include "tyfirc-authmanager.h"

namespace tyfirc {

namespace server {

IrcServerApp::IrcServerApp(unsigned short port, 
													 std::shared_ptr<IAuthManager> auth_manager)
		: service_{}, 
			acceptor_{service_, 
			boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)},
			auth_manager_{auth_manager} {
	ctx_ = boost::asio::ssl::context(boost::asio::ssl::context::sslv23);
	ctx_->set_options(
				boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use);
	ctx_->set_password_callback(boost::bind(&IrcServerApp::GetPassword, this));
	ctx_->use_certificate_chain_file("server.crt");
	ctx_->use_private_key_file("server.key", boost::asio::ssl::context::pem);
	ctx_->use_tmp_dh_file("dh1024.pem");
}

IrcServerApp::IrcServerApp(unsigned short port,
													 std::shared_ptr<IAuthManager> auth_manager,
													 boost::asio::ssl::context ctx)
		: service_{}, acceptor_{ service_,
			boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port) },
			auth_manager_ {auth_manager},
			ctx_{ std::move(ctx) } {}

void IrcServerApp::StartAccept() {
	std::shared_ptr<Session> new_session{std::make_shared<Session>(
			service_, *ctx_, auth_manager_)};
	acceptor_.async_accept(new_session->socket(),
		[this, new_session](const boost::system::error_code& error) 
		{ HandleAccept(new_session, error); });
}

// After accept we 'atomically' push session to sessions_ and switch session
// to connected state.
void IrcServerApp::HandleAccept(std::shared_ptr<Session> new_session,
		const boost::system::error_code & error) {
	if (!error) {
		sessions_.push_back(new_session);
		new_session->Start();
	}
	else {
		// "delete new_session"
	}
	StartAccept();
}

}  // namespace server

}  // namespace tyfirc

