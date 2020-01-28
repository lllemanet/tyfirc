#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include "tyfirc-msgpack.h"
#include "tyfirc-scmessage.h"
#include "tyfirc-chatrw.h"
#include "tyfirc-irclientapp.h"


int main() {
	using namespace tyfirc;

	client::IrcClientApp app{};
	std::string prompt;
	std::cout << "Write ip of chat mr client." << std::endl;
	std::getline(std::cin, prompt);
	
	std::cout << "Connect:" << app.Connect(
		boost::asio::ip::address::from_string(prompt).to_v4(), 8001)
		<< std::endl;

	std::string username;
	std::string password;
	std::cout << "Username:" << std::endl;
	std::getline(std::cin, username);
	std::cout << "Password:" << std::endl;
	std::getline(std::cin, password);
	std::cout << "Login:" << app.Login(username, password) << std::endl;
	app.BindHandlerOnMessage([](const Message& msg)
	{ std::cout << msg.username << ": " << msg.text << std::endl; });


	std::thread thr(&client::IrcClientApp::StartRead, &app);
	

	while (true) {
		std::getline(std::cin, prompt);
		std::cin.clear();
		app.WriteMessage(prompt);
	}
}