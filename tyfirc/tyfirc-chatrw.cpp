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

template <typename SyncReadStream>
ScMessage ReadScMessage(SyncReadStream& socket)
{
	// std::string is used since read_until requires dynamic_buffer
	// contains current read message
	std::string buffer;
	boost::asio::read_until(socket,
		boost::asio::dynamic_buffer(buffer), '\0');
	ScMessage res;
	try {
		res = ScMessage::Deserialize(buffer);
	}
	catch (std::invalid_argument) {
		res.SetType(ScMessageType::END);
		res.SetProperty("data", buffer);	// copy invalid string into data
	}
	return res;
}

template <typename SyncReadStream>
void WriteScMessage(SyncReadStream& socket, const ScMessage& scmsg) {
	std::string scmsg_str = scmsg.Serialize();
	boost::asio::write(socket, boost::asio::buffer(scmsg_str.c_str(),
			scmsg_str.size() + 1));
}

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

	ScMessage scmsg = GetMessageScMessage(msg);
	WriteScMessage(socket_, scmsg);
}

bool ChatRw::Auth(ScMessageType type, const std::string& username,
		const std::string& password) {
	if (!is_connected())
		throw ConnectionFailException();
	if (type != ScMessageType::LOGIN && type != ScMessageType::REGISTER)
		throw std::invalid_argument("Auth type must be LOGIN or REGISTER");

	// Write login scmsg and read answer.
	ScMessage res_scmsg;
	try {
		ScMessage query_msg = GetAuthScMessage(type, username, password);
		WriteScMessage(socket_, query_msg);
		res_scmsg = ReadScMessage(socket_);
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

boost::signals2::connection ChatRw::BindHandlerOnMessage(
		void(*handler)(const Message&)) {
	return message_signal_.connect(handler);
}

boost::signals2::connection ChatRw::BindHandlerOnDiscard(
		void(*handler)(const std::string&)) {
	return discard_signal_.connect(handler);
}

void ChatRw::StartRead() {
	if (!is_logged_in())
		throw ConnectionFailException("Not logged in");
	is_read_ = true;

	while (is_read_) {
		ScMessage scmsg = ReadScMessage(socket_);
		Message msg;
		switch (scmsg.GetType()) {
		case ScMessageType::MESSAGE:
			msg = Message::Deserialize(scmsg.GetProperty("message[0]"));
			message_signal_(msg);
			break;
		case ScMessageType::END:
			discard_signal_(scmsg.GetProperty("data"));
			break;
		default:
			break;
		}
	}
}

bool ChatRw::VerifyCertificate(bool preverified, boost::asio::ssl::verify_context & ctx)
{
	return true; //TODO
}

}  // namespace client

}  // namespace tyfirc