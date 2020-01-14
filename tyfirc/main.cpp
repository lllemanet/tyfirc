#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include "tyfirc-msgpack.h"
#include "tyfirc-chatsocket.h"

void HelloWorld() {
	std::cout << "Hello world" << std::endl;
}

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

std::string GetCurTime(std::string format) {

	std::time_t now = std::chrono::system_clock::to_time_t(
			std::chrono::system_clock::now());
	std::tm tm;
	::localtime_s(&tm, &now);

	std::string res(30, '\0');
	std::strftime(&res[0], res.size(), format.c_str(), &tm);
	return res;
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


	boost::asio::io_service service{};
	boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23};
	ctx.load_verify_file("server.crt");

	ChatSocket sock{ service, ctx };
	std::cout << sock.Connect(
			boost::asio::ip::address::from_string("127.0.0.1").to_v4(),
			8001);

}