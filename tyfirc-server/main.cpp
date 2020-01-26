#include <iostream>
#include "tyfirc-ircserverapp.h"
#include "tyfirc-authmanager.h"

int main() {
	using namespace tyfirc;
	auto auth_manager(std::make_shared<server::FileAuthManager>());
	auth_manager->Setup("passwords");

	server::IrcServerApp app(8001, auth_manager);
	app.StartAccept();
	app.Run();
}