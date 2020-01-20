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
	if (is_connected())
		return false;

	boost::asio::ip::tcp::endpoint endpoint{ address, port };
	socket_.set_verify_mode(boost::asio::ssl::verify_peer);
	socket_.set_verify_callback(
		boost::bind(&ChatRw::VerifyCertificate, this, _1, _2));

	try {
		socket_.lowest_layer().connect(endpoint);
		socket_.handshake(boost::asio::ssl::stream_base::client);
		con_state_ = ConnectionState::Connected;
	}
	catch (std::exception&) {
		return false;
	}

	return true;
}

bool ChatRw::Login(const std::string& username, const std::string& password) {
	return Auth(ScMessageType::LOGIN, username, password);
}

bool ChatRw::Register(const std::string& username, const std::string& password)
{
	return Auth(ScMessageType::REGISTER, username, password);
}

void ChatRw::WriteMessage(const Message& msg) {
	if (!is_connected())
		throw ConnectionFailException();

	std::string	scmsg = MessageScMessage(msg).Serialize();
	boost::asio::write(socket_, boost::asio::buffer(scmsg.c_str(), scmsg.size()));
}

bool ChatRw::Auth(ScMessageType type, const std::string& username,
		const std::string& password) {
	if (!is_connected())
		throw ConnectionFailException();
	if (type != ScMessageType::LOGIN && type != ScMessageType::REGISTER)
		throw std::invalid_argument("Auth type must be LOGIN or REGISTER");

	// Write login scmsg and read answer.
	std::string scmsg = AuthScMessage(type, username, password).
		Serialize();
	ScMessage res_scmsg;
	try {
		boost::asio::write(socket_, boost::asio::buffer(scmsg.c_str(),
			scmsg.size() + 1));
		res_scmsg = ReadScMessage();
	}
	catch (boost::system::system_error&) {
		con_state_ = ConnectionState::NotConnected;
		throw;
	}

	ScMessageType success_type = 
			type == ScMessageType::LOGIN ? ScMessageType::LOGIN_SUCCESS :
			ScMessageType::REGISTER_SUCCESS;
	if (res_scmsg.GetType() == success_type) {
		con_state_ = ConnectionState::LoggedIn;
		return true;
	}
	else {
		return false;
	}
}

void ChatRw::BindHandler(ScMessageType type, void(*handler)(ScMessage)) {
	short type_ind = static_cast<short>(type);
	read_signals_[type_ind].connect(handler);
}

void ChatRw::BindHandlerOnDiscard(void(*handler)(std::string)) {
	discard_signal_.connect(handler);
}

void ChatRw::Run() {
	//check if logged in	
}

ScMessage ChatRw::ReadScMessage()
{
	boost::asio::read_until(socket_,
			boost::asio::dynamic_buffer(write_buffer_), '\0');
	ScMessage res;
	try {
		res = ScMessage::Deserialize(write_buffer_);
	}
	catch (std::invalid_argument) {
		res.SetType(ScMessageType::END);
	}
	return res;
}

bool ChatRw::VerifyCertificate(bool preverified, boost::asio::ssl::verify_context & ctx)
{
	return true; //TODO
}

}  // namespace client

}  // namespace tyfirc