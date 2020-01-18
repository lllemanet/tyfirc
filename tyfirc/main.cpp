#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include "tyfirc-msgpack.h"
#include "tyfirc-scmessage.h"
#include "tyfirc-controllers.h"
#include "tyfirc-chatrw.h"

void HelloWorld() {
	std::cout << "Hello world" << std::endl;
}

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

char buffer[512];
boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* socket_;

std::string GetCurTime(std::string format) {

	std::time_t now = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now());
	std::tm tm;
	::localtime_s(&tm, &now);

	std::string res(30, '\0');
	std::strftime(&res[0], res.size(), format.c_str(), &tm);
	return res;
}

void OnRead(boost::system::error_code error, size_t bytes_transferred) {
	buffer[bytes_transferred] = '\0';
	std::cout << "\nOnRead: " << buffer << '\n';
}

void OnWrite(boost::system::error_code error, size_t bytes_transferred) {
	std::cout << "OnWrite" << std::endl;
	//sock->AsyncRead(boost::asio::buffer(buffer, 512), &OnRead);
	socket_->async_read_some(boost::asio::buffer(buffer, 512), &OnRead);
}


int main() {
	std::string date_format = "%d.%m.%Y %H:%M:%S";
	//std::cout << GetCurTime(date_format) << std::endl;

	struct tm t;
	std::stringstream ss(GetCurTime(date_format));
	ss >> std::get_time(&t, date_format.c_str());
	std::string res = ss.str();
	std::cout << std::put_time(&t, date_format.c_str());
	
	boost::signals2::signal<void()> sig;
	sig.connect(&HelloWorld);
	
	sig();

	using namespace tyfirc;
	Message m1{"username1", "text1", std::chrono::system_clock::now() };
	Message m2{"username2", "text2", std::chrono::system_clock::now() + std::chrono::system_clock::duration(2)};
	Message m3{"username3", "text3", std::chrono::system_clock::now() + std::chrono::system_clock::duration(3)};
	Message m4{"username4", "text4", std::chrono::system_clock::now() + std::chrono::system_clock::duration(4)};
	Message m5{"username5", "text5", std::chrono::system_clock::now() + std::chrono::system_clock::duration(5)};
	Message m6{"username6", "text6", std::chrono::system_clock::now() + std::chrono::system_clock::duration(6)};

	std::vector<Message> msgs1{ m3, m1, m5 };
	MessagePack pack1(msgs1);

	std::vector<Message> msgs2{ m2, m4, m6 };
	MessagePack pack2(msgs2);
	pack1.Insert(pack2);



	/*sock = std::make_shared<ChatSocket>(service, ctx);


	client::ConnectionController con_controller(sock);
	std::cout << con_controller.Connect(
		boost::asio::ip::address::from_string("127.0.0.1").to_v4(),
		8001) << std::endl;
	sock->AsyncReadSome(boost::asio::buffer(buffer, 512), &OnRead);
	std::cout << con_controller.Login("username", "12345678") << std::endl;*/
	//sock->AsyncWrite(boost::asio::buffer("some", 5), &OnWrite);
	//boost::asio::async_read(*socket_, boost::asio::buffer(buffer, 512), &OnRead);
	//boost::asio::async_write(*socket_, boost::asio::buffer("some", 5), &OnWrite);
	boost::asio::io_service service{};
	boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23};
	ctx.load_verify_file("server.crt");

	client::ChatRw chatRw{ service, ctx };
	std::cout << chatRw.Connect(
		boost::asio::ip::address::from_string("127.0.0.1").to_v4(),
		8001) << std::endl; 

	
	service.run();
/*
	ScMessage msg{ ScMessageType::LOGIN };
	msg.SetProperty("username", "name");
	msg.SetProperty("password", "12345678");
	std::cout << msg.ToString() << std::endl;

	ScMessageType type1 = ScMessageTypeFromStr("LOGIN");
	std::string stype1 = ScMessageTypeToStr(type1);

	ScMessageType type2 = ScMessageTypeFromStr("djsakj");
	std::string stype2 = ScMessageTypeToStr(type2);

	ScMessage msg2{ScMessage::FromString("LOGIN\nusername: name\npassword: 12345678")};*/
}