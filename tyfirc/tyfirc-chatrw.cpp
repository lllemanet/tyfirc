// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of class that reads and saves ScMessages blocking thread.
#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-exceptions.h"
#include "tyfirc-chatrw.h"

namespace tyfirc {

namespace client {

bool ChatRw::Connect(boost::asio::ip::address_v4 address, unsigned short port){
	if (is_connected_)
		return false;

	boost::asio::ip::tcp::endpoint endpoint{ address, port };
	socket_.set_verify_mode(boost::asio::ssl::verify_peer);
	socket_.set_verify_callback(
		boost::bind(&ChatRw::VerifyCertificate, this, _1, _2));

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

bool ChatRw::Login(std::string username, std::string password) {
	if (!is_connected_)
		throw ConnectionFailException();

	std::string scmsg = AuthScMessage(ScMessageType::LOGIN, username, password).
		ToString();
	//try { 
	//	/*size_t read_len = */socket_.Write(boost::asio::buffer(scmsg.c_str(),
	//			scmsg.size() + 1));
	//	// temp
	//	char buff[512];
	//	socket_.AsyncReadSome(boost::asio::buffer(buff, 512), )
	//}



	
	return true;
}

bool ChatRw::Register(std::string username, std::string password)
{
	return false;
}

void ChatRw::BindHandler(ScMessageType type, void(*handler)(ScMessage)) {
	short type_ind = static_cast<short>(type);
	read_signals_[type_ind].connect(handler);
}

void ChatRw::BindHandlerOnDiscard(void(*handler)(std::string)) {
	discard_signal_.connect(handler);
}

void ChatRw::Run() {

}

bool ChatRw::VerifyCertificate(bool preverified, boost::asio::ssl::verify_context & ctx)
{
	return true; //TODO
}

}  // namespace client

}  // namespace tyfirc