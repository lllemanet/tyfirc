// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Definition of ChatSocket.


#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "tyfirc-chatsocket.h"
#include "tyfirc-scmessage.h"
#include "tyfirc-exceptions.h"

namespace tyfirc {

bool ChatSocket::Connect(boost::asio::ip::address_v4 address, unsigned short port) {
	if (is_connected_)
		return false;

	boost::asio::ip::tcp::endpoint endpoint{ address, port };
	socket_.set_verify_mode(boost::asio::ssl::verify_peer);
	socket_.set_verify_callback(
			boost::bind(&ChatSocket::VerifyCertificate, this, _1, _2));
	
	try {
		socket_.lowest_layer().connect(endpoint);
		socket_.handshake(boost::asio::ssl::stream_base::client);
		is_connected_ = true;
	}
	catch (std::exception& e) {
		return false;
	}

	return true;
}

// Check if certificate is valid for the peer.
bool ChatSocket::VerifyCertificate(bool preverified,
		boost::asio::ssl::verify_context& ctx) {
	// TODO
	return true;
}

}	 // namespace tyfirc